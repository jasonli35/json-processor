//
// Created by Mark on 1/30/2024.
//

#include "Model.h"
#include "Debug.h"
#include <string>

namespace ECE141 {

	// ---Model---
    const char null = '\0';

    

	Model::Model() {
        std::unique_ptr<ModelNode> parentNode = std::make_unique<ModelNode>(nullptr);
        current_node = ModelNode(ECE141::ModelNode::hashmap(), parentNode);
    }

	Model::Model(const Model& aModel) {
        *this = aModel;
	}

	Model &Model::operator=(const Model& aModel) {
        current_node = aModel.current_node;
		return *this;
	}

	ModelQuery Model::createQuery() {
		return ModelQuery(*this);
	}
    
    bool isLong(const std::string &aString) {
        size_t index = aString.find('.');
        if(index != std::string::npos) {return false;}
        for (size_t i = 0; i < aString.size(); i++) {
            if (index != i and std::isdigit(static_cast<unsigned char>(aString[i])) == false) {
                return false;
            }
        }
        return true;
    }

    //possibly refactor to visitor pattern later
    ECE141::ModelNode ECE141::Model::getVariantNonQuoteType(const std::string &aString) {
        ModelNode result;
        
        if(aString == "true") {
            result = ModelNode(true);
        }
        else if(aString == "false"){
            result = ModelNode(false);
        }
        else if(aString == "null") {
            result = ModelNode(null_obj());
        }
        else {
            if(isLong(aString)) {
                result = ModelNode(stol(aString));
            }
            else {
                double double_result = std::strtod(aString.c_str(), nullptr);
                if(double_result != 0.0 or aString.substr(0,3) == "0.0") {
                    result = ModelNode(double_result);
                }
                else {
                    std::cerr << "Caught exception with unknown data type" << std::endl;
                }
            }
        }
        return result;
    }



	bool Model::addKeyValuePair(const std::string& aKey, const std::string& aValue, Element aType) {
		
		// Print statement for debugging, remove after implementation
		DBG("\t'" << aKey << "' : '" << aValue << "'");
        ModelNode::hashmap thisHmap = std::get<ModelNode::hashmap>(current_node());
        if(aType == Element::quoted) {
            ModelNode stringVal(aValue);
            thisHmap[aKey] = stringVal;
        }
        else if(aType == Element::constant){
            thisHmap[aKey] = getVariantNonQuoteType(aValue);
        }
        else {
            return false;
        }
		return true;
	}

	bool Model::addItem(const std::string& aValue, Element aType) {
		TODO;
		// Print statement for debugging, remove after implementation
		DBG("\t'" << aValue << "'");
		return true;
	}

bool Model::openContainer(const std::string& aContainerName, Element aType) {
    
    // Print statement for debugging, remove after implementation
    //		DBG((aContainerName.empty() ? "EMPTY" : aContainerName) << " " << (aType == Element::object ? "{" : "["));
    ModelNode aNewNode;
    std::unique_ptr<ModelNode> this_node(std::make_unique<ModelNode>(current_node));
    switch (aType) {
        case Element::object:
            // ModelNode(hashmap value, std::unique_ptr<ModelNode> parent_node = nullptr): aNode(value)
            aNewNode = ModelNode(ECE141::ModelNode::hashmap(), this_node);
            break;
        case Element::array:
            //ModelNode(const std::vector<ModelNode> value, std::unique_ptr<ModelNode> parent_node = nullptr): aNode(value)
            aNewNode = ModelNode(std::vector<ModelNode>(), this_node);
            break;
        default:
            return false;
    }
    
    if(aContainerName.empty()) {//inside list
        //addItem(aNewNode, aType);
        std::get<std::vector<ModelNode>>(current_node.get_variant()).push_back(aNewNode);
    }
    else {//inside object
        ModelNode::hashmap thisHmap = std::get<ModelNode::hashmap>(current_node());
        thisHmap[aContainerName] = aNewNode;
    }
    
    current_node = aNewNode;
    
    //to create is object or list
    
}
	bool Model::closeContainer(const std::string& aContainerName, Element aType) {
		TODO;
		// Print statement for debugging, remove after implementation
		DBG(" " << (aType == Element::object ? "}" : "]"));
		return true;
	}


	// ---ModelQuery---

	ModelQuery::ModelQuery(Model &aModel) : model(aModel) {}

	ModelQuery& ModelQuery::select(const std::string& aQuery) {
		DBG("select(" << aQuery << ")");
		TODO;

		return *this;
	}

	ModelQuery& ModelQuery::filter(const std::string& aQuery) {
		DBG("filter(" << aQuery << ")");
		TODO;

		return *this;
	}

	size_t ModelQuery::count() {
		DBG("count()");
		TODO;

		return 0;
	}

	double ModelQuery::sum() {
		DBG("sum()");
		TODO;

		return 0.0;
	}

	std::optional<std::string> ModelQuery::get(const std::string& aKeyOrIndex) {
		DBG("get(" << aKeyOrIndex << ")");
		TODO;

		return std::nullopt;
	}

} // namespace ECE141
