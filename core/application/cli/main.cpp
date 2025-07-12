/**
 * @id: akao:executable:core:application:cli:main:v1
 * @doc: Main entry point for Akao CLI application using core infrastructure libraries. Routes all commands through core orchestrator via existing YAML-RPC implementation while leveraging core YAML parsing. Eliminates code duplication by reusing core foundation and engine components.
 * @specification: CLI main executable using core libraries
 * @scope: Core application CLI entry point
 * @timeline: 2025-07-12
 * @rationale: Provide CLI entry point leveraging existing core infrastructure
 * @methodology: Command parsing with core library integration
 * @references: ["akao:class:core:application:cli:v1"]
 */

#include "v1.hpp"
#include <iostream>
#include <exception>
#include <cstdlib>

int main(int argc, char* argv[]) {
    try {
        akao::core::application::cli::CLI cli;
        return cli.run(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "akao: error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "akao: unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }
}