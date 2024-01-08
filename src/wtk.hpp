#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

int wtk_set_active_kmers(const char* parameters);
int wtk_set_kmer_weights(const char* parameters);
int wtk_initialize(int threads);
int wtk_is_active_kmer(uint64_t kmer);
#ifdef __cplusplus
}   // extern "C" {
#endif 