#include <iostream>
#include <yaml-cpp/yaml.h> //yaml相关头文件
#include <cxxopts.hpp> //选项相关头文件
#include<glog/logging.h> //log相关的头文件
#include<fstream> //创建文件并且读取&& 写入
#include <dirent.h> //判断路径是否存在
#include<iomanip>//添加std::fill && std setw

//通过这两个案例熟悉嵌套结构体的使用
typedef struct TagPoint 
{
    double x;
    double y;
    double z;
} Tagpoint;

typedef struct AprilTagDateBase 
{
    int32_t id;
    Tagpoint* center;// tag center
    std::vector<Tagpoint> corner;// tag corner --- counter-clockwise from 逆时针                                         ///< lower left, ll->lr->ur->ul
} AprilTagDB;


int main(int argc, char** argv)
{
//首先需要确定的是,标定相机内参,你这边输出是什么?通过yaml输出 .. 然后一样通过一个脚本来启动
//我这边输入的所有的txt的东西,然后每个txt当中有四个角点在空间当中位置
   cxxopts::Options 
   options(argv[0], "d_cal_intrinsic");
        options.add_options()
        ("h,help", "show this message")
        ("i,image_path", "jpg file path", cxxopts::value<std::string>()->default_value("/home/davidhan/holo_work_old/data/calib/camera/~~front_center05/intrinsic/image_in/front_center/"))
        ("d,database", "database file path", cxxopts::value<std::string>()->default_value("/home/davidhan/holo_work_old/data/calib/final/good/"))
        ("o,output_path", "database file path", cxxopts::value<std::string>()->default_value("/home/davidhan/holo_work_old/data/calib/result_davidhan/"))
        ;
        auto result=   options.parse(argc, argv);
        if(result.count("option"))//要检索选项选项，请使用获取选项result.count("option")的次数，然后 使用result["opt"].as<type>(),来获取参数
        {
            std::cout<<"开始检索"<<std::endl;
        }

        //然后创建一个log文件,在某个目录下面 }
        //@1 通过glog创建.log文件 
        std::string log_file =  result["output_path"].as<std::string>()+"/d_cal_intrinsic.log"; //创建
        google::InitGoogleLogging(argv[0]);
        google::SetLogDestination(google::FATAL, "");
        google::SetLogDestination(google::ERROR, "");
        google::SetLogDestination(google::WARNING, "");
        google::SetLogDestination(google::INFO, log_file.c_str());
        LOG(INFO) <<"first code in log";//这就写入的log文件
        
        //@ 2读取文件
        //接下来我不打算注意那种细节
        //关联容器 本质上来说,就是我能够实现,给我一个txt的名称,然后我能够读取里面面的point点的坐标
        //读取到所有的txt
        //    if(!boost::filesystem::exists(file_name)) 也可以判断
        // {
        //     LOG(WARNING)<< "file: "
        //                 << file_name
        //                 << "does not exist";
        //     continue; }
        int start=351;
        int end=948;
     //   std::fstream data_file;

        std::string data_path=result["database"].as<std::string>();
       std::ifstream data_file;//std::ifstream 读取,std::ofstream写入
       std::unordered_map<int32_t,AprilTagDB> apriltags;
        for(int i=start;i<=end;i++)
        {
            //@2.1打开相关的文件读取参数
           std::stringstream ss;
           ss<<std::setfill('0')<< std::setw(4)<<i<<".txt";//setw设置有总共的数据长度,不够的弥补0
           std::string data_txt_path = data_path+ ss.str(); //不能写'/0'
           data_file.open(data_txt_path);
           //实例化一个tag
           AprilTagDB tag;
           //提取每个文件中的数值,然后进行存取
           for(size_t k = 0; k < 4; k++)
           {
               TagPoint Pt;
               data_file>>Pt.x>>Pt.y>>Pt.z;
               tag.corner.emplace_back(Pt);//这个emplace_back是stl库里的东西,跟push_back差不多,后面可以尝试替换一下 
           }
           tag.id=int32_t(i);
           apriltags[i]=tag;//感觉所谓的db数据库,就是将所有码的空间位置都存到一起,然后提取...
        }
     data_file.close();

     // @3 数据的存储
     std::string SaveDataPath=result["output_path"].as<std::string>()+'a';
     std::ofstream os(SaveDataPath.c_str());
     const std::string family_name="tag36h9";
     size_t family_name_length=family_name.length();
     os.write((char*)&family_name_length, sizeof(size_t));
    os.write(family_name.c_str(), family_name_length);
    size_t num = apriltags.size();//返回的是整个db库的大小
    os.write((char*)& num, sizeof(size_t));
    //save tage
    for(auto it=apriltags.cbegin();it!=apriltags.cend();it++)
    {
        os.write((const char*)& it->second.id, sizeof(int32_t));
        os.write((const char*)& it->second.center->x, sizeof(int32_t));
        os.write((const char*)& it->second.center->y, sizeof(int32_t));
        os.write((const char*)& it->second.center->z, sizeof(int32_t));
           for (size_t j = 0; j < 4; j++)
        {
            os.write((const char*)& it->second.corner[j].x,sizeof(int32_t));
            os.write((const char*)& it->second.corner[j].y,sizeof(int32_t));
            os.write((const char*)& it->second.corner[j].z,sizeof(int32_t));
        }
    }
    os.close();
return 0;
}



