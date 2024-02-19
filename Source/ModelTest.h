//
//  ModelTest.h
//  Assignment_3
//
//  Created by Jason Li on 2024/2/15.
//

#pragma once
#include "Model.h"

namespace ECE141 {
class ModelTest: public ECE141::Testable {
public:
    ModelTest() : Testable() {
        count = kList.size();
    }
    
    
    using Callable = bool (ModelTest::*)();
    std::map<std::string, Callable> kList {
        {"test model getVariantNonQuoteType double", &ModelTest::test_model_getVariantNonQuoteType_double},
        {"test model getVariantNonQuoteType null", &ModelTest::test_model_getVariantNonQuoteType_null},
        
    };
    OptString getTestName(size_t anIndex) const override {
        size_t thePos{0};
        for(auto thePair: kList) {
            if(anIndex == thePos++) {
                return thePair.first;
            }
        }
        return std::nullopt;
        
    }
    
    bool operator()(const std::string &aName) override {
        return kList.count(aName) ? (this->*kList[aName])() : false;
        
    }
    

    bool test_model_getVariantNonQuoteType_double() {
        std::string adoub = "1.2";
        double expected = 1.2;
        ECE141::Model::myVariant actual = ECE141::Model::getVariantNonQuoteType(adoub);
        if(expected != std::get<double>(actual)) {
            return false;
        }
        
        adoub = "0.0";
        expected = 0.0;
        actual = ECE141::Model::getVariantNonQuoteType(adoub);
        if(expected != std::get<double>(actual)) {
            return false;
        }
        
        adoub = "-3.0";
        expected = -3.0;
        actual = ECE141::Model::getVariantNonQuoteType(adoub);
        if(expected != std::get<double>(actual)) {
            return false;
        }
        
        adoub = "99999999.9999999";
        expected = 99999999.9999999;
        actual = ECE141::Model::getVariantNonQuoteType(adoub);
        if(expected != std::get<double>(actual)) {
            return false;
        }
        
        adoub = "0.000000000000000001";
        expected = 0.000000000000000001;
        actual = ECE141::Model::getVariantNonQuoteType(adoub);
        if(expected != std::get<double>(actual)) {
            return false;
        }
        
        return true;
    }
    
    bool test_model_getVariantNonQuoteType_null() {
        ECE141::Model::myVariant actual = ECE141::Model::getVariantNonQuoteType("null");
        if(!std::holds_alternative<null_obj>(actual)) {
            return false;
        }
        return true;
    }
    
    
    
};
}

