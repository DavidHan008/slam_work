#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
//#include <Eigen/Core>
//这其中的区别在哪里,一个是有& 一个是没有
void find_kp_match(cv::Mat &image1_,cv::Mat &image2_,std::vector<cv::KeyPoint> &kp1,std::vector<cv::KeyPoint> &kp2,    std::vector<cv::DMatch> &good_match_)
{
    cv::Mat image1(image1_),image2(image2_);
    cv::Ptr<cv::ORB> orb= cv::ORB::create();
    orb->detect(image1,kp1);
    orb->detect(image2,kp2);
    cv::Mat des1,des2;
    orb->compute(image1,kp1,des1);
    orb->compute(image2,kp2,des2);
    //然后进行匹配
    std::vector<cv::DMatch>  match12;
    cv::BFMatcher mater(cv::NORM_HAMMING);
    mater.match(des1,des2,match12);
    for(int i=0;i<kp1.size();i++)
    {
        if(match12[i].distance<=30)
        {
            good_match_.push_back(match12[i]);
        }
    }
    std::cout<<"good_:"<<good_match_.size()<<std::endl;
    /*
    cv::Mat out;
    cv::drawMatches(image1,kp1,image2,kp2,good_match,out);
    cv::imshow("d",out);
    cv::waitKey(0);
    */


}
 int main(int argc, const char** argv) {
    //导入图像
    cv::Mat image1=cv::imread(argv[1]);
    cv::Mat image2=cv::imread(argv[2]);
    std::vector<cv::KeyPoint> kp1,kp2;
    std::vector<cv::DMatch> good_match;
    find_kp_match(image1,image2,kp1,kp2,good_match);
    //std::cout<<"sucess="<<good_match.size()<<std::endl;
    //通过已经好的kp1,kp2,和good match 
    //结合相机的内参,fx,fy,cx,cy,来计算本质矩阵,求解R和t
    //cv::Mat K(520.9, 0, 325.1, 0, 521.0, 249.7, 0, 0, 1);
    //double fx(520.9),fy(521.0),cx(325.1),cy(249.7);
    double fx,fy,cx,cy;
    fx=520.9;fy=521.0;cx=325.1;cy=249.7;
    cv::Point2d principal_point ( cx, cy );	//相机光心, TUM dataset标定值 其实就是cx cy
    cv::Mat essential_matrix;
    //主要的核心,就会通过两个findEssentialMat函数,和recoverPose函数,一个通过输入来求解本质矩阵E,另外一个通过
    //来求recoverpose来从本质矩阵中恢复出r&t
    //需要注意的地方,就是这个points,这两个函数输入的部分,主要是point2d,那么需要对他们进行修改类型

    //-- 把匹配点转换为vector<Point2f>的形式
    std::vector<cv::Point2d> points1;
    std::vector<cv::Point2d> points2;
    //对所有匹配上的点进行遍历
    for ( int i = 0; i < ( int ) good_match.size(); i++ )
    {
        points1.push_back (kp1[good_match[i].queryIdx].pt );//需要匹配的描述子
        points2.push_back (kp2[good_match[i].trainIdx].pt );//被匹配的描述子
    }
    essential_matrix = cv::findEssentialMat ( points1, points2, fx, principal_point );
    std::cout<<"essentail_matix sucess"<<kp2.size()<<std::endl;
    //当你有了本质矩阵,如何求解t & R
    cv::Mat R,t;
    cv::recoverPose ( essential_matrix, points1, points2, R, t, fx, principal_point );
    std::cout<<"R:"<<R<<std::endl;
    std::cout<<"t:"<<t<<std::endl;
    return 0;
}