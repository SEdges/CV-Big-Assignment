#pragma once

#include<opencv2\core\core.hpp>
#include<opencv2\features2d\features2d.hpp>

#include<vector>
#include<string>
#include<iostream>

const double dis_ratio1 = 0.75;			//最近邻和次近邻距离比阈值，就目前测试来看 dis_ratio = 0.75 时正确匹配的数量相对较多
const double dis_ratio2 = 0.8;
const double dis_ratio3 = 0.9;

const float ransac_error = 1.5;			//ransac算法误差阈值

const double FSC_ratio_low = 0.8;

const double FSC_ratio_up = 1;

const int pointsCount = 9;				// k均值聚类数据点个数
const int clusterCount = 3;				// k均值聚类质心的个数


enum DIS_CRIT { Euclidean = 0, COS };	//距离度量准则

using namespace std;
//using namespace cv;


class myMatch
{
public:

	myMatch();
	~myMatch();

	//该函数根据正确的匹配点对，计算出图像之间的变换关系
	cv::Mat LMS(const cv::Mat& match1_xy, const cv::Mat& match2_xy, string model, float& rmse);

	//改进版LMS超定方程
	cv::Mat improve_LMS(const cv::Mat& match1_xy, const cv::Mat& match2_xy, string model, float& rmse);

	//该函数删除错误的匹配点对
	cv::Mat ransac(const vector<cv::Point2f>& points_1, const vector<cv::Point2f>& points_2, string model, float threshold, vector<bool>& inliers, float& rmse);

	//绘制棋盘图像
	void mosaic_map(const cv::Mat& image_1, const cv::Mat& image_2, cv::Mat& chessboard_1, cv::Mat& chessboard_2, cv::Mat& mosaic_image, int width);

	//该函数把两幅配准后的图像进行融合镶嵌
	void image_fusion(const cv::Mat& image_1, const cv::Mat& image_2, const cv::Mat T, cv::Mat& fusion_image, cv::Mat& matched_image);

	//该函数进行描述子的最近邻和次近邻匹配
	void match_des(const cv::Mat& des_1, const cv::Mat& des_2, vector<vector<cv::DMatch>>& dmatchs, DIS_CRIT dis_crite);

	//建立尺度直方图、ROM 直方图
	void scale_ROM_Histogram(const vector<cv::DMatch>& matches, float* scale_hist, float* ROM_hist, int n);

	//该函数删除错误匹配点对，并进行配准
	cv::Mat match(const cv::Mat& image_1, const cv::Mat& image_2, const vector<vector<cv::DMatch>>& dmatchs, vector<cv::KeyPoint> keys_1,
		vector<cv::KeyPoint> keys_2, string model, vector<cv::DMatch>& right_matchs, cv::Mat& matched_line, vector<cv::DMatch>& init_matchs);
};

