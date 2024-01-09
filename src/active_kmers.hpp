#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

#include "../ext/bloom/bloom_filter.hpp"

class KmerGroup {
public:
    KmerGroup(const std::string &kmer_fname, double kmer_weight, size_t threads=1);
    bool contains(uint64_t kmer) const { return bloom_->contains(kmer); }

protected:
    bool load(const std::string& fname, size_t threads);
    std::shared_ptr<bloom_filter> make_bloom(size_t count);
    static size_t actual_threads(size_t threads)  { return std::min<size_t>(threads, 20); }
    double weight_ { 1.0 };
    std::shared_ptr<bloom_filter> bloom_;
};

class ActiveKmers {
public:
    ActiveKmers(const std::vector<std::string> &kmer_fnames, const std::vector<double> &kmer_weights, size_t threads=1);

    bool is_active_kmer(uint64_t kmer) {
        for (auto& g : kmer_groups_) {
            if (g.contains(kmer)) {
                return true;
            }
        }
        return false;
    }
    
    std::vector<KmerGroup> kmer_groups_;
};

