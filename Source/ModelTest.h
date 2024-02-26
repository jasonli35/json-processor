//////
//////  ModelTest.h
//////  Assignment_3
//////
//////  Created by Jason Li on 2024/2/15.
//////
////
////#pragma once
////#include "Model.h"
////#include <iterator>
////#include <string>
////
//namespace ECE141 {
//class ModelTest: public ECE141::Testable {
//public:
//    ModelTest() : Testable() {
//        count = kList.size();
//    }
//
//
//    using Callable = bool (ModelTest::*)();
//    std::map<std::string, Callable> kList {
//        {"test model getVariantNonQuoteType double", &ModelTest::test_model_getVariantNonQuoteType_double},
//        {"test model getVariantNonQuoteType long", &ModelTest::test_model_getVariantNonQuoteType_long},
//        {"test model getVariantNonQuoteType null", &ModelTest::test_model_getVariantNonQuoteType_null},
//        {"test model getVariantNonQuoteType boolean", &ModelTest::test_model_getVariantNonQuoteType_bool},
////        {"test model addKeyValuePair double", &ModelTest::testAddKeyVal_double},
////        {"test model addKeyValuePair long", &ModelTest::testAddKeyVal_long},
////        {"test model addKeyValuePair null", &ModelTest::testAddKeyVal_null},
//
//    };
//    OptString getTestName(size_t anIndex) const override {
//        size_t thePos{0};
//        for(auto thePair: kList) {
//            if(anIndex == thePos++) {
//                return thePair.first;
//            }
//        }
//        return std::nullopt;
//
//    }
//
////
//    bool operator()(const std::string &aName) override {
//        return kList.count(aName) ? (this->*kList[aName])() : false;
//
//    }
//
//    template<typename T, typename Iterator>
//    bool test_nonQuote(Iterator begin, Iterator end) {
//        for (auto it = begin; it != end; ++it) {
//            T expected = *it;
//            ECE141::ModelNode actual = ECE141::Model::getVariantNonQuoteType(std::to_string(expected));
//            if(expected != std::get<T>(actual())) {
//                return false;
//            }
//        }
//        return true;
//    }
//
//    bool test_model_getVariantNonQuoteType_double() {
//        double expected_arr[] = {1.2, 0.0, -3.0, 8786434.999}; //0.000000000000000001, 99999999.9999999
//        return test_nonQuote<double>(std::begin(expected_arr), std::end(expected_arr));
//    }
//
//
//    bool test_model_getVariantNonQuoteType_long() {
//        long long_array[] = {12, 0, 87588866};
//        return test_nonQuote<long>(std::begin(long_array), std::end(long_array));
//
//    }
//
//    bool test_model_getVariantNonQuoteType_bool() {
//        bool bool_array[] = {true, false};
//        return test_nonQuote<long>(std::begin(bool_array), std::end(bool_array));
//
//    }
//    bool test_model_getVariantNonQuoteType_null() {
//        ECE141::ModelNode actual = ECE141::Model::getVariantNonQuoteType("null");
//        if(!std::holds_alternative<null_obj>(actual())) {
//            return false;
//        }
//        return true;
//    }
//
//    struct Entry {
//        const std::string& aKey;
//        const std::string& aValue;
//        const Element aType;
//    };
////
//    ModelNode::hashmap getMapFromVar(ModelNode::myVariant aVar) {
//        return std::get<ModelNode::hashmap>(aVar);
//    }
////
//    template<typename T, typename Iterator>
//    bool test_addKeyValuePair(Iterator begin, Iterator end) {
////        Model aModel;
////
////        for (auto it = begin; it != end; ++it) {
////            Entry thisEntry = *it;
////            const std::string thisKey = thisEntry.aKey;
////            const std::string thisValue = thisEntry.aValue;
////            aModel.addKeyValuePair(thisKey, thisValue, thisEntry.aType);
////            ECE141::ModelNode aNode = *aModel.current_node;
////
////            ModelNode::hashmap ahmap = getMapFromVar(aNode());
////
////            T actual_value = std::get<T>(ahmap.at(thisKey)());
////
////            if(std::to_string(actual_value).substr(0, thisValue.length()).compare(thisValue) != 0) {
////                return false;
////            }
////        }
//        return true;
////    }
//////
////    bool testAddKeyVal_double() {
////        Entry double_array[] = {{"key1", "1.2", Element::constant}, {"key2", "99999999.9", Element::constant}, {"key3", "0.9", Element::constant}, {"key4", "0.0", Element::constant}};
////        return test_addKeyValuePair<double>(std::begin(double_array), std::end(double_array));
////    }
////
////    bool testAddKeyVal_long() {
////        Entry double_array[] = {{"key1", "0", Element::constant}, {"key2", "1", Element::constant}, {"key3", "999999", Element::constant}};
////        return test_addKeyValuePair<long>(std::begin(double_array), std::end(double_array));
////    }
////
////    bool testAddKeyVal_null() {
//////        Model aModel;
//////        aModel.addKeyValuePair("akey", "null",  Element::constant);
//////        ECE141::ModelNode::myVariant root_Var = aModel->current_node.get_variant();
//////
//////        if(!std::holds_alternative<null_obj>(getMapFromVar(root_Var).at("akey").get_variant())) {
//////            return false;
//////        }
////        return true;
////    }
////
//};
//}
