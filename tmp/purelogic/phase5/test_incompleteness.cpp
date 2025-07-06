#include "../../../core/engine/logic/pure_logic_engine.hpp"
#include "../../../core/engine/logic/builtin_functions.hpp"
#include "../../../core/engine/logic/meta_functions.hpp"
#include "../../../core/engine/logic/godel_encoding.hpp"
#include "../../../core/engine/logic/formal_system.hpp"
#include "../../../core/engine/logic/incompleteness_proofs.hpp"
#include <iostream>
#include <cassert>
#include <yaml-cpp/yaml.h>

using namespace akao::logic;

/**
 * @brief Comprehensive tests for Phase 5: Formal Incompleteness Theorem Framework
 * 
 * Tests all aspects of the formal system and incompleteness framework:
 * - Formal system consistency checking
 * - Proof verification accuracy  
 * - Undecidable statement generation
 * - First and Second Incompleteness Theorems demonstration
 * - Independence proof construction
 */

void testFormalSystemDefinition() {
    std::cout << "Testing formal system definition and analysis..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test axiom system definition
    {
        std::vector<Value> axioms = {
            Value("forall x: (x = x)"),                    // Reflexivity
            Value("forall x, y: (x = y) implies (y = x)"), // Symmetry
            Value("forall x, y, z: ((x = y) and (y = z)) implies (x = z)"), // Transitivity
            Value("0 is a natural number"),
            Value("forall x: successor(x) is a natural number")
        };
        
        std::vector<Value> args = {Value(axioms)};
        Value result = engine.executeFunction("system.define_axioms", args);
        assert(result.isObject());
        
        auto systemInfo = result.asObject();
        std::string systemName = systemInfo.at("system_name").asString();
        int axiomCount = systemInfo.at("axiom_count").asInteger();
        
        std::cout << "  ✓ Defined formal system: " << systemName << std::endl;
        std::cout << "  ✓ Axiom count: " << axiomCount << std::endl;
        assert(axiomCount == 5);
    }
    
    // Test consistency checking
    {
        std::vector<Value> consistentAxioms = {
            Value("A implies A"),
            Value("(A and B) implies A"),
            Value("A implies (A or B)")
        };
        
        std::vector<Value> args = {Value(consistentAxioms)};
        Value result = engine.executeFunction("system.is_consistent", args);
        assert(result.isBoolean());
        bool isConsistent = result.asBoolean();
        
        std::cout << "  ✓ Consistency check for valid axioms: " << (isConsistent ? "consistent" : "inconsistent") << std::endl;
        assert(isConsistent);
    }
    
    // Test inconsistency detection
    {
        std::vector<Value> inconsistentAxioms = {
            Value("P"),
            Value("not(P)")
        };
        
        std::vector<Value> args = {Value(inconsistentAxioms)};
        Value result = engine.executeFunction("system.is_consistent", args);
        assert(result.isBoolean());
        bool isConsistent = result.asBoolean();
        
        std::cout << "  ✓ Consistency check for contradictory axioms: " << (isConsistent ? "consistent" : "inconsistent") << std::endl;
        assert(!isConsistent);
    }
    
    std::cout << "✅ Formal system definition test passed" << std::endl;
}

