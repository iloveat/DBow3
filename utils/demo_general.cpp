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


int nfeatures = 400;


//command line parser
class CmdLineParser{int argc; char **argv; public: CmdLineParser(int _argc,char **_argv):argc(_argc),argv(_argv){}  bool operator[] ( string param ) {int idx=-1;  for ( int i=0; i<argc && idx==-1; i++ ) if ( string ( argv[i] ) ==param ) idx=i;    return ( idx!=-1 ) ;    } string operator()(string param,string defvalue="-1"){int idx=-1;    for ( int i=0; i<argc && idx==-1; i++ ) if ( string ( argv[i] ) ==param ) idx=i; if ( idx==-1 ) return defvalue;  else  return ( argv[  idx+1] ); }};


vector<string> readImagePaths(int argc, char **argv, int start){
    vector<string> paths;
    for(int i = start; i < argc; i++) {
		paths.push_back(argv[i]);
	}
    return paths;   
}

vector<cv::Mat> loadFeatures(std::vector<string> path_to_images) throw (std::exception) {
    cv::Ptr<cv::Feature2D> fdetector;
    ORB_SLAM2::ORBextractor ORB_1(nfeatures, 1.2, 8, 20, 7);
    vector<cv::Mat> features;

    cout << "Extracting  features..." << endl;
    for(size_t i = 0; i < path_to_images.size(); ++i) {
		cv::Mat descriptors;
        vector<cv::KeyPoint> keypoints;

        cout<<"reading image: "<<path_to_images[i]<<endl;
        cv::Mat image = cv::imread(path_to_images[i], 0);
		if(image.empty()) throw std::runtime_error("Could not open image"+path_to_images[i]);
        //cvtColor(image, image, COLOR_BGR2GRAY);
        GaussianBlur(image, image, Size(7, 7), 0, 0);
        equalizeHist(image, image);

        cout<<"extracting features"<<endl;
        Mat im_copy = image.clone();
        (ORB_1)(image, im_copy, keypoints, descriptors);
        features.push_back(descriptors);

        drawKeypoints(image, keypoints, image, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    }
    return features;
}

vector<cv::Mat> loadImages(std::vector<string> path_to_images) throw (std::exception) {
    cv::Ptr<cv::Feature2D> fdetector;
    ORB_SLAM2::ORBextractor ORB_1(nfeatures, 1.2, 8, 20, 7);
	vector<cv::Mat> image_list;

    for(size_t i = 0; i < path_to_images.size(); ++i) {
		cv::Mat descriptors;
        vector<cv::KeyPoint> keypoints;
        
        cout<<"reading image: "<<path_to_images[i]<<endl;
        cv::Mat image = cv::imread(path_to_images[i], 0);
		if(image.empty()) throw std::runtime_error("Could not open image"+path_to_images[i]);
        //cvtColor(image, image, COLOR_BGR2GRAY);
        GaussianBlur(image, image, Size(7, 7), 0, 0);
        equalizeHist(image, image);

		cout<<"extracting features"<<endl;
        Mat im_copy = image.clone();
        (ORB_1)(image, im_copy, keypoints, descriptors);

        drawKeypoints(image, keypoints, image, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
		image_list.push_back(image);
    }
    return image_list;
}

void generate_db_file(vector<string> image_names)
{
	vector<cv::Mat> features = loadFeatures(image_names);
	const int k = 18;
	const int L = 6;
	const WeightingType weight = TF_IDF;
	const ScoringType score = L1_NORM;
	DBoW3::Vocabulary voc(k, L, weight, score);
	voc.create(features);

	Database db(voc, false, 0); 
    for(size_t i = 0; i < features.size(); i++)
		db.add(features[i]);
	db.save("small_db.yml.gz");
	cout<<"database file small_db.yml.gz saved"<<endl;
}


int main(int argc,char **argv)
{
	CmdLineParser cml(argc,argv);
	if (cml["-h"] || argc <= 2) {
		cerr<<"Usage: "<<endl;
		cerr<<"       1 image0 image1 ... (generate database file small_db.yml.gz)"<<endl;
		cerr<<"       2 image0 image1 ... (camera preview test with small_db.yml.gz)"<<endl;
		return -1;
	}
	vector<string> image_names = readImagePaths(argc, argv, 2);

	int type = atoi(argv[1]);
	if(type == 1) {
		generate_db_file(image_names);
		return 0;
	}

	// load dictionary from file
	Database db("small_db.yml.gz");
	vector<cv::Mat> all_images = loadImages(image_names);
	VideoCapture cap(0);
	cv::Mat im;

	while(true)
	{
		cap >> im;
		if(im.empty())
		{
		    cout << "Could not open im" << endl;
			return -1;
		}
		imshow("org_rgb", im);

		cvtColor(im, im, COLOR_BGR2GRAY);
		GaussianBlur(im, im, Size(7, 7), 0, 0);
		equalizeHist(im, im);

		vector<cv::KeyPoint> keypoints_test;
		cv::Mat descriptors_test;
		vector<cv::Mat> features_test;
		ORBextractor ORB_test1(nfeatures, 1.2, 8, 20, 7);
		Mat im_copy = im.clone();
		(ORB_test1)(im, im_copy, keypoints_test, descriptors_test);

		drawKeypoints(im, keypoints_test, im, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
		features_test.push_back(descriptors_test);
		
		imshow("keypoints",im);
		waitKey(10);

		QueryResults ret;
		db.query(features_test[0], ret, 3);
		
		int index_max = ret[0].Id;
		float score_max = ret[0].Score;
		cout<<"score_max: "<<score_max<<endl;
		
		if(score_max > 0.085)
		{
		    int ans_idx = index_max % 80;
		    cout << "ans_idx :" << ans_idx << endl;
		    char str_lx[60];
		    sprintf(str_lx, "score: %d", (int)(score_max*100));
			cout<<str_lx<<endl;

			Mat img_src = all_images[ans_idx].clone();
		    cv::putText(img_src, str_lx, cv::Point(100, 100), CV_FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255));
		    imshow("answer", img_src);
		   	waitKey(10);
		}
		else
		{
			cout << "can not recognise the book ...... " << endl;
		}

	}
	
	return 0;
}












