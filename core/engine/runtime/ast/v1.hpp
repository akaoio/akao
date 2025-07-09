#pragma once

/**
 * @id: akao:file:core:engine:runtime_ast_nodes:v1
 * @doc: Complete AST node definitions for Akao runtime engine supporting Pure Logic language execution and runtime management. Provides comprehensive node types including literals, symbols, function calls, assignments, conditionals, loops, and blocks with proper memory management and type safety. Features virtual destructors, move semantics, and disabled copy operations for performance optimization. Supports user-defined functions with parameter management and body execution for complete runtime AST representation in the Pure Logic execution environment.
 * @specification: Header file defining complete AST node type hierarchy for runtime execution engine
 * @scope: Core engine runtime AST node definitions with memory management and type safety
 * @timeline: 2025-07-09
 * @rationale: Provide complete AST node definitions to resolve compilation errors and enable Pure Logic runtime execution
 * @methodology: Standard C++ class hierarchy with virtual destructors, move semantics, and type safety patterns
 * @references: ["akao:file:core:engine:runtime_control:v1", "akao:file:core:engine:logic_pure:v1"]
 */

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace akao {
namespace core {
namespace engine {
namespace runtime {

/**
 * @brief AST node types enumeration
 */
enum class ASTNodeType {
    LITERAL,
    SYMBOL,
    FUNCTION_CALL,
    ASSIGNMENT,
    CONDITIONAL,
    LOOP,
    BLOCK
};

/**
 * @brief Base AST node class
 */
class ASTNode {
public:
    ASTNodeType type;
    
    explicit ASTNode(ASTNodeType node_type) : type(node_type) {}
    virtual ~ASTNode() = default;
    
    // Disable copy constructor and assignment operator
    ASTNode(const ASTNode&) = delete;
    ASTNode& operator=(const ASTNode&) = delete;
    
    // Enable move semantics
    ASTNode(ASTNode&&) = default;
    ASTNode& operator=(ASTNode&&) = default;
};

/**
 * @brief Literal value node (numbers, strings, booleans)
 */
class LiteralNode : public ASTNode {
public:
    enum class LiteralType { BOOLEAN, INTEGER, FLOAT, STRING };
    
    LiteralType literal_type;
    std::string value;
    
    LiteralNode(LiteralType type, const std::string& val) 
        : ASTNode(ASTNodeType::LITERAL), literal_type(type), value(val) {}
    
    virtual ~LiteralNode() = default;
};

/**
 * @brief Symbol/identifier node
 */
class SymbolNode : public ASTNode {
public:
    std::string name;
    
    explicit SymbolNode(const std::string& symbol_name) 
        : ASTNode(ASTNodeType::SYMBOL), name(symbol_name) {}
    
    virtual ~SymbolNode() = default;
};

/**
 * @brief Function call node
 */
class FunctionCallNode : public ASTNode {
public:
    std::string function_name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    explicit FunctionCallNode(const std::string& name) 
        : ASTNode(ASTNodeType::FUNCTION_CALL), function_name(name) {}
    
    virtual ~FunctionCallNode() = default;
};

/**
 * @brief Assignment node
 */
class AssignmentNode : public ASTNode {
public:
    std::string variable_name;
    std::unique_ptr<ASTNode> value;
    
    AssignmentNode(const std::string& var_name, std::unique_ptr<ASTNode> val) 
        : ASTNode(ASTNodeType::ASSIGNMENT), variable_name(var_name), value(std::move(val)) {}
    
    virtual ~AssignmentNode() = default;
};

/**
 * @brief Conditional node (if/else)
 */
class ConditionalNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> true_branch;
    std::unique_ptr<ASTNode> false_branch;
    
    ConditionalNode(std::unique_ptr<ASTNode> cond, 
                   std::unique_ptr<ASTNode> true_br, 
                   std::unique_ptr<ASTNode> false_br = nullptr) 
        : ASTNode(ASTNodeType::CONDITIONAL), 
          condition(std::move(cond)), 
          true_branch(std::move(true_br)), 
          false_branch(std::move(false_br)) {}
    
    virtual ~ConditionalNode() = default;
};

/**
 * @brief Loop node
 */
class LoopNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    LoopNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> loop_body) 
        : ASTNode(ASTNodeType::LOOP), condition(std::move(cond)), body(std::move(loop_body)) {}
    
    virtual ~LoopNode() = default;
};

/**
 * @brief Block node (sequence of statements)
 */
class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    BlockNode() : ASTNode(ASTNodeType::BLOCK) {}
    
    void add_statement(std::unique_ptr<ASTNode> statement) {
        statements.push_back(std::move(statement));
    }
    
    virtual ~BlockNode() = default;
};

/**
 * @brief User-defined function
 */
class UserFunction {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<ASTNode> body;
    
    UserFunction(const std::string& func_name, 
                const std::vector<std::string>& params, 
                std::unique_ptr<ASTNode> func_body) 
        : name(func_name), parameters(params), body(std::move(func_body)) {}
    
    virtual ~UserFunction() = default;
    
    // Disable copy constructor and assignment operator
    UserFunction(const UserFunction&) = delete;
    UserFunction& operator=(const UserFunction&) = delete;
    
    // Enable move semantics
    UserFunction(UserFunction&&) = default;
    UserFunction& operator=(UserFunction&&) = default;
};

} // namespace runtime
} // namespace engine
} // namespace core
} // namespace akao
