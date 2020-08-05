//
//  plotting_sample.cpp
//  
//
//  Created by Jay on 18/07/20.
//

#include "plotting_sample.hpp"
#include <iostream>
#include <vector>
#include "matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;

int main(){
    
    vector<double> y_i, x_i;
    
    double start = 0.0;
    double end = 5.0;
    double val = start;
    while(val <= end){
        y_i.push_back(val);
        x_i.push_back(val);
    }
    
    plt::plot(x_i, y_i, "r--");
    plt::show();
    return 0;
}
