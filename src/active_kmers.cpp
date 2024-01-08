#include "active_kmers.hpp"

#include "utils.hpp"

bool KmerGroup::Load(const std::string& fname, size_t threads) {
 
    std::ifstream ifs(fname);
    size_t count = count_lines_in_file(ifs, 4);
    bloom = std::shared_ptr<bloom_filter>(make_bloom(count));

    BlockReader reader(ifs, '\n');
    auto gen_func = [&reader](char* block, size_t bsize) {
        std::lock_guard<std::mutex> lock;
        return reader.Get(block, bsize);
    }

    auto work_func = [](size_t id) {
        size_t batch_size = 1000;
        std::vector<uint64_t> batch(batch_size);

        char block[1024*1000];
        size_t bmax = sizeof(block);
        for (size_t bsize = gen_func(block, bmax); bsize > 0; bsize = gen_func(block, bmax)) {
            std::istringstream iss;
        }
    }
}

bloom_filter* KmerGroup::make_bloom(size_t count) {

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
    return new bloom_filter(parameters);
}

class KmerCount {
public:
    KmerCount() {    }
    void set_kc_fname(const std::string& fname) {
        kc_fname_ = fname;
    }

    void set_kmer_levels(const std::string grade) {
        
    }

    bool load() {

        s_table['A'] = 0;
        s_table['C'] = 1;
        s_table['G'] = 2;
        s_table['T'] = 3;

        std::ifstream file(kc_fname_);
        if (file.is_open()) {
            fprintf(stderr, "[M::%s::%.3f*%.2f] load kmer count\n",
                    __func__, realtime() - mm_realtime0, cputime() / (realtime() - mm_realtime0));

            size_t line_numer = roughly_count_lines_in_file(file);
            fprintf(stderr, "[M::%s::%.3f*%.2f] load kmer count r %zd\n",
                    __func__, realtime() - mm_realtime0, cputime() / (realtime() - mm_realtime0), line_numer);

            line_numer = count_lines_in_file(file, 4);
            fprintf(stderr, "[M::%s::%.3f*%.2f] load kmer count z %zd\n",
                    __func__, realtime() - mm_realtime0, cputime() / (realtime() - mm_realtime0), line_numer);

            std::string kmer;
	        uint64_t cnt = 0;
	        uint64_t freq;
            char buff[1000*16];

            while (file.read(buff, sizeof(buff))) {
                // for (size_t i = 0; i < file.gcount(); ++i) {
                //     if (buff[i] =='\n') {
                //         cnt++;
                //     }
                // }
                char* end = buff + file.gcount();
                for (char* p = buff; p < end; ++p) {
                    if (*p == '\n') {
                        cnt ++;
                    }
                }
            }
            // char line[1024];
            // while (!file.eof()) {
            //     file.get(line, 1024);
            //     cnt ++;
            //     file.get();
            //     //printf("%llu, %zd, %s\n", cnt, file.gcount(), line);
            // }

	        // while (file >> kmer >> freq) {
            //     cnt ++;
            // }

            fprintf(stderr, "[M::%s::%.3f*%.2f] load kmer count %llu\n",
                    __func__, realtime() - mm_realtime0, cputime() / (realtime() - mm_realtime0), cnt);
            
            filter_ = make_bloom(cnt);
            file.clear();
            file.seekg(0);
            if (file >> kmer >> freq) {
                k_ = kmer.size();
                //kmers_.insert(str2kmer(kmer));
                filter_->insert(str2kmer(kmer));
                cnt++;
            }
            while (file >> kmer >> freq) {
                //kmers_.insert(str2kmer(kmer));
                filter_->insert(str2kmer(kmer));
		        cnt++;
            }

            fprintf(stderr, "[M::%s::%.3f*%.2f] load kmer count %llu\n",
                    __func__, realtime() - mm_realtime0, cputime() / (realtime() - mm_realtime0), cnt);
        } else {
        }
    }

    bool is_valid_kmer(uint64_t kmer) {
        return filter_->contains(kmer);
    }

protected:
    bloom_filter* make_bloom(size_t count) {

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
        return new bloom_filter(parameters);

    }
protected:
    std::string kc_fname_;
    std::unordered_set<uint64_t> kmers_;
    uint8_t k_ { 0 };
	bloom_filter *filter_ { nullptr };
};