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

Model::~Model() {
    if(current_node != nullptr) {
        delete current_node;
        
    }
    if(root_node != nullptr) {
        delete root_node;
    }
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
    double operator()(null_obj) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type null_obj is not number value");
    }
    double operator()(bool) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type bool is not number value");
    }
    double operator()(std::string) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type string is not number value");
    }
    double operator()(ModelNode::vec_ptr) const { //how can this be all datatype
        throw std::runtime_error("An error occurred! Type ModelNode::vec_ptr is not number value");
    }
    double operator()(ModelNode::hashmap) const { //how can this be all datatype
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
		DBG("\t'" << aKey << "' : '" << aValue << "'");
        ModelNode::hashmap& thisHmap = current_node->getMap();
        
        if(isAstring(aType)) {
            ModelNode insertStringNode("\"" + aValue + "\"");
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
		DBG("\t'" << aValue << "'");
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
        
        ModelNode::vec_ptr& current_vec = current_node->getVector();

        current_vec.push_back(std::make_shared<ModelNode>(insert_node));
		return true;
	}



bool Model::openContainer(const std::string& aContainerName, Element aType) {
    
    // Print statement for debugging, remove after implementation
    		DBG((aContainerName.empty() ? "EMPTY" : aContainerName) << " " << (aType == Element::object ? "{" : "["));
    
    ModelNode::hashmap new_hmap = ModelNode::hashmap(ModelNode::hashmap());
    if(current_node == nullptr) {
        current_node = new ModelNode(new_hmap);
        root_node = current_node;
        return true;
    }

    if(aType == Element::object) {
        current_node = new ModelNode(new_hmap, current_node);
    }
    else if(aType ==  Element::array) {
        ModelNode::vec_ptr new_vec_ptr = ModelNode::vec_ptr(ModelNode::vec_ptr());
        current_node = new ModelNode(new_vec_ptr, current_node);
    }
    else {
        return false;
    }
    
    ModelNode* parent_node = current_node->parent_node;
    
    std::shared_ptr<ModelNode> current_ptr = std::shared_ptr<ModelNode>(current_node);
    if(aContainerName.empty()) {//inside list
        //addItem(aNewNode, aType);
        parent_node->getVector().push_back(current_ptr);

    }
    else {//inside object
        if(parent_node != nullptr and !parent_node->isNull()) {
            ModelNode::hashmap& thisHmap = parent_node->getMap();
            thisHmap[aContainerName] = current_ptr;
        }
       
//        std::cout << aContainerName << std::endl;
    }
 
    return true;
    //to create is object or list
    
}
	bool Model::closeContainer(const std::string& aContainerName, Element aType) {

		DBG(" " << (aType == Element::object ? "}" : "]"));
        
        if(current_node == nullptr or current_node->isNull()) {
            return false;
        }
        current_node = current_node->parent_node;
		return true;
	}


	// ---ModelQuery---
        
	ModelQuery::ModelQuery(Model &aModel) : model(aModel) {
        if(model.root_node->isNull()) {
            std::cerr << "The Model is null" << std::endl;
        }
        root = model.root_node;
        current_node = root;
        update_matching();

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
            return nullptr;
        }
        else {
            result_node = (it->second).get();
        }
    }
    else {
        size_t index = std::stoi(aQuery);
        ModelNode::vec_ptr aVec = current_node->getVector();
        if(index < 0 or aVec.size() <= index) {
            return nullptr;
        }
        else {
            result_node = aVec[index].get();
        }
        
    }
    return result_node;
}
        
ModelNode::vec_ptr& ModelNode::getVector() {
    return std::get<ModelNode::vec_ptr>(aNode);
}

ModelNode::hashmap& ModelNode::getMap() {
    
    return std::get<ModelNode::hashmap>(get_variant());
}

bool ModelNode::isObj() {
    return std::holds_alternative<hashmap>(aNode);
}

bool ModelNode::isVec() {
    return std::holds_alternative<ModelNode::vec_ptr>(aNode);
}
        

        
ModelNode& Model::operator()(){
    if(current_node == nullptr) {
        throw std::runtime_error("it is empty");
    }
    return *current_node;
}

    void ModelQuery::update_matching() {
        matching_set_obj.clear();
        matching_set_list.clear();
        if(current_node != nullptr){
            if(current_node->isObj()) {
                for (const auto& kVpair : current_node->getMap()) {
                    matching_set_obj.insert(kVpair.first);
                }
            }
            if(current_node->isVec()) {
                for (size_t i = 0; i < current_node->getVector().size(); ++i) {
                    matching_set_list.insert(i);
                }
            }
        }
    }

	ModelQuery& ModelQuery::select(const std::string& aQuery) {

		DBG("select(" << aQuery << ")");
        
        std:: stringstream sStream(aQuery);
        std::string nextQuery;
        current_node = root;
        while (current_node != nullptr and std::getline(sStream, nextQuery, period)) {
            current_node = handleQueryRequest(nextQuery, current_node);
        }
        update_matching();

		return *this;
	}


