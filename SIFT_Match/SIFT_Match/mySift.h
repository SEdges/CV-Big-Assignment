#pragma once                    //��ֹͷ�ļ��ظ���������������һ��

#include<iostream>
#include<vector>
#include<opencv2\core\core.hpp>
#include<opencv2\features2d\features2d.hpp>

using namespace std;
//using namespace cv;

/*************************���峣��*****************************/

//��˹�˴�С�ͱ�׼���ϵ��size=2*(GAUSS_KERNEL_RATIO*sigma)+1,��������GAUSS_KERNEL_RATIO=2-3֮��
const double GAUSS_KERNEL_RATIO = 3;

const int MAX_OCTAVES = 8;					//�������������

const float CONTR_THR = 0.04f;				//Ĭ���ǵĶԱȶ���ֵ(D(x))

const float CURV_THR = 10.0f;				//�ؼ�����������ֵ

const float INIT_SIGMA = 0.5f;				//����ͼ��ĳ�ʼ�߶�

const int IMG_BORDER = 2;					//ͼ��߽���ԵĿ�ȣ�Ҳ���Ը�Ϊ 1 

const int MAX_INTERP_STEPS = 5;				//�ؼ��㾫ȷ��ֵ����

const int ORI_HIST_BINS = 36;				//���������㷽��ֱ��ͼ��BINS����

const float ORI_SIG_FCTR = 1.5f;			//����������������ʱ�򣬸�˹���ڵı�׼�����ӣ��߶ȿռ����ӣ�

const float ORI_RADIUS = 3 * ORI_SIG_FCTR;	//����������������ʱ�����ڰ뾶����

const float ORI_PEAK_RATIO = 0.8f;			//����������������ʱ��ֱ��ͼ�ķ�ֵ��

const int DESCR_WIDTH = 4;					//������ֱ��ͼ�������С(4x4)

const int DESCR_HIST_BINS = 8;				//ÿ��������ֱ��ͼ�Ƕȷ����ά��

const float DESCR_MAG_THR = 0.2f;			//�����ӷ�����ֵ

const float DESCR_SCL_FCTR = 3.0f;			//����������ʱ��ÿ������Ĵ�С����

const int SAR_SIFT_GLOH_ANG_GRID = 8;		//GLOH�����ؽǶȷ���ȷ��������

const int SAR_SIFT_DES_ANG_BINS = 8;		//�����ݶȷ�����0-360���ڵȷ��������

const float SAR_SIFT_RADIUS_DES = 12.0f;	//����������뾶		

const int Mmax = 8;							//�����ݶȷ�����0-360���ڵȷ��������

const double T = 100.0;						  //sobel����ȥ���������������ֵ

const float SAR_SIFT_GLOH_RATIO_R1_R2 = 0.73f;//GLOH�����м�Բ�뾶����Բ�뾶֮��

const float SAR_SIFT_GLOH_RATIO_R1_R3 = 0.25f;//GLOH�������ڲ�Բ�뾶����Բ�뾶֮��

#define Feature_Point_Minimum 1500			  //����ͼ����������С����

#define We 0.2

#define Wn 0.5

#define Row_num 3

#define Col_num 3

#define SIFT_FIXPT_SCALE 48					//����⣬����ɲ�ԭ����

/************************sift��*******************************/
class mySift
{

public:
	//Ĭ�Ϲ��캯��
	mySift(int nfeatures = 0, int nOctaveLayers = 3, double contrastThreshold = 0.03,
		double edgeThreshold = 10, double sigma = 1.6, bool double_size = true) :nfeatures(nfeatures),
		nOctaveLayers(nOctaveLayers), contrastThreshold(contrastThreshold),
		edgeThreshold(edgeThreshold), sigma(sigma), double_size(double_size) {}


	//��ó߶ȿռ�ÿ���м����
	int get_nOctave_layers() { return nOctaveLayers; }

	//���ͼ��߶��Ƿ�����һ��
	bool get_double_size() { return double_size; }

	//�������������
	int num_octaves(const cv::Mat& image);

	//���ɸ�˹��������һ�飬��һ��ͼ��
	void create_initial_image(const cv::Mat& image, cv::Mat& init_image);

	//ʹ�� sobel ���Ӵ�����˹��������һ��ͼ���Լ�������������
	void sobel_create_initial_image(const cv::Mat& image, cv::Mat& init_image);

	//������˹������
	void build_gaussian_pyramid(const cv::Mat& init_image, vector<vector<cv::Mat>>& gauss_pyramid, int nOctaves);

	//������˹��ֽ�����
	void build_dog_pyramid(vector<vector<cv::Mat>>& dog_pyramid, const vector<vector<cv::Mat>>& gauss_pyramid);

	//�ú������ɸ�˹��ֽ�����
	void amplit_orient(const cv::Mat& image, vector<cv::Mat>& amplit, vector<cv::Mat>& orient, float scale, int nums);

	//DOG��������������
	void find_scale_space_extrema(const vector<vector<cv::Mat>>& dog_pyr, const vector<vector<cv::Mat>>& gauss_pyr,
		vector<cv::KeyPoint>& keypoints);

	//DOG�������������⣬�����㷽��ϸ����
	void find_scale_space_extrema1(const vector<vector<cv::Mat>>& dog_pyr, vector<vector<cv::Mat>>& gauss_pyr,
		vector<cv::KeyPoint>& keypoints);

	//�����������������
	void calc_sift_descriptors(const vector<vector<cv::Mat>>& gauss_pyr, vector<cv::KeyPoint>& keypoints,
		cv::Mat& descriptors, const vector<cv::Mat>& amplit, const vector<cv::Mat>& orient);

	//�����ռ�߶ȡ���Ҫ��Ϊ�˻�ȡ amplit �� orient ��ʹ�� GLOH �����ӵ�ʱ��ʹ��
	void build_sar_sift_space(const cv::Mat& image, vector<cv::Mat>& sar_harris_fun, vector<cv::Mat>& amplit, vector<cv::Mat>& orient);

	//GLOH ����һ������������
	void calc_gloh_descriptors(const vector<cv::Mat>& amplit, const vector<cv::Mat>& orient, const vector<cv::KeyPoint>& keys, cv::Mat& descriptors);

	//��������
	void detect(const cv::Mat& image, vector<vector<cv::Mat>>& gauss_pyr, vector<vector<cv::Mat>>& dog_pyr, vector<cv::KeyPoint>& keypoints,
		vector<vector<vector<float>>>& all_cell_contrasts,
		vector<vector<float>>& average_contrast, vector<vector<int>>& n_cells, vector<int>& num_cell, vector<vector<int>>& available_n,
		vector<int>& available_num, vector<cv::KeyPoint>& final_keypoints,
		vector<cv::KeyPoint>& Final_keypoints, vector<cv::KeyPoint>& Final_Keypoints);

	//����������
	void comput_des(const vector<vector<cv::Mat>>& gauss_pyr, vector<cv::KeyPoint>& final_keypoints, const vector<cv::Mat>& amplit,
		const vector<cv::Mat>& orient, cv::Mat& descriptors);


private:
	int nfeatures;				//�趨����������ĸ���ֵ,�����ֵ����Ϊ0����Ӱ����
	int nOctaveLayers;			//ÿ��������м����
	double contrastThreshold;	//�Աȶ���ֵ��D(x)��
	double edgeThreshold;		//�������Ե������ֵ
	double sigma;				//��˹�߶ȿռ��ʼ��ĳ߶�
	bool double_size;			//�Ƿ��ϲ���ԭʼͼ��


};//ע��������ķֺ