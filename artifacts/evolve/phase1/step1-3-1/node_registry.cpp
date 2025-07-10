/**
 * @id akao.evolve.node_registry.v1
 * @doc Implementation of central node registry with thread-safe registration, discovery, and management capabilities including singleton pattern, indexing, and comprehensive validation.
 * @specification Complete registry implementation with factory management, query processing, statistics collection, and thread-safe operations supporting all registry operations.
 * @scope artifacts/evolve/phase1/step1-3-1
 * @timeline 2025-07-10
 * @rationale Provide robust, thread-safe central registry for all node types with efficient discovery and instantiation capabilities.
 * @methodology Singleton pattern with std::mutex synchronization, indexed storage for fast lookups, and factory pattern for type-safe node instantiation.
 * @references akao.evolve.node_registry.v1, akao.evolve.node_definition.v1
 */

#include "node_registry.hpp"
#include "../step1-2-2/node_parameter.hpp"
#include <algorithm>
#include <sstream>
#include <thread>

namespace akao {
namespace evolve {

// Forward declare a minimal INode interface for this implementation
class INode {
public:
    virtual ~INode() = default;
    virtual std::string getNodeId() const = 0;
    virtual std::string getNodeType() const = 0;
};

// Static member definitions
std::unique_ptr<NodeRegistry> NodeRegistry::instance_ = nullptr;
std::mutex NodeRegistry::instance_mutex_;

// NodeDefinition implementation
NodeDefinition::NodeDefinition(const NodeDefinition& other)
    : node_id(other.node_id)
    , node_type(other.node_type)
    , display_name(other.display_name)
    , description(other.description)
    , category(other.category)
    , tags(other.tags)
    , author(other.author)
    , license(other.license)
    , version(other.version)
    , min_akao_version(other.min_akao_version)
    , capabilities(other.capabilities)
    , max_parallel_instances(other.max_parallel_instances)
    , timeout_seconds(other.timeout_seconds)
    , help_text(other.help_text)
    , example_usage(other.example_usage)
    , related_nodes(other.related_nodes)
    , akao_validation_rule(other.akao_validation_rule)
    , metadata(other.metadata)
{
    if (other.parameters) {
        parameters = std::make_unique<ParameterSet>(*other.parameters);
    }
}

NodeDefinition::NodeDefinition(NodeDefinition&& other) noexcept
    : node_id(std::move(other.node_id))
    , node_type(std::move(other.node_type))
    , display_name(std::move(other.display_name))
    , description(std::move(other.description))
    , category(other.category)
    , tags(std::move(other.tags))
    , author(std::move(other.author))
    , license(std::move(other.license))
    , version(other.version)
    , min_akao_version(other.min_akao_version)
    , capabilities(other.capabilities)
    , max_parallel_instances(other.max_parallel_instances)
    , timeout_seconds(other.timeout_seconds)
    , parameters(std::move(other.parameters))
    , help_text(std::move(other.help_text))
    , example_usage(std::move(other.example_usage))
    , related_nodes(std::move(other.related_nodes))
    , akao_validation_rule(std::move(other.akao_validation_rule))
    , metadata(std::move(other.metadata))
{
}

NodeDefinition& NodeDefinition::operator=(const NodeDefinition& other) {
    if (this != &other) {
        node_id = other.node_id;
        node_type = other.node_type;
        display_name = other.display_name;
        description = other.description;
        category = other.category;
        tags = other.tags;
        author = other.author;
        license = other.license;
        version = other.version;
        min_akao_version = other.min_akao_version;
        capabilities = other.capabilities;
        max_parallel_instances = other.max_parallel_instances;
        timeout_seconds = other.timeout_seconds;
        help_text = other.help_text;
        example_usage = other.example_usage;
        related_nodes = other.related_nodes;
        akao_validation_rule = other.akao_validation_rule;
        metadata = other.metadata;
        
        if (other.parameters) {
            parameters = std::make_unique<ParameterSet>(*other.parameters);
        } else {
            parameters.reset();
        }
    }
    return *this;
}

NodeDefinition& NodeDefinition::operator=(NodeDefinition&& other) noexcept {
    if (this != &other) {
        node_id = std::move(other.node_id);
        node_type = std::move(other.node_type);
        display_name = std::move(other.display_name);
        description = std::move(other.description);
        category = other.category;
        tags = std::move(other.tags);
        author = std::move(other.author);
        license = std::move(other.license);
        version = other.version;
        min_akao_version = other.min_akao_version;
        capabilities = other.capabilities;
        max_parallel_instances = other.max_parallel_instances;
        timeout_seconds = other.timeout_seconds;
        parameters = std::move(other.parameters);
        help_text = std::move(other.help_text);
        example_usage = std::move(other.example_usage);
        related_nodes = std::move(other.related_nodes);
        akao_validation_rule = std::move(other.akao_validation_rule);
        metadata = std::move(other.metadata);
    }
    return *this;
}

void NodeDefinition::setParameters(std::unique_ptr<ParameterSet> params) {
    parameters = std::move(params);
}

const ParameterSet* NodeDefinition::getParameters() const {
    return parameters.get();
}

NodeDefinition& NodeDefinition::addTag(const std::string& tag) {
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
    }
    return *this;
}

NodeDefinition& NodeDefinition::setCategory(NodeCategory cat) {
    category = cat;
    return *this;
}

NodeDefinition& NodeDefinition::setCapabilities(const NodeCapabilities& caps) {
    capabilities = caps;
    return *this;
}

NodeDefinition& NodeDefinition::setVersion(const NodeVersion& ver) {
    version = ver;
    return *this;
}

NodeDefinition& NodeDefinition::setMinAkaoVersion(const NodeVersion& ver) {
    min_akao_version = ver;
    return *this;
}

NodeDefinition& NodeDefinition::setAuthor(const std::string& author_name, const std::string& license_info) {
    author = author_name;
    if (!license_info.empty()) {
        license = license_info;
    }
    return *this;
}

NodeDefinition& NodeDefinition::setDescription(const std::string& desc) {
    description = desc;
    return *this;
}

NodeDefinition& NodeDefinition::setDocumentation(const std::string& help, const std::string& example) {
    help_text = help;
    if (!example.empty()) {
        example_usage = example;
    }
    return *this;
}

NodeDefinition& NodeDefinition::addRelatedNode(const std::string& node_id) {
    if (std::find(related_nodes.begin(), related_nodes.end(), node_id) == related_nodes.end()) {
        related_nodes.push_back(node_id);
    }
    return *this;
}

NodeDefinition& NodeDefinition::setAkaoValidation(const std::string& rule) {
    akao_validation_rule = rule;
    return *this;
}

NodeDefinition& NodeDefinition::addMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
    return *this;
}