std::map<std::string, ModelQuery::filterOpt> ModelQuery::handleFilterOpts = {
    
    {"key", [](std::string action, std::string value, ModelQuery& mQuery) {
        // Function for "key" operation
        if(action == "contains") {
            std::set<std::string> key_set = mQuery.matching_set_obj;
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

        int i = (int) (mQuery.current_node->getVector().size() - 1);
        while (i >= 0) {
            if (!aFilter.apply(i)) {
                mQuery.matching_set_list.erase(i);
            }
            --i;
        }

    }
 
}};
    
	ModelQuery& ModelQuery::filter(const std::string& aQuery) {
		DBG("filter(" << aQuery << ")");
        ModelNode::myVariant container = current_node->get_variant();
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
		DBG("count()");

		return std::max(matching_set_obj.size(), matching_set_list.size());
	}

	double ModelQuery::sum() {
		DBG("sum()");
        double result = 0;
        std::vector<std::shared_ptr<ModelNode>> holdingVec = current_node->getVector();
        for(size_t index: matching_set_list) {
            result += holdingVec[index].get()->getNumberValue();
        }
		return result;
	}
    // using myVariant = std::variant<null_obj, bool, long, double, std::string, vec_ptr, hashmap>;
    struct getVariantVisistor {
        std::string operator()(null_obj value) {
            return std::string("null");
        }
        std::string operator()(bool value) {
            return std::string(value ? "true" : "false");
        }
        std::string operator()(long value) {
            return std::to_string(value);
        }

        std::string operator()(double value) {
            return doubleToString(value) ;
        }

        std::string operator()(const std::string& value) {
            return value;
        }
        std::string operator()(const ModelNode::vec_ptr value) {
            return std::string("vector place holder");
        }
        std::string operator()(const ModelNode::hashmap value) {
            std::stringstream ss;
            ss << "{\"";
            for (const auto& pair : value) {
                ss << pair.first << "\":" <<  std::visit(getVariantVisistor{} ,pair.second.get()->get_variant());
            }
            ss << "}";
            return ss.str();
        }

    };



	std::optional<std::string> ModelQuery::get(const std::string& aKeyOrIndex) {
		DBG("get(" << aKeyOrIndex << ")");
        if (current_node == nullptr) {
            return std::nullopt;
        }

        std::stringstream ss;

        if (current_node->isObj()) { // If current node is an object
            auto& obj = current_node->getMap();
            if (aKeyOrIndex == "*") { // Handle wildcard character
                ss << "{";
                for (const std::string& key : matching_set_obj) {
                    ss << "\"" +  key + "\"" << ": " << std::visit(getVariantVisistor{} , obj.at(key).get()->get_variant()) << ",";
                }
                ss << "}";
                std::string result = ss.str();  // Convert to string
                if (result.size() > 1) {
                    result.erase(result.size() - 2, 1);  // Remove second to last character
                }
                return result;
            } else { // Handle specific key
                std::string string_key = earaseQuote(aKeyOrIndex);
                auto it = matching_set_obj.find(string_key);
                if (it != matching_set_obj.end()) {
                    ss <<  std::visit(getVariantVisistor{}, obj.at(string_key).get()->get_variant());
                } else {
                    return std::nullopt;
                }
            }
        } else if (current_node->isVec()) { // If current node is a list
            auto& vec = current_node->getVector();
            if (aKeyOrIndex == "*") { // Handle wildcard character
                for (const size_t& index : matching_set_list) {
                    ss << std::visit(getVariantVisistor{} , vec[index].get()->get_variant()) << "\n";
                }
            } else { // Handle specific index
                size_t index = std::stoi(aKeyOrIndex);
                if (index >= 0 && index < vec.size() && matching_set_list.find(index) != matching_set_list.end()) {
                    ss << std::visit(getVariantVisistor{} ,vec[index].get()->get_variant());
                } else {
                    return std::nullopt;
                }
            }
        }
        return ss.str();
	}

} // namespace ECE141
