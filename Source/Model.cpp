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
		
	}

	Model::Model(const Model& aModel) {
        *this = aModel;
	}

	Model &Model::operator=(const Model& aModel) {
        data = aModel.data;
		return *this;
	}

	ModelQuery Model::createQuery() {
		return ModelQuery(*this);
	}
    
    bool isInt(const std::string &aString) {
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
    ECE141::Model::myVariant ECE141::Model::getVariantNonQuoteType(const std::string &aString) {
        Model::myVariant result;
        if(aString == "true") {
            result = true;
        }
        else if(aString == "false"){
            result = false;
        }
        else if(aString == "null") {
            result = null_obj();
        }
        else {
            if(isInt(aString)) {
                result = stol(aString);
            }
            else {
                double double_result = std::strtod(aString.c_str(), nullptr);
                if(double_result != 0.0 or aString.substr(0,3) == "0.0") {
                    result = double_result;
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
//		DBG("\t'" << aKey << "' : '" << aValue << "'");
        if(aType == Element::quoted) {
            ECE141::Model::myVariant stringVar = aValue;
            data[aKey] = stringVar;
        }
        else if(aType == Element::constant){
            data[aKey] = getVariantNonQuoteType(aValue);
        }
        else {
            throw std::invalid_argument("Received type other than Element::quoted and Element::constant");
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
		TODO;
		// Print statement for debugging, remove after implementation
		DBG((aContainerName.empty() ? "EMPTY" : aContainerName) << " " << (aType == Element::object ? "{" : "["));
		return true;
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
