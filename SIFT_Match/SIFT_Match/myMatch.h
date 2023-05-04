#pragma once

#include<opencv2\core\core.hpp>
#include<opencv2\features2d\features2d.hpp>

#include<vector>
#include<string>
#include<iostream>

const double dis_ratio1 = 0.75;			//����ںʹν��ھ������ֵ����Ŀǰ�������� dis_ratio = 0.75 ʱ��ȷƥ���������Խ϶�
const double dis_ratio2 = 0.8;
const double dis_ratio3 = 0.9;

const float ransac_error = 1.5;			//ransac�㷨�����ֵ

const double FSC_ratio_low = 0.8;

const double FSC_ratio_up = 1;

const int pointsCount = 9;				// k��ֵ�������ݵ����
const int clusterCount = 3;				// k��ֵ�������ĵĸ���


enum DIS_CRIT { Euclidean = 0, COS };	//�������׼��

using namespace std;
//using namespace cv;


class myMatch
{
public:

	myMatch();
	~myMatch();

	//�ú���������ȷ��ƥ���ԣ������ͼ��֮��ı任��ϵ
	cv::Mat LMS(const cv::Mat& match1_xy, const cv::Mat& match2_xy, string model, float& rmse);

	//�Ľ���LMS��������
	cv::Mat improve_LMS(const cv::Mat& match1_xy, const cv::Mat& match2_xy, string model, float& rmse);

	//�ú���ɾ�������ƥ����
	cv::Mat ransac(const vector<cv::Point2f>& points_1, const vector<cv::Point2f>& points_2, string model, float threshold, vector<bool>& inliers, float& rmse);

	//��������ͼ��
	void mosaic_map(const cv::Mat& image_1, const cv::Mat& image_2, cv::Mat& chessboard_1, cv::Mat& chessboard_2, cv::Mat& mosaic_image, int width);

	//�ú�����������׼���ͼ������ں���Ƕ
	void image_fusion(const cv::Mat& image_1, const cv::Mat& image_2, const cv::Mat T, cv::Mat& fusion_image, cv::Mat& matched_image);

	//�ú������������ӵ�����ںʹν���ƥ��
	void match_des(const cv::Mat& des_1, const cv::Mat& des_2, vector<vector<cv::DMatch>>& dmatchs, DIS_CRIT dis_crite);

	//�����߶�ֱ��ͼ��ROM ֱ��ͼ
	void scale_ROM_Histogram(const vector<cv::DMatch>& matches, float* scale_hist, float* ROM_hist, int n);

	//�ú���ɾ������ƥ���ԣ���������׼
	cv::Mat match(const cv::Mat& image_1, const cv::Mat& image_2, const vector<vector<cv::DMatch>>& dmatchs, vector<cv::KeyPoint> keys_1,
		vector<cv::KeyPoint> keys_2, string model, vector<cv::DMatch>& right_matchs, cv::Mat& matched_line, vector<cv::DMatch>& init_matchs);
};

