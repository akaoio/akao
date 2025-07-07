/**
 * @brief Architecture Compliance Test - Phase 1 Step 2
 * 
 * Verifies that CLI command structure fixes resolve one-file-per-directory violations
 */

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

bool checkOneFilePerDirectory(const std::string& directory) {
    std::vector<std::string> files;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }
    
    return files.size() <= 1;
}

int main() {
    std::cout << "🏗️ Architecture Compliance Test - Phase 1 Step 2" << std::endl;
    
    // Input: CLI command directories after restructuring
    std::vector<std::string> command_dirs = {
        "interfaces/cli/command/additional/header",
        "interfaces/cli/command/additional/implementation", 
        "interfaces/cli/command/build/header",
        "interfaces/cli/command/build/implementation",
        "interfaces/cli/command/generate/header", 
        "interfaces/cli/command/generate/implementation",
        "interfaces/cli/command/init/header",
        "interfaces/cli/command/init/implementation",
        "interfaces/cli/command/test/header",
        "interfaces/cli/command/test/implementation",
        "interfaces/cli/command/validate/header",
        "interfaces/cli/command/validate/implementation"
    };
    
    // Expected: Each directory should contain only one file
    bool all_compliant = true;
    int compliant_dirs = 0;
    int total_dirs = command_dirs.size();
    
    std::cout << "📁 Checking directory compliance:" << std::endl;
    
    for (const auto& dir : command_dirs) {
        bool compliant = checkOneFilePerDirectory(dir);
        if (compliant) {
            compliant_dirs++;
            std::cout << "  ✅ " << dir << " - COMPLIANT" << std::endl;
        } else {
            all_compliant = false;
            std::cout << "  ❌ " << dir << " - VIOLATION" << std::endl;
        }
    }
    
    // Test architectural compliance score calculation
    double compliance_percentage = (double)compliant_dirs / total_dirs * 100.0;
    
    std::cout << "📊 Compliance Results:" << std::endl;
    std::cout << "  Compliant directories: " << compliant_dirs << "/" << total_dirs << std::endl;
    std::cout << "  Compliance percentage: " << compliance_percentage << "%" << std::endl;
    
    // Assertion: All directories should be compliant now (fraud prevention)
    if (all_compliant && compliance_percentage == 100.0) {
        std::cout << "✅ Architecture compliance test PASSED" << std::endl;
        std::cout << "🎯 One-file-per-directory rule violations resolved" << std::endl;
        return 0;
    } else {
        std::cout << "❌ Architecture compliance test FAILED" << std::endl;
        return 1;
    }
}