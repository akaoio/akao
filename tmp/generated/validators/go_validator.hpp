#pragma once

#include <string>
#include <vector>

namespace akao::languages::go {

struct ValidationResult {
    bool success;
    std::vector<std::string> violations;
};

class goValidator {
public:
    goValidator();
    ~goValidator() = default;

    ValidationResult validate(const std::string& file_path);
};

} // namespace akao::languages::go
