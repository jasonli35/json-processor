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
        
      
        
        ModelNode() {aNode = null_obj();}
        
        ModelNode(const ModelNode& aCopy) {*this = aCopy;}
        
        ModelNode& operator=(const ModelNode& aCopy);
        void setParentPtr(std::unique_ptr<ModelNode>& parent);
        
        ModelNode(const std::vector<ModelNode> value, std::unique_ptr<ModelNode>& parent_node): aNode(value) {setParentPtr(parent_node);}
        
        ModelNode(hashmap value, std::unique_ptr<ModelNode>& parent_node): aNode(value) {setParentPtr(parent_node);}
        

        
        ModelNode(myVariant aVar): aNode(aVar) {}
        
        bool isNull() {return std::holds_alternative<null_obj>(aNode);}
        
        myVariant get_variant() {return aNode;}
        
        myVariant operator()(){return aNode;}
        
        ModelNode::hashmap getMap();
        std::vector<ECE141::ModelNode> getVector();

    protected:
        myVariant aNode;
        std::unique_ptr<ModelNode> parent_node;
        
	};

    class Observer {
    public:
        Observer() {}
        Observer(const Observer &) {}
        virtual void update_matching(std::variant<std::string, size_t> aKey);
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

        void resetCurrentNode(ModelNode setNode);
        
        void addObserver(const Observer& observer);
        void notifyObservers(const std::variant<std::string, size_t>& addedModelNode);
        
        
        ModelNode operator()();
        
        friend class ModelTest;
	protected:
		// JSONListener methods
		bool addKeyValuePair(const std::string &aKey, const std::string &aValue, Element aType) override;
		bool addItem(const std::string &aValue, Element aType) override;
		bool openContainer(const std::string &aKey, Element aType) override;
		bool closeContainer(const std::string &aKey, Element aType) override;
        
       
		// STUDENT: Your model will contain a collection of ModelNode*'s
		//          Choose your container(s) wisely

        
        ModelNode current_node;
        
        std::vector<std::unique_ptr<Observer>> observers; 
        //Observer aOb;
     

	};


	class ModelQuery: public Observer {
	public:
		ModelQuery(Model& aModel);
        
        void update_matching(std::variant<std::string, size_t> aKey) override;


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
        std::unordered_set<std::string> map_matching_set; //only use for model with object
        std::unordered_set<size_t> vector_matching_set; //only use for model with vector
        using filterOpt = void(*)(std::string, std::string, ModelQuery&);
        static std::map<std::string, ModelQuery::filterOpt> handleFilterOpts;
    
	};


} // namespace ECE141
