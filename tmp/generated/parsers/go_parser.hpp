#pragma once

#include <string>
#include <vector>

namespace akao::languages::go {

class goParser {
public:
    goParser();
    ~goParser() = default;

    bool parseFile(const std::string& file_path);
    std::vector<std::string> getClasses() const;
    std::vector<std::string> getFunctions() const;
};

} // namespace akao::languages::go