std::optional<std::string> NodeDefinition::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    if (it != metadata.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool NodeDefinition::isValid() const {
    return !node_id.empty() && !node_type.empty() && !display_name.empty();
}

std::vector<std::string> NodeDefinition::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (node_id.empty()) {
        errors.push_back("Node ID cannot be empty");
    }
    if (node_type.empty()) {
        errors.push_back("Node type cannot be empty");
    }
    if (display_name.empty()) {
        errors.push_back("Display name cannot be empty");
    }
    
    return errors;
}

bool NodeDefinition::isCompatibleWith(const NodeVersion& akao_version) const {
    return min_akao_version.isCompatible(akao_version);
}

std::string NodeDefinition::getSummary() const {
    std::ostringstream oss;
    oss << display_name << " (" << node_id << ") v" << version.toString();
    if (!author.empty()) {
        oss << " by " << author;
    }
    return oss.str();
}

// Factory methods
NodeDefinition NodeDefinition::createCoreNode(const std::string& id, const std::string& name) {
    NodeDefinition def(id, id, name);
    return def.setCategory(NodeCategory::CORE)
              .setCapabilities(NodeCapabilities::safe())
              .setAuthor("Akao System", "MIT")
              .setMinAkaoVersion(NodeVersion(0, 0, 0)); // No specific version requirement
}

NodeDefinition NodeDefinition::createFilesystemNode(const std::string& id, const std::string& name) {
    NodeDefinition def(id, id, name);
    return def.setCategory(NodeCategory::FILESYSTEM)
              .setCapabilities(NodeCapabilities::filesystem())
              .setAuthor("Akao System", "MIT");
}

