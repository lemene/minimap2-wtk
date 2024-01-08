#include "utils.hpp"


size_t roughly_count_lines_in_file(std::ifstream& ifs) {
    ifs.seekg(0);

    size_t line_size = 0;
    for (size_t i = 0; i < 10; ++i) {   // check the first 10 line
        std::string line;
        if (std::getline(ifs, line)) {
            if (line_size == 0 || line_size > line.size()) {
                line_size = line.size();
            }
        }
    }

    if (line_size > 0) {
        ifs.seekg(0, std::ios_base::end);
        return ifs.tellg() / line_size;
    } else {
        return 0;
    }
}


template<typename W>
void MultiThreadRun(size_t thread_size, W work_func) {
    std::vector<std::thread> workers;
    for (size_t i = 0; i < thread_size; ++i) {
        workers.push_back(std::thread(work_func, i));
    }
    for (size_t i = 0; i < workers.size(); ++i) {
        workers[i].join();
    }
}

class BlockReader {
public:
    BlockReader(std::ifstream &ifs, char delim) : ifs_(ifs), delim_(delim) { ifs_.seekg(0); }

    size_t Get(char* block, size_t bsize) {
        size_t bindex = 0;
        if (bufsize_ > 0) {
            assert(bufsize_ < bsize);
            std::copy(buf_, buf_+bufsize_, block);
            bindex = bufsize_;
            bufsize_ = 0;
        }

        ifs_.read(block+bindex, bsize-bindex);
        bindex += ifs_.gcount();

        if (bindex == bsize) {
            size_t end = 0;
            for (size_t i = 0; i < bindex; ++i) {
                if (block[bindex-i-1] == delim_) {
                    end = bindex - i; 
                }
            }

            assert(bindex >= end && bufsize_ > bindex - end);
            std::copy(block+end, block+bindex, buf_);
            bufsize_ = bindex - end;
            bindex = end;

        }
        return bindex;
    }

protected:
    char buf_[1024];
    size_t bufsize_ { 0 };
    std::ifstream &ifs_; 
    char delim_;
};

size_t count_lines_in_file(std::ifstream& ifs, size_t threads) {
    ifs.seekg(0);
    std::string line;
    std::mutex mutex;
    std::atomic<size_t> count { 0 };

    auto generate_func = [&ifs, &mutex](char* buf, size_t bufsize) -> size_t {
        std::lock_guard<std::mutex> lock(mutex);

        ifs.read(buf, bufsize);
        return ifs.gcount();
    };
    auto worker_func = [&count, &generate_func](size_t id) {
        char buf[1024*1000];
        size_t bsize = generate_func(buf, sizeof(buf));
        size_t cnt = 0;
        while (bsize > 0) {
            for (char* p = buf; p < buf+bsize; ++p) {
                if (*p == '\n') cnt ++;
            }
            bsize = generate_func(buf, sizeof(buf));
        }
        count.fetch_add(cnt);
    };

    MultiThreadRun(threads, worker_func);
    return count.load();
}



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