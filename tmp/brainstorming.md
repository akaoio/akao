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