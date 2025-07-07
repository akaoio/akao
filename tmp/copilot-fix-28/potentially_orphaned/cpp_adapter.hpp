#pragma once

namespace akao::languages::cpp {

/**
 * @brief C++ language adapter for universal validation
 */
class CppLanguageAdapter {
public:
    /**
     * @brief Initialize the C++ adapter
     */
    bool initialize();

    /**
     * @brief Validate C++ specific constructs
     */
    bool validateCppFile(const std::string& file_path);

    /**
     * @brief Get C++ specific validation rules
     */
    std::vector<std::string> getCppValidationRules();
};

} // namespace akao::languages::cpp
