//
//  semi_brute.cpp
//  
//
//  Created by Jay on 18/08/20.
//

#include<iostream>
#include<vector>
#include<sstream>
#include<fstream>
#include<string>
#include<algorithm>
#include<cmath>
#include<numeric>

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
    double xx = std::sqrt(dot_prod(v1, v1));
    double yy = std::sqrt(dot_prod(v2, v2));
    c_s = xy / (xx * yy);
    return c_s;
}


int main(){
    
    ifstream unified_train("unified_train.csv");
    ifstream bf_template("bf_template.csv");
    
    string values;
    
    vector<vector<double>> unified_train_mat;
    vector<vector<double>> bf_template_mat;

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
    
    while(getline(bf_template, values)){
        
        stringstream ss(values);
        vector<double> vec;
        
        for(double i; ss >> i;){
            vec.push_back(double(i));
            if(ss.peek() == ',')
                ss.ignore();
        }
        bf_template_mat.push_back(vec);
    }
    
    vector<double> g;
    vector<double> im;
    
    int n_sub = 100;
    for(int i=0; i<n_sub; i++){
        for(int j=0; j<n_sub; j++){
            if(i == j){
                g.push_back(std::max(cs(unified_train_mat[i], bf_template_mat[j]), 0.0));
            }
            else{
                im.push_back(std::max(cs(unified_train_mat[i], bf_template_mat[j]), 0.0));
            }
        }
    }
    
    double var_g = var(g);
    double var_im = var(im);
    double avg_g = mean(g);
    double avg_im = mean(im);
    
    double di = (avg_g - avg_im) / std::sqrt((var_g + var_im) / 2);
    cout<<"decidability: "<<di<<endl;
    
    // SCORE NORMALIZATION
    double muG = mean(g);
    double sigG = std::sqrt(var(g));
    
    for(auto i=0; i<g.size(); i++){
        g[i] = (0.5 * ((tanh(0.01 * ((g[i] - muG) / sigG))) + 1));
    }
    for(auto i=0; i<im.size(); i++){
        im[i] = (0.5 * ((tanh(0.01 * ((im[i] - muG) / sigG))) + 1));
    }
    
    double R_avg = mean(im);
    double R_std = std::sqrt(var(im));
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
    var_g = var(g);
    var_im = var(im);
    avg_g = mean(g);
    avg_im = mean(im);
    
    di = (avg_g - avg_im) / std::sqrt((var_g + var_im) / 2);
    cout<<"new decidability: "<<di<<endl;
    
    ofstream temp_g("bf_g.csv");
    for(auto i=0; i<g.size() - 1; i++){
        temp_g << g[i] << ",";
    }
    temp_g << g[g.size() - 1];
    
    ofstream temp_im("bf_im.csv");
    for(auto i=0; i<im.size()-1; i++){
        temp_im << im[i] << ",";
    }
    temp_im << im[im.size() - 1];
    return 0;
}
