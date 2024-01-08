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

