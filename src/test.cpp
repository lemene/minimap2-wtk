
#include <thread>
#include <atomic>
#include <mutex>

#include <fstream>
#include <vector>

template<typename W>
void MultiThreadRun(size_t thread_size, W work_func) {
    std::vector<std::thread> workers;
    printf("sdfasdf: ww %zd\n", thread_size); fflush(stdout);
    for (size_t i = 0; i < thread_size; ++i) {
        workers.push_back(std::thread(work_func, i));
    }
    printf("sdfasdf: 1 %zd\n", thread_size); fflush(stdout);

    for (size_t i = 0; i < workers.size(); ++i) {
        workers[i].join();
    }
    printf("sdfasdf: 2 %zd\n", thread_size); fflush(stdout);
}


size_t count_lines_in_file(std::ifstream& ifs, size_t threads) {
    ifs.seekg(0);
    std::string line;
    std::mutex mutex;
    std::atomic<size_t> total { 0 };

    auto generate_block = [&ifs, &mutex](char* buf, size_t bufsize) -> size_t {
        //std::lock_guard<std::mutex> lock(mutex);
    printf("sdfasdf: %zd\n", bufsize); fflush(stdout);

        return 0;
        //ifs.read(buf, bufsize);
        //return ifs.gcount();
    };
    auto worker_count = [](size_t id) {
        //char buf[1024*10];
    printf("sdfasdf: id %zd\n", id); fflush(stdout);
        //size_t bsize = generate_block(buf, sizeof(buf));
    //printf("sdfasdf: bsize %zd\n", 0); fflush(stdout);
        //size_t cnt = 0;
        // while (bsize > 0) {
        //     printf("%zd, \n", bsize);fflush(stdout);
        //     for (char* p = buf; p < buf+bsize; ++p) {
        //     printf("%zd, \n", *p);fflush(stdout);
        //         if (*p != '\n') cnt ++;
        //     }
        //     bsize = generate_block(buf, sizeof(buf));
        // }
        //total.fetch_add(cnt);
    };

    printf("sdfasdf:th %zd\n", threads); fflush(stdout);
    MultiThreadRun(threads, worker_count);
    printf("sdfasdf:th %zd\n", threads+1); fflush(stdout);
    return 0;//total.load();
}




int main() {
    std::ifstream ifs("../a.txt");
    
    size_t line_numer = count_lines_in_file(ifs, 1);
}