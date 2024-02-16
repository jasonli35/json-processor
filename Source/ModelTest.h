//
//  ModelTest.h
//  Assignment_3
//
//  Created by Jason Li on 2024/2/15.
//

#ifndef ModelTest_h
#define ModelTest_h


#endif /* ModelTest_h */
class ModelTest: public ECE141::Testable {
public:
    ModelTest() : Testable() {
        count = kList.size();
    }
    

    using Callable = bool (ModelTest::*)();
    std::map<std::string, Callable> kList {
        {"test model data", &ModelTest::test_model_data},
        
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
    
    //test and validate
    bool test_model_data() {
        const std::string adoub = "1.2";
        double expected = 1.2;
        ECE141::Model::myVariant actual = ECE141::Model::getVariantNonQuoteType(adoub);
        if(expected != std::get<double>(actual)) {
            return false;
        }
        ECE141::Model testModel;
        testModel.data;

        return true;
    }
    
    

};

