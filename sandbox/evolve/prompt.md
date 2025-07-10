# 🤖 AI AGENT EXECUTION PROMPT

## 🎯 MISSION STATEMENT

You are tasked with executing the Akao Node-Based Evolution Plan. You must transform Akao from a hardcoded workflow system to a fully configurable node-based architecture while maintaining 100% backward compatibility and producing executable, testable artifacts at every step.

---

## 📋 MANDATORY REQUIREMENTS

### **🔒 STRICT COMPLIANCE RULES:**

1. **READ FIRST**: You MUST read and understand:
   - `/data/data/com.termux/files/home/akao/CONTRIBUTING.md` - Project standards and conventions
   - `/data/data/com.termux/files/home/akao/sandbox/evolve/plan.md` - Your execution roadmap

2. **STEP-BY-STEP EXECUTION**: 
   - Execute ONLY ONE step at a time in exact order
   - NEVER skip steps or work on multiple steps simultaneously
   - NEVER proceed to next step until current step is 100% complete

3. **ARTIFACT GENERATION**:
   - Create ALL artifacts specified in plan.md for each step
   - Place artifacts in exact directory structure: `artifacts/evolve/phase{N}/step{N}-{N}-{N}/`
   - Every artifact must be functional, compilable code - NO placeholder/fake code

4. **EXECUTABLE TESTING**:
   - Create working test executable for every step
   - Tests must compile and run successfully with `make` and `./test_*`
   - Tests must produce REAL computational results, not fake data
   - Tests must match expected outcomes specified in plan.md

5. **VERIFICATION PROTOCOL**:
   - After completing each step, run the test to verify success
   - Document actual test results vs expected results
   - Only declare step complete when tests pass with expected outcomes

---

## 🛠️ EXECUTION WORKFLOW

### **Before Starting Any Work:**

```bash
# MANDATORY: Read project guidelines
cat /data/data/com.termux/files/home/akao/CONTRIBUTING.md
cat /data/data/com.termux/files/home/akao/sandbox/evolve/plan.md

# MANDATORY: Understand current codebase structure
find /data/data/com.termux/files/home/akao -name "*.hpp" -o -name "*.cpp" | head -20
```

### **For Each Step:**

#### **Step 1: Analysis Phase**
- Analyze what needs to be implemented for this specific step
- Identify dependencies on existing code or previous steps
- Review expected artifacts and test requirements from plan.md

#### **Step 2: Implementation Phase**
- Create the exact directory structure specified in plan.md
- Implement ALL required artifacts (headers, source files, tests)
- Follow CONTRIBUTING.md standards for code style and structure
- Ensure code compiles without errors

#### **Step 3: Testing Phase**
- Create executable test that demonstrates the functionality
- Test must process REAL data and produce REAL results
- Verify test output matches expected results from plan.md
- Fix any issues until test passes completely

#### **Step 4: Verification Phase**
- Run the test executable and document results
- Compare actual results with plan.md expected outcomes
- Ensure backward compatibility is maintained
- Only proceed to next step when current step is verified successful

### **Example Step Execution:**

```bash
# For Phase 1, Step 1.1.1: NodeValue Implementation

# 1. Create directory structure
mkdir -p artifacts/evolve/phase1/step1-1-1

# 2. Implement required artifacts
# - nodevalue.hpp (complete header with all methods)
# - nodevalue.cpp (complete implementation)  
# - test_nodevalue.cpp (executable test)
# - Makefile (builds and runs tests)

# 3. Verify compilation
cd artifacts/evolve/phase1/step1-1-1
make test_nodevalue

# 4. Execute test and verify results
./test_nodevalue
# Expected output: All 20+ type conversion tests PASS

# 5. Document completion before proceeding
echo "Step 1.1.1 COMPLETE: NodeValue system implemented and tested"
```

---

## 🚫 FORBIDDEN ACTIONS

### **YOU MUST NEVER:**

1. **Skip Implementation**: Never create placeholder, stub, or fake implementations
2. **Generate Mock Data**: All tests must process real, meaningful data
3. **Skip Tests**: Every step MUST have executable tests that pass
4. **Work Ahead**: Never implement future steps before current step is complete
5. **Break Compatibility**: Never break existing functionality
6. **Ignore Standards**: Always follow CONTRIBUTING.md guidelines
7. **Assume Success**: Always verify test results match expected outcomes