void testProofVerification() {
    std::cout << "Testing proof verification system..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test basic derivation verification
    {
        std::vector<Value> premises = {
            Value("A"),
            Value("A implies B")
        };
        std::string conclusion = "B";
        std::vector<Value> steps = {
            Value("modus ponens applied to premises 1 and 2")
        };
        
        std::vector<Value> args = {Value(premises), Value(conclusion), Value(steps)};
        Value result = engine.executeFunction("proof.verify_derivation", args);
        assert(result.isBoolean());
        
        bool isValid = result.asBoolean();
        std::cout << "  ✓ Modus ponens verification: " << (isValid ? "valid" : "invalid") << std::endl;
        assert(isValid);
    }
    
    // Test automated proof search
    {
        std::string statement = "true";
        int maxDepth = 5;
        
        std::vector<Value> args = {Value(statement), Value(maxDepth)};
        Value result = engine.executeFunction("proof.search_proof", args);
        assert(result.isObject());
        
        auto proofResult = result.asObject();
        bool proofFound = proofResult.at("proof_found").asBoolean();
        
        std::cout << "  ✓ Proof search for 'true': " << (proofFound ? "proof found" : "no proof") << std::endl;
        assert(proofFound);
    }
    
    // Test provability predicate
    {
        std::string statement = "A implies A";
        std::string system = "propositional_logic";
        
        std::vector<Value> args = {Value(statement), Value(system)};
        Value result = engine.executeFunction("proof.is_provable", args);
        assert(result.isBoolean());
        
        bool isProvable = result.asBoolean();
        std::cout << "  ✓ Provability of 'A implies A': " << (isProvable ? "provable" : "not provable") << std::endl;
        assert(isProvable);
    }
    
    std::cout << "✅ Proof verification test passed" << std::endl;
}

void testFirstIncompletenessTheorem() {
    std::cout << "Testing First Incompleteness Theorem demonstration..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test First Incompleteness Theorem
    {
        std::string system = "Peano_Arithmetic";
        std::vector<Value> args = {Value(system)};
        Value result = engine.executeFunction("incompleteness.first_theorem", args);
        assert(result.isObject());
        
        auto theoremResult = result.asObject();
        std::string theorem = theoremResult.at("theorem").asString();
        std::string godelSentence = theoremResult.at("godel_sentence").asString();
        int godelNumber = theoremResult.at("godel_number").asInteger();
        bool isConsistent = theoremResult.at("is_consistent").asBoolean();
        bool isComplete = theoremResult.at("is_complete").asBoolean();
        
        std::cout << "  ✓ Theorem: " << theorem << std::endl;
        std::cout << "  ✓ System: " << system << std::endl;
        std::cout << "  ✓ Gödel sentence: " << godelSentence.substr(0, 50) << "..." << std::endl;
        std::cout << "  ✓ Gödel number: " << godelNumber << std::endl;
        std::cout << "  ✓ System consistent: " << (isConsistent ? "yes" : "no") << std::endl;
        std::cout << "  ✓ System complete: " << (isComplete ? "yes" : "no") << std::endl;
        
        // Key verification: if consistent, then incomplete
        assert(isConsistent && !isComplete);
    }
    
    std::cout << "✅ First Incompleteness Theorem test passed" << std::endl;
}

void testSecondIncompletenessTheorem() {
    std::cout << "Testing Second Incompleteness Theorem demonstration..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test Second Incompleteness Theorem
    {
        std::string system = "ZFC_Set_Theory";
        std::vector<Value> args = {Value(system)};
        Value result = engine.executeFunction("incompleteness.second_theorem", args);
        assert(result.isObject());
        
        auto theoremResult = result.asObject();
        std::string theorem = theoremResult.at("theorem").asString();
        std::string consistencyStatement = theoremResult.at("consistency_statement").asString();
        std::string keyInsight = theoremResult.at("key_insight").asString();
        
        std::cout << "  ✓ Theorem: " << theorem << std::endl;
        std::cout << "  ✓ System: " << system << std::endl;
        std::cout << "  ✓ Consistency statement: " << consistencyStatement.substr(0, 50) << "..." << std::endl;
        std::cout << "  ✓ Key insight: " << keyInsight << std::endl;
        
        // Verify key insight (more flexible matching)
        assert(keyInsight.find("prove its own consistency") != std::string::npos ||
               keyInsight.find("cannot prove") != std::string::npos);
    }
    
    std::cout << "✅ Second Incompleteness Theorem test passed" << std::endl;
}

