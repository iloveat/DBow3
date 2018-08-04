/**
* Date:  2016
* Author: Rafael Muñoz Salinas
* Description: demo application of DBoW3
* License: see the LICENSE.txt file
*/
#include <iostream>
#include <vector>

// DBoW3
#include "DBoW3.h"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#ifdef USE_CONTRIB
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>
#endif
#include "DescManip.h"
#include <map>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <sys/time.h>

#include <fstream>
#include "orb/ORBextractor.h"

using namespace DBoW3;
using namespace std;
using namespace cv;
using namespace ORB_SLAM2;

//vector< cv::Mat  >  loadFeatures( std::vector<string> path_to_images,string descriptor="") ;

int nfeatures = 400;
cv::Mat im;
VideoCapture cap(0);
vector <cv::Mat> all_images;
ofstream myfile("example.txt",ios::out);  //example.txt是你要输出的文件的名字
int index_ = 1;//every book have index_ image


struct timeval tpstart,tpend;
float timeuse;

struct timeval tpstart_database,tpend_database;
float timeuse_database;


//command line parser
class CmdLineParser{int argc; char **argv; public: CmdLineParser(int _argc,char **_argv):argc(_argc),argv(_argv){}  bool operator[] ( string param ) {int idx=-1;  for ( int i=0; i<argc && idx==-1; i++ ) if ( string ( argv[i] ) ==param ) idx=i;    return ( idx!=-1 ) ;    } string operator()(string param,string defvalue="-1"){int idx=-1;    for ( int i=0; i<argc && idx==-1; i++ ) if ( string ( argv[i] ) ==param ) idx=i; if ( idx==-1 ) return defvalue;  else  return ( argv[  idx+1] ); }};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// extended surf gives 128-dimensional vectors
const bool EXTENDED_SURF = false;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void wait()
{
    cout << endl << "Press enter to continue" << endl;
    getchar();
    //loadFeatures();
}

vector<string> readImagePaths(int argc,char **argv,int start){
    vector<string> paths;
    for(int i=start;i<argc;i++)    paths.push_back(argv[i]);
        return paths;
        
}

vector< cv::Mat  >  loadFeatures( std::vector<string> path_to_images,string descriptor="") throw (std::exception){
    //select detector
    cv::Ptr<cv::Feature2D> fdetector;
    //if (descriptor=="orb")
        //fdetector=cv::ORB::create();
    ORB_SLAM2::ORBextractor ORB_1(nfeatures, 1.2,8,20, 7);

    /*
    else if (descriptor=="brisk") fdetector=cv::BRISK::create();
#ifdef OPENCV_VERSION_3
    else if (descriptor=="akaze") fdetector=cv::AKAZE::create();
#endif
#ifdef USE_CONTRIB
    else if(descriptor=="surf" )  fdetector = cv::xfeatures2d::SURF::create(400, 4, 2, EXTENDED_SURF);
#endif*/

    //else throw std::runtime_error("Invalid descriptor");
    assert(!descriptor.empty());
    vector<cv::Mat>    features;

    cout << "Extracting  features..." << endl;
    for(size_t i = 0; i < path_to_images.size(); ++i)
    {
        vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        cout<<"reading image: "<<path_to_images[i]<<endl;
        cv::Mat image = cv::imread(path_to_images[i], 0);
        //cvtColor(image, image, COLOR_BGR2GRAY);

        GaussianBlur(image, image, Size(7, 7), 0, 0);
        equalizeHist(image,image);


        cout<<"the size of all_images is :  "<<all_images.size()<<endl;

        if(image.empty())throw std::runtime_error("Could not open image"+path_to_images[i]);
        cout<<"extracting features"<<endl;
        //fdetector->detectAndCompute(image, cv::Mat(), keypoints, descriptors);

        Mat im_copy = image.clone();
        (ORB_1)(image,im_copy,keypoints,descriptors);
        features.push_back(descriptors);

        if(i%index_ == 0)
        {
                drawKeypoints(image, keypoints,image,Scalar::all(-1), DrawMatchesFlags::DEFAULT);
                all_images.push_back(image);
        }
        cout<<"done detecting features"<<endl;
    }
    return features;
}

// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void creatVoc(const vector<cv::Mat> &features,DBoW3::Vocabulary &voc)
{
    //cout << "Creating a small " << k << "^" << L << " vocabulary..." << endl;
    voc.create(features);
    voc.save("small_voc.yml.gz");
    cout << "... done!" << endl;
}


void testVocCreation(const vector<cv::Mat> &features,DBoW3::Vocabulary &voc,vector <cv::Mat> &all_images , Mat mapx,Mat mapy)
{

    //cout<<"voc size is : "<<voc.size()<<endl;
    map<float,int> img_scores;

    gettimeofday(&tpstart,NULL);
    cap>>im;  //get image from camera
    imshow("org_rgb",im);

    cv::remap(im,im,mapx, mapy, INTER_LINEAR);
    cvtColor(im, im, COLOR_BGR2GRAY);

    equalizeHist(im,im);

    if(im.empty())
        throw std::runtime_error("Could not open im");

    //cv::Ptr<cv::Feature2D> fdetector_test;
    //fdetector_test = cv::ORB::create();
    //fdetector_test= cv::xfeatures2d::SURF::create(400, 4, 2, EXTENDED_SURF);


    vector<cv::Mat>    features_test;
    cout << "Extracting  features..." << endl;
    vector<cv::KeyPoint> keypoints_test;
    cv::Mat descriptors_test;
    //ORB_SLAM2::ORBextractor ORB(1000, 1.2,8,20, 7);

    cout << "Extracting  features........." << endl;
    Mat im_copy1 = im.clone();

    ORB_SLAM2::ORBextractor ORB_test(nfeatures, 1.2,8,20, 7);
    (ORB_test)(im,im_copy1,keypoints_test,descriptors_test);



    cout<<"extracting features"<<endl;
    //fdetector_test->detectAndCompute(im, cv::Mat(), keypoints_test, descriptors_test);


    drawKeypoints(im, keypoints_test,im,Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    features_test.push_back(descriptors_test);

    imshow("org_afterEqual",im);
    waitKey(10);

    // lets do something with this vocabulary
    cout << "Matching images against themselves (0 low, 1 high): " << endl;
    BowVector v1, v2;

    vector<BowVector> features_v;

    for(size_t i = 0; i < features_test.size(); i++)
    {
        cout << "test i is :  "<<i << endl;
        voc.transform(features_test[i], v1);
        myfile<<"v1 is :"<<v1<<endl;
        for(size_t j = 0; j < features.size(); j++)
        {
            voc.transform(features[j], v2);
            features_v.push_back(v2);

            double score = voc.score(v1, v2);
            cout << "Image " << i << " vs Image " << j << ": " << score << endl;

            img_scores[score] = j;
        }
    }

    gettimeofday(&tpend,NULL);
    timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+ tpend.tv_usec-tpstart.tv_usec; //该时间单位为微妙
    timeuse/=1000000; //将微妙的时间换算为妙
    printf("Used Time:%fn",timeuse);

    cout<<"img_scores size is :"<<img_scores.size()<<endl;
    map<float , int>::const_iterator it = img_scores.end();
    map<float , int>::const_iterator it_show = img_scores.end();

    //for (it = img_scores.begin(); it != img_scores.end(); ++it)
    //cout << it->first << "=" << it->second << endl;
    //cout << endl;

    --it;
    cout << it->first << "=" << it->second << endl;  //the last is the max key mean the max score

    float max_score = it->first;
    float distance = it->first - (--it)->first;
    cout<<"distance is :"<<distance<<endl;

    if(distance/max_score>=0.2)
    {
        //cout<<"v2 is : "<<features_v[(--it_show)->second]<<endl;

        myfile<<"v2 is : "<<features_v[(--it_show)->second]<<endl;

        int index_ans = ((--it_show)->second)%80;
        cout<<"index_ans is :"<<index_ans<<endl;
        imshow("answer",all_images[index_ans]);
        waitKey(10);
    }
    else
    {
        cout<<"cannot recognise book ......"<<endl;
    }


    // save the vocabulary to disk
    //cout << endl << "Saving vocabulary..." << endl;
    //voc.save("small_voc.yml.gz");
  // cout << "Done" << endl;
}

////// ----------------------------------------------------------------------------

void testDatabase(const  vector<cv::Mat > &features,DBoW3::Vocabulary &voc,vector <cv::Mat> &all_images , Mat mapx,Mat mapy)
{

    cap>>im;  //get image from camera
    imshow("org_rgb",im);

    Database db(voc, false, 0); // false = do not use direct index
    // (so ignore the last param)
    // The direct index is useful if we want to retrieve the features that
    // belong to some vocabulary node.
    // db creates a copy of the vocabulary, we may get rid of "voc" now

    // add images to the database
    for(size_t i = 0; i < features.size(); i++)
        db.add(features[i]);

    cout << "... done!" << endl;
    cout << "Database information: " << endl << db << endl;

    cv::remap(im,im,mapx, mapy, INTER_LINEAR);
    cvtColor(im, im, COLOR_BGR2GRAY);

    gettimeofday(&tpstart_database,NULL);

    GaussianBlur(im, im, Size(7, 7), 0, 0);

    equalizeHist(im,im);



    if(im.empty())
        throw std::runtime_error("Could not open im");

    //cv::Ptr<cv::Feature2D> fdetector_test;
    //fdetector_test = cv::ORB::create();
    //fdetector_test =  cv::xfeatures2d::SURF::create(400, 4, 2, EXTENDED_SURF);

    vector<cv::Mat>    features_test;
    cout << "Extracting  features..." << endl;
    vector<cv::KeyPoint> keypoints_test;
    cv::Mat descriptors_test;

    cout<<"extracting features"<<endl;
    //fdetector_test->detectAndCompute(im, cv::Mat(), keypoints_test, descriptors_test);


    ORBextractor ORB_test1(nfeatures, 1.2,8,20, 7);
    cout << "Extracting  features........." << endl;
    Mat im_copy = im.clone();

    (ORB_test1)(im,im_copy,keypoints_test,descriptors_test);


    drawKeypoints(im, keypoints_test,im,Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    features_test.push_back(descriptors_test);
    cout<<"features_test size is : "<<features_test.size()<<endl;
    //cout<<"features_test size is : "<<features_test[0]<<endl;

    imshow("keypoints",im);
    waitKey(10);

    cout << "Creating a small database..." << endl;

    // load the vocabulary from disk
    //Vocabulary voc("small_voc.yml.gz");

    // and query the database
    cout << "Querying the database: " << endl;

    QueryResults ret;
    //for(size_t i = 0; i < features.size(); i++)
    {
        cout<<"find the image ..."<<endl;
        //db.query(features_test[0], ret, 4);
        db.query(features_test[0], ret, 4);
        // ret[0] is always the same image in this case, because we added it to the
        // database. ret[1] is the second best match.

        //cout<<"ret size is :"<<ret.size()<<endl;
        cout<< "ret size is :"<<ret.size()<<endl;
        cout << "Searching for Image " << 0 << ". " << ret << endl;
    }

  cout<<"ret[1] id is :"<<ret[0].Id<<endl;

    //int max_score = ret[1];
    float score_max,score_max_2th,score_max_3th,score_total;
    int index_max,index_max_2th,index_max_3th;
    score_total = 0.0;
    for(int k = 0; k < ret.size(); k++)
    {
        score_total +=ret[k].Score;

    }

    score_total = score_total/(int)ret.size();

    score_max = ret[0].Score;
    score_max_2th = ret[1].Score;
    score_max_3th = ret[2].Score;

    index_max = ret[0].Id;
    index_max_2th = ret[1].Id;
    index_max_3th = ret[2].Id;
    float thre = 0.25;//0.25


    //score_max
    //L1 0.035
    //L2 0.03
    //dat 0.02
    cout<<"score_max is :"<<score_max<<endl;
    if(score_max>0.035)//&&(score_max-score_total)/score_max>thre)
    {
        int index_ans = ret[0].Id%80;
        cout<<"index_ans is :"<<index_ans<<endl;
        char str_lx[60];
        int  score =ret[0].Score*10000;
        sprintf(str_lx,"score_%d",score);

        Mat img_src = all_images[index_ans].clone();
        cv::putText(img_src,str_lx,cv::Point(100,100),CV_FONT_HERSHEY_SIMPLEX,1,cv::Scalar(100,100));
        imshow("answer",img_src);
        int key = waitKey(30);

        //if('s'==key)
        {
            //myfile<<ret<<endl;
        }

    }
    else
    {
        cout<<"can not recognise the book ...... " <<endl;
    }

    cout << endl;
    gettimeofday(&tpend_database,NULL);

    timeuse_database=1000000*(tpend_database.tv_sec-tpstart_database.tv_sec)+ tpend_database.tv_usec-tpstart_database.tv_usec; //该时间单位为微妙
    timeuse_database/=1000000; //将微妙的时间换算为妙

      /*cout<<"Used Time is : " <<timeuse_database<<endl;*/

    // we can save the database. The created file includes the vocabulary
    // and the entries added
    /*
    cout << "Saving database..." << endl;
    db.save("small_db.yml.gz");
    cout << "... done!" << endl;

    // once saved, we can load it again
    cout << "Retrieving database once again..." << endl;
    Database db2("small_db.yml.gz");
    cout << "... done! This is: " << endl << db2 << endl;*/
}


// ----------------------------------------------------------------------------


void Init(Mat &mapx,Mat &mapy )
{
    float intrinsic[9] = {421.9990299642099, 0, 329.7548290680634,
                          0, 422.6720358434988, 232.4714058766005,
                          0, 0, 1};
    float distortion[5] = {-0.4133597168995417, 0.2189966631182828, -0.0009415959480780424, -7.895668923449999e-05, -0.06797981130789157};
    Mat intrinsic_matrix = Mat(3,3,CV_32FC1,intrinsic);
    Mat distortion_coeffs = Mat(1,5,CV_32FC1,distortion);
    Mat R = Mat::eye(3,3,CV_32FC1);
    Size image_size = Size(im.size());
    initUndistortRectifyMap(intrinsic_matrix,distortion_coeffs,R,intrinsic_matrix,image_size,CV_32FC1,mapx,mapy);
}

int main(int argc,char **argv)
{



    try{
      CmdLineParser cml(argc,argv);
        if (cml["-h"] || argc<=2){
            cerr<<"Usage:  descriptor_name    image0 image1 ... \n\t descriptors:brisk,surf,orb ,akaze(only if using opencv 3)"<<endl;
            return -1;
        }

        string descriptor=argv[1];

        auto images = readImagePaths(argc,argv,2);

        vector< cv::Mat>  features = loadFeatures(images,descriptor);
        //features_test = loadFeatures(images,descriptor);


        // branching factor and depth levels
        //const int k = 18;
        //const int L = 6;
        const int k = 9;
        const int L = 3;
        const WeightingType weight = TF_IDF;
        const ScoringType score = L1_NORM;
        DBoW3::Vocabulary voc(k, L, weight, score);

        creatVoc(features,voc);
        if(!cap.isOpened())
        {
            cout << "Cannot open video/camera!" << endl;
            return 0;
        }

        cap >> im;
        Mat mapx(im.size(),CV_32FC1);
        Mat mapy(im.size(),CV_32FC1);
        Init(mapx,mapy);

        while(1)
        {
            //testVocCreation(features,voc,all_images,mapx,mapy);
            testDatabase(features,voc,all_images,mapx,mapy);
        }

        cout<<"finish save voc"<<endl;
        myfile.close();


    }catch(std::exception &ex)
    {
        cerr<<ex.what()<<endl;
    }
    return 0;
}