NodeDefinition NodeDefinition::createParserNode(const std::string& id, const std::string& name) {
    NodeDefinition def(id, id, name);
    return def.setCategory(NodeCategory::PARSER)
              .setCapabilities(NodeCapabilities::safe())
              .setAuthor("Akao System", "MIT");
}

NodeDefinition NodeDefinition::createAkaoNode(const std::string& id, const std::string& name) {
    NodeDefinition def(id, id, name);
    return def.setCategory(NodeCategory::AKAO)
              .setCapabilities(NodeCapabilities::akao())
              .setAuthor("Akao System", "MIT");
}

NodeDefinition NodeDefinition::createAnalysisNode(const std::string& id, const std::string& name) {
    NodeDefinition def(id, id, name);
    return def.setCategory(NodeCategory::ANALYSIS)
              .setCapabilities(NodeCapabilities::safe())
              .setAuthor("Akao System", "MIT");
}

NodeDefinition NodeDefinition::createValidationNode(const std::string& id, const std::string& name) {
    NodeDefinition def(id, id, name);
    return def.setCategory(NodeCategory::VALIDATION)
              .setCapabilities(NodeCapabilities::akao())
              .setAuthor("Akao System", "MIT");
}

// Utility functions
std::string categoryToString(NodeCategory category) {
    switch (category) {
        case NodeCategory::CORE: return "Core";
        case NodeCategory::FILESYSTEM: return "Filesystem";
        case NodeCategory::PARSER: return "Parser";
        case NodeCategory::ANALYSIS: return "Analysis";
        case NodeCategory::VALIDATION: return "Validation";
        case NodeCategory::CONTROL: return "Control";
        case NodeCategory::DATA: return "Data";
        case NodeCategory::IO: return "IO";
        case NodeCategory::AKAO: return "Akao";
        case NodeCategory::COMMUNITY: return "Community";
        case NodeCategory::CUSTOM: return "Custom";
    }
    return "Unknown";
}

std::optional<NodeCategory> stringToCategory(const std::string& category_str) {
    if (category_str == "Core") return NodeCategory::CORE;
    if (category_str == "Filesystem") return NodeCategory::FILESYSTEM;
    if (category_str == "Parser") return NodeCategory::PARSER;
    if (category_str == "Analysis") return NodeCategory::ANALYSIS;
    if (category_str == "Validation") return NodeCategory::VALIDATION;
    if (category_str == "Control") return NodeCategory::CONTROL;
    if (category_str == "Data") return NodeCategory::DATA;
    if (category_str == "IO") return NodeCategory::IO;
    if (category_str == "Akao") return NodeCategory::AKAO;
    if (category_str == "Community") return NodeCategory::COMMUNITY;
    if (category_str == "Custom") return NodeCategory::CUSTOM;
    return std::nullopt;
}

// NodeRegistry implementation
NodeRegistry& NodeRegistry::getInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<NodeRegistry>(new NodeRegistry());
    }
    return *instance_;
}

RegistrationResult NodeRegistry::registerNode(NodeDefinition definition, NodeFactory factory) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    // Validate definition
    auto errors = validateDefinition(definition);
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Validation failed: ";
        for (size_t i = 0; i < errors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << errors[i];
        }
        return RegistrationResult::createFailure(oss.str(), definition.node_id);
    }
    
    // Check for existing registration
    if (entries_.find(definition.node_id) != entries_.end()) {
        return RegistrationResult::createFailure("Node ID already registered: " + definition.node_id);
    }
    
    // Create entry
    auto entry = std::make_unique<RegistryEntry>(std::move(definition), std::move(factory));
    
    // Update indices
    updateIndices(entry->definition.node_id, entry->definition);
    
    // Store entry
    std::string node_id = entry->definition.node_id;
    entries_[node_id] = std::move(entry);
    
    return RegistrationResult::createSuccess(node_id);
}

bool NodeRegistry::unregisterNode(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    auto it = entries_.find(node_id);
    if (it == entries_.end()) {
        return false;
    }
    
    // Remove from indices
    removeFromIndices(node_id, it->second->definition);
    
    // Remove entry
    entries_.erase(it);
    
    return true;
}

bool NodeRegistry::hasNode(const std::string& node_type) const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    for (const auto& [id, entry] : entries_) {
        if (entry->definition.node_type == node_type && entry->is_enabled) {
            return true;
        }
    }
    return false;
}

