//
//  Filter.h
//  Assignment_3
//
//  Created by Jason Li on 2024/2/20.
//

#ifndef Filter_h
#define Filter_h
namespace ECE141 {

struct Filter {
    size_t compareIndex;
    std::string oper;
    Filter(size_t num, std::string aOperator) {
        compareIndex = num;
        oper = aOperator;
    }
    bool apply(size_t aNum) {
        if (oper == "<") {
            return aNum < compareIndex;
        } else if (oper == "==") {
            return aNum == compareIndex;
        } else if (oper == "<=") {
            return aNum <= compareIndex;
        } else if (oper == ">=") {
            return aNum >= compareIndex;
        } else if (oper == ">") {
            return aNum > compareIndex;
        } else {
            std::cerr << "Error: Unsupported operator" << std::endl;
            return false;
        }
        
        return true;
    }
    
};

}



#endif /* Filter_h */
