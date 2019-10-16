#include <sophus/se3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h> 

// need pangolin for plotting trajectory
#include <pangolin/pangolin.h>

using namespace std;

// path to trajectory file
string trajectory_file = "./trajectory.txt";
string ground_file="./groundtruth.txt";
string estimated_file="./estimated.txt";

// function for plotting trajectory, don't edit this code
// start point is red and end point is blue
void DrawTrajectory(vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>>);
void DrawTrajectory_davidhan(vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>>,vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>>);

void RMSE(vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>>,vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>>);


int main(int argc, char **argv) {

    vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>> poses;
    vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>> poses_g;
    vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>> poses_e;
    /// implement pose reading code
    // start your code here (5~10 lines)
    //给这个pose付数值
    ifstream input(trajectory_file);
    if(!input)
    {
      cout<<"no file"<<endl;
      return 0;
    }
    //再所有的行当中提取
    while(!input.eof())
    {
        //一行一行的提取
        double data[8]={0};
        for(auto& d:data) input>>d;//按行依次去提取数组中的值
        //相当于已经给这个8个数值进行了赋值，都给了data当中，做的事情，就是遍历+赋值
        Eigen::Quaterniond q(data[7], data[4], data[5], data[6]);
        Eigen::Vector3d t(data[1], data[2], data[3]);
        Sophus::SE3 SE3(q,t);
        //使用SE3的话，就是直接将这个SE赋值进去就可以
        poses.push_back(SE3);
    }



    ifstream input_g(ground_file);
    if(!input_g)
    {
      cout<<"no file"<<endl;
      return 0;
    }
    //再所有的行当中提取
    while(!input_g.eof())
    {
        //一行一行的提取
        double data[8]={0};
        for(auto& d:data) input_g>>d;//按行依次去提取数组中的值
        //相当于已经给这个8个数值进行了赋值，都给了data当中，做的事情，就是遍历+赋值
        Eigen::Quaterniond q(data[7], data[4], data[5], data[6]);
        Eigen::Vector3d t(data[1], data[2], data[3]);
        Sophus::SE3 SE3(q,t);
        //使用SE3的话，就是直接将这个SE赋值进去就可以
        poses_g.push_back(SE3);
    }
    //std::cout<<"pose_g:\n"<<poses_g[0]<<endl;


    ifstream input_e(estimated_file);
    if(!input_e)
    {
      cout<<"no file"<<endl;
      return 0;
    }
    //再所有的行当中提取
    while(!input_e.eof())
    {
        //一行一行的提取
        double data[8]={0};
        for(auto& d:data) input_e>>d;//按行依次去提取数组中的值
        //相当于已经给这个8个数值进行了赋值，都给了data当中，做的事情，就是遍历+赋值
        Eigen::Quaterniond q(data[7], data[4], data[5], data[6]);
        Eigen::Vector3d t(data[1], data[2], data[3]);
        Sophus::SE3 SE3(q,t);
        //使用SE3的话，就是直接将这个SE赋值进去就可以
        poses_e.push_back(SE3);
    }
    //std::cout<<"pose_e:\n"<<poses_e[0]<<endl;

    // end your code here

 /*值得关于一下
 #include<iostream>
 using namespace std;
 int main()
 {
    string s("hello world");
    for(auto c:s)
    c='t';
    cout<<s<<endl;//结果为hello world c会改变s的数值
    for(auto &c:s)
    c='t';
    cout<<s<<endl; //结果为ttttttttttt  &c,c会改变s的数值
 */

    // draw trajectory in pangolin
    //DrawTrajectory(poses);
    DrawTrajectory_davidhan(poses_g,poses_e);
    RMSE(poses_g,poses_e);

    return 0;
}

/*******************************************************************************************/
void DrawTrajectory(vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>> poses) {
    if (poses.empty()) {
        cerr << "Trajectory is empty!" << endl;
        return;
    }

    // create pangolin window and plot the trajectory
    pangolin::CreateWindowAndBind("Trajectory Viewer davidhan ", 1024, 768);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pangolin::OpenGlRenderState s_cam(
            pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
            pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
    );

    pangolin::View &d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f / 768.0f)
            .SetHandler(new pangolin::Handler3D(s_cam));


    while (pangolin::ShouldQuit() == false) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        d_cam.Activate(s_cam);
        glClearColor(0.0, .0f, .0f, .0f);//改变背景颜色

        glLineWidth(7);
        for (size_t i = 0; i < poses.size() - 1; i++) {
            glColor3f(0 , 1, 0); //rgb 改变线条的颜色
            glBegin(GL_LINES);
            auto p1 = poses[i], p2 = poses[i + 1];
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
            glEnd();
        }
        pangolin::FinishFrame();
        usleep(5000);   // sleep 5 ms
    }

}