bool NodeRegistry::hasNodeId(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    return entries_.find(node_id) != entries_.end();
}

std::optional<NodeDefinition> NodeRegistry::getDefinition(const std::string& node_type) const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    for (const auto& [id, entry] : entries_) {
        if (entry->definition.node_type == node_type && entry->is_enabled) {
            return entry->definition;
        }
    }
    return std::nullopt;
}

std::optional<NodeDefinition> NodeRegistry::getDefinitionById(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    auto it = entries_.find(node_id);
    if (it != entries_.end() && it->second->is_enabled) {
        return it->second->definition;
    }
    return std::nullopt;
}

std::unique_ptr<INode> NodeRegistry::createNode(const std::string& node_type) const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    for (auto& [id, entry] : entries_) {
        if (entry->definition.node_type == node_type && entry->is_enabled) {
            entry->instance_count++;
            return entry->factory();
        }
    }
    return nullptr;
}

std::unique_ptr<INode> NodeRegistry::createNodeById(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    auto it = entries_.find(node_id);
    if (it != entries_.end() && it->second->is_enabled) {
        it->second->instance_count++;
        return it->second->factory();
    }
    return nullptr;
}

std::vector<NodeDefinition> NodeRegistry::getAvailableNodes() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::vector<NodeDefinition> nodes;
    for (const auto& [id, entry] : entries_) {
        if (entry->is_enabled) {
            nodes.push_back(entry->definition);
        }
    }
    return nodes;
}

std::vector<NodeDefinition> NodeRegistry::queryNodes(const NodeQuery& query) const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::vector<NodeDefinition> result;
    for (const auto& [id, entry] : entries_) {
        if (query.enabled_only && !entry->is_enabled) {
            continue;
        }
        
        if (matchesQuery(entry->definition, query)) {
            result.push_back(entry->definition);
        }
    }
    return result;
}

std::vector<NodeDefinition> NodeRegistry::getNodesByCategory(NodeCategory category) const {
    return queryNodes(NodeQuery::byCategory(category));
}

std::vector<NodeDefinition> NodeRegistry::getNodesByTag(const std::string& tag) const {
    return queryNodes(NodeQuery::byTag(tag));
}

std::vector<NodeDefinition> NodeRegistry::getNodesByAuthor(const std::string& author) const {
    return queryNodes(NodeQuery::byAuthor(author));
}

bool NodeRegistry::setNodeEnabled(const std::string& node_id, bool enabled) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    auto it = entries_.find(node_id);
    if (it != entries_.end()) {
        it->second->is_enabled = enabled;
        return true;
    }
    return false;
}

bool NodeRegistry::isNodeEnabled(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    auto it = entries_.find(node_id);
    if (it != entries_.end()) {
        return it->second->is_enabled;
    }
    return false;
}

NodeRegistry::RegistryStats NodeRegistry::getStats() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    RegistryStats stats;
    stats.total_nodes = entries_.size();
    
    std::vector<std::pair<std::string, size_t>> usage_counts;
    
    for (const auto& [id, entry] : entries_) {
        if (entry->is_enabled) {
            stats.enabled_nodes++;
        }
        stats.total_instances += entry->instance_count;
        stats.nodes_by_category[entry->definition.category]++;
        
        usage_counts.emplace_back(id, entry->instance_count);
    }
    
    // Sort by usage count
    std::sort(usage_counts.begin(), usage_counts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Get top 5 most used nodes
    for (size_t i = 0; i < std::min(usage_counts.size(), size_t(5)); ++i) {
        stats.most_used_nodes.push_back(usage_counts[i].first);
    }
    
    return stats;
}

std::vector<std::string> NodeRegistry::validateRegistry() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::vector<std::string> all_errors;
    for (const auto& [id, entry] : entries_) {
        auto errors = validateDefinition(entry->definition);
        for (const auto& error : errors) {
            all_errors.push_back(id + ": " + error);
        }
    }
    return all_errors;
}

void NodeRegistry::clear() {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    entries_.clear();
    category_index_.clear();
    tag_index_.clear();
    author_index_.clear();
}

