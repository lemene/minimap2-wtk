#include "wtk.hpp"

#include <algorithm>
#include <memory>
#include <vector>

#include "active_kmers.hpp"
#include "utils.hpp"

class Wtk {
public:
    bool set_active_kmers(const std::string& paramters);
    bool set_kmer_weights(const std::string& wts);
    bool initialize(size_t threads); 
    bool is_active_kmer(uint64_t kmer) const { kmers_->is_active_kmer(kmer); };

protected:
    std::vector<std::string> kmer_fnames_;
    std::vector<double> kmer_weights_;
    std::shared_ptr<ActiveKmers> kmers_;
};

bool Wtk::set_active_kmers(const std::string& fnames) {
    // fnames = "fname0;fname1;fname2"
    kmer_fnames_ = split_string(fnames, [](char c) { return c == ';'; });
    return kmer_weights_.size() == 0 || kmer_weights_.size() == kmer_fnames_.size();
}

bool Wtk::set_kmer_weights(const std::string& weights) {
    // weights = "3;2;1"
    auto wts = split_string(weights, [](char c) { return c == ';'; });

    if (kmer_fnames_.size() == 0 || kmer_fnames_.size() == wts.size()) {
        kmer_weights_.clear();
        for (auto w : wts) {
            kmer_weights_.push_back(std::stod(w));
        }
        for (size_t i = 1; i < kmer_weights_.size(); ++i) {
            if (kmer_weights_[i-1] < kmer_weights_[i]) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

bool Wtk::initialize(size_t threads) {
    kmers_ = std::shared_ptr<ActiveKmers>(new ActiveKmers(kmer_fnames_, kmer_weights_, threads));
}



static Wtk s_wtk;

extern "C" int wtk_set_active_kmers(const char* parameters) {
    return s_wtk.set_active_kmers(parameters) ? 1 : 0;
}

extern "C" int wtk_set_kmer_weights(const char* parameters) {
    return s_wtk.set_kmer_weights(parameters) ? 1 : 0;
}

extern "C" int wtk_initialize(int threads) {
    return s_wtk.initialize((size_t)threads) ? 1 : 0;
}

extern "C" int wtk_is_active_kmer(uint64_t kmer) {
    return s_wtk.is_active_kmer(kmer) ? 1 : 0;
}