### **EXAMPLE OF FORBIDDEN CODE:**
```cpp
// ❌ FORBIDDEN: Placeholder implementation
class NodeValue {
    // TODO: implement later
    std::string placeholder;
};

// ❌ FORBIDDEN: Mock/fake test results
TEST(NodeValueTest, TypeConversion) {
    // This would always pass - FORBIDDEN
    EXPECT_TRUE(true);
}

// ❌ FORBIDDEN: Non-executable test
// A test that doesn't actually test the functionality
```

### **EXAMPLE OF REQUIRED CODE:**
```cpp
// ✅ REQUIRED: Complete, functional implementation
class NodeValue {
private:
    ValueType type_;
    std::variant<std::string, int64_t, double, bool, Object, Array> value_;
    
public:
    template<typename T> T as() const {
        // Complete implementation that actually works
        if constexpr (std::is_same_v<T, std::string>) {
            if (type_ != ValueType::STRING) 
                throw std::runtime_error("Type mismatch");
            return std::get<std::string>(value_);
        }
        // ... complete implementation for all types
    }
    
    bool isString() const { return type_ == ValueType::STRING; }
    // ... all other methods fully implemented
};

// ✅ REQUIRED: Real, functional test
TEST(NodeValueTest, StringConversion) {
    NodeValue val("hello world");
    EXPECT_TRUE(val.isString());
    EXPECT_EQ(val.as<std::string>(), "hello world");
    
    // Test actual type conversion logic
    NodeValue num(42);
    EXPECT_TRUE(num.isNumber());
    EXPECT_EQ(num.as<int64_t>(), 42);
}
```

---

## 📊 PROGRESS TRACKING

### **Report Format for Each Step:**

```markdown
## Step {Phase}.{Step}.{Substep} - {Description}

**Status**: COMPLETE ✅ / IN_PROGRESS 🔄 / FAILED ❌

**Artifacts Created:**
- `artifacts/evolve/phase{N}/step{N}-{N}-{N}/{filename}`
- `artifacts/evolve/phase{N}/step{N}-{N}-{N}/{filename}`

**Test Results:**
```bash
cd artifacts/evolve/phase{N}/step{N}-{N}-{N}
make test_{name}
./test_{name}
# Actual output:
# Expected: [expected results from plan.md]
# Actual: [actual results from test execution]
# Status: PASS ✅ / FAIL ❌
```

**Dependencies Satisfied:**
- [List any dependencies on previous steps or existing code]

**Next Step**: Step {Next.Phase}.{Next.Step}.{Next.Substep}
```

---

## 🎯 SUCCESS VALIDATION

### **Step Completion Criteria:**

Each step is only complete when ALL of the following are satisfied:

1. ✅ **All Required Artifacts Created**: Every file specified in plan.md exists
2. ✅ **Code Compiles Successfully**: No compilation errors with current build system  
3. ✅ **Tests Execute Successfully**: Test executable runs without crashes
4. ✅ **Expected Results Achieved**: Test output matches plan.md expectations
5. ✅ **Real Data Processed**: No mock/fake data in test results
6. ✅ **Documentation Updated**: Brief summary of what was implemented
7. ✅ **Standards Followed**: Code follows CONTRIBUTING.md guidelines

### **Phase Completion Criteria:**

Each phase is only complete when:

1. ✅ **All Steps in Phase Complete**: Every step meets completion criteria
2. ✅ **Integration Tests Pass**: Phase-level integration test succeeds
3. ✅ **Performance Maintained**: No regression vs existing system
4. ✅ **Backward Compatibility**: All existing .akao functionality works
5. ✅ **Documentation Complete**: Phase summary with key achievements

---

## 🚀 EXECUTION COMMAND

**BEGIN EXECUTION:**

Start with Phase 1, Step 1.1.1 as specified in plan.md. Read all requirements carefully, implement completely, test thoroughly, and report results before proceeding to the next step.

**Your first action must be:**
```bash
cat /data/data/com.termux/files/home/akao/CONTRIBUTING.md
cat /data/data/com.termux/files/home/akao/sandbox/evolve/plan.md
```

Then proceed with systematic implementation of Phase 1, Step 1.1.1: NodeValue and NodeItem Implementation.

Remember: Quality over speed. Complete implementation over quick progress. Real results over fake demonstrations.

**EXECUTE STEP BY STEP. BUILD THE FUTURE OF AKAO.**