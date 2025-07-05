# Akao Universal Validation Framework Makefile
# Implements philosophy: akao:philosophy:build:duality:v1 - Multiple build systems

# Variables
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
DEBUGFLAGS := -g -O0 -DDEBUG
RELEASEFLAGS := -O3 -DNDEBUG
SRCDIR := .
BUILDDIR := build
BINDIR := $(BUILDDIR)/bin
TARGET := $(BINDIR)/akao

# Build mode (debug or release)
MODE ?= release
ifeq ($(MODE),debug)
    CXXFLAGS += $(DEBUGFLAGS)
    BUILDSUBDIR := $(BUILDDIR)/debug
else
    CXXFLAGS += $(RELEASEFLAGS)
    BUILDSUBDIR := $(BUILDDIR)/release
endif

# Source files
CORE_SOURCES := $(shell find core -name "*.cpp" 2>/dev/null)
INTERFACE_SOURCES := $(shell find interfaces -name "*.cpp" 2>/dev/null)
TEST_SOURCES := $(shell find tests -name "*.cpp" 2>/dev/null)
MAIN_SOURCE := main.cpp

ALL_SOURCES := $(MAIN_SOURCE) $(CORE_SOURCES) $(INTERFACE_SOURCES)
TEST_ALL_SOURCES := $(CORE_SOURCES) $(INTERFACE_SOURCES) $(TEST_SOURCES)
OBJECTS := $(ALL_SOURCES:%.cpp=$(BUILDSUBDIR)/%.o)
TEST_OBJECTS := $(TEST_ALL_SOURCES:%.cpp=$(BUILDSUBDIR)/%.o)

# Test target
TEST_TARGET := $(BINDIR)/akao_tests

# Default target
.PHONY: all
all: $(TARGET)

# Create directories
$(BUILDSUBDIR) $(BINDIR):
	@mkdir -p $@

# Object files
$(BUILDSUBDIR)/%.o: %.cpp | $(BUILDSUBDIR)
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Main target
$(TARGET): $(OBJECTS) | $(BINDIR)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(OBJECTS) -o $@
	@echo "Build complete: $(TARGET)"

# Test target
$(TEST_TARGET): $(TEST_OBJECTS) | $(BINDIR)
	@echo "Linking $(TEST_TARGET)..."
	@$(CXX) $(TEST_OBJECTS) -o $@
	@echo "Test build complete: $(TEST_TARGET)"

# Build modes
.PHONY: debug release
debug:
	@$(MAKE) MODE=debug

release:
	@$(MAKE) MODE=release

# Installation
.PHONY: install
install: $(TARGET)
	@echo "Installing Akao..."
	@install -d $(DESTDIR)/usr/local/bin
	@install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin/
	@install -d $(DESTDIR)/usr/local/share/akao
	@if [ -d philosophies ]; then \
		cp -r philosophies $(DESTDIR)/usr/local/share/akao/; \
	fi
	@if [ -d rules ]; then \
		cp -r rules $(DESTDIR)/usr/local/share/akao/; \
	fi
	@echo "Installation complete"

# Uninstall
.PHONY: uninstall
uninstall:
	@echo "Uninstalling Akao..."
	@rm -f $(DESTDIR)/usr/local/bin/akao
	@rm -rf $(DESTDIR)/usr/local/share/akao
	@echo "Uninstall complete"

# Testing and validation
.PHONY: test
test: $(TARGET)
	@echo "Running basic functionality test..."
	@$(TARGET) --version
	@echo "Test complete"

.PHONY: test-build
test-build: $(TEST_TARGET)
	@echo "Test suite built successfully"

.PHONY: test-self-validation
test-self-validation: $(TEST_TARGET)
	@echo "Running Akao self-validation tests..."
	@$(TEST_TARGET) self-validation

.PHONY: test-philosophy-compliance
test-philosophy-compliance: $(TEST_TARGET)
	@echo "Running Akao philosophy compliance tests..."
	@$(TEST_TARGET) philosophy-compliance

.PHONY: test-all-compliance
test-all-compliance: $(TEST_TARGET)
	@echo "Running all Akao compliance tests..."
	@$(TEST_TARGET) all

.PHONY: self-validate
self-validate: $(TARGET)
	@echo "Running Akao self-validation..."
	@$(TARGET) self-validate

.PHONY: validate-project
validate-project: $(TARGET)
	@echo "Validating current project..."
	@$(TARGET) validate .

