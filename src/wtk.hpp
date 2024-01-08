#ifdef __cplusplus
extern "C" {
#endif

int wtk_set_parameters(const char* parameters);
int wtk_initialize();
int wtk_is_active_kmer(uint64_t kmer);
#ifdef __cplusplus
}   // extern "C" {
#endif 