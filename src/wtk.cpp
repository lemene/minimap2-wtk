#include "utils.hpp"


class Wtk {
public:
    bool set_active_kmers(const std::string& paramters);
    bool set_kmer_weights(const std::string& wts);
    bool initialize(); 

protected:
    std::vector<std::string> kmer_fnames_;
    std::vector<int> kmer_weights_;
    std::shared_ptr<ActiveKmers> kmers_;
};

bool Wtk::set_active_kmers(const std::string& fnames) {
    // fnames = "fname0;fname1;fname2"
    kmer_fnames_ = split_string(fnames, [](char c) { return c == ';'; });
    return kmer_weights_.size() == 0 || kmer_weights_.size() == kmer_fnames_.size();
}

bool set_kmer_weights(const std::string& weights) {
    // weights = "3;2;1"
    auto wts = split_string(weights, [](char c) { return c == ';'; });

    if (kmer_fnames_.size() == 0 || kmer_fnames_.size() == wts.size()) {
        kmer_weights_.clear();
        for (auto w : wts) {
            kmer_weigths.push_back(std::stoi(w));
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

bool Wtk::initialize() {

}
static Wtk s_wtk;

extern "C" int wtk_set_parameters(const char* parameters) {
    s_wtk.set_parameters(paramters);
}

extern "C" int wtk_initialize() {
    return s_wtk->initialize() ? 1 : 0;
}

extern "C" int wtk_is_active_kmer(uint64_t kmer) {
    return s_kmer_count->is_valid_kmer(kmer) ? 1 : 0;
}



