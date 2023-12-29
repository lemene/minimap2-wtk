#ifdef __cplusplus
extern "C" {
#endif

void wtk_set_kc_fname(const char* fname);
void wtk_initialize();
int wtk_is_valid_kmer(uint64_t kmer);
#ifdef __cplusplus
}   // extern "C" {
#endif 