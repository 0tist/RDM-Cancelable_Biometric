//
//  main.cpp
//  performance_matrix_EER
//
//  Created by Jay on 06/07/20.
//  Copyright © 2020 0tist. All rights reserved.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <fstream>
#include <sstream>
#include <string>
#include "csv.h"
 #include "matplotlibcpp.h"
// #include <boost/python.hpp>
//#include "CSVreader.h"

using namespace std;
namespace plt=matplotlibcpp;
//using namespace matplotlibcpp;

//calculates false acceptance rate
double calculate_far(vector<int> true_labels, vector<double> scores, double threshold){
    double ratio;
    // if -300 is displayed that means the size of the true_labels and the scores(probability) doesn't match.
    double error = -300;
    long size = true_labels.size();
    long count_false = 0;
    long count = 0;
    if(size != scores.size())
        return error;
    // 0 - rejected as a match
    // 1 - accepted match
    for(auto i=0; i<size; i++){
        if(true_labels.at(i) == 1)
            count += 1;
        if(scores.at(i) >= threshold && true_labels.at(i) == 0)
            count_false += 1;
    }
    ratio = double(count_false) / double(count);
//    cout<<"FAR: "<<ratio<<endl;
    return ratio;
}


// calculates false rejection rate
double calculate_frr(vector<int> true_labels, vector<double> scores, double threshold){
    double ratio;
    // if -300 is displayed that means the size of the true_labels and the scores(probability) doesn't match.
    double error = -300;
    long size = true_labels.size();
    long count_false = 0;
    long count = 0;
    if(size != scores.size())
        return error;
    // 0 - rejected as a math
    // 1 - rejected match
    for(auto i=0; i<size; i++){
        if(true_labels.at(i) == 0)
            count += 1;
        if(scores.at(i) < threshold && true_labels.at(i) == 1)
            count_false += 1;
    }
    ratio = double(count_false) / double(count);
//    cout<<"FRR: "<<ratio<<endl;
    return ratio;
}
double get_eer(vector<int> true_labels, vector<double> scores, double step = 0.1){
    double min = 0.00;
    double max = 11.00;
    long size = true_labels.size();
//    vector<double> T;
    // step parameter specifies the increment in threshold
    // 0.01 is the threshold step, that means, the threshold will be increased by 0.01 after every iteration.
    double far, frr, eer = 0.0;
    double val = min - step;
    double diff = abs(calculate_far(true_labels, scores, val) - calculate_frr(true_labels, scores, val));
    while(val <= max + step){
//        cout<<"Threshold: "<<val<<endl;
        far = calculate_far(true_labels, scores, val);
        frr = calculate_frr(true_labels, scores, val);
        if(abs(far - frr) < diff){
            diff = abs(far - frr);
            eer = std::min(far, frr);
        }
        val += step;
    }
//    double diff = abs(calculate_far(true_labels, scores, T.at(0)) - calculate_frr(true_labels, scores, T.at(0)));
    return eer;
}

vector<double> get_impostor(vector<int> true_labels, vector<double> score, double threshold){
    vector<double> impostor;
    long size = true_labels.size();
    for(auto i=0; i<size; i++){
        if(score.at(i) >= threshold && true_labels.at(i) == 0)
            impostor.push_back(score.at(i));
        else if(score.at(i) <  threshold && true_labels.at(i) == 1)
            impostor.push_back(score.at(i));
    }
    
    return impostor;
}

vector<double> get_genuine(vector<int> true_labels, vector<double> scores, double threshold){
    vector<double> genuine;
    long size = true_labels.size();
    for(auto i=0; size; i++){
        if(scores.at(i) >= threshold && true_labels.at(i) == 1)
            genuine.push_back(scores.at(i));
        else if(scores.at(i) < threshold && true_labels.at(i) == 0)
            genuine.push_back(scores.at(i));
    }
    
    return genuine;
}

double get_DI(vector<double> impostor, vector<double> genuine){
    double mean_impostor = std::accumulate(impostor.begin(), impostor.end(), 0.0) / impostor.size();
//    cout<<mean_impostor<<endl;
    double mean_genuine = std::accumulate(impostor.begin(), impostor.end(), 0.0) / genuine.size();
//    cout<<mean_genuine<<endl;
    
    double sq_sum = std::inner_product(impostor.begin(), impostor.end(), impostor.begin(), 0.0);
    double std_impostor = std::sqrt(sq_sum / impostor.size() - mean_impostor * mean_impostor);
//    cout<<endl<<std_impostor<<endl;
    sq_sum = std::inner_product(genuine.begin(), genuine.end(), genuine.begin(), 0.0);
//    cout<<sq_sum<<endl;
    double std_genuine = std::sqrt(std::abs(sq_sum / genuine.size() - mean_genuine * mean_genuine));
//    cout<<sq_sum / genuine.size() -  mean_genuine<<endl;
    double di = std::abs(mean_genuine - mean_impostor) / std::sqrt((std_impostor * std_impostor + std_genuine * std_genuine) / 2);
    
    return di;
}


// source : https://stackoverflow.com/questions/26234673/matlab-api-reading-mat-file-from-c-using-stl-container

