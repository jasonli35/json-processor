//
// Created by Mark on 1/30/2024.
//

#pragma once

#include <string>
#include <optional>
#include "JSONParser.h"
#include <unordered_map>
#include <list>
#include <variant>



namespace ECE141 {

	class ModelQuery; // Forward declare
    
    struct null_obj{};
	// STUDENT: Your Model is built from a bunch of these...

	class ModelNode {
        
		// Sometimes a node holds a basic value (null, bool, number, string)
		// Sometimes a node holds a list of other nodes (list)
		// Sometimes a node holds a collection key-value pairs, where the value is a node (an object)
    public:
        friend class ModelTest;
        friend class Model;
        using hashmap = std::unordered_map<std::string, ModelNode>;
        using myVariant = std::variant<null_obj, bool, long, double, std::string, std::vector<ModelNode>, hashmap>;
        
        void setParentPtr(std::unique_ptr<ModelNode>& parent) {
            if(parent != nullptr) {
                parent_node.reset(parent.release());
            }
        }
        ModelNode() {
            aNode = null_obj();
            parent_node = nullptr;
        }
        ModelNode(const ModelNode& aCopy){
            *this = aCopy;
        }
        
        ModelNode& operator=(const ModelNode& aCopy) {
            aNode = aCopy.aNode;
            parent_node = std::make_unique<ModelNode>(*(aCopy.parent_node));
            return *this;
        }
        

        ModelNode(const std::vector<ModelNode> value, std::unique_ptr<ModelNode>& parent_node): aNode(value) {
            setParentPtr(parent_node);
        }
        ModelNode(hashmap value, std::unique_ptr<ModelNode>& parent_node): aNode(value) {
            setParentPtr(parent_node);
        }
        
        ModelNode(myVariant aVar): aNode(aVar) {}
        
        bool isNull() {
            return std::holds_alternative<null_obj>(aNode);
        }
        
        myVariant get_variant() {
            return aNode;
        }
        
        myVariant operator()(){
            return aNode;
        }
        
        
        
        
    protected:
        myVariant aNode;
        std::unique_ptr<ModelNode> parent_node = std::make_unique<ModelNode>(nullptr);
        
	};

  
	class Model : public JSONListener {
	public:
		Model();
		~Model() override = default;
		Model(const Model& aModel);
		Model &operator=(const Model& aModel);
        
		ModelQuery createQuery();
        
//        using myVariant = std::variant<null_obj, bool, long, double, std::string, std::vector<Model>, Model>;
        
        static ModelNode getVariantNonQuoteType(const std::string &aString);
        

	protected:
		// JSONListener methods
		bool addKeyValuePair(const std::string &aKey, const std::string &aValue, Element aType) override;
		bool addItem(const std::string &aValue, Element aType) override;
		bool openContainer(const std::string &aKey, Element aType) override;
		bool closeContainer(const std::string &aKey, Element aType) override;

		// STUDENT: Your model will contain a collection of ModelNode*'s
		//          Choose your container(s) wisely
       
        ModelNode current_node;
        friend class ModelTest;

	};

	class ModelQuery {
	public:
		ModelQuery(Model& aModel);

		// ---Traversal---
		ModelQuery& select(const std::string& aQuery);

		// ---Filtering---
		ModelQuery& filter(const std::string& aQuery);

		// ---Consuming---
		size_t count();
		double sum();
		std::optional<std::string> get(const std::string& aKeyOrIndex);

	protected:
		Model &model;

	};


} // namespace ECE141