# Development helpers
.PHONY: check
check: $(TARGET)
	@echo "Running comprehensive checks..."
	@$(TARGET) check .
	@$(TARGET) validate .

.PHONY: fix
fix: $(TARGET)
	@echo "Attempting to fix violations..."
	@$(TARGET) fix .

.PHONY: report
report: $(TARGET)
	@echo "Generating compliance report..."
	@$(TARGET) report --output compliance-report.yaml .

# Cleanup
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILDDIR)

.PHONY: clean-all
clean-all: clean
	@echo "Cleaning all generated files..."
	@rm -f compliance-report.yaml
	@rm -f *.log

# Documentation (if available)
.PHONY: docs
docs:
	@if command -v doxygen >/dev/null 2>&1; then \
		echo "Generating documentation..."; \
		doxygen docs/Doxyfile 2>/dev/null || echo "No Doxyfile found"; \
	else \
		echo "Doxygen not found, skipping documentation generation"; \
	fi

# Packaging
.PHONY: package
package: clean $(TARGET)
	@echo "Creating source package..."
	@tar --exclude='.git' --exclude='build' --exclude='*.tar.gz' \
		-czf akao-$(shell date +%Y%m%d).tar.gz .
	@echo "Package created: akao-$(shell date +%Y%m%d).tar.gz"

# Development workflow targets
.PHONY: dev-setup
dev-setup:
	@echo "Setting up development environment..."
	@mkdir -p .akao/reports
	@echo "Development environment ready"

.PHONY: dev-validate
dev-validate: debug test-all-compliance self-validate validate-project

.PHONY: dev-clean
dev-clean: clean-all
	@rm -rf .akao

.PHONY: build-all
build-all: $(TARGET) $(TEST_TARGET)
	@echo "All targets built successfully"

# Information targets
.PHONY: info
info:
	@echo "Akao Build Information:"
	@echo "  Version: 1.0.0"
	@echo "  Build mode: $(MODE)"
	@echo "  Compiler: $(CXX)"
	@echo "  Flags: $(CXXFLAGS)"
	@echo "  Target: $(TARGET)"
	@echo "  Sources found:"
	@echo "    Core: $(words $(CORE_SOURCES)) files"
	@echo "    Interfaces: $(words $(INTERFACE_SOURCES)) files"
	@echo "    Tests: $(words $(TEST_SOURCES)) files"
	@echo "    Total: $(words $(ALL_SOURCES)) files"

.PHONY: help
help:
	@echo "Akao Universal Validation Framework - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all                     - Build Akao (default)"
	@echo "  debug                   - Build in debug mode"
	@echo "  release                 - Build in release mode (default)"
	@echo "  build-all               - Build main executable and tests"
	@echo "  install                 - Install Akao system-wide"
	@echo "  uninstall               - Remove Akao from system"
	@echo "  test                    - Run basic tests"
	@echo "  test-build              - Build test suite"
	@echo "  test-self-validation    - Run self-validation tests"
	@echo "  test-philosophy-compliance - Run philosophy compliance tests"
	@echo "  test-all-compliance     - Run all compliance tests"
	@echo "  self-validate           - Run Akao self-validation via CLI"
	@echo "  validate-project        - Validate current project via CLI"
	@echo "  check                   - Run comprehensive checks"
	@echo "  fix                     - Auto-fix violations"
	@echo "  report                  - Generate compliance report"
	@echo "  clean                   - Clean build artifacts"
	@echo "  clean-all               - Clean all generated files"
	@echo "  docs                    - Generate documentation"
	@echo "  package                 - Create source package"
	@echo "  dev-setup               - Setup development environment"
	@echo "  dev-validate            - Full development validation"
	@echo "  dev-clean               - Clean development artifacts"
	@echo "  info                    - Show build information"
	@echo "  help                    - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  make                         # Build in release mode"
	@echo "  make debug                   # Build in debug mode"
	@echo "  make build-all               # Build main and test executables"
	@echo "  make test-all-compliance     # Run comprehensive compliance tests"
	@echo "  make self-validate           # Validate Akao via CLI"
	@echo "  make install                 # Install system-wide"
	@echo ""
	@echo "Philosophy compliance: akao:philosophy:build:duality:v1"

# Ensure build directories exist for object files
$(OBJECTS): | $(BUILDSUBDIR)

# Dependency generation (optional, for incremental builds)
-include $(OBJECTS:.o=.d)

$(BUILDSUBDIR)/%.d: %.cpp | $(BUILDSUBDIR)
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< > $@
