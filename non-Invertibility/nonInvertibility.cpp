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
    double xp = double(fp.first);
    double yp = double(fp.second);

    double xo = double(rp.first);
    double yo = double(rp.second);
    if(xp-xo != 0){
        double slope = double(yp - yo) / double(xp - xo);
        double intercept = double(yo*xp - yp*xo) / double(xp - xo);
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
    
    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}

pair<double, double> shift_origin(pair<int, int> primary, pair<int, int> secondary){
    double yp = primary.second;
    double xp = primary.first;
    
    double xs = secondary.first;
    double ys = secondary.second;
    
    return make_pair(yp-ys, xp-xs);
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
        nrows--;

    cv::Mat fp = cv::Mat(200, 200, CV_8UC(1), cv::Scalar::all(0));
    cv::Mat rp = cv::Mat(200, 200, CV_8UC(1), cv::Scalar::all(0));
    
    for(int i=0; i<200; i++){
        for(int j=0; j<200; j++){
            fp.at<uchar>(i, j) = fp_orig.at<uchar>((nrows-200)/2 - 1 + i, (ncols-200)/2 - 1 + j);
            rp.at<uchar>(i, j) = rp_orig.at<uchar>((nrows-200)/2 - 1 + i, (ncols-200)/2 - 1 + j);
        }
    }
    nrows = fp.rows;
    ncols = fp.cols;
//    cout<<fp<<endl<<rp<<endl;

//    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
//    cv::imshow("image", fp);
//    cv::waitKey();
//
//    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
//    cv::imshow("image", rp);
//    cv::waitKey();
    
    vector<pair<double, double>> feature_points, random_points;
    vector<pair<double, double>> ONE;
/*
 extracting feature points and random points from the images in the 'data' folder within the current working directory
 */
    
    for(int i=0, j=nrows/2; i<nrows/2 && j<nrows; i++, j++){
        for(int y=0; y<ncols; y++){
            double fp_x = fp.at<uchar>(i, y);
            double fp_y = fp.at<uchar>(j, y);
            
            double rp_x = rp.at<uchar>(i, y);
            double rp_y = rp.at<uchar>(j, y);
            
            feature_points.push_back(make_pair(fp_x, fp_y));
            random_points.push_back(make_pair(rp_x, rp_y));
            ONE.push_back(shift_origin(make_pair(fp_x, fp_y), make_pair(rp_x, rp_y)));
        }
    }
    
    
//    cout<<"feature_points size: "<<feature_points.size()<<endl;
//    cout<<"random_points size: "<<random_points.size()<<endl;
    
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
//        cout<<"slope: "<<s_i.first<<" intercept: "<<s_i.second<<endl;
        slope_intercept.push_back(s_i);
        //L2 distance between the points
        double d_sq = L2_dist(feature_points.at(i), random_points.at(i));
        distance.push_back(d_sq);
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
            double C = yo*yo - 2*c*yo + xo*xo - d_sq;
//            cout<<i<<endl;
//            cout<<"A: "<<A<<" B: "<<B<<" C: "<<C<<endl;
            pair<int, int> roots = return_quadRoots(A, B, C);
            from_root1.push_back(make_pair(roots.first, return_y(roots.first, m, c)));
            from_root2.push_back(make_pair(roots.second, return_y(roots.second, m, c)));
        }
        else{
            from_root1.push_back(make_pair(random_points.at(i).first, random_points.at(i).second-sqrt(d_sq)));
            from_root2.push_back(make_pair(random_points.at(i).first, random_points.at(i).second+sqrt(d_sq)));
        }
    }

    /*
     This is an auxilary technique, which considers sifting of origin, such that the obtained circle is at origin and the line connecting the rp and fp is passing from origin, therefore we don't have to deal with intercept of the line, making the whole equation simpler.
     */
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************

    
    
    vector<pair<double, double>> o_from_root1, o_from_root2;
    
    for(auto i=0; i<ONE.size(); i++){
        double o_dist = ONE.at(i).first * ONE.at(i).first + ONE.at(i).second * ONE.at(i).second;
        double o_slope = ONE.at(i).second / ONE.at(i).first;
        
        double o_root1_x = sqrt(ONE.at(i).first * ONE.at(i).first + ONE.at(i).second * ONE.at(i).second / (ONE.at(i).second / ONE.at(i).first * ONE.at(i).second / ONE.at(i).first + 1));
        double o_root2_x =  - sqrt(ONE.at(i).first * ONE.at(i).first + ONE.at(i).second * ONE.at(i).second / (ONE.at(i).second / ONE.at(i).first * ONE.at(i).second / ONE.at(i).first + 1));
        
        double o_root1_y = ONE.at(i).second / ONE.at(i).first * o_root1_x;
        double o_root2_y = ONE.at(i).second / ONE.at(i).first * o_root2_x;
        
        o_from_root1.push_back(make_pair(o_root1_x, o_root1_y));
        o_from_root2.push_back(make_pair(o_root1_x, o_root2_y));
    }
    
    cv::Mat o_image_from_root1, o_image_from_root2;
    o_image_from_root1 = cv::Mat(nrows, ncols, CV_8UC(1), cv::Scalar::all(0));
    o_image_from_root2 = cv::Mat(nrows, ncols, CV_8UC(1), cv::Scalar::all(0));
    
    for(auto i=0; i<ONE.size(); i++){
        o_image_from_root1.at<uchar>(i/ncols, i%ncols) = o_from_root1.at(i).first;
        o_image_from_root1.at<uchar>(nrows/2 + i/ncols, i%ncols) = o_from_root1.at(i).second;
        
        o_image_from_root2.at<uchar>(i/ncols, i%ncols) = o_from_root2.at(i).first;
        o_image_from_root2.at<uchar>(nrows/2 + i/ncols, i%ncols) = o_from_root2.at(i).second;
    }
    
    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", o_image_from_root1);
    cv::waitKey();

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", o_image_from_root2);
    cv::waitKey();
    
    
    
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
    // *********************************************************************************************************************************
/*
 Converting the vectors into images
 */
    
    cv::Mat image_from_root1, image_from_root2;
    image_from_root1 = cv::Mat(nrows, ncols, CV_8UC(1), cv::Scalar::all(0));
    image_from_root2 = cv::Mat(nrows, ncols, CV_8UC(1), cv::Scalar::all(0));
    
//    cout<<image_from_root1.size()<<" "<<image_from_root1.channels()<<endl;
//    cout<<image_from_root2.size()<<" "<<image_from_root2.channels()<<endl;
    
    for(auto i=0; i<from_root1.size(); i++){
        image_from_root1.at<uchar>(i/ncols, i%ncols) = from_root1.at(i).first;
        image_from_root1.at<uchar>(nrows/2 + i/ncols, i%ncols) = from_root1.at(i).second;
        
        image_from_root2.at<uchar>(i/ncols, i%ncols) = from_root2.at(i).first;
        image_from_root2.at<uchar>(nrows/2 + i/ncols, i%ncols) = from_root2.at(i).second;
    }
    
//    cout<<"****************************************";
//    cout<<image_from_root2<<endl;
//    cout<<"*****************************************";
//    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
//    cv::imshow("image", image_from_root1);
//    cv::waitKey();
//
//    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
//    cv::imshow("image", image_from_root2);
//    cv::waitKey();
    return 0;
}
