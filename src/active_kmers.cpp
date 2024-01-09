#include "active_kmers.hpp"

#include <cassert>
#include <thread>
#include <mutex>
#include <sstream>

#include "utils.hpp"

struct BaseTable {
    BaseTable() {
        table['A'] = 0;
        table['C'] = 1;
        table['G'] = 2;
        table['T'] = 3;
        table['a'] = 0;
        table['c'] = 1;
        table['g'] = 2;
        table['t'] = 3;
    }
    uint8_t get(uint8_t c) const { return table[c]; }
    uint8_t table[256];
};
BaseTable s_base_table;
uint64_t shift1 = 2 * (19 - 1), mask = (1ULL<<2*19) - 1;

uint64_t str2kmer(const std::string &str) {
    
    uint64_t kmer = 0;
    for (auto a : str) {
		kmer = (kmer << 2 | s_base_table.get(a)) & mask;           // forward k-mer

    }
    return kmer;
}

KmerGroup::KmerGroup(const std::string &kmer_fname, double kmer_weight, size_t threads) {
    weight_ = kmer_weight;
    load(kmer_fname, threads);
}

bool KmerGroup::load(const std::string& fname, size_t threads) {
 
    std::atomic<size_t> scount {0};
    std::mutex mutex_gen;
    std::mutex mutex_comb;
    std::ifstream ifs(fname);
    size_t count = count_lines_in_file(ifs, actual_threads(threads));
    bloom_ = std::shared_ptr<bloom_filter>(make_bloom(count));

    ifs.clear();
    ifs.seekg(0);
    BlockReader reader(ifs, '\n');
    auto gen_func = [&reader, &mutex_gen](char* block, size_t bsize) {
        std::lock_guard<std::mutex> lock(mutex_gen);
        return reader.get(block, bsize);
    };

    auto comb_func = [this, &mutex_comb, &scount](const std::vector<uint64_t> kmers) {
        //std::lock_guard<std::mutex> lock(mutex_comb);

        for (auto k : kmers) {
            bloom_->insert(k);
        }
        scount.fetch_add(kmers.size());
    };

    auto work_func = [&gen_func, &comb_func](size_t id) {
        printf("work_func\n");
        size_t batch_size = 10000;
        std::vector<uint64_t> batch;

        char block[1*1024*1024];
        size_t bmax = sizeof(block);
        for (size_t bsize = gen_func(block, bmax); bsize > 0; bsize = gen_func(block, bmax)) {
            std::istringstream iss(std::string(block, block+bsize));
            
            std::string kmer;
            uint64_t freq;
            while (iss >> kmer >> freq) {
                batch.push_back(str2kmer(kmer));
            }

            if (batch.size() >= batch_size) {
                comb_func(batch);
                batch.clear();
            }
        }
        comb_func(batch);

    };

    MultiThreadRun(actual_threads(threads), work_func);
    printf("count = %zd\n", scount.load());
}

std::shared_ptr<bloom_filter> KmerGroup::make_bloom(size_t count) {

    //set up bloom filter
    bloom_parameters parameters;
    parameters.projected_element_count = std::max<size_t>(count, (uint64_t)1000);
    parameters.false_positive_probability = 0.001; 
    parameters.maximum_number_of_hashes = 2;

    // if (!parameters)
    // {
    //     std::cout << "Error - Invalid set of bloom filter parameters!" << std::endl;
    //     exit(1);
    // }

    parameters.compute_optimal_parameters();
    return std::shared_ptr<bloom_filter>(new bloom_filter(parameters));
}

ActiveKmers::ActiveKmers(const std::vector<std::string> &kmer_fnames, const std::vector<double> &kmer_weights, size_t threads) {
    assert(kmer_fnames.size() == kmer_weights.size() || kmer_weights.size() == 0);

    for (size_t i = 0; i < kmer_fnames.size(); ++i) {
        double wt = kmer_weights.size() == 0 ? 1.0 : kmer_weights[i];
        kmer_groups_.push_back(KmerGroup(kmer_fnames[i], wt, threads));
    }
}
