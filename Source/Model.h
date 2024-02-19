//
// Created by Mark on 1/30/2024.
//

#pragma once

#include <string>
#include <optional>
#include "JSONParser.h"
#include <unordered_map>
#include <list>


namespace ECE141 {

	class ModelQuery; // Forward declare
    
    struct null_obj{};
	// STUDENT: Your Model is built from a bunch of these...

	class ModelNode {
		// Sometimes a node holds a basic value (null, bool, number, string)
		// Sometimes a node holds a list of other nodes (list)
		// Sometimes a node holds a collection key-value pairs, where the value is a node (an object)
        friend class ModelTest;
        friend class Model;
        using myVariant = std::variant<null_obj, bool, long, double, std::string, std::vector<ModelNode>, std::unordered_map<std::string, ModelNode>>;
        
        ModelNode(null_obj value): aNode(value){}
        ModelNode(bool value): aNode(value){}
        ModelNode(long value): aNode(value){}
        ModelNode(double value): aNode(value){}
        ModelNode(std::string value): aNode(value){}
        ModelNode(std::vector<ModelNode> value): aNode(value){}
        ModelNode(std::unordered_map<std::string, ModelNode> value): aNode(value){}
        
        myVariant get() {
            return aNode;
        }
        
    protected:
        myVariant aNode;

	};

  
	class Model : public JSONListener {
	public:
		Model();
		~Model() override = default;
		Model(const Model& aModel);
		Model &operator=(const Model& aModel);

		ModelQuery createQuery();
        
//        using myVariant = std::variant<null_obj, bool, long, double, std::string, std::vector<Model>, Model>;
        
        static ModelNode::myVariant getVariantNonQuoteType(const std::string &aString);
        

	protected:
		// JSONListener methods
		bool addKeyValuePair(const std::string &aKey, const std::string &aValue, Element aType) override;
		bool addItem(const std::string &aValue, Element aType) override;
		bool openContainer(const std::string &aKey, Element aType) override;
		bool closeContainer(const std::string &aKey, Element aType) override;

		// STUDENT: Your model will contain a collection of ModelNode*'s
		//          Choose your container(s) wisely
       

        std::unordered_map<std::string, ModelNode> data;
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
