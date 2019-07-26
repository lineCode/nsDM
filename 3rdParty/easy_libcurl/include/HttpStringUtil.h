#pragma once
#include <string>

bool split_header_line(const std::string& line, std::string& k, std::string& v);
std::string trim_item(const std::string& item);
