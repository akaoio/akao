# 🧠 BÀN BẠC: HỆ THỐNG LOGIC HÌNH THỨC CHO AKAO

## 🎯 **NGUYÊN TẮC CỐT LÕI: ZERO HARDCODED LOGIC**

**Engine = Pure Runtime Executor**
- Engine chỉ biết cách parse và execute logic
- Mọi philosophy/rule logic đều trong YAML
- Engine không biết gì về "one class per file" hay bất kỳ rule cụ thể nào
- Engine chỉ biết cách interpret syntax logic

## 🏗️ **KIẾN TRÚC MỚI**

### **1. Pure Logic Engine**
```cpp
class PureLogicEngine {
    // Engine chỉ biết execute, không biết logic gì cả
    ExecutionResult execute(const LogicExpression& expr, const Context& ctx);
    bool evaluate_condition(const std::string& condition, const Context& ctx);
    
    // Built-in operators (chỉ syntax, không biết semantic)
    bool op_equals(const Value& a, const Value& b);
    bool op_less_than(const Value& a, const Value& b);
    bool op_contains(const Collection& collection, const Value& item);
    int op_count(const Collection& collection);
    
    // NO HARDCODED RULES - chỉ có operators
};
```

### **2. Logic Syntax trong YAML**
```yaml
# rules/structure/one_class_per_file.yaml
rule:
  id: "akao:rule:structure:one_class_per_file:v1"
  name: "One Class Per File"
  
  # Logic hoàn toàn trong YAML
  logic:
    forall:
      variable: "file"
      domain: "filesystem.get_cpp_files()"
      condition:
        operator: "less_equal"
        left:
          function: "count"
          argument:
            function: "extract_classes"
            argument: { var: "file" }
        right: 1
  
  violation:
    type: "multiple_classes_in_file"
    message: "File {file} contains {count} classes, expected 1"
    severity: "error"

# Self-validation logic cũng trong YAML
self_validation:
  - check:
      description: "This rule file follows single responsibility"
      logic:
        operator: "equals"
        left:
          function: "count"
          argument:
            function: "extract_rules"
            argument: "this_file"
        right: 1
```

### **3. Philosophy Logic trong YAML**
```yaml
# philosophies/structure/enforcement.yaml
philosophy:
  id: "akao:philosophy:structure:enforcement:v1"
  principle: "Structure shapes behavior"
  
  # Formal definition trong YAML
  formal_logic:
    axioms:
      - name: "structure_consistency"
        statement:
          forall:
            variable: "structure"
            domain: "all_structures"
            implies:
              condition:
                function: "is_consistent"
                argument: { var: "structure" }
              result:
                function: "is_predictable"
                argument:
                  function: "behavior_of"
                  argument: { var: "structure" }
    
    theorems:
      - name: "consistency_implies_quality"
        proof_steps:
          - from: "structure_consistency"
          - apply: "transitivity"
          - conclude: "consistent_structure_leads_to_quality"
  
  # Self-proof trong YAML
  self_proof:
    claim: "This philosophy is internally consistent"
    proof:
      - step: "Check all axioms are satisfiable"
        logic:
          forall:
            variable: "axiom"
            domain: "this.axioms"
            condition:
              function: "is_satisfiable"
              argument: { var: "axiom" }
      
      - step: "Check no contradictions exist"
        logic:
          not_exists:
            variables: ["axiom1", "axiom2"]
            domain: "this.axioms"
            condition:
              function: "contradicts"
              arguments: [{ var: "axiom1" }, { var: "axiom2" }]
```

### **4. Built-in Functions (Engine cung cấp)**
```yaml
# core/engine/builtin_functions.yaml
# Đây là DUY NHẤT file engine biết - chỉ là function registry
builtin_functions:
  filesystem:
    - name: "get_cpp_files"
      returns: "collection<file>"
      description: "Returns all .cpp files in current directory"
    
    - name: "extract_classes"
      takes: "file"
      returns: "collection<class>"
      description: "Extracts class definitions from file"
  
  collection:
    - name: "count"
      takes: "collection<T>"
      returns: "integer"
      description: "Returns number of items in collection"
    
    - name: "contains"
      takes: ["collection<T>", "T"]
      returns: "boolean"
      description: "Checks if collection contains item"
  
  logic:
    - name: "is_consistent"
      takes: "structure"
      returns: "boolean"
      description: "Checks if structure is internally consistent"

# Engine chỉ biết execute các functions này, không biết logic cụ thể
```

