#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main ( int argc, char** argv )
{
   Mat image=imread(argv[1]);
   Mat image2=imread(argv[2]);
   vector<KeyPoint>  kp,kp2;
   Ptr<ORB>  orb=ORB::create();
   orb->detect(image,kp);
   orb->detect(image2,kp2);
   Mat image_show,Descriptor,Descriptor2;
   //drawKeypoints(image,kp,image_show);
   orb->compute(image,kp,Descriptor);
   orb->compute(image2,kp2,Descriptor2);
   vector<DMatch> match12;
   BFMatcher matchter_bf(NORM_HAMMING2);
   matchter_bf.match(Descriptor,Descriptor2,match12);
   std::cout<<"match:size:"<<match12.size()<<endl;
   //选出了500个点
   //然后对这500个点进行优化
  cv::drawMatches(image,kp,image2,kp2,match12,image_show);
  imshow("ddd",image_show);
  //然后进行优化,首先要选择
  //选择的特征点的个数在30~2*min_dist
     vector<DMatch> good_match;
     double min_dist=1000000000;
     std::cout<<kp.size()<<endl;
     std::cout<<Descriptor.rows<<endl;
  for(int i=0;i<kp.size();i++)
  {
      //对内部的数据进行遍历
      if(match12[i].distance<min_dist)
      min_dist=match12[i].distance;
  }
  cout<<"min:"<<min_dist<<endl;
  for(int i=0;i<kp.size();i++)
  {
      if(match12[i].distance< 30)
      {
          good_match.push_back(match12[i]);
      }
  }
  //max函数就是返回比较大的,
  cout<<"good:"<<good_match.size()<<endl;
  cv::drawMatches(image,kp,image2,kp2,good_match,image_show);
  imshow("good",image_show);
  //从500个点,减少到了87个点



   cv::waitKey(0);




    


    //第三部对两张图片特征进行匹配
    
}
