/**
* Date:  2016
* Author: Rafael Muñoz Salinas
* Description: demo application of DBoW3
* License: see the LICENSE.txt file
*/
#include <jni.h>
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
#include "android_nativelog.h"

using namespace DBoW3;
using namespace std;
using namespace cv;
using namespace ORB_SLAM2;


class BookRecognizer
{
public:
	float m_scores[3];

public:
	BookRecognizer(const char *db_path) {
		m_db = new Database(db_path);
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

	int query_book(Mat &img_3c) {
		QueryResults ret;
		cv::Mat descriptors = this->extract_features(img_3c);
		m_db->query(descriptors, ret, 3);

		for(int i = 0; i < 3; i++) {
			m_scores[i] = ret[i].Score;
		}

		float score = ret[0].Score;
		if(score > m_thresh) {
			int ans_idx = ret[0].Id;
			return ans_idx;
		}
		return -1;
	}

private:
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
		return descriptors;
	}
};


BookRecognizer *pRecognizer = NULL;


#ifdef __cplusplus
extern "C" {
#endif

jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    if(vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        LOGE("GetEnv failed!");
        return -1;
    }
    return JNI_VERSION_1_6;
}

jboolean JNIEXPORT JNICALL Java_com_turingapi_turingstory_BookRecognizer_jniInit(JNIEnv* env, jobject thiz, jstring resourcePath)
{
    const char* path = env->GetStringUTFChars(resourcePath, 0);
    if(pRecognizer == NULL)
    {
        pRecognizer = new BookRecognizer(path);
		LOGI("Initialized.");
    }
    env->ReleaseStringUTFChars(resourcePath, path);
    return pRecognizer != NULL;
}

jboolean JNIEXPORT JNICALL Java_com_turingapi_turingstory_BookRecognizer_jniFree(JNIEnv* env, jobject thiz)
{
    if(pRecognizer)
    {
        delete pRecognizer;
        pRecognizer = NULL;
    }
    return pRecognizer == NULL;
}

jint JNIEXPORT JNICALL Java_com_turingapi_turingstory_BookRecognizer_jniQueryBook(JNIEnv* env, jobject thiz, jbyteArray img_data, jint width, jint height)
{
	if(pRecognizer == NULL)
	{
		LOGE("Please call init() first.");
		return -2;
	}
	else
	{
		cv::Mat img_3c;
		jbyte* pbytes = env->GetByteArrayElements(img_data, NULL);
		cv::Mat img_rgba = cv::Mat(height, width, CV_8UC4, (uchar*)pbytes);
		cv::cvtColor(img_rgba, img_3c, CV_RGBA2BGR);
		int ans_idx = pRecognizer->query_book(img_3c);
		env->ReleaseByteArrayElements(img_data, pbytes, JNI_ABORT);
		return ans_idx;
	}
}

jint JNIEXPORT JNICALL Java_com_turingapi_turingstory_BookRecognizer_jniSetThreshold(JNIEnv* env, jobject thiz, jfloatArray params)
{
	if(pRecognizer == NULL)
	{
		LOGE("Please call init() first.");
		return -2;
	}
	else
	{
		jfloat* fData = env->GetFloatArrayElements(params, NULL);
    	float thresh = fData[0];
		env->ReleaseFloatArrayElements(params, fData, JNI_ABORT);
		pRecognizer->set_threshold(thresh);
		return 1;
	}
}

jint JNIEXPORT JNICALL Java_com_turingapi_turingstory_BookRecognizer_jniSetNumFeatures(JNIEnv* env, jobject thiz, jint nfeatures)
{
	if(pRecognizer == NULL)
	{
		LOGE("Please call init() first.");
		return -2;
	}
	else
	{
		pRecognizer->set_num_features(nfeatures);
		return 1;
	}
}

jfloatArray JNIEXPORT JNICALL Java_com_turingapi_turingstory_BookRecognizer_jniGetScores(JNIEnv* env, jobject thiz)
{
	const int len = 3;
	jfloatArray array = env->NewFloatArray(len);

	if(pRecognizer == NULL)
	{
		LOGE("Please call init() first.");
		float scores[3];
		scores[0] = -1;
		scores[1] = -1;
		scores[2] = -1;
		env->SetFloatArrayRegion(array, 0, len, (jfloat*)scores);
		return array;
	}
	else
	{
		env->SetFloatArrayRegion(array, 0, len, (jfloat*)(pRecognizer->m_scores));
		return array;
	}
}


#ifdef __cplusplus
}
#endif










