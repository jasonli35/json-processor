//
// Created by Mark on 1/30/2024.
//

#pragma once

#include <string>
#include <optional>
#include "JSONParser.h"
#include <set>
#include <list>
#include <variant>
#include "Filter.h"
#include <map>
#include "Formatting.h"
#include <memory>
#include <vector>


namespace ECE141 {

	class ModelQuery; // Forward declare
    
    struct null_obj{};
	// STUDENT: Your Model is built from a bunch of these...

	class ModelNode {
        
		// Sometimes a node holds a basic value (null, bool, number, string)
		// Sometimes a node holds a list of other nodes (list)
		// Sometimes a node holds a collection key-value pairs, where the value is a node (an object)
    public:
        ModelNode* parent_node;
        friend class ModelTest;
        friend class Model;
        using hashmap = std::map<std::string, std::shared_ptr<ModelNode>>;
        using vec_ptr = std::vector<std::shared_ptr<ModelNode>>;
        using myVariant = std::variant<null_obj, bool, long, double, std::string, vec_ptr, hashmap>;
        

        ModelNode() : parent_node(nullptr), aNode(null_obj()) {}
            

        ModelNode(myVariant aVar, ModelNode* p_node): parent_node(p_node), aNode(aVar) {}
        
        ModelNode(myVariant aVar): parent_node(nullptr), aNode(aVar) {}
        
        ModelNode(const ModelNode& aCopy) {*this = aCopy;}
        
        ModelNode& operator=(const ModelNode& aCopy);
        
        ~ModelNode(){
            
        }
        
//        
//        ModelNode(vec_ptr value, ModelNode* p_node): aNode(value), parent_node(p_node) {}
//        
//        ModelNode(hashmap value,  ModelNode* p_node): aNode(value), parent_node(p_node) {
//            
//        }
        
        double getNumberValue();
        
        
        bool isNull() {return std::holds_alternative<null_obj>(aNode);}
        
        bool isObj();
        bool isVec();
        
        myVariant& get_variant() {return aNode;}
        
        myVariant operator()(){return aNode;}
        
        hashmap& getMap();
        vec_ptr& getVector();

    protected:
        myVariant aNode;

	};
  
	class Model : public JSONListener {
	public:
        ModelNode* root_node = nullptr;
		Model();
		~Model();
		Model(const Model& aModel);

        Model(ModelNode* node_ptr): root_node(node_ptr) {}
		Model &operator=(const Model& aModel);
        
		ModelQuery createQuery();
        
        
        static ModelNode getVariantNonQuoteType(const std::string &aString);

        void resetCurrentNode(ModelNode setNode);

        
        
        ModelNode& operator()();
        
        friend class ModelTest;
	protected:
		// JSONListener methods
		bool addKeyValuePair(const std::string &aKey, const std::string &aValue, Element aType) override;
		bool addItem(const std::string &aValue, Element aType) override;
		bool openContainer(const std::string &aKey, Element aType) override;
		bool closeContainer(const std::string &aKey, Element aType) override;
        
        ModelNode* current_node = nullptr;
        
         
	};


	class ModelQuery {
	public:

		ModelQuery(Model& aModel);

        // ModelQuery gets constructed with an Model that means
        // ModelQuery has a Model so that you can query
        // get Something search the Model for the something

        void update_matching();
		// ---Traversal---
		ModelQuery& select(const std::string& aQuery);

		// ---Filtering---
		ModelQuery& filter(const std::string& aQuery);

		// ---Consuming---
		size_t count();
		double sum();
		std::optional<std::string> get(const std::string& aKeyOrIndex);
        
        ModelNode* handleQueryRequest(std::string aQuery, ModelNode* current_mNode);
        
        
        friend class NotifyMatchVisitor;

	protected:
		Model &model;
        std::set<std::string> matching_set_obj; //only use for model with object
        std::set<size_t> matching_set_list;
        using filterOpt = void(*)(std::string, std::string, ModelQuery&);
        static std::map<std::string, ModelQuery::filterOpt> handleFilterOpts;
        ModelNode* current_node;
        ModelNode* root;
    
	};


} // namespace ECE141