void testUndecidableStatementGeneration() {
    std::cout << "Testing undecidable statement generation..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test undecidable statement generation
    {
        std::string system = "Peano_Arithmetic";
        std::vector<Value> args = {Value(system)};
        Value result = engine.executeFunction("incompleteness.generate_undecidable", args);
        assert(result.isObject());
        
        auto genResult = result.asObject();
        auto undecidableStatements = genResult.at("undecidable_statements").asCollection();
        int count = genResult.at("count").asInteger();
        auto explanations = genResult.at("explanations").asCollection();
        
        std::cout << "  ✓ Generated " << count << " undecidable statements:" << std::endl;
        
        for (size_t i = 0; i < undecidableStatements.size() && i < 3; i++) {
            std::string statement = undecidableStatements[i].asString();
            std::cout << "    " << (i + 1) << ". " << statement.substr(0, 60);
            if (statement.length() > 60) std::cout << "...";
            std::cout << std::endl;
        }
        
        assert(count > 0);
        assert(undecidableStatements.size() == explanations.size());
    }
    
    std::cout << "✅ Undecidable statement generation test passed" << std::endl;
}

void testGodelSentenceVerification() {
    std::cout << "Testing Gödel sentence verification..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test Gödel sentence verification
    {
        std::string sentence = "The statement with Gödel number 12345 is not provable in PA";
        std::string system = "PA";
        
        std::vector<Value> args = {Value(sentence), Value(system)};
        Value result = engine.executeFunction("theorem.verify_godel_sentence", args);
        assert(result.isObject());
        
        auto verificationResult = result.asObject();
        bool isSelfreferential = verificationResult.at("is_self_referential").asBoolean();
        bool hasUnprovabilityAssertion = verificationResult.at("has_unprovability_assertion").asBoolean();
        bool refersToItself = verificationResult.at("refers_to_itself").asBoolean();
        bool isValidGodel = verificationResult.at("is_valid_godel_sentence").asBoolean();
        std::string verificationResultStr = verificationResult.at("verification_result").asString();
        
        std::cout << "  ✓ Sentence: " << sentence.substr(0, 50) << "..." << std::endl;
        std::cout << "  ✓ Self-referential: " << (isSelfreferential ? "yes" : "no") << std::endl;
        std::cout << "  ✓ Has unprovability assertion: " << (hasUnprovabilityAssertion ? "yes" : "no") << std::endl;
        std::cout << "  ✓ Refers to itself: " << (refersToItself ? "yes" : "no") << std::endl;
        std::cout << "  ✓ Valid Gödel sentence: " << (isValidGodel ? "yes" : "no") << std::endl;
        std::cout << "  ✓ Verification result: " << verificationResultStr << std::endl;
        
        assert(hasUnprovabilityAssertion);
        assert(refersToItself);
    }
    
    std::cout << "✅ Gödel sentence verification test passed" << std::endl;
}

void testIndependenceProofs() {
    std::cout << "Testing independence proof construction..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test independence proof for known independent statement
    {
        std::string statement = "continuum hypothesis";
        std::string system = "ZFC";
        
        std::vector<Value> args = {Value(statement), Value(system)};
        Value result = engine.executeFunction("incompleteness.independence_proof", args);
        assert(result.isObject());
        
        auto independenceResult = result.asObject();
        bool isIndependent = independenceResult.at("is_independent").asBoolean();
        std::string method = independenceResult.at("method").asString();
        
        std::cout << "  ✓ Statement: " << statement << std::endl;
        std::cout << "  ✓ System: " << system << std::endl;
        std::cout << "  ✓ Is independent: " << (isIndependent ? "yes" : "no") << std::endl;
        std::cout << "  ✓ Method: " << method << std::endl;
        
        // Continuum hypothesis is known to be independent of ZFC
        assert(isIndependent);
    }
    
    std::cout << "✅ Independence proof test passed" << std::endl;
}

