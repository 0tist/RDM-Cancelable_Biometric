#include <iostream>
#include <vector>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include "NumCpp.hpp"
#include "NumCpp/Core/Internal/Error.hpp"
#include "NumCpp/Core/Internal/Filesystem.hpp"
#include "NumCpp/Core/Internal/StaticAsserts.hpp"
#include "NumCpp/Core/Types.hpp"
#include "NumCpp/Functions/linspace.hpp"
#include "NumCpp/Functions/zeros.hpp"
#include "NumCpp/NdArray.hpp"
#include <utility>
//#include "/Users/jay.0tist/Development/matplotlib-cpp/matplotlibcpp.h"
typedef long long ll;
#define pb push_back

using namespace std;

// namespace nc;

nc::NdArray<double> numpy_divide(nc::NdArray<double> in1, nc::NdArray<double> in2){
    vector<double> out;
    if(in1.size() != in2.size())
        return out;
    
    for(auto i=0; i<in1.size(); i++){
        if(in2.at(i) != 0){
            double o = in1.at(i) / in2.at(i);
            out.pb(o);
        }
        else
            out.pb(1);
    }
    nc::NdArray<double> out_ndarray = out;
    return out_ndarray;
}

int main(){

	nc::NdArray<double> mated_scores, nonMated_scores;

	mated_scores = nc::fromfile<double>("./data/Mated_Scores.csv");
    nonMated_scores = nc::fromfile<double>("./data/Non_Mated_Scores.csv");
    
    cout<<"mated_scores shape: "<<mated_scores.shape()<<endl;
    cout<<"nonMated_scores shape: "<<nonMated_scores.shape()<<endl;
    
    int nBins;
    cout<<"enter number of bins: ";
    cin>>nBins;
    if(nBins == -1)
        nBins = std::min<int>((mated_scores.size() / 10), 100);
    
    double start = std::min<double>(*min_element(mated_scores.begin(), mated_scores.end()), *min_element(nonMated_scores.begin(), nonMated_scores.end()));
    double end = std::max<double>(*max_element(mated_scores.begin(), mated_scores.end()), *max_element(nonMated_scores.begin(), nonMated_scores.end()));
    
    nc::NdArray<double> bin_edges;
    bin_edges = nc::linspace<double>(start, end, nBins+1, true);
    
    vector<double> bin_centres_vec;
    for(auto i=0; i<bin_edges.size()-1; i++){
        double sum = bin_edges.at(i) + bin_edges.at(i+1);
        sum = sum / 2;
        bin_centres_vec.pb(sum);
    }
    
    nc::NdArray<double> bin_centres = bin_centres_vec;
    
    pair<nc::NdArray<nc::uint32>, nc::NdArray<double>> Y1, Y2;
    
    Y1 = nc::histogram(mated_scores, nBins - 1);
    Y2 = nc::histogram(nonMated_scores, nBins - 1);
    nc::NdArray<nc::uint32> y1_1, y2_1;
    nc::NdArray<double> y1_2, y2_2;
    
    y1_1 = Y1.first;
    y2_1 = Y2.first;
    
    y1_2 = Y1.second;
    y2_2 = Y2.second;
    
    cout<<endl<<"y1_1 shape: "<<y1_2.shape()<<endl;
    cout<<"y2_1 shape: "<<y1_2.shape()<<endl;
    
    
    cout<<"y1_2: "<<y1_2<<endl;
    cout<<"y2_2: "<<y2_2<<endl;
    nc::NdArray<double> LR = numpy_divide(y1_2, y2_2);
    if(LR.size() == 0){
        cout<<"Error: the size of the 2 input errors isn't same";
        return 0;
    }
    
    int choice;
    double omega;
    cout<<"do u you wish to enter omega ? (ans in 0/1) : ";
    cin>>choice;
    if(choice){
        cout<<"enter Omega: ";
        cin>>omega;
    }
    else{
        omega = 1.0;
    }
    vector<double> D_vec;
    
    cout<<"LR: "<<LR<<endl;
    
    for(auto i=0; i<LR.size(); i++){
        cout<<i<<":";
        D_vec.pb(2*(omega*LR.at(i) / (1 + omega*LR.at(i))) - 1);
    }
//    for(auto i=0; i<LR.size(); i++){
//        if(omega*LR.at(i) <= 1)
//            D_vec.at(i) = 0;
//    }
    for(auto i=0; i<y2_2.size(); i++){
        if(y2_2.at(i) == 0)
            D_vec.at(i) = 1;
    }
    
    cout<<endl<<"processed 0.1"<<endl;
    
    nc::NdArray<double> D = D_vec;
    
    cout<<D.shape()<<endl;
    cout<<y1_1.shape()<<endl;
        
//    vector<double> y1_vec;
    
//    for(auto i=0; i<y1_2.size(); i++){
//        y1_vec.pb(y1_2.at(i));
//    }
    
//    nc::NdArray<double> y1 = y1_vec;
    
    cout<<"mutiply size : "<<nc::multiply<double>(D, y1_2).shape()<<endl;
//    cout<<"bin_centres : "<<bin_centres.shape();
    cout<<"D: "<<D<<endl;
    cout<<"y1_1: "<<y1_2<<endl;
    cout<<"D X y1_1 : "<<nc::multiply<double>(D, y1_2)<<endl<<nc::multiply<double>(D, y1_2).shape();
    
    cout<<"bin_centres shape: "<<bin_centres.shape();
    
    nc::NdArray<double> Dsys = nc::trapz(bin_centres, nc::multiply<double>(D, y1_2));
    cout<<"****************************************************************************"<<endl;
    cout<<"****************************************************************************"<<endl;
    cout<<"Outcome: *"<<Dsys<<"*";
    cout<<"****************************************************************************"<<endl;
    cout<<"****************************************************************************";
    
    return 0;
}
