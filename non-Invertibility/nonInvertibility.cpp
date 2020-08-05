//
//  nonInvertibility.cpp
//  
//
//  Created by Jay on 30/07/20.
//

#include "nonInvertibility.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/hal/interface.h>
#include <algorithm>
#include <math.h>
#include <string>
#include <vector>
#include <utility>
#include <filesystem>
#include <limits>

using namespace std;
namespace fs = std::filesystem;


#define MAX_double numeric_limits<double>::infinity()

pair<double, double> m_and_c(pair<double, double> fp, pair<double, double> rp){
    double xp = fp.first;
    double yp = fp.second;

    double xo = rp.first;
    double yo = rp.second;
    if(xp-xo != 0){
        double slope = (yp - yo) / (xp - xo);
        double intercept = (yo*xp - yp*xo) / (xp - xo);
        return make_pair(slope, intercept);
    }
    else return make_pair(MAX_double, MAX_double);
}

double return_y(double x, double slope, double intercept){

    return(slope*x + intercept);
}

pair<int, int> return_quadRoots(double a, double b, double c){
    double root1, root2;

    if((b*b - 4*a*c) < 0){
        cout<<"b^2 - 4ac < 0"<<endl;
    }
    root1 = (-b + sqrt(b*b - 4*a*c)) / 2*a;
    root2 = (-b - sqrt(b*b - 4*a*c)) / 2*a;

    return make_pair(int(abs(root1)), int(abs(root2)));
}

double L2_dist(pair<double, double> p1, pair<double, double> p2){
    double x1, y1, x2, y2;
    x1 = p1.first;
    y1 = p1.second;
    
    x2 = p2.first;
    y2 = p2.second;
    
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


int main(){
    
    string path = "./data";
    vector<string> images_fp, images_rp;
    
    for(const auto & entry : fs::directory_iterator(path)){
        string img_path = entry.path();
        if(img_path.find("f") != string::npos)
            images_fp.push_back(entry.path());
        else
            images_rp.push_back(entry.path());
    }
    cv::Mat fp_orig = cv::imread("./data/fp112.png", cv::IMREAD_GRAYSCALE);
    cv::Mat rp_orig = cv::imread("./data/rp112.png", cv::IMREAD_GRAYSCALE);
    
    int nrows, ncols;
    nrows = fp_orig.rows;
    ncols = fp_orig.cols;
    
    if(nrows % 2 != 0)
        nrows -= nrows;

    cv::Mat fp = cv::Mat(216, 216, CV_8UC(1), cv::Scalar::all(0));
    cv::Mat rp = cv::Mat(216, 216, CV_8UC(1), cv::Scalar::all(0));
    
    for(int i=0; i<216; i++){
        for(int j=0; j<216; j++){
            fp.at<uchar>(i, j) = fp_orig.at<uchar>((nrows-216)/2 - 1 + i, (ncols-216)/2 - 1 + j);
            rp.at<uchar>(i, j) = rp_orig.at<uchar>((nrows-216)/2 - 1 + i, (ncols-216)/2 - 1 + j);
        }
    }
    nrows = fp.rows;
    ncols = fp.cols;
//    cout<<fp<<endl<<rp<<endl;
    
    vector<pair<double, double>> feature_points, random_points;
    
/*
 extracting feature points and random points from the images in the 'data' folder within the current working directory
 */
    
    for(int i=0, j= nrows/2; i<nrows/2 && j<nrows; i++, j++){
        for(int y=0; y<ncols; y++){
            double fp_x = fp.at<uchar>(i, y);
            double fp_y = fp.at<uchar>(j, y);
            
            double rp_x = rp.at<uchar>(i, y);
            double rp_y = rp.at<uchar>(j, y);
            
            feature_points.push_back(make_pair(fp_x, fp_y));
            random_points.push_back(make_pair(rp_x, rp_y));
        }
    }
    
    vector<pair<double, double>> slope_intercept;
    vector<double> distance;
    //the vectors we have obtained from each of the 2 solutions of the quadratic equation
    vector<pair<double, double>> from_root1, from_root2;
    
/*
 Creating Distance vector and slope_and_intercept vector from feature points and random points.
 */

    for(auto i=0; i<feature_points.size(); i++){
//        cout<<random_points.at(i).first<<" : "<<random_points.at(i).second<<endl;
        pair<double, double> s_i;
        //slope and intercept
        s_i = m_and_c(feature_points.at(i), random_points.at(i));
        slope_intercept.push_back(s_i);
        //L2 distance between the points
        double d = L2_dist(feature_points.at(i), random_points.at(i));
        distance.push_back(d);
        
        //obtaining the two roots using the general equation of finding roots in a quadratic function
//        cout<<i<<endl;
//        cout<<"s_i: "<<s_i.first<<" "<<s_i.second<<endl;
        if(s_i.first != MAX_double || s_i.second != MAX_double){
            
            double xo = random_points.at(i).first;
            double yo = random_points.at(i).second;
            double m = s_i.first;
            double c = s_i.second;
            double A = m*m + 1;
            double B = 2*m*c - 2*m*yo - 2*xo;
            double C = yo*yo - 2*c*yo + xo*xo - d*d;
            cout<<i<<endl;
            cout<<"A: "<<A<<" B: "<<B<<" C: "<<C<<endl;
            pair<int, int> roots = return_quadRoots(A, B, C);
            from_root1.push_back(make_pair(roots.first, return_y(roots.first, m, c)));
            from_root2.push_back(make_pair(roots.second, return_y(roots.second, m, c)));
        }
        else{
            from_root1.push_back(make_pair(random_points.at(i).first, random_points.at(i).second-d));
            from_root2.push_back(make_pair(random_points.at(i).first, random_points.at(i).second+d));
        }
    }
    
    
/*
 Converting the vectors into images
 */
    
    cv::Mat image_from_root1, image_from_root2;
    image_from_root1 = cv::Mat(nrows, ncols, CV_8UC(1), cv::Scalar::all(0));
    image_from_root2 = cv::Mat(nrows, ncols, CV_8UC(1), cv::Scalar::all(0));
    
//    cout<<image_from_root1.size()<<" "<<image_from_root1.channels()<<endl;
//    cout<<image_from_root2.size()<<" "<<image_from_root2.channels()<<endl;
    
    for(auto i=0, j=nrows/2; i<nrows/2 && j<nrows; i++, j++){
        for(auto y=0; y<ncols; y++){
//            cout<<from_root1.at(i).first<<endl;
//            cout<<from_root1.at(i).second<<endl;
//            cout<<"-------------------------------"<<endl;
//            cout<<from_root2.at(i).first<<endl;
//            cout<<from_root1.at(i).second<<endl;
//            cout<<"*******************************"<<endl;
            image_from_root1.at<uchar>(i, y) = from_root1.at(i).first;
            image_from_root1.at<uchar>(j, y) = from_root1.at(i).second;
            
            image_from_root2.at<uchar>(i, y) = from_root2.at(i).first;
            image_from_root2.at<uchar>(j, y) = from_root2.at(i).second;
        }
    }
//    cout<<"****************************************";
//    cout<<image_from_root2<<endl;
//    cout<<"*****************************************";
    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", image_from_root1);
    cv::waitKey();
    cout<<"image size: "<<image_from_root1.size()<<endl;
    
    
//    cv::Mat my_photo = cv::imread("/Users/jay.0tist/Downloads/00100sPORTRAIT_00100_BURST20191207170526443_COVER.jpg");
//    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
//    cv::imshow("image", my_photo);
//    cv::waitKey();
    return 0;
}
