#ifndef AKAO_CORE_ENGINE_LOGIC_DATALOG_ENGINE_HPP
#define AKAO_CORE_ENGINE_LOGIC_DATALOG_ENGINE_HPP

#include <string>
#include <vector>
#include <map>

namespace akao::core::engine::logic {

class DatalogEngine {
public:
    DatalogEngine();
    
    // Core Datalog/Prolog functionality
    bool loadFacts(const std::string& facts_file);
    bool loadRules(const std::string& rules_file);
    std::vector<std::string> query(const std::string& query);
    bool addFact(const std::string& fact);
    bool addRule(const std::string& rule);
    
private:
    std::map<std::string, std::vector<std::string>> facts_;
    std::vector<std::string> rules_;
};

} // namespace akao::core::engine::logic

#endif // AKAO_CORE_ENGINE_LOGIC_DATALOG_ENGINE_HPP
