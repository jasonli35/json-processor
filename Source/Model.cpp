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

    Model::Model(ModelNode* node_ptr): root_node(node_ptr) {}

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
/**
 * @brief Retrieves a `ModelNode` object representing the non-quoted variant of the given string.
 *
 * This function interprets the input string `aString` and converts it into a corresponding `ModelNode` object.
 * The conversion is based on the content of the string:
 *
 * Note: `isLong` and `null_obj` are presumably external functions or methods used to check if a string is a long integer and to create a null object, respectively.
 *
 * @param aString The string to be converted into a `ModelNode`.
 * @return A `ModelNode` object representing the value of `aString` in its non-quoted form.
 * @throws Standard error output in case of unknown data type.
 */
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

/* * @struct GetNumberVisitor
 *
 * A visitor struct for extracting a numeric value from a myVariant.
 * Usage:
 * myVariant var;
 * GetNumberVisitor visitor;
 * double number = std::visit(visitor, var);
 * */
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

/**
 * @brief Adds a basic key-value pair to the current node in the model.
 *
 * This method is used to add key-value pairs to a map.
 *
 * @param aKey The key as a string.
 * @param aValue The value as a string.
 * @param aType The type of the value, indicated by the Element enum. It can be
 *              Element::quoted for strings, and Element::constant for other types.
 *
 * @return bool Returns true if the key-value pair was successfully added, false otherwise.
 *
 * Usage:
 * - When aType is Element::quoted, aValue is treated as a string and added to the hashmap.
 * - When aType is Element::constant, aValue is parsed into a non-string type (null, bool, number)
 *   based on its content and added to the hashmap.
 *
 * Note:
 * - This method assumes that it's being called within the context of an object container.
 * - The actual parsing and type determination of aValue based on aType is handled internally.
 * - A debug print statement is included for development purposes and should be removed in production.
 *

 */
    bool Model::addKeyValuePair(const std::string& aKey, const std::string& aValue, Element aType) {
		
		// Print statement for debugging, remove after implementation
		DBG("\t'" << aKey << "' : '" << aValue << "'");
        ModelNode::hashmap& thisHmap = current_node->getMap();
        
        if(isAstring(aType)) {
            std::stringstream astream;
            astream << "\"" << aValue << "\"";
            thisHmap[aKey] = std::make_shared<ModelNode>(ModelNode(astream.str()));
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


    /**
 * @brief Adds an item to the current list container in the model.
 *
 * This function is intended to be used when the current context is a list
 * rather than an object in the model.
 *
 * @param aValue The value of the item to be added to the list.
 * @param aType The type of the element, which determines how it is processed and
 *              stored in the model.
 * @return True if the item was successfully added, false otherwise.
 *         The function returns false if the item type is not supported or if
 *         the current context is not a list.
 */
	bool Model::addItem(const std::string& aValue, Element aType) {
	
		// Print statement for debugging, remove after implementation
		DBG("\t'" << aValue << "'");
        ModelNode insert_node;
        if(isAstring(aType)) {
            std::stringstream aStream;
            aStream << "\"" << aValue << "\"";
            insert_node = ModelNode(aStream.str());
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


/**
 * Opens a new container (object or array) in the model.
 *
 * This method is invoked when the parser detects the start of a new container. If the container
 * is an object, it will have an associated key ('aContainerName'), otherwise,
 * for arrays, 'aContainerName' will be an empty string.
 *
 * @param aContainerName The name of the container, empty for arrays.
 * @param aType The type of the container (object or array).
 * @return True if the container is successfully opened, false otherwise.
 */
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

    }
 
    return true;
    //to create is object or list
    
}

/**
 * Closes the current container (object or array) in the model.
 *
 * This method is called when the parser reaches the end of a container.
 * It logs the closing of the container for debugging purposes and then
 * moves the 'current_node' pointer back to the parent node, effectively
 * closing the current container. If 'current_node' is nullptr or represents
 * a null node, the function returns false, indicating an error in closing
 * the container.
 *
 * @param aType The type of the container being closed (object or array).
 * @return True if the container is successfully closed, false otherwise.
 */

	bool Model::closeContainer(const std::string&, Element aType) {

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


    /**
     * handleQueryRequest processes a query to retrieve a specific ModelNode from the current ModelNode's hashmap or vector.
 * @param aQuery A string representing the query, which can be a key (prefixed with a quote) for hashmap retrieval or an index for vector retrieval.
 * @param current_mNode A pointer to the current ModelNode from which the hashmap or vector will be accessed.
 * @return ModelNode* A pointer to the retrieved ModelNode based on the query, or nullptr if the key/index is not found or on error.
 */
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
        if(aVec.size() <= index) {
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

// @param None: This function does not take any parameters.
// @return void: This function does not return any value.
// If current_node is an object, insert keys into matching_set_obj.
// If current_node is a vector, insert indices into matching_set_list.
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

/* @select: Navigates through the JSON structure based on the given query string.
 * @param aQuery: A string representing a path through the JSON structure.
 * Each part of the path is separated by periods ('.').
 * Keys for objects are enclosed in single quotes (''), and indices for arrays are plain numbers.
 * Example: "'firstNode'.'secondNode'.5" navigates to 'firstNode', then its child 'secondNode', and then its 5th element.
 * Invalid keys or indices are handled gracefully without crashing the program.
 * @return ModelQuery&: Returns a reference to the current instance for method chaining.
 */
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

/* handleFilterOpts: Defines filter operations for 'key' and 'index' in a JSON structure.
 * - "key": Filters keys in the current node's object based on the specified action.
 *   If the action is "contains", it retains keys containing the specified value and removes others.
 * - "index": Applies a filter to indices in the current node's vector based on the specified action.
 */
std::map<std::string, ModelQuery::filterOpt> ModelQuery::handleFilterOpts = {
    
    {"key", [](std::string action, std::string value, ModelQuery& mQuery) {
        // Function for "key" operation
        if(action == "contains") {
            std::set<std::string>& key_set = mQuery.matching_set_obj;
            std::string stringToFind = earaseQuote(value);
            for (auto it = key_set.begin(); it != key_set.end(); /* no increment here */) {
                if (it->find(stringToFind) == std::string::npos) {
                    it = key_set.erase(it); // Erase element and update iterator
                } else {
                    ++it; // Increment iterator
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

/* @filter: Applies a filter to elements within the currently selected JSON container (either a list or an object).
 * @param aQuery: A string specifying the filter operation and criteria.
 * - Filtering by key: "filter('key {action} '{value}')".
 *   Action 'contains' checks if the key includes the specified substring.
 *   Example: "filter('key contains 'hello'')" keeps elements whose key contains "hello".
 *   Applies only to key-value pairs within objects.
 * - Filtering by index: "filter('index {comparison} {value}')".
 *   Supports all 6 comparisons (<, >, <=, >=, ==, !=).
 *   Example: "filter('index > 2')" keeps elements with an index greater than 2.
 *   Applies only to elements within lists.
 * Filters do not affect nested child elements, only those in the selected container.
 * @return ModelQuery&: Returns a reference to the current instance for method chaining.
 */
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

/* @sum: Calculates the sum of values in a list of numbers.
 * This function is applicable only when the current node is a list of numbers.
 * The sum operation considers only those elements that pass the active filter criteria.
 * @return double: Returns the sum of the filtered elements in the list.
 * If the current node is not a list, or the list does not contain numbers, the result is undefined.
 */
	double ModelQuery::sum() {
		DBG("sum()");
        double result = 0;
        std::vector<std::shared_ptr<ModelNode>> holdingVec = current_node->getVector();
        for(size_t index: matching_set_list) {
            result += holdingVec[index].get()->getNumberValue();
        }
		return result;
	}



/* getVariantVisitor: A struct providing overloaded operator() for various types to convert them to strings.
* Handles null_obj, bool, long, double, string, vec_ptr (vector), and hashmap (object) types.
 * */
struct getVariantVisistor {
    std::string operator()(null_obj) {
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
    std::string operator()(const ModelNode::vec_ptr) {
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


/* @get: Retrieves values based on a specified key or index from the current JSON node.
 * @param aKeyOrIndex: A string representing either a key (for objects) or an index (for lists).
 * If the argument is "*", it returns all child elements of the current node, applying any active filters.
 * For objects, it returns key-value pairs; for lists, it returns the value at the specified index.
 * @return std::optional<std::string>: Returns the retrieved value(s) as a string.
 * If the key or index is invalid or not found, returns std::nullopt.
 */
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
            if (index < vec.size() && matching_set_list.find(index) != matching_set_list.end()) {
                ss << std::visit(getVariantVisistor{} ,vec[index].get()->get_variant());
            } else {
                return std::nullopt;
            }
        }
    }
    return ss.str();
}

} // namespace ECE141
