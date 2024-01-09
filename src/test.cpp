#include <cstdio>
#include <cassert>
#include <fstream>
#include <string>
#include <stdint.h>

#include "wtk.hpp"


int main(int argc, const char* argv[]) {
    if (argc > 1) {
        if (wtk_set_active_kmers(argv[1])) {
            printf("active ok\n");
        } else {
            printf("active failed\n");
        }
    }

    if (argc > 2) {
        if (wtk_set_kmer_weights(argv[2])) {
            printf("weight ok\n");
        } else {
            printf("weight failed\n");
        }
    }

    wtk_initialize(10);
    std::ifstream ifs(argv[1]);
    std::string kmer;
    int freq;
    extern uint64_t str2kmer(const std::string &str);
    int test_cnt = 0;
    while (ifs >> kmer >> freq) {
        assert(wtk_is_active_kmer(str2kmer(kmer)));
        test_cnt ++;
    }
    printf("test_cnt = %d\n", test_cnt);

    
}