void DrawTrajectory_davidhan(vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>> poses_g,vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>> poses_e) {
    if (poses_e.empty()) {
        cerr << "Trajectory e is empty!" << endl;
        return;
    }
    if (poses_g.empty()) {
        cerr << "Trajectory g is empty!" << endl;
        return;
    }

    // create pangolin window and plot the trajectory
    pangolin::CreateWindowAndBind("Trajectory Viewer davidhan ", 1024, 768);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pangolin::OpenGlRenderState s_cam(
            pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
            pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
    );

    pangolin::View &d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f / 768.0f)
            .SetHandler(new pangolin::Handler3D(s_cam));


    while (pangolin::ShouldQuit() == false) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        d_cam.Activate(s_cam);
        glClearColor(0.0, .0f, .0f, .0f);//改变背景颜色

        glLineWidth(7);
        for (size_t i = 0; i < poses_e.size() - 1; i++) {
            glColor3f(0 , 1, 0); //rgb 改变线条的颜色
            glBegin(GL_LINES);
            auto p1 = poses_e[i], p2 = poses_e[i + 1];
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
            glEnd();
        }
        for (size_t i = 0; i < poses_g.size() - 1; i++) {
            glColor3f(0 , 0, 1); //rgb 改变线条的颜色
            glBegin(GL_LINES);
            auto p1 = poses_g[i], p2 = poses_g[i + 1];
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
            glEnd();
        }
        pangolin::FinishFrame();
        usleep(5000);   // sleep 5 ms
    }

}


void RMSE(vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>> poses_g,vector<Sophus::SE3, Eigen::aligned_allocator<Sophus::SE3>> poses_e) 
{
    // for (size_t i = 0; i < poses_g.size() - 1; i++) {
    //         glColor3f(0 , 0, 1); //rgb 改变线条的颜色
    //         glBegin(GL_LINES);
    //         auto p1 = poses_g[i], p2 = poses_g[i + 1];
    //         glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
    //         glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
    //     }
    std::vector<double> error_sum;
    double error_i_2=0;
    double error_sum_2=0;
    //Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic > se3_gi;
    //Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic > se3_ei;
    //Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic > davidhan_x;


    
    for(size_t i = 0; i < poses_g.size()-1 ; i++)
    {
        auto se3_gi=poses_g[i].matrix();
        //std::cout<<"g:\n"<<se3_gi.matrix()<<endl;;
        //std::cout<<"se3_gi:"<<se3_gi<<endl;
        auto se3_ei=poses_e[i].matrix();
       // std::cout<<"e:\n"<<se3_ei.matrix()<<endl;
        auto davidhan_x=se3_gi.inverse()*se3_ei;
        //std::cout<<"daivdhan_x"<<davidhan_x<<endl;
        //std::cout<<"davidhan_x2:"<<(poses_g[i].matrix().inverse())*poses_e[i].matrix()<<endl;
        
        Eigen::Matrix<double,3,3> R = davidhan_x.topLeftCorner<3,3>();
        Eigen::Matrix<double,3,1> t = davidhan_x.topRightCorner<3,1>();
        Sophus::SE3 davidhan_e(R,t);
        //std::cout<<"se3:\n"<<davidhan_x<<endl;
        error_i_2=davidhan_e.log().squaredNorm();
        //std::cout<<"error_i_2:"<<error_i_2<<endl;
        error_sum_2=error_sum_2+error_i_2;

        // //std::cout<<"se:"<<se3_g<<endl;
        // //std::cout<<"se[0]"<<se3_g[0]<<endl;
        //但是这个公式还是学到不少的
        // Eigen::VectorXd se3_e = poses_e[i].log();//各自对应的李代数 //当前数值对应的李代数 // 观察输出，会发现在Sophus中，se(3)的平移在前，旋转在后.
        // (se3_g[0]-se3_e[0])*(se3_g[0]-se3_e[0])+(se3_g[1]-se3_e[1])*(se3_g[1]-se3_e[1])+(se3_g[2]-se3_e[2])*(se3_g[2]-se3_e[2]);
        // //(se3_g[3]-se3_e[3])*(se3_g[3]-se3_e[3])+(se3_g[4]-se3_e[4])*(se3_g[4]-se3_e[4])+(se3_g[5]-se3_e[5])*(se3_g[5]-se3_e[5]);
        // error_sum.push_back(error_i);
       
        //std::cout<<"error_sum:"<<error_sum_2<<endl;
    }
    // double RMSE_SUM=0;
    // vector<double>::iterator iter = error_sum.begin();
    // //遍历整个数组
	// while (iter != error_sum.end()-1)
    // {
    //     RMSE_SUM += *iter++;
    //    // std::cout<<"SUM:"<< RMSE_SUM<<endl;;
    // }
		

    // double result =sqrt(RMSE_SUM/error_sum.size());
   // std::cout<<"poses_g.size()"<<poses_g.size()<<endl;
    double rmse_result=sqrt(error_sum_2/(poses_g.size()-1));
    // std::cout<<"RMSE:"<<result<<endl;
    std::cout<<"RMSE2:"<<rmse_result<<endl;


 }
  
