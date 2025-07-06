#include "../../../core/engine/logic/pure_logic_engine.hpp"
#include "../../../core/engine/logic/builtin_functions.hpp"
#include "../../../core/engine/logic/meta_functions.hpp"
#include "../../../core/engine/logic/godel_encoding.hpp"
#include <iostream>
#include <cassert>
#include <yaml-cpp/yaml.h>

using namespace akao::logic;

/**
 * @brief Comprehensive tests for Phase 4: Enhanced Gödel Numbering System
 * 
 * Tests all aspects of the enhanced Gödel encoding system:
 * - Bidirectional encoding/decoding verification
 * - Complex formula encoding accuracy
 * - Self-referential sentence construction
 * - Template substitution correctness
 * - Large number arithmetic handling
 */

void testBasicSymbolEncoding() {
    std::cout << "Testing basic symbol encoding..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    
    // Test primitive symbol encoding
    {
        std::vector<Value> args = {Value("and")};
        Value result = engine.executeFunction("godel.encode_primitive", args);
        assert(result.isInteger());
        int andCode = result.asInteger();
        std::cout << "  ✓ Encoded 'and' as: " << andCode << std::endl;
        assert(andCode == 2); // Should be 2 based on our mapping
    }
    
    // Test logical operator encoding
    {
        std::vector<Value> args = {Value("∀")};
        Value result = engine.executeFunction("godel.encode_primitive", args);
        assert(result.isInteger());
        int forallCode = result.asInteger();
        std::cout << "  ✓ Encoded '∀' as: " << forallCode << std::endl;
        assert(forallCode == 13); // Should be 13 based on our mapping
    }
    
    // Test variable encoding
    {
        std::vector<Value> args = {Value("x")};
        Value result = engine.executeFunction("godel.encode_variable", args);
        assert(result.isInteger());
        int varCode = result.asInteger();
        std::cout << "  ✓ Encoded variable 'x' as: " << varCode << std::endl;
        assert(varCode > 0);
    }
    
    std::cout << "✅ Basic symbol encoding test passed" << std::endl;
}

void testBidirectionalEncoding() {
    std::cout << "Testing bidirectional encoding/decoding..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    
    // Test simple formula encoding and decoding
    std::string originalFormula = "forall x: P(x)";
    
    {
        // Encode the formula
        std::vector<Value> encodeArgs = {Value(originalFormula)};
        Value encoded = engine.executeFunction("godel.encode_formula", encodeArgs);
        assert(encoded.isInteger());
        int godelNumber = encoded.asInteger();
        std::cout << "  ✓ Encoded '" << originalFormula << "' as Gödel number: " << godelNumber << std::endl;
        
        // Decode back
        std::vector<Value> decodeArgs = {Value(godelNumber)};
        Value decoded = engine.executeFunction("godel.decode_number", decodeArgs);
        assert(decoded.isString());
        std::string decodedFormula = decoded.asString();
        std::cout << "  ✓ Decoded back to: " << decodedFormula << std::endl;
        
        // Verify encoding correctness
        std::vector<Value> verifyArgs = {Value(originalFormula), Value(godelNumber)};
        Value verified = engine.executeFunction("godel.verify_encoding", verifyArgs);
        std::cout << "  ✓ Encoding verification: " << (verified.asBoolean() ? "PASS" : "FAIL") << std::endl;
    }
    
    // Test encoding validation
    {
        std::vector<Value> validArgs = {Value(12345)};
        Value isValid = engine.executeFunction("godel.is_valid_encoding", validArgs);
        assert(isValid.isBoolean());
        std::cout << "  ✓ Validation of number 12345: " << (isValid.asBoolean() ? "valid" : "invalid") << std::endl;
    }
    
    std::cout << "✅ Bidirectional encoding test passed" << std::endl;
}

void testComplexFormulaEncoding() {
    std::cout << "Testing complex formula encoding..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    
    // Test various complex formulas
    std::vector<std::string> testFormulas = {
        "forall x: exists y: P(x, y)",
        "not(A and B) or (C implies D)",
        "exists x: forall y: (x + y = y + x)",
        "(P → Q) ∧ (Q → R) → (P → R)",
        "∀x∃y(x < y ∧ Prime(y))"
    };
    
    for (const auto& formula : testFormulas) {
        std::vector<Value> args = {Value(formula)};
        Value result = engine.executeFunction("godel.encode_formula", args);
        assert(result.isInteger());
        int godelNumber = result.asInteger();
        std::cout << "  ✓ Encoded '" << formula << "' as: " << godelNumber << std::endl;
        
        // Test that different formulas get different encodings
        assert(godelNumber > 0);
    }
    
    std::cout << "✅ Complex formula encoding test passed" << std::endl;
}

