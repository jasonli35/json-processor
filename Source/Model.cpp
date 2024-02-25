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


    

	Model::Model() {}

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
        // no need for modelQuery to ove changes because modelquery is created after model is created
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

    ECE141::ModelNode ECE141::Model::getVariantNonQuoteType(const std::string &aString) {
        ModelNode result;

        if(aString == "true") {
            result = ECE141::ModelNode(true);
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

struct GetNumberVisitor {

    //this is variant std::variant<null_obj, bool, long, double, std::string, std::vector<ModelNode>, hashmap>;
    double operator()(const double& aNum) const {
        return aNum;
    }
    double operator()(const long& aNum) const {
        return static_cast<double>(aNum);
    }
    double operator()(null_obj value) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type null_obj is not number value");
    }
    double operator()(bool value) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type bool is not number value");
    }
    double operator()(std::string value) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type string is not number value");
    }
    double operator()(ModelNode::vec_ptr value) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type ModelNode::vec_ptr is not number value");
    }
    double operator()(ModelNode::hashmap value) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type ModelNode::hashmap is not number value");
    }
};

double ModelNode::getNumberValue() {
    return std::visit(GetNumberVisitor{}, aNode);
}


ModelNode& ModelNode::operator=(const ModelNode& aCopy) {
    aNode = aCopy.aNode;
    //std::make_shared<MyClass>(*original);
    if(aCopy.parent_node != nullptr) {
        parent_node = aCopy.parent_node;
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
        ModelNode::hashmap thisHmap = std::get<ModelNode::hashmap>((*current_node)());
        
        if(isAstring(aType)) {
            ModelNode insertStringNode(aValue);
            thisHmap[aKey] = std::make_shared<ModelNode>(insertStringNode);
        }
        else if(isConstant(aType)){
            ModelNode insertConsNode(getVariantNonQuoteType(aValue));
            thisHmap[aKey] = std::make_shared<ModelNode>(insertConsNode);
        }
        else {
            return false;
        }
      
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
        
        ModelNode::vec_ptr current_vec = current_node->getVector();

        current_vec.push_back(std::make_shared<ModelNode>(insert_node));
		return true;
	}



bool Model::openContainer(const std::string& aContainerName, Element aType) {
    
    // Print statement for debugging, remove after implementation
    //		DBG((aContainerName.empty() ? "EMPTY" : aContainerName) << " " << (aType == Element::object ? "{" : "["));
    
    ModelNode::hashmap new_hmap = ModelNode::hashmap(ModelNode::hashmap());
    if(current_node == nullptr) {
        current_node = std::make_shared<ModelNode>(ModelNode(new_hmap));
        root_node = current_node.get();
   
        
        return true;
    }

    if(aType == Element::object) {
       
        current_node = std::make_shared<ModelNode>(ModelNode(new_hmap));
    }
    else if(aType ==  Element::array) {
        //ModelNode(const std::vector<ModelNode> value, std::unique_ptr<ModelNode> parent_node = nullptr): aNode(value)
//        aNewNode = std::make_shared<ModelNode>(ModelNode(new_vec_ptr, current_node));
        ModelNode::vec_ptr new_vec_ptr = ModelNode::vec_ptr(ModelNode::vec_ptr());
        current_node = std::make_shared<ModelNode>(ModelNode(new_vec_ptr, current_node.get()));
    }
    else {
        return false;
    }
    
    ModelNode* parent_node = current_node->parent_node;

    if(aContainerName.empty()) {//inside list
        //addItem(aNewNode, aType);
        parent_node->getVector().push_back(current_node);

    }
    else {//inside object
        if(parent_node != nullptr and !parent_node->isNull()) {
            ModelNode::hashmap& thisHmap = parent_node->getMap();
            thisHmap[aContainerName] = current_node;
        }
       
//        std::cout << aContainerName << std::endl;
    }
 
    return true;
    //to create is object or list
    
}
	bool Model::closeContainer(const std::string& aContainerName, Element aType) {

//		DBG(" " << (aType == Element::object ? "}" : "]"));
        
        if(current_node.get()->isNull()) {
            return false;
        }
        current_node = (*(current_node.get()->parent_node));
		return true;
	}


	// ---ModelQuery---
        
	ModelQuery::ModelQuery(Model &aModel) : model(aModel) {
        if(aModel().isNull()) {
            std::cerr << "The Model is null" << std::endl;
        }
        bool holdObj = aModel().isObj();
        bool holdVec = aModel().isVec();
        if(!(holdObj or holdVec)) {
            std::cerr << "This is not anContainer" << std::endl;
        }
        if(holdVec) {
            for(size_t i = 0; i < aModel().getVector().size(); i++) {
                matching_set_list.insert(i);
            }
        }
        if(holdObj) {
            for (const auto& kVpair : aModel().getMap()) {
                matching_set_obj.insert(kVpair.first);
            }
        }
    }
    
    const char period = '.';

std::string earaseQuote(std::string aString) {
    return aString.substr(1, aString.length() - 2);
}


    
ModelNode* ModelQuery::handleQueryRequest(std::string aQuery, ModelNode* current_mNode) {
    ModelNode* result_node = nullptr;
    if(aQuery[0] == '\'') {
        std::string key = earaseQuote(aQuery);
        ModelNode::hashmap currentMap = current_mNode->getMap();
        auto it = currentMap.find(key);
        if(it == currentMap.end()) {
            throw std::runtime_error("Error occurred: key: " + key + " not found in the currently object");
        }
        else {
            result_node = (it->second).get();
        }
    }
    else {
        size_t index = std::stoi(aQuery);
        ModelNode::vec_ptr aVec = model().getVector();
        if(index < 0 or aVec.size() <= index) {
            throw std::runtime_error("Error occurred: index: " + std::to_string(index) + " is out of bound in the currently object");
        }
        else {
            result_node = aVec[index].get();
        }
        
    }
    return result_node;
}
        
ModelNode::vec_ptr ModelNode::getVector() {
    return std::get<ModelNode::vec_ptr>(aNode);
}

ModelNode::hashmap& ModelNode::getMap() {
    //using myVariant = std::variant<null_obj, bool, long, double, std::string, std::vector<ModelNode>, hashmap>
    
//    if(std::holds_alternative<null_obj>(aNode)) {
//        
//    }
//    if(std::holds_alternative<bool>(aNode)){
//        
//    }
//    if(std::holds_alternative<long>(aNode)){
//        
//    }
//    if(std::holds_alternative<double>(aNode)){
//        
//    }
//    if(std::holds_alternative<std::string>(aNode)){
//        
//    }
//    if(std::holds_alternative<hashmap>(aNode)){
//        
//    }
//    if(std::holds_alternative<std::vector<ModelNode>>(aNode)){
//        
//    }
    
    return std::get<ModelNode::hashmap>(get_variant());
}

bool ModelNode::isObj() {
    return std::holds_alternative<hashmap>(aNode);
}

bool ModelNode::isVec() {
    return std::holds_alternative<ModelNode::vec_ptr>(aNode);
}
        

        
ModelNode Model::operator()(){
    return *current_node;
}
        

	ModelQuery& ModelQuery::select(const std::string& aQuery) {

		DBG("select(" << aQuery << ")");
        
        std:: stringstream sStream(aQuery);
        std::string nextQuery;
        ModelNode* current_node = model.root_node;
        while (std::getline(sStream, nextQuery, period)) {
            current_node = handleQueryRequest(nextQuery, current_node);
        }
        
		return *this;
	}


std::map<std::string, ModelQuery::filterOpt> ModelQuery::handleFilterOpts = {
    
    {"key", [](std::string action, std::string value, ModelQuery& mQuery) {
        // Function for "key" operation
        if(action == "contains") {
            std::unordered_set<std::string> key_set = mQuery.matching_set_obj;
            std::string stringToFind = earaseQuote(value);
            for (std::string aKey:  key_set) {
                if(aKey.find(stringToFind) == std::string::npos) {
                    key_set.erase(aKey);
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
        
        for (size_t i = mQuery.model().getVector().size() - 1; i >= 0; i--) {
            if (!aFilter.apply(i)) {
                mQuery.matching_set_list.erase(i);
            }
        }

    }
 
}};
    
	ModelQuery& ModelQuery::filter(const std::string& aQuery) {
//		DBG("filter(" << aQuery << ")");
        ModelNode::myVariant container = model().get_variant();
        if(!std::holds_alternative<ModelNode::vec_ptr>(container) and !std::holds_alternative<ModelNode::hashmap>(container)) {
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
//		DBG("count()");

		return std::max(matching_set_obj.size(), matching_set_list.size());
	}

	double ModelQuery::sum() {
		DBG("sum()");
        double result = 0;
        std::vector<std::shared_ptr<ModelNode>> holdingVec = model().getVector();
        for(size_t index: matching_set_list) {
            result += holdingVec[index].get()->getNumberValue();
        }
		return result;
	}

	std::optional<std::string> ModelQuery::get(const std::string& aKeyOrIndex) {
		DBG("get(" << aKeyOrIndex << ")");
		TODO;

		return std::nullopt;
	}

} // namespace ECE141
