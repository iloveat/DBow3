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
    for(size_t i = 0; i < path_to_images.size(); ++i) {
		cv::Mat descriptors;
        vector<cv::KeyPoint> keypoints;
        cv::Mat image = cv::imread(path_to_images[i], 0);
		if(image.empty()) throw std::runtime_error("could not open image"+path_to_images[i]);
        GaussianBlur(image, image, Size(7, 7), 0, 0);
        equalizeHist(image, image);
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
		if(image.empty()) throw std::runtime_error("could not open image"+path_to_images[i]);
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
	const int k = 12;
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


class BookRecognizer
{
public:
	BookRecognizer() {
		m_db = new Database("small_db.yml.gz");
		m_bshow = false;
		m_thresh = 0.086;
		m_nfeatures = 400;
	}

	~BookRecognizer() {
		if(m_db) {
			delete m_db;
		}
	}

	void set_threshold(float thresh) {
		m_thresh = thresh;
	}

	void set_num_features(int n_feat) {
		m_nfeatures = n_feat;
	}

	void show_result(bool show) {
		m_bshow = show;
	}

	int query_book(Mat &img_3c) {
		QueryResults ret;
		cv::Mat descriptors = this->extract_features(img_3c);
		m_db->query(descriptors, ret, 4);

		float score = ret[0].Score;
		if(m_bshow) {
			//cout<<"score: "<<score<<endl;
			for(int i = 0; i < 4; i++) {
				printf("%.8f ", ret[i].Score);
			}
			cout<<"| ";
		}

		if(score > m_thresh) {
			int ans_idx = ret[0].Id;
			return ans_idx;
		}
		return -1;
	}

private:
	bool m_bshow;
	float m_thresh;
	float m_nfeatures;
	Database *m_db = NULL;

private:
	cv::Mat extract_features(Mat &img_3c) {
		ORBextractor ORB_ext(m_nfeatures, 1.2, 8, 20, 7);
		vector<cv::KeyPoint> keypoints;
		cv::Mat descriptors;
	
		Mat im = img_3c.clone();
		cvtColor(im, im, COLOR_BGR2GRAY);
		GaussianBlur(im, im, Size(7, 7), 0, 0);
		equalizeHist(im, im);

		Mat im_copy = im.clone();
		(ORB_ext)(im, im_copy, keypoints, descriptors);
		if(m_bshow) {
			drawKeypoints(im, keypoints, im, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
			imshow("keypoints", im);
			waitKey(10);
		}
		return descriptors;
	}
};


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

	BookRecognizer recognizer;
	recognizer.show_result(true);

	vector<cv::Mat> all_images = loadImages(image_names);
	VideoCapture cap(0);
	cv::Mat im;

	cout<<endl<<"test dataset"<<endl;
	for(int i = 0; i < image_names.size(); i++) 
	{
		string cur_name = image_names[i];
		cv::Mat im = cv::imread(cur_name, 1);
		if(im.empty()) {
		    cout<<"could not open im"<<endl;
			return -1;
		}

		int ans_idx = recognizer.query_book(im);
		if(ans_idx >= 0) {
			cout<<"ans_idx: "<<ans_idx<<", matched: "<<cur_name<<", "<<image_names[ans_idx]<<endl;
		} else {
			cout<<"xxxxxxx: "<<cur_name<<endl;
		}
	}

	cout<<"press any key to test camera"<<endl;
	waitKey(0);

	cout<<endl<<"test camera"<<endl;
	while(true)
	{
		cap >> im;
		if(im.empty()) {

		    cout << "could not open im" << endl;
			return -1;
		}
		int ans_idx = recognizer.query_book(im);
		if(ans_idx >= 0) {
			Mat img_src = all_images[ans_idx].clone();
		    imshow("answer", img_src);
			cout<<"ans_idx: "<<ans_idx<<endl;
			waitKey(10);
		} else {
			cout<<"..."<<endl;
		}
	}

	return 0;
}