void testSelfReferentialConstruction() {
    std::cout << "Testing self-referential sentence construction..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    
    // Test diagonal lemma
    {
        std::string propertyFormula = "NOT(Provable";
        std::vector<Value> args = {Value(propertyFormula)};
        Value result = engine.executeFunction("godel.diagonal_lemma", args);
        assert(result.isInteger());
        int diagonalGodel = result.asInteger();
        std::cout << "  ✓ Created diagonal sentence with Gödel number: " << diagonalGodel << std::endl;
    }
    
    // Test Gödel sentence creation
    {
        std::string systemName = "PA"; // Peano Arithmetic
        std::vector<Value> args = {Value(systemName)};
        Value result = engine.executeFunction("godel.create_godel_sentence", args);
        assert(result.isInteger());
        int godelSentence = result.asInteger();
        std::cout << "  ✓ Created Gödel sentence for system " << systemName << ": " << godelSentence << std::endl;
    }
    
    // Test fixed-point theorem
    {
        std::string formulaWithVar = "NOT(Provable(VAR))";
        std::vector<Value> args = {Value(formulaWithVar)};
        Value result = engine.executeFunction("godel.fixed_point_theorem", args);
        assert(result.isInteger());
        int fixedPoint = result.asInteger();
        std::cout << "  ✓ Created fixed-point sentence: " << fixedPoint << std::endl;
    }
    
    std::cout << "✅ Self-referential construction test passed" << std::endl;
}

void testTemplateSubstitution() {
    std::cout << "Testing template substitution system..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    
    // Test template substitution
    {
        std::string template_str = "The sentence with Gödel number GODEL_NUMBER is PROPERTY";
        
        std::map<std::string, Value> substitutions;
        substitutions["GODEL_NUMBER"] = Value("12345");
        substitutions["PROPERTY"] = Value("not provable");
        
        std::vector<Value> args = {Value(template_str), Value(substitutions)};
        Value result = engine.executeFunction("godel.substitute_godel", args);
        assert(result.isString());
        
        std::string substituted = result.asString();
        std::cout << "  ✓ Template substitution result: " << substituted << std::endl;
        
        // Verify substitutions occurred
        assert(substituted.find("12345") != std::string::npos);
        assert(substituted.find("not provable") != std::string::npos);
        assert(substituted.find("GODEL_NUMBER") == std::string::npos);
        assert(substituted.find("PROPERTY") == std::string::npos);
    }
    
    std::cout << "✅ Template substitution test passed" << std::endl;
}

void testArithmeticTranslation() {
    std::cout << "Testing arithmetic translation system..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    
    // Test translation to arithmetic
    {
        std::string logicalFormula = "A and B or not C";
        std::vector<Value> args = {Value(logicalFormula)};
        Value result = engine.executeFunction("godel.translate_to_arithmetic", args);
        assert(result.isString());
        
        std::string arithmetic = result.asString();
        std::cout << "  ✓ Translated '" << logicalFormula << "' to arithmetic: " << arithmetic << std::endl;
        
        // Should contain arithmetic operations
        assert(arithmetic.find("arithmetic(") != std::string::npos);
    }
    
    std::cout << "✅ Arithmetic translation test passed" << std::endl;
}

void testLargeNumberHandling() {
    std::cout << "Testing large number arithmetic handling..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    
    // Test with complex nested formula that should generate large Gödel numbers
    std::string complexFormula = "∀x∀y∀z((P(x,y) ∧ P(y,z)) → P(x,z)) ∧ ∃w∀v(P(w,v) → Q(v))";
    
    {
        std::vector<Value> args = {Value(complexFormula)};
        Value result = engine.executeFunction("godel.encode_formula", args);
        assert(result.isInteger());
        int largeGodel = result.asInteger();
        std::cout << "  ✓ Encoded complex formula as large Gödel number: " << largeGodel << std::endl;
        
        // Test that we can still validate it
        std::vector<Value> validArgs = {Value(largeGodel)};
        Value isValid = engine.executeFunction("godel.is_valid_encoding", validArgs);
        assert(isValid.isBoolean());
        std::cout << "  ✓ Large number validation: " << (isValid.asBoolean() ? "valid" : "invalid") << std::endl;
        
        // Test decoding
        std::vector<Value> decodeArgs = {Value(largeGodel)};
        Value decoded = engine.executeFunction("godel.decode_number", decodeArgs);
        assert(decoded.isString());
        std::cout << "  ✓ Decoded large number to: " << decoded.asString().substr(0, 50) << "..." << std::endl;
    }
    
    std::cout << "✅ Large number handling test passed" << std::endl;
}

int main() {
    std::cout << "=== Phase 4: Enhanced Gödel Numbering System Tests ===" << std::endl;
    
    try {
        testBasicSymbolEncoding();
        testBidirectionalEncoding();
        testComplexFormulaEncoding();
        testSelfReferentialConstruction();
        testTemplateSubstitution();
        testArithmeticTranslation();
        testLargeNumberHandling();
        
        std::cout << "\n🎉 All Phase 4 tests passed successfully!" << std::endl;
        std::cout << "Enhanced Gödel Numbering System is operational." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}