#pragma once

#include <fstream>
#include <string>

size_t roughly_count_lines_in_file(std::ifstream& ifs);


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