void testCompleteIncompletenessDemo() {
    std::cout << "Testing complete incompleteness demonstration..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test complete demonstration
    {
        std::string systemName = "Peano_Arithmetic";
        std::vector<Value> args = {Value(systemName)};
        Value result = engine.executeFunction("theorem.demonstrate_incompleteness", args);
        assert(result.isObject());
        
        auto demoResult = result.asObject();
        std::string system = demoResult.at("system").asString();
        std::string demoType = demoResult.at("demonstration_type").asString();
        bool canRepresentArithmetic = demoResult.at("can_represent_arithmetic").asBoolean();
        std::string conclusion = demoResult.at("conclusion").asString();
        
        std::cout << "  ✓ System: " << system << std::endl;
        std::cout << "  ✓ Demo type: " << demoType << std::endl;
        std::cout << "  ✓ Can represent arithmetic: " << (canRepresentArithmetic ? "yes" : "no") << std::endl;
        std::cout << "  ✓ Conclusion: " << conclusion << std::endl;
        
        assert(canRepresentArithmetic);
        assert(conclusion.find("demonstrated successfully") != std::string::npos);
        
        // Should have both theorem results
        assert(demoResult.find("first_theorem_result") != demoResult.end());
        assert(demoResult.find("second_theorem_result") != demoResult.end());
        assert(demoResult.find("undecidable_statements") != demoResult.end());
    }
    
    std::cout << "✅ Complete incompleteness demonstration test passed" << std::endl;
}

void testConsistencyImplications() {
    std::cout << "Testing consistency implications analysis..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    godel::registerGodelFunctions(engine);
    formal::registerFormalSystemFunctions(engine);
    incompleteness::registerIncompletenessProofFunctions(engine);
    
    // Test consistency implications
    {
        std::string assumption = "System PA is consistent";
        std::vector<Value> args = {Value(assumption)};
        Value result = engine.executeFunction("theorem.consistency_implications", args);
        assert(result.isObject());
        
        auto implicationResult = result.asObject();
        auto implications = implicationResult.at("implications").asCollection();
        std::string analysis = implicationResult.at("analysis").asString();
        
        std::cout << "  ✓ Assumption: " << assumption << std::endl;
        std::cout << "  ✓ Number of implications: " << implications.size() << std::endl;
        
        for (size_t i = 0; i < implications.size() && i < 3; i++) {
            std::cout << "    " << (i + 1) << ". " << implications[i].asString() << std::endl;
        }
        
        assert(implications.size() > 0);
        assert(analysis.find("impossible triangle") != std::string::npos);
    }
    
    std::cout << "✅ Consistency implications test passed" << std::endl;
}

int main() {
    std::cout << "=== Phase 5: Formal Incompleteness Theorem Framework Tests ===" << std::endl;
    
    try {
        testFormalSystemDefinition();
        testProofVerification();
        testFirstIncompletenessTheorem();
        testSecondIncompletenessTheorem();
        testUndecidableStatementGeneration();
        testGodelSentenceVerification();
        testIndependenceProofs();
        testCompleteIncompletenessDemo();
        testConsistencyImplications();
        
        std::cout << "\n🎉 All Phase 5 tests passed successfully!" << std::endl;
        std::cout << "Formal Incompleteness Theorem Framework is operational." << std::endl;
        std::cout << "\n📊 SUMMARY:" << std::endl;
        std::cout << "✅ Formal system definition and consistency checking" << std::endl;
        std::cout << "✅ Proof verification and automated proof search" << std::endl;
        std::cout << "✅ First Incompleteness Theorem demonstration" << std::endl;
        std::cout << "✅ Second Incompleteness Theorem demonstration" << std::endl;
        std::cout << "✅ Undecidable statement generation" << std::endl;
        std::cout << "✅ Gödel sentence verification" << std::endl;
        std::cout << "✅ Independence proof construction" << std::endl;
        std::cout << "✅ Complete incompleteness demonstration" << std::endl;
        std::cout << "✅ Consistency implications analysis" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}