std::vector<std::string> NodeRegistry::getAllNodeTypes() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::vector<std::string> types;
    for (const auto& [id, entry] : entries_) {
        if (entry->is_enabled) {
            types.push_back(entry->definition.node_type);
        }
    }
    return types;
}

std::vector<std::string> NodeRegistry::getAllNodeIds() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::vector<std::string> ids;
    for (const auto& [id, entry] : entries_) {
        if (entry->is_enabled) {
            ids.push_back(id);
        }
    }
    return ids;
}

std::vector<NodeCategory> NodeRegistry::getAvailableCategories() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::vector<NodeCategory> categories;
    for (const auto& [category_str, node_ids] : category_index_) {
        auto category = stringToCategory(category_str);
        if (category.has_value() && !node_ids.empty()) {
            categories.push_back(category.value());
        }
    }
    return categories;
}

std::vector<std::string> NodeRegistry::getAvailableTags() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::vector<std::string> tags;
    for (const auto& [tag, node_ids] : tag_index_) {
        if (!node_ids.empty()) {
            tags.push_back(tag);
        }
    }
    return tags;
}

std::vector<std::string> NodeRegistry::getAvailableAuthors() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::vector<std::string> authors;
    for (const auto& [author, node_ids] : author_index_) {
        if (!node_ids.empty()) {
            authors.push_back(author);
        }
    }
    return authors;
}

std::string NodeRegistry::exportToString() const {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    std::ostringstream oss;
    oss << "NodeRegistry Export:\n";
    oss << "Total Nodes: " << entries_.size() << "\n";
    
    for (const auto& [id, entry] : entries_) {
        oss << "- " << entry->definition.getSummary() 
            << " [" << categoryToString(entry->definition.category) << "]"
            << " (enabled: " << (entry->is_enabled ? "yes" : "no") << ")"
            << " (instances: " << entry->instance_count << ")\n";
    }
    
    return oss.str();
}

bool NodeRegistry::isNodeCompatible(const std::string& node_id, const NodeVersion& akao_version) const {
    auto def = getDefinitionById(node_id);
    if (def.has_value()) {
        return def->isCompatibleWith(akao_version);
    }
    return false;
}

// Private helper methods
void NodeRegistry::updateIndices(const std::string& node_id, const NodeDefinition& definition) {
    // Category index
    std::string category_str = categoryToString(definition.category);
    category_index_[category_str].push_back(node_id);
    
    // Tag index
    for (const auto& tag : definition.tags) {
        tag_index_[tag].push_back(node_id);
    }
    
    // Author index
    if (!definition.author.empty()) {
        author_index_[definition.author].push_back(node_id);
    }
}

void NodeRegistry::removeFromIndices(const std::string& node_id, const NodeDefinition& definition) {
    // Remove from category index
    std::string category_str = categoryToString(definition.category);
    auto& category_list = category_index_[category_str];
    category_list.erase(std::remove(category_list.begin(), category_list.end(), node_id), 
                       category_list.end());
    
    // Remove from tag index
    for (const auto& tag : definition.tags) {
        auto& tag_list = tag_index_[tag];
        tag_list.erase(std::remove(tag_list.begin(), tag_list.end(), node_id), 
                      tag_list.end());
    }
    
    // Remove from author index
    if (!definition.author.empty()) {
        auto& author_list = author_index_[definition.author];
        author_list.erase(std::remove(author_list.begin(), author_list.end(), node_id), 
                         author_list.end());
    }
}

bool NodeRegistry::matchesQuery(const NodeDefinition& definition, const NodeQuery& query) const {
    if (query.category.has_value() && definition.category != query.category.value()) {
        return false;
    }
    
    if (query.tag.has_value()) {
        const auto& tag = query.tag.value();
        if (std::find(definition.tags.begin(), definition.tags.end(), tag) == definition.tags.end()) {
            return false;
        }
    }
    
    if (query.author.has_value() && definition.author != query.author.value()) {
        return false;
    }
    
    if (query.min_version.has_value() && definition.version < query.min_version.value()) {
        return false;
    }
    
    if (query.max_version.has_value() && query.max_version.value() < definition.version) {
        return false;
    }
    
    return true;
}

std::vector<std::string> NodeRegistry::validateDefinition(const NodeDefinition& definition) const {
    return definition.getValidationErrors();
}

} // namespace evolve
} // namespace akao