### **5. Engine Implementation**
```cpp
// core/engine/logic/pure_logic_engine.hpp
class PureLogicEngine {
private:
    std::map<std::string, BuiltinFunction> builtin_functions_;
    
public:
    // Load builtin functions từ YAML
    void loadBuiltinFunctions(const std::string& builtin_yaml);
    
    // Execute logic expression từ YAML
    Value executeLogic(const YAML::Node& logic_node, Context& context);
    
    // Không có hardcoded rules
    std::vector<Violation> validateWithRules(
        const std::vector<std::string>& rule_files,
        const CodeBase& codebase
    );
    
    // Self-validation cũng từ YAML
    bool selfValidate(const std::string& philosophy_or_rule_file);
    
private:
    // Pure operators - không biết semantic
    Value executeForall(const YAML::Node& forall_node, Context& ctx);
    Value executeExists(const YAML::Node& exists_node, Context& ctx);
    Value executeOperator(const std::string& op, const std::vector<Value>& args);
    Value executeFunction(const std::string& func, const std::vector<Value>& args);
};
```

## 🔄 **WORKFLOW**

1. **Engine khởi động**: Load builtin functions từ YAML
2. **Load rules**: Parse logic từ rule YAML files  
3. **Execute**: Chạy logic expressions với context
4. **Self-validate**: Apply self-validation logic từ YAML
5. **Report**: Format kết quả

**Engine không bao giờ biết:**
- "One class per file" là gì
- Philosophy cụ thể nào
- Rule business logic nào

**Engine chỉ biết:**
- Cách parse YAML logic syntax
- Cách execute operators (`forall`, `exists`, `equals`, etc.)
- Cách call builtin functions (`count`, `extract_classes`, etc.)

## 🧪 **VÍ DỤ SELF-VALIDATION**

```yaml
# philosophies/meta/self_validation.yaml
philosophy:
  id: "akao:philosophy:meta:self_validation:v1"
  
  # Meta-logic để validate mọi philosophy/rule
  meta_rules:
    - name: "every_philosophy_has_self_proof"
      logic:
        forall:
          variable: "phil_file"
          domain: "filesystem.get_philosophy_files()"
          condition:
            function: "has_field"
            arguments: [{ var: "phil_file" }, "self_proof"]
    
    - name: "every_rule_has_self_check"
      logic:
        forall:
          variable: "rule_file" 
          domain: "filesystem.get_rule_files()"
          condition:
            function: "has_field"
            arguments: [{ var: "rule_file" }, "self_validation"]

# Engine sẽ chạy meta-rules này để validate toàn hệ thống
```

## 🎯 **KẾT LUẬN**

**Approach này:**
✅ **Pure separation**: Logic ở YAML, engine ở C++
✅ **Self-contained**: Mọi logic đều có thể self-validate
✅ **Extensible**: Thêm rule mới không cần code engine
✅ **Formal**: Logic syntax rõ ràng, có thể verify được

**Bạn thấy approach này như thế nào?**

---

## 🚀 **NEXT STEPS: IMPLEMENTATION PLAN**

### **Phase 1: Core Engine Foundation**
1. **Pure Logic Engine skeleton** (`core/engine/logic/pure_logic_engine.cpp`)
2. **YAML Logic Parser** để parse các construct như `forall`, `exists`, `operator`
3. **Built-in Function Registry** load từ `builtin_functions.yaml`
4. **Basic operators**: `equals`, `less_than`, `count`, `contains`

### **Phase 2: Domain Functions** 
1. **Filesystem functions**: `get_cpp_files()`, `extract_classes()`, etc.
2. **Code analysis functions**: `extract_functions()`, `get_includes()`, etc.
3. **Meta functions**: `has_field()`, `is_satisfiable()`, etc.

### **Phase 3: Self-Validation System**
1. **Philosophy self-proof execution** 
2. **Rule self-validation execution**
3. **Meta-validation** cho toàn hệ thống
4. **Unit test execution** cũng bằng logic engine

### **Phase 4: Integration**
1. **CLI integration** route tất cả qua logic engine
2. **Reporting system** format violations và validation results
3. **Performance optimization** với caching và lazy evaluation

---

## 🔬 **TECHNICAL DETAILS**

### **YAML Logic Language Specification**

#### **Basic Constructs**
```yaml
# Logical operators
logic:
  operator: "and|or|not|implies|equals|less_than|greater_than"
  left: <expression>
  right: <expression>

# Quantifiers  
logic:
  forall|exists:
    variable: "var_name"
    domain: <expression>  # Evaluates to collection
    condition: <expression>  # Boolean expression

# Function calls
logic:
  function: "function_name"
  argument: <expression>
  # hoặc
  arguments: [<expr1>, <expr2>, ...]

# Variables and literals
logic:
  var: "variable_name"
  # hoặc
  literal: "string|number|boolean"
```

#### **Advanced Constructs**
```yaml
# Conditional logic
logic:
  if:
    condition: <expression>
    then: <expression>
    else: <expression>

# Let bindings (local variables)
logic:
  let:
    bindings:
      var1: <expression>
      var2: <expression>
    in: <expression>

# Recursion với fixed-point
logic:
  fixpoint:
    variable: "func_var"
    parameter: "input_var"  
    body: <expression>  # Can reference func_var for recursion
    argument: <expression>
```

