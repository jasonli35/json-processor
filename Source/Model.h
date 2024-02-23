//
// Created by Mark on 1/30/2024.
//

#pragma once

#include <string>
#include <optional>
#include "JSONParser.h"
#include <unordered_set>
#include <list>
#include <variant>
#include "Filter.h"
#include <map>



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
        
        
        static null_obj& defaultNullObj;
        
        ModelNode() : aNode(defaultNullObj) {}
        
        ModelNode(const ModelNode& aCopy) {*this = aCopy;}
        
        ModelNode& operator=(const ModelNode& aCopy);
        
        void setParentPtr(std::unique_ptr<ModelNode>& parent);
        
        ModelNode(std::vector<ModelNode>& value, std::unique_ptr<ModelNode>& parent_node): aNode(value) {setParentPtr(parent_node);}
        
        ModelNode(hashmap& value, std::unique_ptr<ModelNode>& parent_node): aNode(value) {setParentPtr(parent_node);}
        
        double getNumberValue();
        

        
        ModelNode(myVariant aVar): aNode(aVar) {}
        
        bool isNull() {return std::holds_alternative<null_obj>(aNode);}
        
        bool isObj();
        bool isVec();
        
        myVariant& get_variant() {return aNode;}
        
        myVariant operator()(){return aNode;}
        
        hashmap& getMap();
        std::vector<ECE141::ModelNode> getVector();

    protected:
        myVariant aNode;
        std::unique_ptr<ModelNode> parent_node;
        
	};



  
	class Model : public JSONListener {
	public:
		Model();
		~Model() override = default;
		Model(const Model& aModel);
		Model &operator=(const Model& aModel);
        
		ModelQuery createQuery();
        
        
        static ModelNode getVariantNonQuoteType(const std::string &aString);

        void resetCurrentNode(ModelNode setNode);

        
        
        ModelNode operator()();
        
        friend class ModelTest;
	protected:
		// JSONListener methods
		bool addKeyValuePair(const std::string &aKey, const std::string &aValue, Element aType) override;
		bool addItem(const std::string &aValue, Element aType) override;
		bool openContainer(const std::string &aKey, Element aType) override;
		bool closeContainer(const std::string &aKey, Element aType) override;
        
        ModelNode current_node;
        
	};


	class ModelQuery {
	public:

		ModelQuery(Model& aModel);

        // ModelQuery gets constructed with an Model that means
        // ModelQuery has a Model so that you can query
        // get Something search the Model for the something


		// ---Traversal---
		ModelQuery& select(const std::string& aQuery);

		// ---Filtering---
		ModelQuery& filter(const std::string& aQuery);

		// ---Consuming---
		size_t count();
		double sum();
		std::optional<std::string> get(const std::string& aKeyOrIndex);
        
        void handleQueryRequest(std::string aQuery);
        
        friend class NotifyMatchVisitor;

	protected:
		Model &model;
        std::unordered_set<std::string> matching_set_obj; //only use for model with object
        std::unordered_set<size_t> matching_set_list;
        using filterOpt = void(*)(std::string, std::string, ModelQuery&);
        static std::map<std::string, ModelQuery::filterOpt> handleFilterOpts;
    
	};


} // namespace ECE141
