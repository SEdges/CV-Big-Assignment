#pragma

#include<iostream>
#include<opencv2\core\core.hpp>
#include<opencv2\features2d\features2d.hpp>
#include<vector>
#include<Windows.h>


//using namespace cv;
using namespace std;

std::string UnicodeToANSI(const std::wstring& wstr);

class myDisplay
{
public:
	myDisplay() {};
	
	void mosaic_pyramid(const vector<vector<cv::Mat>>&pyramid, cv::Mat & pyramid_image, int nOctaceLayers, string str);

	void write_mosaic_pyramid(const vector<vector<cv::Mat>>&gauss_pyr_1, const vector<vector<cv::Mat>>&dog_pyr_1,
	const vector<vector<cv::Mat>>&gauss_pyr_2, const vector<vector<cv::Mat>>&dog_pyr_2, int nOctaveLayers);

		//	//没用到，后文没有对其进行定义
		//	void write_keys_image(vector<cv::KeyPoint>& keypoints_1, vector<cv::KeyPoint>& keypoints_2,
		//		const Mat& image_1, const Mat& image_2, Mat& image_1_keys, Mat& image_2_keys, bool double_size);
		//};

};