### **Unit Testing in YAML**
```yaml
# rules/structure/one_class_per_file.yaml
unit_tests:
  - name: "single_class_file_passes"
    setup:
      mock_filesystem:
        - file: "Person.cpp"
          content: "class Person { };"
    expected: "no_violations"
    
  - name: "multiple_classes_fail"
    setup:
      mock_filesystem:
        - file: "Multiple.cpp"
          content: "class A { }; class B { };"
    expected: 
      violations:
        - type: "multiple_classes_in_file"
          file: "Multiple.cpp"
          count: 2

# Engine sẽ execute tests này bằng chính logic engine
test_execution:
  logic:
    forall:
      variable: "test"
      domain: "this.unit_tests"
      condition:
        function: "test_passes"
        argument: { var: "test" }
```

### **Error Handling and Debugging**
```yaml
# Mọi logic expression có thể có debugging info
logic:
  operator: "equals"
  left: { function: "count", argument: { var: "classes" } }
  right: 1
  debug:
    description: "Check if file has exactly one class"
    trace_variables: ["classes"]
    log_level: "info"

# Engine sẽ generate detailed traces khi logic fails
```

---

## 🎭 **PHILOSOPHY: PURE INTERPRETATION**

**Core Insight**: Engine là một **pure interpreter** cho logic language

**Analogy**: 
- **JavaScript Engine** không biết gì về DOM, React, hay business logic
- **SQL Engine** không biết gì về schema hay business rules  
- **AKAO Logic Engine** không biết gì về filesystem rules hay code quality

**Benefits**:
1. **Separation of Concerns**: Logic ở YAML, execution ở C++
2. **Testability**: Logic có thể test riêng biệt khỏi engine
3. **Extensibility**: Thêm rules không cần rebuild engine
4. **Verifiability**: Logic có thể formal verify bằng mathematical tools
5. **Portability**: Logic có thể chạy trên engines khác nhau

**Trade-offs**:
- **Performance**: YAML parsing + interpretation có thể chậm hơn compiled code
- **Complexity**: Cần design language syntax carefully
- **Debugging**: Stack traces sẽ ở logic level, không phải C++ level

**Mitigation**:
- **Caching**: Compile YAML logic to internal representation
- **JIT compilation**: Generate native code cho hot paths
- **Rich debugging**: Provide detailed traces và error messages

---

## 🔄 **COMPLETE EXAMPLE: END-TO-END**

**File**: `rules/structure/one_class_per_file.yaml`
```yaml
rule:
  id: "akao:rule:structure:one_class_per_file:v1"
  name: "One Class Per File"
  
  logic:
    forall:
      variable: "file"
      domain: 
        function: "filesystem.get_files"
        argument:
          function: "filesystem.current_directory"
      condition:
        if:
          condition:
            function: "filesystem.has_extension"
            arguments: [{ var: "file" }, ".cpp"]
          then:
            operator: "less_equal"
            left:
              function: "cpp.count_classes"
              argument: { var: "file" }
            right: 1
          else: true  # Non-C++ files are ignored
  
  self_validation:
    tests:
      - name: "logic_is_well_formed"
        check:
          function: "logic.is_well_formed"
          argument: "this.logic"
      
      - name: "uses_only_available_functions"
        check:
          function: "logic.all_functions_exist"
          arguments: ["this.logic", "builtin_functions"]
    
    meta_check:
      logic:
        forall:
          variable: "test"
          domain: "this.self_validation.tests"
          condition:
            function: "test.passes"
            argument: { var: "test" }

unit_tests:
  - name: "single_class_passes"
    mock_context:
      filesystem:
        files:
          - name: "Person.cpp"
            content: |
              class Person {
                public:
                  void getName();
              };
    expected_result: "no_violations"
    
  - name: "multiple_classes_fail"  
    mock_context:
      filesystem:
        files:
          - name: "Multiple.cpp"
            content: |
              class Person {};
              class Student {};
    expected_result:
      violations:
        - type: "multiple_classes_in_file"
          file: "Multiple.cpp"
          details: { class_count: 2 }
```

**Execution Flow**:
1. **CLI** calls `engine.validateRules(["rules/structure/one_class_per_file.yaml"])`
2. **Engine** loads rule, parses logic section
3. **Engine** calls `filesystem.get_files(filesystem.current_directory())`
4. **Engine** loops through files với `forall` logic
5. **Engine** calls `cpp.count_classes(file)` cho mỗi .cpp file
6. **Engine** evaluates `count <= 1` condition
7. **Engine** generates violations nếu condition fails
8. **Engine** runs `self_validation` tests
9. **Engine** runs `unit_tests` với mock context
10. **CLI** displays formatted results

**Key Points**:
- Engine chỉ biết cách execute `forall`, `if`, `operator`, `function`
- Engine không biết gì về "class", "file", hay "C++"
- Tất cả domain knowledge ở built-in functions và YAML logic
- Self-validation và unit tests cũng chạy qua engine

---

**Ready để implement?** 🚀