#pragma once

namespace akao::builders::cmake {

/**
 * @brief CMake build system adapter
 */
class CMakeBuilder {
public:
    /**
     * @brief Initialize CMake builder
     */
    bool initialize();

    /**
     * @brief Build project using CMake
     */
    bool build(const std::string& source_dir, const std::string& build_dir, 
               const std::string& mode = "Release");

    /**
     * @brief Configure CMake project
     */
    bool configure(const std::string& source_dir, const std::string& build_dir);

    /**
     * @brief Clean CMake build
     */
    bool clean(const std::string& build_dir);
};

} // namespace akao::builders::cmake
