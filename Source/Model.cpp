//
// Created by Mark on 1/30/2024.
//

#include "Model.h"
#include "Debug.h"
#include <string>
#include <sstream>

#include <functional>


namespace ECE141 {

	// ---Model---


    

	Model::Model() {
        std::unique_ptr<ModelNode> parentNode = NULL;
      
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
        // you create the query from the model,
        // no need for modelQuery to observe changes because modelquery is created after model is created
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

void ModelNode::setParentPtr(std::unique_ptr<ModelNode>& parent) {
    if(parent != nullptr) {
        parent_node.reset(parent.release());
    }
}

ModelNode& ModelNode::operator=(const ModelNode& aCopy) {
    aNode = aCopy.aNode;
    if(aCopy.parent_node != NULL) {
        parent_node = std::make_unique<ModelNode>(*(aCopy.parent_node));
    }
    return *this;
}
    



    bool isAstring(Element aType) {
        return aType == Element::quoted;
    }

    bool isConstant(Element aType) {
        return aType == Element::constant;
    }

    bool Model::addKeyValuePair(const std::string& aKey, const std::string& aValue, Element aType) {
		
		// Print statement for debugging, remove after implementation
//		DBG("\t'" << aKey << "' : '" << aValue << "'");
        ModelNode::hashmap thisHmap = std::get<ModelNode::hashmap>(current_node());
        
        if(isAstring(aType)) {
            ModelNode insertStringNode(aValue);
            thisHmap[aKey] = insertStringNode;
        }
        else if(isConstant(aType)){
            ModelNode insertConsNode(getVariantNonQuoteType(aValue));
            thisHmap[aKey] = insertConsNode;
        }
        else {
            return false;
        }
        notifyObservers(aKey);
		return true;
	}

	bool Model::addItem(const std::string& aValue, Element aType) {
	
		// Print statement for debugging, remove after implementation
//		DBG("\t'" << aValue << "'");
        ModelNode insert_node;
        if(isAstring(aType)) {
            insert_node = ModelNode(aValue);
        }
        else if(isConstant(aType)) {
            insert_node = getVariantNonQuoteType(aValue);
        }
        else {
            return false;
        }
        
        std::vector<ECE141::ModelNode> current_vec = current_node.getVector();
        notifyObservers(current_vec.size());
        current_vec.push_back(insert_node);
		return true;
	}

    // Model which is in memory representation of the JSON
    // You have a query which has a model that it can search
    // You have modelNodes which make up the JSON and model has the root modelNode
        

        void Model::addObserver(const Observer& observer) {
//            std::unique_ptr<Observer> observer_ptr = std::make_unique<Observer>(observer);
//            observers.push_back(observer_ptr);
            // what does push_back do?
            // yes it adds the element to the end of the vector
            // but doing so it copies the element into the vector

            // we need to make the last element of the vector be a unique_ptr to the observer
            //observers.push_back(std::make_unique<Observer>(observer));

        }
        

           // Method to notify observers
        void Model::notifyObservers(const std::variant<std::string, size_t>& addedModelNode) {
            for (std::unique_ptr<Observer>& observer: observers) {
                observer->update_matching(addedModelNode);
            }
        }



bool Model::openContainer(const std::string& aContainerName, Element aType) {
    
    // Print statement for debugging, remove after implementation
    //		DBG((aContainerName.empty() ? "EMPTY" : aContainerName) << " " << (aType == Element::object ? "{" : "["));
    ModelNode aNewNode;
    std::unique_ptr<ModelNode> this_node(std::make_unique<ModelNode>(current_node));
    switch (aType) {
        case Element::object:
  
            aNewNode = ModelNode(ECE141::ModelNode::hashmap(), this_node);
            break;
        case Element::array:
            //ModelNode(const std::vector<ModelNode> value, std::unique_ptr<ModelNode> parent_node = nullptr): aNode(value)
            aNewNode = ModelNode(std::vector<ModelNode>(), this_node);
            break;
        default:
            return false;
    }
    
    //addObserver(createQuery());

    if(aContainerName.empty()) {//inside list
        //addItem(aNewNode, aType);
        if(current_node.isNull()) {
            current_node = aNewNode;
        }
        else {
            current_node.getVector().push_back(aNewNode);
        }
        
    }
    else {//inside object
        ModelNode::hashmap thisHmap = std::get<ModelNode::hashmap>(current_node());
        thisHmap[aContainerName] = aNewNode;
    }
    
    current_node = aNewNode;
    return true;
    //to create is object or list
    
}
	bool Model::closeContainer(const std::string& aContainerName, Element aType) {
	
		// Print statement for debugging, remove after implementation
//		DBG(" " << (aType == Element::object ? "}" : "]"));
        
        if(current_node.parent_node == nullptr) {
            return false;
        }
        current_node = *(current_node.parent_node);
		return true;
	}


	// ---ModelQuery---
        


	ModelQuery::ModelQuery(Model &aModel) : model(aModel) {
//        if(aModel.)
    }
    
    
        

    const char period = '.';

std::string earaseQuote(std::string aString) {
    return aString.substr(1, aString.length() - 2);
}


    
void ModelQuery::handleQueryRequest(std::string aQuery) {
    if(aQuery[0] == '\'') {
        std::string key = earaseQuote(aQuery);
        ModelNode::hashmap currentMap = model().getMap();
        model.resetCurrentNode(currentMap[key]);
    }
    else {
        int index = std::stoi(aQuery);
        model.resetCurrentNode(model().getVector()[index]);
    }
}
        
std::vector<ECE141::ModelNode> ModelNode::getVector() {
    return std::get<std::vector<ModelNode>>(aNode);
}

ModelNode::hashmap ModelNode::getMap() {
    return std::get<ModelNode::hashmap>(get_variant());
}
        
void Model::resetCurrentNode(ModelNode setNode) {
    current_node = setNode;
}
        
ModelNode Model::operator()(){
    return current_node;
}
        
class NotifyMatchVisitor {
public:
    NotifyMatchVisitor(ModelQuery& mQuery): a_observer(mQuery) {}
    void operator()(const std::string& insertKey) {
        a_observer.map_matching_set.insert(insertKey);
    }
    void operator()(const size_t theSize) {
        a_observer.vector_matching_set.insert(theSize);
     }
protected:
    ModelQuery& a_observer;
};
        
//void ModelQuery::update_matching(std::variant<std::string, size_t> aKey) {
//    std::visit(NotifyMatchVisitor(*this), aKey);
//}

	ModelQuery& ModelQuery::select(const std::string& aQuery) {

//		DBG("select(" << aQuery << ")");
        
        std:: stringstream sStream(aQuery);
        std::string nextQuery;
        while (std::getline(sStream, nextQuery, period)) {
            handleQueryRequest(nextQuery);
        }
        
		return *this;
	}


std::map<std::string, ModelQuery::filterOpt> ModelQuery::handleFilterOpts = {
    {"key", [](std::string action, std::string value, ModelQuery& mQuery) {
        // Function for "key" operation
        if(action == "contains") {
            std::string stringToFind = earaseQuote(value);
            std::unordered_set<std::string> keys_set = mQuery.map_matching_set;
            for (std::string akey: keys_set) {
                if(akey.find(stringToFind) == std::string::npos) {
                    keys_set.erase(akey);
                }
            }
        }
        else {
            std::cerr << "action is not available for filter" << std::endl;
        }
    }},
    {"index", [](std::string action, std::string value, ModelQuery& mQuery) {
        // Function for "index" operation
        ECE141::Filter aFilter(std::stoi(value), action);
        std::unordered_set<size_t> int_set = mQuery.vector_matching_set;
        
        for (size_t i = mQuery.model().getVector().size() - 1; i >= 0; i--) {
            if (!aFilter.apply(i)) {
                int_set.erase(i);
            }
        }

    }
 
}};
    
	ModelQuery& ModelQuery::filter(const std::string& aQuery) {
//		DBG("filter(" << aQuery << ")");
        ModelNode::myVariant container = model().get_variant();
        if(!std::holds_alternative<std::vector<ModelNode>>(container) and !std::holds_alternative<ModelNode::hashmap>(container)) {
            std::cerr << "Model is not currently holding valid container" << std::endl;
        }
        else {
            std::istringstream stringStream(aQuery);
            std::vector<std::string> operations_vec;
            std::string a_op;
            while (std::getline(stringStream, a_op, ' ')) {
                operations_vec.push_back(a_op);
            }
            std::string identifier = operations_vec[0];
            auto it = handleFilterOpts.find(identifier);
            if (it != handleFilterOpts.end()) {
                it->second(operations_vec[1], operations_vec[2], *this);
            } else {
                std::cout << "Unknown operation: " << identifier << std::endl;
            }
            

        }
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
