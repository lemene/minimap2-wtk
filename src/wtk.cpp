#include <unordered_set>
#include <fstream>
#include <memory>
#include <algorithm>

#include "mmpriv.h"
#include <iostream>
#include "../ext/bloom/bloom_filter.hpp"

std::vector<std::string> SplitStringBySpace(const std::string &str) {
    std::vector<std::string> substrs;

    auto IsSpace = [](char c) { return ::isspace(c); };
    auto IsNotSpace = [](char c) { return !::isspace(c); };

    auto begin = std::find_if(str.begin(), str.end(), IsNotSpace);

    while (begin != str.end()) {
        auto end = std::find_if(begin, str.end(), IsSpace);
        substrs.push_back(std::string(begin, end));
        begin = std::find_if(end, str.end(), IsNotSpace);
    }

    return substrs;
}


static inline uint64_t hash64(uint64_t key)
{
	key = (~key + (key << 21));
	key = key ^ key >> 24;
	key = ((key + (key << 3)) + (key << 8));
	key = key ^ key >> 14;
	key = ((key + (key << 2)) + (key << 4));
	key = key ^ key >> 28;
	key = (key + (key << 31));
	return key;
}

uint8_t s_table[256];
uint64_t shift1 = 2 * (19 - 1), mask = (1ULL<<2*19) - 1;

uint64_t str2kmer(const std::string &str) {
    
    uint64_t kmer = 0;
    for (auto a : str) {
		kmer = (kmer << 2 | s_table[a]) & mask;           // forward k-mer

    }
    return kmer;
}



class KmerCount {
public:
    KmerCount() {    }
    void set_kc_fname(const std::string& fname) {
        kc_fname_ = fname;
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

            
            std::string kmer;
	        uint64_t cnt = 0;
	        uint64_t freq;
	        while (file >> kmer >> freq) {
                cnt ++;
            }
            
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

static std::shared_ptr<KmerCount> s_kmer_count = std::shared_ptr<KmerCount>(new KmerCount);

extern "C" int wtk_is_valid_kmer(uint64_t kmer) {
    return s_kmer_count->is_valid_kmer(kmer) ? 1 : 0;
}

extern "C" int wtk_set_kc_fname(const char* fname) {
    s_kmer_count->set_kc_fname(fname);
}

extern "C" int wtk_initialize() {
    return s_kmer_count->load() ? 1 : 0;
}


