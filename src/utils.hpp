#pragma once

#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>

size_t roughly_count_lines_in_file(std::ifstream& ifs);
size_t count_lines_in_file(std::ifstream& ifs, size_t threads=1);


template<typename C>
std::vector<std::string> split_string(const std::string &str, C check) {
    auto is_not_split_point = [check](char c) { return !check(c); };
    auto is_split_point = [check](char c) { return check(c); };

    std::vector<std::string> substrs;
    auto begin = std::find_if(str.begin(), str.end(), is_not_split_point);

    while (begin != str.end()) {
        auto end = std::find_if(begin, str.end(), is_split_point);
        substrs.push_back(std::string(begin, end));
        begin = std::find_if(end, str.end(), is_not_split_point);
    }

    return substrs;
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

    size_t get(char* block, size_t bsize) {
        size_t bindex = 0;
        if (buf_size_ > 0) {
            assert(buf_size_ < bsize);
            std::copy(buf_, buf_+buf_size_, block);
            bindex = buf_size_;
            buf_size_ = 0;
        }

        ifs_.read(block+bindex, bsize-bindex);
        bindex += ifs_.gcount();

        if (bindex == bsize) {
            size_t end = 0;
            for (size_t i = 0; i < bindex; ++i) {
                if (block[bindex-i-1] == delim_) {
                    end = bindex - i; 
                    break;
                }
            }
            //printf("%zd >= %zd && %zd > %zd - %zd\n", bindex, end, max_buf_size_, bindex, end); fflush(stdout);
            assert(bindex >= end && max_buf_size_ > bindex - end);
            std::copy(block+end, block+bindex, buf_);
            buf_size_ = bindex - end;
            bindex = end; 

        } 
        return bindex;
    }

protected:
    const static size_t max_buf_size_ = 1000;
    char buf_[max_buf_size_];
    size_t buf_size_ { 0 };
    std::ifstream &ifs_; 
    char delim_;
};

