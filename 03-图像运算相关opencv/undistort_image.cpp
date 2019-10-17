//
// Created by 高翔 on 2017/12/15.
//

#include <opencv2/opencv.hpp>
#include <string>
#include<ctime>

using namespace std;
using namespace cv;

string image_file = "./test.png";   // 请确保路径正确

int main(int argc, char **argv) {

    // 本程序需要你自己实现去畸变部分的代码。尽管我们可以调用OpenCV的去畸变，但自己实现一遍有助于理解。
    // 畸变参数
    double k1 = -0.28340811, k2 = 0.07395907, p1 = 0.00019359, p2 = 1.76187114e-05;
    // 内参
    double fx = 458.654, fy = 457.296, cx = 367.215, cy = 248.375;

    cv::Mat image = cv::imread(image_file,0);   // 图像是灰度图，CV_8UC1
    int rows = image.rows, cols = image.cols;
    cv::Mat image_undistort = cv::Mat(rows, cols, CV_8UC1);   // 去畸变以后的图
    //方案1 opencv自带的函数
     Mat cameraMatrix = Mat::eye(3, 3, CV_64F);//构建内参矩阵
     cameraMatrix.at<double>(0, 0) = fx;
     cameraMatrix.at<double>(0, 1) = 0;
     cameraMatrix.at<double>(0, 2) = cx;
     cameraMatrix.at<double>(1, 1) = fy;
     cameraMatrix.at<double>(1, 2) = cy;
     cameraMatrix.at<double>(2, 2) = 1;
     Mat coeff = Mat(1, 4, CV_64F);//构建畸变系数
     coeff.at<double>(0, 0) = k1;
     coeff.at<double>(0, 1) = k2;
     coeff.at<double>(0, 2) = p1;
     coeff.at<double>(0, 3) = p2;
     clock_t time_stt=clock();
     undistort(image,image_undistort,cameraMatrix,coeff); //opencv真的强大，只要构建好了方程，给他就行了
     cout <<"time use in normal inverse is " << 1000* (clock() - time_stt)/(double)CLOCKS_PER_SEC << "ms"<< endl;



    //方案2 按照要去写公式
    // 计算去畸变后图像的内容
    //v，x表示行 u表示列


      time_stt=clock();
     for (int v = 0; v < rows; v++)
         for (int u = 0; u < cols; u++) {

             double u_distorted = 0, v_distorted = 0;
             
             // TODO 按照公式，计算点(u,v)对应到畸变图像中的坐标(u_distorted, v_distorted) (~6 lines)
             // start your code here
             //step 1 : 再原始图像中，求出V和U对应的x,y
             double x_orign=(u-cx)/fx;
             double y_orign=(v-cy)/fy;
             double r=sqrt(x_orign*x_orign+y_orign*y_orign);
             double x_corrected=x_orign*(1+k1*r*r+k2*pow(r,4))+2*p1*x_orign*y_orign
             +p2*(r*r+2*x_orign*x_orign);
             double y_corrected=y_orign*(1+k1*r*r+k2*pow(r,4))+p1*(r*r+2*y_orign*y_orign)
             +2*p2*x_orign*y_orign;
             u_distorted=fx*x_corrected+cx;
             v_distorted=fy*y_corrected+cy;
 
             // end your code here

             // 赋值 (最近邻插值)
             if (u_distorted >= 0 && v_distorted >= 0 && u_distorted < cols && v_distorted < rows) {
                 image_undistort.at<uchar>(v, u) = image.at<uchar>((int) v_distorted, (int) u_distorted);
             } else {
                 image_undistort.at<uchar>(v, u) = 0;
             }
         }


         cout <<"time use in normal inverse is " << 1000* (clock() - time_stt)/(double)CLOCKS_PER_SEC << "ms"<< endl;

    // 画图去畸变后图像
   // cv::imshow("frint",image);
    //cv::imshow("image undistorted", image_undistort);
     //至少证明OPENCV 还是很牛的，自己写的算法要700ms,opencv 只用了200ms
    cv::waitKey();

    return 0;
}
