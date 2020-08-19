//
//  brute-force.cpp
//  
//
//  Created by Jay on 17/08/20.
//

#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#include<numeric>
#include<stdlib.h>
#include<fstream>
#include<sstream>
#include<cmath>
#include<stdlib.h>

using namespace std;

double mean(vector<double> v1){
    return (std::accumulate(v1.begin(), v1.end(), 0.0) / v1.size());
}
double var(vector<double> v1){
    double sq_sum = std::inner_product(v1.begin(), v1.end(), v1.begin(), 0.0);
    return( sq_sum / v1.size() - mean(v1)* mean(v1) );
}

double dot_prod(vector<double> v1, vector<double> v2){
    double sum = 0;
    for(auto i=0; i<v1.size(); i++){
        sum += v1[i] * v2[i];
    }
    return sum;
}

double cs(vector<double> v1, vector<double> v2){
    double c_s;
    double xy = dot_prod(v1, v2);
    double xx = sqrt(dot_prod(v1, v1));
    double yy = sqrt(dot_prod(v2, v2));
    c_s = xy / (xx * yy);
    return c_s;
}


int main(){
    
    ifstream unified_train("unified_train.csv");
    ifstream unified_test("unified_test.csv");
    
    string values;
    
    vector<vector<double>> unified_train_mat;
    vector<vector<double>> unified_test_mat;
    
    while(getline(unified_train, values)){
        
        stringstream ss(values);
        vector<double> vec;
        
        for(double i; ss >> i;){
            vec.push_back(double(i));
            if(ss.peek() == ',')
                ss.ignore();
        }
        unified_train_mat.push_back(vec);
    }
    
    while(getline(unified_test, values)){
        
        stringstream ss(values);
        vector<double> vec;
        
        for(double i; ss >> i;){
            vec.push_back(double(i));
            if(ss.peek() == ',')
                ss.ignore();
        }
        unified_test_mat.push_back(vec);
    }
    
    
//    cout<<unified_test_mat.size()<<" "<<unified_test_mat[0].size()<<endl;
//
//    cout<<unified_train_mat.size()<<" "<<unified_train_mat[0].size();
    
//  we will make all the changes in bf_mat
    vector<vector<double>> bf_mat = unified_test_mat;
    
// Number of clients - unified_train_mat.size()
// Template size - unified_train_mat[i].size()
    
    
    int per;
    cout<<"enter the percentage of change you wish to make(0 to 100): ";
    cin>>per;
    
    int num_clients_test = unified_test_mat.size();
    
    double min = unified_test_mat[0][0];
    double max = unified_test_mat[0][0];
    
    for(auto i=0; i<num_clients_test; i++){
        double el = *min_element(unified_test_mat[i].begin(), unified_test_mat[i].end());
        if(min > el)
            min = el;
    }
    
    for(auto i=0; i<num_clients_test; i++){
        double el = *max_element(unified_test_mat[i].begin(), unified_test_mat[i].end());
        if(max < el)
            max = el;
    }
    
    for(auto i=0; i<num_clients_test; i++){
        int temp_size = unified_test_mat[i].size();
        int n_el = per*temp_size / 100;
        for(auto j=0; j<n_el; j++){
            bf_mat[i][j] = rand() % int(max) + int(min);
        }
    }
    
//    for(auto i=0; i<bf_mat.size(); i++){
//        for(auto j=0; j<bf_mat[i].size(); j++){
//            cout<<bf_mat[i][j] - unified_test_mat[i][j]<<" ";
//        }
//    }
    
    ofstream temp("bf_template.csv");
    for(auto i=0; i<bf_mat.size(); i++){
        for(auto j=0; j<bf_mat[i].size() - 1; j++){
            temp << bf_mat[i][j] <<",";
        }
        temp << bf_mat[i][bf_mat.size()-1];
        temp<<"\n";
    }
    
    
    vector<double> g;
    vector<double> im;
    
    int n_sub = 100;
    for(int i=0; i<n_sub; i++){
        for(int j=0; j<n_sub; j++){
            if(i == j){
                g.push_back(std::max(cs(unified_train_mat[i], unified_test_mat[j]), 0.0));
            }
            else{
                im.push_back(std::max(cs(unified_train_mat[i], unified_test_mat[j]), 0.0));
            }
        }
    }
    
    double var_g = var(g);
    double var_im = var(im);
    double avg_g = mean(g);
    double avg_im = mean(im);
    
    // SCORE NORMALIZATION
    double muG = mean(g);
    double sigG = sqrt(var(g));
    
    for(auto i=0; i<g.size(); i++){
        g[i] = (0.5 * ((tanh(0.01 * ((g[i] - muG) / sigG))) + 1));
    }
    for(auto i=0; i<im.size(); i++){
        im[i] = (0.5 * ((tanh(0.01 * ((im[i] - muG) / sigG))) + 1));
    }
    
    double R_avg = mean(im);
    double R_std = sqrt(var(im));
    double R_anchor = R_avg + R_std;

    double Th = R_anchor;
    double mn = *min_element(im.begin(), im.end());
    double mx = *max_element(im.begin(), im.end());
    
    for(auto i=0; i<g.size(); i++){
        if(g[i] <= Th){
            g[i] = (g[i] - mn) / (2*(Th - mn));
        }
        else{
            g[i] = 0.5 + (g[i] - Th) / (mx - Th);
        }
    }
    for(auto i=0; i<im.size(); i++){
        if(im[i] <= Th){
            im[i] = (im[i] - mn) / (2*(Th - mn));
        }
        else{
            im[i] = 0.5 + (im[i] - Th) / (mx - Th);
        }
    }
    
    ofstream temp_g("true_g.csv");
    
       for(auto i=0; i<g.size() - 1; i++){
           temp_g << g[i] << ",";
       }
       temp_g << g[g.size() - 1];
       
       ofstream temp_im("true_im.csv");
       for(auto i=0; i<im.size()-1; i++){
           temp_im << im[i] << ",";
       }
       temp_im << im[im.size() - 1];
    
    return 0;
}
