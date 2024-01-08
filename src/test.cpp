#include <cstdio>

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

    
}