//void matread(const char *file, std::vector<double>& v)
//{
//    // open MAT-file
//    MATFile *pmat = matOpen(file, "r");
//    if (pmat == NULL) return;
//
//    // extract the specified variable
//    mxArray *arr = matGetVariable(pmat, "LocalDouble");
//    if (arr != NULL && mxIsDouble(arr) && !mxIsEmpty(arr)) {
//        // copy data
//        mwSize num = mxGetNumberOfElements(arr);
//        double *pr = mxGetPr(arr);
//        if (pr != NULL) {
//            v.reserve(num); //is faster than resize :-)
//            v.assign(pr, pr+num);
//        }
//    }
//
//    // cleanup
//    mxDestroyArray(arr);
//    matClose(pmat);
//}

struct record
{
    double val;
    int label;
};

int main(int argc, const char * argv[]) {
    
//    fstream file;
    io::CSVReader<2> in("data/data_cropped.csv");
    
        in.read_header(io::ignore_extra_column, "scores", "labels");
        vector<double> scores;
        vector<int> labels;
        vector<double> impostor,genuine;
    //    std::string vendor;
        int label; double score;
        while(in.read_row(score, label)){
            scores.push_back(score);
            labels.push_back(label);
        // do stuff with the data
        }
    double eer, DI;
    eer = get_eer(labels, scores);
    for(auto i=0; i<scores.size(); i++){
        if(labels[i] == 0){
            impostor.push_back(scores[i]);
        }
        else{
            genuine.push_back(scores[i]);
        }
    }
    DI = get_DI(impostor, genuine);
    
//    for(auto i=0; i<scores.size(); i++){
//        cout<<scores[i]<<endl;
//    }
//    cout<<scores.size()<<" "<<labels.size()<<endl;
//    cout<<impostor.size()<<" "<<genuine.size()<<endl;
    cout<<"Equal Error rate (pre data): "<<eer<<endl;
    cout<<"Decidability Index (pre data): "<<DI<<endl;
//    vector<double> v;
//    matread("fusion_iqa.mat", v);
//    for (size_t i=0; i<v.size(); ++i)
//        std::cout << v[i] << std::endl;
    
//    double min = scores.at(0);
//    double max = scores.at(0);
//    long size = labels.size();
//    for(auto i=1; i<size; i++){
//        if(scores.at(i) < min)
//            min = scores.at(i);
//        if(scores.at(i) > max)
//            max = scores.at(i);
//    }
//    double step = 0.1;
//    vector<double> far, frr;
//    double val = min - step;
//    while(val <= max + step){
//        far.push_back(calculate_far(labels, scores, val));
//        frr.push_back(calculate_frr(labels, scores, val));
//        val += step;
//    }
//    matplotlibcpp::plot(far, frr);
    
    cout<<endl;
    
    io::CSVReader<2> in_1("data/calculated.csv");
    
        in_1.read_header(io::ignore_extra_column, "scores", "labels");
        vector<double> s;
        vector<int> l;
        vector<double> im,g;
    //    std::string vendor;
        int lbl; double sc;
        while(in_1.read_row(sc, lbl)){
            s.push_back(sc);
            l.push_back(lbl);
        // do stuff with the data
        }
    double EER, di;
    EER = get_eer(l, s);
    for(auto i=0; i<s.size(); i++){
        if(l[i] == 0){
            im.push_back(s[i]);
        }
        else{
            g.push_back(s[i]);
        }
    }
//    for(auto i=0; i<im.size(); i++){
//        cout<<im[i]<<":";
//    }
//    cout<<endl;
//    cout<<"**************************";
//    cout<<endl;
//    for(auto i=0; i<g.size(); i++){
//        cout<<g[i]<<":";
//    }
    di = get_DI(im, g);
    
    cout<<"Equal Error rate (calculated data): "<<EER<<endl;
    cout<<"Decidability Index (calcukated data): "<<di<<endl;
    
//*********************************************************************************************
    vector<double> T, FAR, FRR;
    double start = -11.0;
    double end = 11.0;
    double val = start;
    while(val <= end){
        T.push_back(val);
        FAR.push_back(calculate_far(l, s, val));
        FRR.push_back(calculate_frr(l, s, val));
        val += 0.1;
    }
//*********************************************************************************************
    
    
    io::CSVReader<2> in_2("data/plot.csv");
    in_2.read_header(io::ignore_extra_column, "far", "frr");
    vector<double> far, frr;
    double ar, rr;
    
    while(in_2.read_row(ar, rr)){
        far.push_back(ar);
        frr.push_back(rr);
    }
    
    vector<double> y_i, x_i;
    
    start = 0.0;
    end = 5.0;
    val = start;
    while(val <= end){
        y_i.push_back(val);
        x_i.push_back(val);
        val += 0.1;
    }
    
    
//    plt::figure();
    plt::plot(far, frr);
//    plt::savefig("./figures/roc.png");
    plt::show();
//
////    plt::figure();
//    plt::plot(T, FAR);
//    plt::savefig("./figures/FAR.png");
////    plt::show();
//
////    plt::figure();
//    plt::plot(T, FRR);
//    plt::savefig("./figures/FRR.png");
////    plt::show();
//
////    plt::figure();
//    plt::plot(far);
//    plt::savefig("./figures/far.png");
////    plt::show();
//
////    plt::figure();
//    plt::plot(frr);
//    plt::savefig("./figures/frr.png");
////    plt::show();
    return 0;
}
