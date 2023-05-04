#include"mySift.h"

#include<string>
#include<cmath>
#include<iostream>			//�������
#include<vector>			//vector
#include<algorithm>
#include<numeric>			//��������Ԫ�����

#include<opencv2\core\hal\hal.hpp>
#include<opencv2\core\hal\intrin.hpp>

#include<opencv2\opencv.hpp>
#include<opencv2\core\core.hpp>				//opencv�������ݽṹ
#include<opencv2\highgui\highgui.hpp>		//ͼ�����
#include<opencv2\imgproc\imgproc.hpp>		//����ͼ������
#include<opencv2\features2d\features2d.hpp> //������ȡ
#include<opencv2\imgproc\types_c.h>
#include<opencv2\videoio.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;

/******************��������ͼ���С�����˹������������****************************/
/*image��ʾԭʼ����Ҷ�ͼ��,inline��������������������
double_size_image��ʾ�Ƿ��ڹ���������֮ǰ�ϲ���ԭʼͼ��
*/
int mySift::num_octaves(const Mat& image)
{
	int temp;
	float size_temp = (float)min(image.rows, image.cols);
	temp = cvRound(log(size_temp) / log((float)2) - 2);

	if (double_size)
		temp += 1;
	if (temp > MAX_OCTAVES)							//�߶ȿռ������������ΪMAX_OCTAVES
		temp = MAX_OCTAVES;

	return temp;
}


/************************sobel�˲��������˹�߶ȿռ�ͼ���ݶȴ�С*****************************/
void sobelfilter(Mat& image, Mat& G)
{
	// image �Ǿ�����һ��������� (0,1)
	int rows = image.rows;
	int cols = image.cols;

	float dx = 0.0, dy = 0.0;

	//cv::Mat Gx = cv::Mat::zeros(rows, cols, CV_32FC1);	//������ͼ��������ˮƽ�����ϵĵ����Ľ���ֵ��ͼ��
	//cv::Mat Gy = cv::Mat::zeros(rows, cols, CV_32FC1);	//������ͼ�������ڴ�ֱ�����ϵĵ����Ľ���ֵ��ͼ��

	G = Mat::zeros(rows, cols, CV_32FC1);					//��ÿ�����ص㴦�ĻҶȴ�С��Gx��Gy��ͬ����

	double v = 0.0, vx, vy;

	//���� sobel �������ݶȷ���ͼ��
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			v = 0.0;

			if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1)
			{
				G.at<float>(i, j) = 0.0;
			}
			else
			{
				float dx = image.at<float>(i - 1, j + 1) - image.at<float>(i - 1, j - 1)
					+ 2 * image.at<float>(i, j + 1) - 2 * image.at<float>(i, j - 1)
					+ image.at<float>(i + 1, j + 1) - image.at<float>(i + 1, j - 1);

				float dy = image.at<float>(i + 1, j - 1) - image.at<float>(i - 1, j - 1)
					+ 2 * image.at<float>(i + 1, j) - 2 * image.at<float>(i - 1, j) +
					image.at<float>(i + 1, j + 1) - image.at<float>(i - 1, j + 1);

				v = abs(dx) + abs(dy);			//�򻯺� G = |Gx| + |Gy|

				//��֤����ֵ����Ч��Χ��
				v = fmax(v, 0);					//���ظ������нϴ��һ��
				v = fmin(v, 255);				//���ظ������н�С��һ��

				if (v > T)						//TΪ��ֵ����50
					G.at<float>(i, j) = (float)v;
				else
					G.at<float>(i, j) = 0.0;
			}
		}
	}

	//ˮƽ�����ϵĵ����Ľ���ֵ��ͼ��
	/*for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			vx = 0;
			if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1)
				Gx.at<float>(i, j) = 0;
			else
			{
				dx = image.at<float>(i - 1, j + 1) - image.at<float>(i - 1, j - 1)
					+ 2 * image.at<float>(i, j + 1) - 2 * image.at<float>(i, j - 1)
					+ image.at<float>(i + 1, j + 1) - image.at<float>(i + 1, j - 1);
				vx = abs(dx);
				vx = fmax(vx, 0); vx = fmin(vx, 255);
				Gx.at<float>(i, j) = (float)vx;
			}
		}
	}*/

	//��ֱ�����ϵĵ����Ľ���ֵ��ͼ��
	/*for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			vy = 0;
			if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1)
				Gy.at<float>(i, j) = 0;
			else
			{
				dy = image.at<float>(i + 1, j - 1) - image.at<float>(i - 1, j - 1)
					+ 2 * image.at<float>(i + 1, j) - 2 * image.at<float>(i - 1, j) +
					image.at<float>(i + 1, j + 1) - image.at<float>(i - 1, j + 1);
				vy = abs(dy);
				vy = fmax(vy, 0); vx = fmin(vy, 255);
				Gy.at<float>(i, j) = (float)vy;
			}
		}
	}*/

	//cv::imshow("Gx", Gx);						// horizontal
	//cv::imshow("Gy", Gy);						// vertical
	//cv::imshow("G", G);						// gradient
}


/*********�ú������ݳ߶Ⱥʹ��ڰ뾶����ROEWA�˲�ģ��************/
/*size��ʾ�˰뾶����˺˿����2*size+1
 scale��ʾָ��Ȩ�ز���
 kernel��ʾ���ɵ��˲���
 */
static void roewa_kernel(int size, float scale, Mat& kernel)
{
	kernel.create(2 * size + 1, 2 * size + 1, CV_32FC1);
	for (int i = -size; i <= size; ++i)
	{
		float* ptr_k = kernel.ptr<float>(i + size);
		for (int j = -size; j <= size; ++j)
		{
			ptr_k[j + size] = exp(-1.f * (abs(i) + abs(j)) / scale);
		}
	}
}


/************************������˹��������һ�飬��һ��ͼ��************************************/
/*image��ʾ����ԭʼͼ��
 init_image��ʾ���ɵĸ�˹�߶ȿռ�ĵ�һ��ͼ��
 */
void mySift::create_initial_image(const Mat& image, Mat& init_image)
{
	Mat gray_image;

	if (image.channels() != 1)
		cvtColor(image, gray_image, CV_RGB2GRAY);		//ת��Ϊ�Ҷ�ͼ��
	else
		gray_image = image.clone();

	Mat floatImage;										//ת����0-1֮��ĸ����������ݹ�һ��������������Ĵ���

	//float_image=(float)gray_image*(1.0/255.0)
	gray_image.convertTo(floatImage, CV_32FC1, 1.0 / 255.0, 0);

	double sig_diff = 0;

	if (double_size)
	{
		Mat temp_image;

		//ͨ����ֵ�ķ����ı�ͼ��ߴ�Ĵ�С
		resize(floatImage, temp_image, Size(2 * floatImage.cols, 2 * floatImage.rows), 0.0, 0.0, INTER_LINEAR);

		//��˹ƽ���ı�׼�ֵ�ϴ�ʱƽ��Ч���Ƚ�����
		sig_diff = sqrt(sigma * sigma - 4.0 * INIT_SIGMA * INIT_SIGMA);

		//��˹�˲����ڴ�Сѡ�����Ҫ������ѡ��(4*sig_diff_1+1)-(6*sig_diff+1)֮�䣬����������
		int kernel_width = 2 * cvRound(GAUSS_KERNEL_RATIO * sig_diff) + 1;

		Size kernel_size(kernel_width, kernel_width);

		//��ͼ�����ƽ������(��˹ģ��)��������ͼ��ķֱ���,��˹ģ����ʵ�ֳ߶ȱ任��Ψһ�任�ˣ�����ʵΨһ�����Ժ�
		GaussianBlur(temp_image, init_image, kernel_size, sig_diff, sig_diff);
	}
	else
	{
		sig_diff = sqrt(sigma * sigma - 1.0 * INIT_SIGMA * INIT_SIGMA);

		//��˹�˲����ڴ�Сѡ�����Ҫ������ѡ��(4*sig_diff_1+1)-(6*sig_diff+1)֮��
		int kernel_width = 2 * cvRound(GAUSS_KERNEL_RATIO * sig_diff) + 1;

		Size kernel_size(kernel_width, kernel_width);

		GaussianBlur(floatImage, init_image, kernel_size, sig_diff, sig_diff);
	}
}


/************************ʹ�� sobel ���Ӵ�����˹��������һ�飬��һ��ͼ��****************************/
//Ŀ����Ϊ�˼�������������
void mySift::sobel_create_initial_image(const Mat& image, Mat& init_image)
{
	Mat gray_image, gray_images;						//gray_images���ڴ�ž���sobel���Ӳ������ͼ��

	if (image.channels() != 1)
		cvtColor(image, gray_image, CV_RGB2GRAY);		//ת��Ϊ�Ҷ�ͼ��
	else
		gray_image = image.clone();

	sobelfilter(gray_image, gray_images);

	Mat floatImage;										//ת����0-1֮��ĸ����������ݹ�һ��������������Ĵ���

	//float_image=(float)gray_image*(1.0/255.0)
	gray_images.convertTo(floatImage, CV_32FC1, 1.0 / 255.0, 0);

	double sig_diff = 0;

	if (double_size)
	{
		Mat temp_image;

		//ͨ����ֵ�ķ����ı�ͼ��ߴ�Ĵ�С
		resize(floatImage, temp_image, Size(2 * floatImage.cols, 2 * floatImage.rows), 0.0, 0.0, INTER_LINEAR);

		//��˹ƽ���ı�׼�ֵ�ϴ�ʱƽ��Ч���Ƚ�����
		sig_diff = sqrt(sigma * sigma - 4.0 * INIT_SIGMA * INIT_SIGMA);

		//��˹�˲����ڴ�Сѡ�����Ҫ������ѡ��(4*sig_diff_1+1)-(6*sig_diff+1)֮�䣬����������
		int kernel_width = 2 * cvRound(GAUSS_KERNEL_RATIO * sig_diff) + 1;

		Size kernel_size(kernel_width, kernel_width);

		//��ͼ�����ƽ������(��˹ģ��)��������ͼ��ķֱ���,��˹ģ����ʵ�ֳ߶ȱ任��Ψһ�任�ˣ�����ʵΨһ�����Ժ�
		GaussianBlur(temp_image, init_image, kernel_size, sig_diff, sig_diff);
	}
	else
	{
		sig_diff = sqrt(sigma * sigma - 1.0 * INIT_SIGMA * INIT_SIGMA);

		//��˹�˲����ڴ�Сѡ�����Ҫ������ѡ��(4*sig_diff_1+1)-(6*sig_diff+1)֮��
		int kernel_width = 2 * cvRound(GAUSS_KERNEL_RATIO * sig_diff) + 1;

		Size kernel_size(kernel_width, kernel_width);

		GaussianBlur(floatImage, init_image, kernel_size, sig_diff, sig_diff);
	}
}


/**************************���ɸ�˹������*****************************************/
/*init_image��ʾ�Ѿ����ɵĸ�˹��������һ��ͼ��
 gauss_pyramid��ʾ���ɵĸ�˹������
 nOctaves��ʾ��˹������������
*/
void mySift::build_gaussian_pyramid(const Mat& init_image, vector<vector<Mat>>& gauss_pyramid, int nOctaves)
{
	vector<double> sig;

	sig.push_back(sigma);

	double k = pow(2.0, 1.0 / nOctaveLayers);				//��˹������ÿһ���ϵ�� k

	for (int i = 1; i < nOctaveLayers + 3; ++i)
	{
		double prev_sig = pow(k, i - 1) * sigma;				//ÿһ���߶Ȳ�ĳ߶�
		double curr_sig = k * prev_sig;

		//����ÿ��ĳ߶�������㹫ʽ
		sig.push_back(sqrt(curr_sig * curr_sig - prev_sig * prev_sig));
	}

	gauss_pyramid.resize(nOctaves);

	for (int i = 0; i < nOctaves; ++i)
	{
		gauss_pyramid[i].resize(nOctaveLayers + 3);
	}

	for (int i = 0; i < nOctaves; ++i)						//����ÿһ��
	{
		for (int j = 0; j < nOctaveLayers + 3; ++j)			//�������ڵ�ÿһ��
		{
			if (i == 0 && j == 0)							//��һ�飬��һ��
				gauss_pyramid[0][0] = init_image;

			else if (j == 0)
			{
				resize(gauss_pyramid[i - 1][3], gauss_pyramid[i][0],
					Size(gauss_pyramid[i - 1][3].cols / 2,
						gauss_pyramid[i - 1][3].rows / 2), 0, 0, INTER_LINEAR);
			}
			else
			{
				//��˹�˲����ڴ�Сѡ�����Ҫ������ѡ��(4*sig_diff_1+1)-(6*sig_diff+1)֮��
				int kernel_width = 2 * cvRound(GAUSS_KERNEL_RATIO * sig[j]) + 1;
				Size kernel_size(kernel_width, kernel_width);

				GaussianBlur(gauss_pyramid[i][j - 1], gauss_pyramid[i][j], kernel_size, sig[j], sig[j]);
			}
		}
	}
}


/*******************���ɸ�˹��ֽ���������LOG������*************************/
/*dog_pyramid��ʾDOG������
 gauss_pyramin��ʾ��˹������*/
void mySift::build_dog_pyramid(vector<vector<Mat>>& dog_pyramid, const vector<vector<Mat>>& gauss_pyramid)
{
	vector<vector<Mat>>::size_type nOctaves = gauss_pyramid.size();

	for (vector<vector<Mat>>::size_type i = 0; i < nOctaves; ++i)
	{
		//���ڴ��ÿһ���ݶ��е����г߶Ȳ�
		vector<Mat> temp_vec;

		for (auto j = 0; j < nOctaveLayers + 2; ++j)
		{
			Mat temp_img = gauss_pyramid[i][j + 1] - gauss_pyramid[i][j];

			temp_vec.push_back(temp_img);
		}
		dog_pyramid.push_back(temp_vec);

		temp_vec.clear();
	}
}


/***********���ɸ�˹��ֽ�������ǰ���Ӧ���ݶȷ���ͼ����ݶȷ���ͼ��***********/
/*imageΪ��˹��ֽ�������ǰ��ͼ��
 *amplitΪ��ǰ���ݶȷ���ͼ��
 *orientΪ��ǰ���ݶȷ���ͼ��
 *scale��ǰ��߶�
 *numsΪ��Եײ�Ĳ���
 */
void mySift::amplit_orient(const Mat& image, vector<Mat>& amplit, vector<Mat>& orient, float scale, int nums)
{
	//�����ڴ�
	amplit.resize(Mmax * nOctaveLayers);
	orient.resize(Mmax * nOctaveLayers);

	int radius = cvRound(2 * scale);

	Mat kernel;												//kernel(2 * radius + 1, 2 * radius + 1, CV_32FC1);

	roewa_kernel(radius, scale, kernel);					//�����˲��ˣ�Ҳ��ָ������,����ھ�������½�

	//�ĸ��˲�ģ������
	Mat W34 = Mat::zeros(2 * radius + 1, 2 * radius + 1, CV_32FC1);		//��kernel�����°벿�ָ��Ƶ���Ӧ����
	Mat W12 = Mat::zeros(2 * radius + 1, 2 * radius + 1, CV_32FC1);		//��kernel�����ϰ벿�ָ��Ƶ���Ӧ����
	Mat W14 = Mat::zeros(2 * radius + 1, 2 * radius + 1, CV_32FC1);		//��kernel�����Ұ벿�ָ��Ƶ���Ӧ����
	Mat W23 = Mat::zeros(2 * radius + 1, 2 * radius + 1, CV_32FC1);		//��kernel������벿�ָ��Ƶ���Ӧ����

	kernel(Range(radius + 1, 2 * radius + 1), Range::all()).copyTo(W34(Range(radius + 1, 2 * radius + 1), Range::all()));
	kernel(Range(0, radius), Range::all()).copyTo(W12(Range(0, radius), Range::all()));
	kernel(Range::all(), Range(radius + 1, 2 * radius + 1)).copyTo(W14(Range::all(), Range(radius + 1, 2 * radius + 1)));
	kernel(Range::all(), Range(0, radius)).copyTo(W23(Range::all(), Range(0, radius)));

	//�˲�
	Mat M34, M12, M14, M23;
	double eps = 0.00001;

	//float_imageΪͼ���һ�����ͼ�����ݣ����������
	filter2D(image, M34, CV_32FC1, W34, Point(-1, -1), eps);
	filter2D(image, M12, CV_32FC1, W12, Point(-1, -1), eps);
	filter2D(image, M14, CV_32FC1, W14, Point(-1, -1), eps);
	filter2D(image, M23, CV_32FC1, W23, Point(-1, -1), eps);

	//����ˮƽ�ݶȺ���ֱ�ݶ�
	Mat Gx, Gy;
	log((M14) / (M23), Gx);
	log((M34) / (M12), Gy);

	//�����ݶȷ��Ⱥ��ݶȷ���
	magnitude(Gx, Gy, amplit[nums]);					//�ݶȷ���ͼ��ƽ���Ϳ�ƽ��
	phase(Gx, Gy, orient[nums], true);					//�ݶȷ���ͼ��

}


/***********************�ú�������߶ȿռ�����������������ں���������ļ��***************************/
/*image��ʾ����������λ�õĸ�˹ͼ�񣬺���ɶ���Դ������޸�
 pt��ʾ�������λ������(x,y)
 scale������ĳ߶�
 n��ʾֱ��ͼbin����
 hist��ʾ����õ���ֱ��ͼ
 ��������ֵ��ֱ��ͼhist�е������ֵ*/
static float clac_orientation_hist(const Mat& image, Point pt, float scale, int n, float* hist)
{
	int radius = cvRound(ORI_RADIUS * scale);			//����������뾶(3*1.5*scale)

	int len = (2 * radius + 1) * (2 * radius + 1);		//���������������ܸ��������ֵ��

	float sigma = ORI_SIG_FCTR * scale;					//�����������˹Ȩ�ر�׼��(1.5*scale)

	float exp_scale = -1.f / (2 * sigma * sigma);		//Ȩ�ص�ָ������

	//ʹ��AutoBuffer����һ���ڴ棬������4���ռ��Ŀ����Ϊ�˷������ƽ��ֱ��ͼ����Ҫ
	AutoBuffer<float> buffer((4 * len) + n + 4);

	//X����ˮƽ��֣�Y������ֵ��֣�Mag�����ݶȷ��ȣ�Ori�����ݶȽǶȣ�W�����˹Ȩ��
	float* X = buffer, * Y = buffer + len, * Mag = Y, * Ori = Y + len, * W = Ori + len;
	float* temp_hist = W + len + 2;						//��ʱ����ֱ��ͼ����

	for (int i = 0; i < n; ++i)
		temp_hist[i] = 0.f;								//��������

	//�����������ص�ˮƽ��ֺ���ֱ���
	int k = 0;

	for (int i = -radius; i < radius; ++i)
	{
		int y = pt.y + i;								//������������

		if (y <= 0 || y >= image.rows - 1)
			continue;

		for (int j = -radius; j < radius; ++j)
		{
			int x = pt.x + j;

			if (x <= 0 || x >= image.cols - 1)
				continue;

			float dx = image.at<float>(y, x + 1) - image.at<float>(y, x - 1);	//ˮƽ���
			float dy = image.at<float>(y + 1, x) - image.at<float>(y - 1, x);	//��ֱ���

			//����ˮƽ��ֺ���ֱ��ּ����Ӧ��Ȩ��
			X[k] = dx;
			Y[k] = dy;
			W[k] = (i * i + j * j) * exp_scale;

			++k;
		}
	}

	len = k;													//������������ĸ���

	cv::hal::exp(W, W, len);									//�����������������صĸ�˹Ȩ��
	cv::hal::fastAtan2(Y, X, Ori, len, true);					//�����������������ص��ݶȷ��򣬽Ƕȷ�Χ0-360��
	cv::hal::magnitude32f(X, Y, Mag, len);						//�����������������ص��ݶȷ��ȣ����������ѧ�����ϵ��ݶ�

	//�������������
	for (int i = 0; i < len; ++i)
	{
		int bin = cvRound((n / 360.f) * Ori[i]);				//�������ص��ݶȷ���Լ��bin�ķ�Χ��[0,(n-1)]

		//���ص��ݶȷ���Ϊ360��ʱ����0��һ��
		if (bin >= n)
			bin = bin - n;

		if (bin < 0)
			bin = bin + n;

		temp_hist[bin] = temp_hist[bin] + Mag[i] * W[i];		//ͳ�����������ظ����������ݶ�ֱ��ͼ�ķ�ֵ(��Ȩ��ķ�ֵ)
	}

	//ƽ��ֱ��ͼ
	temp_hist[-1] = temp_hist[n - 1];
	temp_hist[-2] = temp_hist[n - 2];
	temp_hist[n] = temp_hist[0];
	temp_hist[n + 1] = temp_hist[1];
	for (int i = 0; i < n; ++i)
	{
		hist[i] = (temp_hist[i - 2] + temp_hist[i + 2]) * (1.f / 16.f) +
			(temp_hist[i - 1] + temp_hist[i + 1]) * (4.f / 16.f) +
			temp_hist[i] * (6.f / 16.f);
	}

	//���ֱ��ͼ�����ֵ
	float max_value = hist[0];
	for (int i = 1; i < n; ++i)
	{
		if (hist[i] > max_value)
			max_value = hist[i];
	}
	return max_value;
}


/***********************ʹ�� sobel �˲�����������ݶȼ���߶ȿռ��������������**************************/
static float clac_orientation_hist_2(Mat& image, Point pt, float scale, int n, float* hist)
{
	Mat output_image;									//ʹ�� sobel �˲��������ͼ����ݶȷ���ͼ��

	sobelfilter(image, output_image);					//ʹ�� sobel �˲������˹���ͼ����ݶȷ���ͼ��

	int radius = cvRound(ORI_RADIUS * scale);			//����������뾶(3*1.5*scale)

	int len = (2 * radius + 1) * (2 * radius + 1);		//���������������ܸ��������ֵ��

	float sigma = ORI_SIG_FCTR * scale;					//�����������˹Ȩ�ر�׼��(1.5*scale)

	float exp_scale = -1.f / (2 * sigma * sigma);		//Ȩ�ص�ָ������

	//ʹ��AutoBuffer����һ���ڴ棬������4���ռ��Ŀ����Ϊ�˷������ƽ��ֱ��ͼ����Ҫ
	AutoBuffer<float> buffer((4 * len) + n + 4);

	//X����ˮƽ��֣�Y������ֵ��֣�Mag�����ݶȷ��ȣ�Ori�����ݶȽǶȣ�W�����˹Ȩ��
	float* X = buffer, * Y = buffer + len, * Mag = Y, * Ori = Y + len, * W = Ori + len;
	float* temp_hist = W + len + 2;						//��ʱ����ֱ��ͼ����

	for (int i = 0; i < n; ++i)
		temp_hist[i] = 0.f;								//��������

	//�����������ص�ˮƽ��ֺ���ֱ���
	int k = 0;

	for (int i = -radius; i < radius; ++i)
	{
		int y = pt.y + i;								//�����������꣬��

		if (y <= 0 || y >= output_image.rows - 1)
			continue;

		for (int j = -radius; j < radius; ++j)
		{
			int x = pt.x + j;							//�����������꣬��

			if (x <= 0 || x >= output_image.cols - 1)
				continue;

			//float dx = image.at<float>(y, x + 1) - image.at<float>(y, x - 1);	//ˮƽ���

			float dx = output_image.at<float>(y - 1, x + 1) - output_image.at<float>(y - 1, x - 1)
				+ 2 * output_image.at<float>(y, x + 1) - 2 * output_image.at<float>(y, x - 1)
				+ output_image.at<float>(y + 1, x + 1) - output_image.at<float>(y + 1, x - 1);

			float dy = output_image.at<float>(y + 1, x - 1) - output_image.at<float>(y - 1, x - 1)
				+ 2 * output_image.at<float>(y + 1, x) - 2 * output_image.at<float>(y - 1, x) +
				output_image.at<float>(y + 1, x + 1) - output_image.at<float>(y - 1, x + 1);

			/*float dx = image.at<float>(y - 1, x + 1) - image.at<float>(y - 1, x - 1)
				+ 2 * image.at<float>(y, x + 1) - 2 * image.at<float>(y, x - 1)
				+ image.at<float>(y + 1, x + 1) - image.at<float>(y + 1, x - 1);
			float dy = image.at<float>(y + 1, x - 1) - image.at<float>(y - 1, x - 1)
				+ 2 * image.at<float>(y + 1, x) - 2 * image.at<float>(y - 1, x) +
				image.at<float>(y + 1, x + 1) - image.at<float>(y - 1, x + 1);*/

				//����ˮƽ��ֺ���ֱ��ּ����Ӧ��Ȩ��
			X[k] = dx;
			Y[k] = dy;
			W[k] = (i * i + j * j) * exp_scale;

			++k;
		}
	}

	len = k;													//������������ĸ���

	cv::hal::exp(W, W, len);									//�����������������صĸ�˹Ȩ��
	cv::hal::fastAtan2(Y, X, Ori, len, true);					//�����������������ص��ݶȷ��򣬽Ƕȷ�Χ0-360��
	cv::hal::magnitude32f(X, Y, Mag, len);						//�����������������ص��ݶȷ��ȣ����������ѧ�����ϵ��ݶ�

	//�������������
	for (int i = 0; i < len; ++i)
	{
		int bin = cvRound((n / 360.f) * Ori[i]);				//�������ص��ݶȷ���Լ��bin�ķ�Χ��[0,(n-1)]

		//���ص��ݶȷ���Ϊ360��ʱ����0��һ��
		if (bin >= n)
			bin = bin - n;

		if (bin < 0)
			bin = bin + n;

		temp_hist[bin] = temp_hist[bin] + Mag[i] * W[i];		//ͳ�����������ظ����������ݶ�ֱ��ͼ�ķ�ֵ(��Ȩ��ķ�ֵ)
	}

	//ƽ��ֱ��ͼ
	temp_hist[-1] = temp_hist[n - 1];
	temp_hist[-2] = temp_hist[n - 2];
	temp_hist[n] = temp_hist[0];
	temp_hist[n + 1] = temp_hist[1];
	for (int i = 0; i < n; ++i)
	{
		hist[i] = (temp_hist[i - 2] + temp_hist[i + 2]) * (1.f / 16.f) +
			(temp_hist[i - 1] + temp_hist[i + 1]) * (4.f / 16.f) +
			temp_hist[i] * (6.f / 16.f);
	}

	//���ֱ��ͼ�����ֵ
	float max_value = hist[0];
	for (int i = 1; i < n; ++i)
	{
		if (hist[i] > max_value)
			max_value = hist[i];
	}
	return max_value;
}


/******************�ú�������������������,����LOGH�汾*********************/
/*amplit��ʾ���������ڲ���ݶȷ��ȣ�������ͼ���Ӧ���ص���ݶȴ����˶�Ӧλ��
  orient��ʾ���������ڲ���ݶȷ���0-360��
  point��ʾ����������
  scale��ʾ����������ڲ�ĳ߶�
  hist��ʾ���ɵ�ֱ��ͼ
  n��ʾ������ֱ��ͼbin����
 */
static float calc_orient_hist(const Mat& amplit, const Mat& orient, Point pt, float scale, float* hist, int n)
{
	//������Ϊ��ֻ�������²�����û�н��и�˹ģ��
	int num_row = amplit.rows;
	int num_col = amplit.cols;

	Point point(cvRound(pt.x), cvRound(pt.y));

	//int radius = cvRound(SAR_SIFT_FACT_RADIUS_ORI * scale);
	int radius = cvRound(6 * scale);

	radius = min(radius, min(num_row / 2, num_col / 2));

	float gauss_sig = 2 * scale;							//��˹��Ȩ��׼��

	float exp_temp = -1.f / (2 * gauss_sig * gauss_sig);	//Ȩ��ָ������

	//��������
	int radius_x_left = point.x - radius;
	int radius_x_right = point.x + radius;
	int radius_y_up = point.y - radius;
	int radius_y_down = point.y + radius;

	//��ֹԽ��
	if (radius_x_left < 0)
		radius_x_left = 0;
	if (radius_x_right > num_col - 1)
		radius_x_right = num_col - 1;
	if (radius_y_up < 0)
		radius_y_up = 0;
	if (radius_y_down > num_row - 1)
		radius_y_down = num_row - 1;

	//��ʱ��������Χ������������ڱ��������������
	int center_x = point.x - radius_x_left;
	int center_y = point.y - radius_y_up;

	//��������ı߽磬�����˹Ȩֵ
	Range x_rng(-(point.x - radius_x_left), radius_x_right - point.x);
	Range y_rng(-(point.y - radius_y_up), radius_y_down - point.y);

	Mat gauss_weight;

	gauss_weight.create(y_rng.end - y_rng.start + 1, x_rng.end - x_rng.start + 1, CV_32FC1);

	//��������ص�ĸ�˹Ȩ��
	for (int i = y_rng.start; i <= y_rng.end; ++i)
	{
		float* ptr_gauss = gauss_weight.ptr<float>(i - y_rng.start);

		for (int j = x_rng.start; j <= x_rng.end; ++j)
			ptr_gauss[j - x_rng.start] = exp((i * i + j * j) * exp_temp);
	}

	//������������Χ�������ݶȷ��ȣ��ݶȷ���
	Mat sub_amplit = amplit(Range(radius_y_up, radius_y_down + 1), Range(radius_x_left, radius_x_right + 1));
	Mat sub_orient = orient(Range(radius_y_up, radius_y_down + 1), Range(radius_x_left, radius_x_right + 1));

	//Mat W = sub_amplit.mul(gauss_weight);			//�����˹Ȩ�أ������˹Ȩ��ʱ����ȷƥ���Է���������
	Mat W = sub_amplit;								//û�Ӹ�˹Ȩ�أ��ݶȷ�ֵ

	//����ֱ��ͼ
	AutoBuffer<float> buffer(n + 4);

	float* temp_hist = buffer + 2;

	for (int i = 0; i < n; ++i)
		temp_hist[i] = 0.f;

	for (int i = 0; i < sub_orient.rows; i++)
	{
		float* ptr_1 = W.ptr<float>(i);
		float* ptr_2 = sub_orient.ptr<float>(i);

		for (int j = 0; j < sub_orient.cols; j++)
		{
			if (((i - center_y) * (i - center_y) + (j - center_x) * (j - center_x)) < radius * radius)
			{
				int bin = cvRound(ptr_2[j] * n / 360.f);

				if (bin > n)
					bin = bin - n;
				if (bin < 0)
					bin = bin + n;
				temp_hist[bin] += ptr_1[j];
			}
		}
	}

	//ƽ��ֱ��ͼ�����Է�ֹͻ��
	temp_hist[-1] = temp_hist[n - 1];
	temp_hist[-2] = temp_hist[n - 2];
	temp_hist[n] = temp_hist[0];
	temp_hist[n + 1] = temp_hist[1];

	for (int i = 0; i < n; ++i)
	{
		hist[i] = (temp_hist[i - 2] + temp_hist[i + 2]) * (1.f / 16.f) +
			(temp_hist[i - 1] + temp_hist[i + 1]) * (4.f / 16.f) +
			temp_hist[i] * (6.f / 16.f);
	}

	//���ֱ��ͼ�����ֵ
	float max_value = hist[0];
	for (int i = 1; i < n; ++i)
	{
		if (hist[i] > max_value)
			max_value = hist[i];
	}
	return max_value;
}

/****************************�ú�����ȷ��λ������λ��(x,y,scale)�����ں���������ļ��*************************/
/*���ܣ�ȷ���������λ�ã���ͨ��������������Ե��Ӧ��,�ð汾�Ǽ򻯰�
 dog_pry��ʾDOG������
 kpt��ʾ��ȷ��λ������������Ϣ
 octave��ʾ��ʼ���������ڵ���
 layer��ʾ��ʼ���������ڵĲ�
 row��ʾ��ʼ��������ͼ���е�������
 col��ʾ��ʼ��������ͼ���е�������
 nOctaveLayers��ʾDOG������ÿ���м������Ĭ����3
 contrastThreshold��ʾ�Աȶ���ֵ��Ĭ����0.04
 edgeThreshold��ʾ��Ե��ֵ��Ĭ����10
 sigma��ʾ��˹�߶ȿռ���ײ�ͼ��߶ȣ�Ĭ����1.6*/
static bool adjust_local_extrema_1(const vector<vector<Mat>>& dog_pyr, KeyPoint& kpt, int octave, int& layer, int& row,
	int& col, int nOctaveLayers, float contrastThreshold, float edgeThreshold, float sigma)
{
	float xi = 0, xr = 0, xc = 0;
	int i = 0;
	for (; i < MAX_INTERP_STEPS; ++i)					//����������
	{
		const Mat& img = dog_pyr[octave][layer];		//��ǰ��ͼ������
		const Mat& prev = dog_pyr[octave][layer - 1];	//֮ǰ��ͼ������
		const Mat& next = dog_pyr[octave][layer + 1];	//��һ��ͼ������

		//������λ��x����y����,�߶ȷ����һ��ƫ����
		float dx = (img.at<float>(row, col + 1) - img.at<float>(row, col - 1)) * (1.f / 2.f);
		float dy = (img.at<float>(row + 1, col) - img.at<float>(row - 1, col)) * (1.f / 2.f);
		float dz = (next.at<float>(row, col) - prev.at<float>(row, col)) * (1.f / 2.f);

		//����������λ�ö���ƫ����
		float v2 = img.at<float>(row, col);
		float dxx = img.at<float>(row, col + 1) + img.at<float>(row, col - 1) - 2 * v2;
		float dyy = img.at<float>(row + 1, col) + img.at<float>(row - 1, col) - 2 * v2;
		float dzz = prev.at<float>(row, col) + next.at<float>(row, col) - 2 * v2;

		//������������Χ��϶���ƫ����
		float dxy = (img.at<float>(row + 1, col + 1) + img.at<float>(row - 1, col - 1) -
			img.at<float>(row + 1, col - 1) - img.at<float>(row - 1, col + 1)) * (1.f / 4.f);
		float dxz = (next.at<float>(row, col + 1) + prev.at<float>(row, col - 1) -
			next.at<float>(row, col - 1) - prev.at<float>(row, col + 1)) * (1.f / 4.f);
		float dyz = (next.at<float>(row + 1, col) + prev.at<float>(row - 1, col) -
			next.at<float>(row - 1, col) - prev.at<float>(row + 1, col)) * (1.f / 4.f);

		Matx33f H(dxx, dxy, dxz, dxy, dyy, dyz, dxz, dyz, dzz);

		Vec3f dD(dx, dy, dz);

		Vec3f X = H.solve(dD, DECOMP_SVD);

		xc = -X[0];		//x����ƫ����
		xr = -X[1];		//y����ƫ����
		xi = -X[2];		//�߶ȷ���ƫ����

		//�����������ƫ������С��0.5��˵���Ѿ��ҵ�������׼ȷλ��
		if (abs(xc) < 0.5f && abs(xr) < 0.5f && abs(xi) < 0.5f)
			break;

		//�������һ�������ƫ����������ɾ���õ�
		if (abs(xc) > (float)(INT_MAX / 3) ||
			abs(xr) > (float)(INT_MAX / 3) ||
			abs(xi) > (float)(INT_MAX / 3))
			return false;

		col = col + cvRound(xc);
		row = row + cvRound(xr);
		layer = layer + cvRound(xi);

		//��������㶨λ�ڱ߽�����ͬ��Ҳ��Ҫɾ��
		if (layer<1 || layer>nOctaveLayers ||
			col<IMG_BORDER || col>img.cols - IMG_BORDER ||
			row<IMG_BORDER || row>img.rows - IMG_BORDER)
			return false;
	}

	//���i=MAX_INTERP_STEPS��˵��ѭ������Ҳû������������������������Ҫ��ɾ��
	if (i >= MAX_INTERP_STEPS)
		return false;

	/**************************�ٴ�ɾ������Ӧ��(�ԱȶȽϵ͵ĵ�)********************************/
	//�ٴμ���������λ��x����y����,�߶ȷ����һ��ƫ����
	//�߶Աȶȵ�������ͼ��ı������ȶ���
	{
		const Mat& img = dog_pyr[octave][layer];
		const Mat& prev = dog_pyr[octave][layer - 1];
		const Mat& next = dog_pyr[octave][layer + 1];

		float dx = (img.at<float>(row, col + 1) - img.at<float>(row, col - 1)) * (1.f / 2.f);
		float dy = (img.at<float>(row + 1, col) - img.at<float>(row - 1, col)) * (1.f / 2.f);
		float dz = (next.at<float>(row, col) - prev.at<float>(row, col)) * (1.f / 2.f);
		Matx31f dD(dx, dy, dz);
		float t = dD.dot(Matx31f(xc, xr, xi));

		float contr = img.at<float>(row, col) + t * 0.5f;	//��������Ӧ |D(x~)| ���Աȶ�

		//Low����contr��ֵ��0.03������RobHess�Ƚ�����ֵΪ0.04/nOctaveLayers
		if (abs(contr) < contrastThreshold / nOctaveLayers)	//��ֵ��Ϊ0.03ʱ��������������
			return false;

		/******************************ɾ����Ե��Ӧ�Ƚ�ǿ�ĵ�************************************/

		//�ٴμ���������λ�ö���ƫ��������ȡ��������� Hessian ����������ͨ�� 2X2 �� Hessian �������
		//һ�����岻�õĸ�˹������ӵļ�ֵ�ں���Ե�ĵط��нϴ�������ʶ��ڴ�ֱ��Ե�ķ����н�С��������
		float v2 = img.at<float>(row, col);
		float dxx = img.at<float>(row, col + 1) + img.at<float>(row, col - 1) - 2 * v2;
		float dyy = img.at<float>(row + 1, col) + img.at<float>(row - 1, col) - 2 * v2;
		float dxy = (img.at<float>(row + 1, col + 1) + img.at<float>(row - 1, col - 1) -
			img.at<float>(row + 1, col - 1) - img.at<float>(row - 1, col + 1)) * (1.f / 4.f);
		float det = dxx * dyy - dxy * dxy;
		float trace = dxx + dyy;

		//�����ʺ���ֵ�ĶԱ��ж�
		if (det < 0 || (trace * trace * edgeThreshold >= det * (edgeThreshold + 1) * (edgeThreshold + 1)))
			return false;

		/*********��ĿǰΪֹ��������������������Ҫ�󣬱������������Ϣ***********/
		kpt.pt.x = ((float)col + xc) * (1 << octave);	//�������ײ��ͼ���x����
		kpt.pt.y = ((float)row + xr) * (1 << octave);	//�������ײ�ͼ���y����
		kpt.octave = octave + (layer << 8);				//��ű����ڵ��ֽڣ���ű����ڸ��ֽ�
		//�������ײ�ͼ��ĳ߶�
		kpt.size = sigma * powf(2.f, (layer + xi) / nOctaveLayers) * (1 << octave);
		kpt.response = abs(contr);						//��������Ӧֵ(�Աȶ�)

		return true;
	}

}


/****************************�ú�����ȷ��λ������λ��(x,y,scale)�����ں���������ļ��*************************/
//�ð汾�� SIFT ԭ�棬���õ�����������������
static bool adjust_local_extrema_2(const vector<vector<Mat>>& dog_pyr, KeyPoint& kpt, int octave, int& layer, int& row,
	int& col, int nOctaveLayers, float contrastThreshold, float edgeThreshold, float sigma)
{
	const float img_scale = 1.f / (255 * SIFT_FIXPT_SCALE);    //SIFT_FIXPT_SCALE=48
	const float deriv_scale = img_scale * 0.5f;
	const float second_deriv_scale = img_scale;
	const float cross_deriv_scale = img_scale * 0.25f;

	float xi = 0, xr = 0, xc = 0;
	int i = 0;

	for (; i < MAX_INTERP_STEPS; ++i)						  //����������
	{
		const Mat& img = dog_pyr[octave][layer];			  //��ǰ��ͼ������
		const Mat& prev = dog_pyr[octave][layer - 1];		  //֮ǰ��ͼ������
		const Mat& next = dog_pyr[octave][layer + 1];		  //��һ��ͼ������

		//����һ��ƫ������ͨ���ٽ��������
		float dx = (img.at<float>(row, col + 1) - img.at<float>(row, col - 1)) * deriv_scale;
		float dy = (img.at<float>(row + 1, col) - img.at<float>(row - 1, col)) * deriv_scale;
		float dz = (next.at<float>(row, col) - prev.at<float>(row, col)) * deriv_scale;

		//����������λ�ö���ƫ����
		//float v2  = img.at<float>(row, col);
		float v2 = (float)img.at<float>(row, col) * 2.f;
		float dxx = (img.at<float>(row, col + 1) + img.at<float>(row, col - 1) - v2) * second_deriv_scale;
		float dyy = (img.at<float>(row + 1, col) + img.at<float>(row - 1, col) - v2) * second_deriv_scale;
		float dzz = (prev.at<float>(row, col) + next.at<float>(row, col) - v2) * second_deriv_scale;

		//������������Χ��϶���ƫ����
		float dxy = (img.at<float>(row + 1, col + 1) + img.at<float>(row - 1, col - 1) -
			img.at<float>(row + 1, col - 1) - img.at<float>(row - 1, col + 1)) * cross_deriv_scale;
		float dxz = (next.at<float>(row, col + 1) + prev.at<float>(row, col - 1) -
			next.at<float>(row, col - 1) - prev.at<float>(row, col + 1)) * cross_deriv_scale;
		float dyz = (next.at<float>(row + 1, col) + prev.at<float>(row - 1, col) -
			next.at<float>(row - 1, col) - prev.at<float>(row + 1, col)) * cross_deriv_scale;

		Matx33f H(dxx, dxy, dxz, dxy, dyy, dyz, dxz, dyz, dzz);

		Vec3f dD(dx, dy, dz);

		Vec3f X = H.solve(dD, DECOMP_SVD);

		xc = -X[0];		//x����ƫ����
		xr = -X[1];		//y����ƫ����
		xi = -X[2];		//�߶ȷ���ƫ����

		//�����������ƫ������С��0.5��˵���Ѿ��ҵ�������׼ȷλ��
		if (abs(xc) < 0.5f && abs(xr) < 0.5f && abs(xi) < 0.5f)
			break;

		//�������һ�������ƫ����������ɾ���õ�
		if (abs(xc) > (float)(INT_MAX / 3) ||
			abs(xr) > (float)(INT_MAX / 3) ||
			abs(xi) > (float)(INT_MAX / 3))
			return false;

		col = col + cvRound(xc);
		row = row + cvRound(xr);
		layer = layer + cvRound(xi);

		//��������㶨λ�ڱ߽�����ͬ��Ҳ��Ҫɾ��
		if (layer<1 || layer>nOctaveLayers ||
			col < IMG_BORDER || col >= img.cols - IMG_BORDER ||
			row < IMG_BORDER || row >= img.rows - IMG_BORDER)
			return false;
	}
	//���i=MAX_INTERP_STEPS��˵��ѭ������Ҳû������������������������Ҫ��ɾ��
	if (i >= MAX_INTERP_STEPS)
		return false;



	/**************************�ٴ�ɾ������Ӧ��(�ԱȶȽϵ͵ĵ�)********************************/
	//�ٴμ���������λ��x����y����,�߶ȷ����һ��ƫ����
	//�߶Աȶȵ�������ͼ��ı������ȶ���

	const Mat& img = dog_pyr[octave][layer];
	const Mat& prev = dog_pyr[octave][layer - 1];
	const Mat& next = dog_pyr[octave][layer + 1];

	float dx = (img.at<float>(row, col + 1) - img.at<float>(row, col - 1)) * deriv_scale;
	float dy = (img.at<float>(row + 1, col) - img.at<float>(row - 1, col)) * deriv_scale;
	float dz = (next.at<float>(row, col) - prev.at<float>(row, col)) * deriv_scale;
	Matx31f dD(dx, dy, dz);
	float t = dD.dot(Matx31f(xc, xr, xi));

	float contr = img.at<float>(row, col) + t * 0.5f;	//��������Ӧ |D(x~)| ���Աȶ�

	//Low����contr��ֵ��0.03������RobHess�Ƚ�����ֵΪ0.04/nOctaveLayers
	if (abs(contr) < contrastThreshold / nOctaveLayers)	//��ֵ��Ϊ0.03ʱ��������������
		return false;


	/******************************ɾ����Ե��Ӧ�Ƚ�ǿ�ĵ�************************************/

	//�ٴμ���������λ�ö���ƫ��������ȡ��������� Hessian ����������ͨ�� 2X2 �� Hessian �������
	//һ�����岻�õĸ�˹������ӵļ�ֵ�ں���Ե�ĵط��нϴ�������ʶ��ڴ�ֱ��Ե�ķ����н�С��������
	float v2 = (float)img.at<float>(row, col) * 2.f;
	float dxx = (img.at<float>(row, col + 1) + img.at<float>(row, col - 1) - v2) * second_deriv_scale;
	float dyy = (img.at<float>(row + 1, col) + img.at<float>(row - 1, col) - v2) * second_deriv_scale;
	float dxy = (img.at<float>(row + 1, col + 1) + img.at<float>(row - 1, col - 1) -
		img.at<float>(row + 1, col - 1) - img.at<float>(row - 1, col + 1)) * cross_deriv_scale;

	float det = dxx * dyy - dxy * dxy;
	float trace = dxx + dyy;

	//�����ʺ���ֵ�ĶԱ��ж�
	if (det <= 0 || (trace * trace * edgeThreshold >= det * (edgeThreshold + 1) * (edgeThreshold + 1)))
		return false;


	/*********�������������Ϣ***********/
	kpt.pt.x = ((float)col + xc) * (1 << octave);		//��˹�����������������������Ӧ�ı���
	kpt.pt.y = ((float)row + xr) * (1 << octave);

	// SIFT ������
	kpt.octave = octave + (layer << 8) + (cvRound((xi + 0.5) * 255) << 16);				//�����㱻����ʱ���ڵĽ�������

	kpt.size = sigma * powf(2.f, (layer + xi) / nOctaveLayers) * (1 << octave) * 2;		//�ؼ�������ֱ��
	kpt.response = abs(contr);							//��������Ӧֵ(�Աȶ�)

	return true;


}


/************�ú�����DOG�������Ͻ����������⣬�����㾫ȷ��λ��ɾ���ͶԱȶȵ㣬ɾ����Ե��Ӧ�ϴ��**********/
/*dog_pyr��ʾ��˹������			ԭʼ SIFT ����
 gauss_pyr��ʾ��˹������
 keypoints��ʾ��⵽��������*/
void mySift::find_scale_space_extrema(const vector<vector<Mat>>& dog_pyr, const vector<vector<Mat>>& gauss_pyr,
	vector<KeyPoint>& keypoints)
{
	int nOctaves = (int)dog_pyr.size();								//�Ӱ˶ȸ���

	//Low���½���thresholdThreshold��0.03��Rob Hess����ʹ��0.04/nOctaveLayers��Ϊ��ֵ
	float threshold = (float)(contrastThreshold / nOctaveLayers);
	const int n = ORI_HIST_BINS;									//n=36
	float hist[n];
	KeyPoint kpt;

	keypoints.clear();												//�����keypoints

	for (int i = 0; i < nOctaves; ++i)								//����ÿһ��
	{
		for (int j = 1; j <= nOctaveLayers; ++j)					//��������ÿһ��
		{
			const Mat& curr_img = dog_pyr[i][j];					//��ǰ��
			const Mat& prev_img = dog_pyr[i][j - 1];				//��һ��
			const Mat& next_img = dog_pyr[i][j + 1];				//��һ��

			int num_row = curr_img.rows;
			int num_col = curr_img.cols;							//��õ�ǰ��ͼ��Ĵ�С
			size_t step = curr_img.step1();							//һ��Ԫ����ռ�ֽ���

			//����ÿһ���߶Ȳ��е���Ч���أ�����ֵ
			for (int r = IMG_BORDER; r < num_row - IMG_BORDER; ++r)
			{
				const float* curr_ptr = curr_img.ptr<float>(r);		//ָ����ǵ� r �е���㣬���ص��� float ���͵�����ֵ
				const float* prev_ptr = prev_img.ptr<float>(r - 1);
				const float* next_ptr = next_img.ptr<float>(r + 1);

				for (int c = IMG_BORDER; c < num_col - IMG_BORDER; ++c)
				{
					float val = curr_ptr[c];						//��ǰ���ĵ���Ӧֵ

					//��ʼ���������
					if (abs(val) > threshold &&
						((val > 0 && val >= curr_ptr[c - 1] && val >= curr_ptr[c + 1] &&
							val >= curr_ptr[c - step - 1] && val >= curr_ptr[c - step] && val >= curr_ptr[c - step + 1] &&
							val >= curr_ptr[c + step - 1] && val >= curr_ptr[c + step] && val >= curr_ptr[c + step + 1] &&
							val >= prev_ptr[c] && val >= prev_ptr[c - 1] && val >= prev_ptr[c + 1] &&
							val >= prev_ptr[c - step - 1] && val >= prev_ptr[c - step] && val >= prev_ptr[c - step + 1] &&
							val >= prev_ptr[c + step - 1] && val >= prev_ptr[c + step] && val >= prev_ptr[c + step + 1] &&
							val >= next_ptr[c] && val >= next_ptr[c - 1] && val >= next_ptr[c + 1] &&
							val >= next_ptr[c - step - 1] && val >= next_ptr[c - step] && val >= next_ptr[c - step + 1] &&
							val >= next_ptr[c + step - 1] && val >= next_ptr[c + step] && val >= next_ptr[c + step + 1]) ||
							(val < 0 && val <= curr_ptr[c - 1] && val <= curr_ptr[c + 1] &&
								val <= curr_ptr[c - step - 1] && val <= curr_ptr[c - step] && val <= curr_ptr[c - step + 1] &&
								val <= curr_ptr[c + step - 1] && val <= curr_ptr[c + step] && val <= curr_ptr[c + step + 1] &&
								val <= prev_ptr[c] && val <= prev_ptr[c - 1] && val <= prev_ptr[c + 1] &&
								val <= prev_ptr[c - step - 1] && val <= prev_ptr[c - step] && val <= prev_ptr[c - step + 1] &&
								val <= prev_ptr[c + step - 1] && val <= prev_ptr[c + step] && val <= prev_ptr[c + step + 1] &&
								val <= next_ptr[c] && val <= next_ptr[c - 1] && val <= next_ptr[c + 1] &&
								val <= next_ptr[c - step - 1] && val <= next_ptr[c - step] && val <= next_ptr[c - step + 1] &&
								val <= next_ptr[c + step - 1] && val <= next_ptr[c + step] && val <= next_ptr[c + step + 1])))
					{
						//++numKeys;
						//����������ʼ�кţ��кţ���ţ����ڲ��
						int octave = i, layer = j, r1 = r, c1 = c;

						if (!adjust_local_extrema_1(dog_pyr, kpt, octave, layer, r1, c1,
							nOctaveLayers, (float)contrastThreshold,
							(float)edgeThreshold, (float)sigma))
						{
							continue;									//����ó�ʼ�㲻�����������򲻱���ĵ�
						}

						float scale = kpt.size / float(1 << octave);	//������������ڱ���ĳ߶�

						//max_histֵ��Ӧ�ķ���Ϊ������
						float max_hist = clac_orientation_hist(gauss_pyr[octave][layer], Point(c1, r1), scale, n, hist);

						//����mag_thrֵ��Ӧ�ķ���Ϊ��������
						float mag_thr = max_hist * ORI_PEAK_RATIO;		//����ֵ 80% �ķ�����Ϊ��������

						//����ֱ��ͼ�е� 36 ��bin
						for (int i = 0; i < n; ++i)
						{
							int left = i > 0 ? i - 1 : n - 1;
							int right = i < n - 1 ? i + 1 : 0;

							//�����µ������㣬��������ֵ 80% �ķ��򣬸�ֵ���������㣬��Ϊһ���µ������㣻���ж�������㣬λ�á��߶���ͬ������ͬ
							if (hist[i] > hist[left] && hist[i] > hist[right] && hist[i] >= mag_thr)
							{
								float bin = i + 0.5f * (hist[left] - hist[right]) / (hist[left] + hist[right] - 2 * hist[i]);
								bin = bin < 0 ? n + bin : bin >= n ? bin - n : bin;

								kpt.angle = (360.f / n) * bin;			//ԭʼ SIFT ����ʹ�õ��������������0-360��
								keypoints.push_back(kpt);				//�����������

							}
						}
					}
				}
			}
		}
	}

	//cout << "��ʼ����Ҫ�������������: " << numKeys << endl;
}


/************�ú�����DOG�������Ͻ����������⣬�����㾫ȷ��λ��ɾ���ͶԱȶȵ㣬ɾ����Ե��Ӧ�ϴ��**********/
//����������з����ϸ�� + ���Ӹ����������汾 ������ʱϸ���Ƕ����Ҫ���ؼ�����и�ֵʱ��ϸ��
//�����Կ���ֱ�ӶԷ���ֱ��ͼ����ϸ��
void mySift::find_scale_space_extrema1(const vector<vector<Mat>>& dog_pyr, vector<vector<Mat>>& gauss_pyr,
	vector<KeyPoint>& keypoints)
{
	int nOctaves = (int)dog_pyr.size();								//�Ӱ˶ȸ���

	//Low���½���thresholdThreshold��0.03��Rob Hess����ʹ��0.04/nOctaveLayers��Ϊ��ֵ
	float threshold = (float)(contrastThreshold / nOctaveLayers);
	const int n = ORI_HIST_BINS;									//n=36
	float hist[n];
	KeyPoint kpt;

	vector<Mat> amplit;			//��Ÿ�˹��ֽ�����ÿһ����ݶȷ���ͼ��
	vector<Mat> orient;			//��Ÿ�˹��ֽ�����ÿһ����ݶȷ���ͼ��

	keypoints.clear();												//�����keypoints

	for (int i = 0; i < nOctaves; ++i)								//����ÿһ��
	{
		for (int j = 1; j <= nOctaveLayers; ++j)					//��������ÿһ��
		{
			const Mat& curr_img = dog_pyr[i][j];					//��ǰ��
			const Mat& prev_img = dog_pyr[i][j - 1];				//��һ��
			const Mat& next_img = dog_pyr[i][j + 1];				//��һ��

			int num_row = curr_img.rows;
			int num_col = curr_img.cols;							//��õ�ǰ��ͼ��Ĵ�С
			size_t step = curr_img.step1();							//һ��Ԫ����ռ�ֽ���

			//����ÿһ���߶Ȳ��е���Ч���أ�����ֵ
			for (int r = IMG_BORDER; r < num_row - IMG_BORDER; ++r)
			{
				const float* curr_ptr = curr_img.ptr<float>(r);		//ָ����ǵ� r �е���㣬���ص��� float ���͵�����ֵ
				const float* prev_ptr = prev_img.ptr<float>(r);
				const float* next_ptr = next_img.ptr<float>(r);

				for (int c = IMG_BORDER; c < num_col - IMG_BORDER; ++c)
				{
					float val = curr_ptr[c];						//��ǰ���ĵ���Ӧֵ

					//��ʼ���������
					if (abs(val) > threshold &&
						((val > 0 && val >= curr_ptr[c - 1] && val >= curr_ptr[c + 1] &&
							val >= curr_ptr[c - step - 1] && val >= curr_ptr[c - step] && val >= curr_ptr[c - step + 1] &&
							val >= curr_ptr[c + step - 1] && val >= curr_ptr[c + step] && val >= curr_ptr[c + step + 1] &&
							val >= prev_ptr[c] && val >= prev_ptr[c - 1] && val >= prev_ptr[c + 1] &&
							val >= prev_ptr[c - step - 1] && val >= prev_ptr[c - step] && val >= prev_ptr[c - step + 1] &&
							val >= prev_ptr[c + step - 1] && val >= prev_ptr[c + step] && val >= prev_ptr[c + step + 1] &&
							val >= next_ptr[c] && val >= next_ptr[c - 1] && val >= next_ptr[c + 1] &&
							val >= next_ptr[c - step - 1] && val >= next_ptr[c - step] && val >= next_ptr[c - step + 1] &&
							val >= next_ptr[c + step - 1] && val >= next_ptr[c + step] && val >= next_ptr[c + step + 1]) ||
							(val < 0 && val <= curr_ptr[c - 1] && val <= curr_ptr[c + 1] &&
								val <= curr_ptr[c - step - 1] && val <= curr_ptr[c - step] && val <= curr_ptr[c - step + 1] &&
								val <= curr_ptr[c + step - 1] && val <= curr_ptr[c + step] && val <= curr_ptr[c + step + 1] &&
								val <= prev_ptr[c] && val <= prev_ptr[c - 1] && val <= prev_ptr[c + 1] &&
								val <= prev_ptr[c - step - 1] && val <= prev_ptr[c - step] && val <= prev_ptr[c - step + 1] &&
								val <= prev_ptr[c + step - 1] && val <= prev_ptr[c + step] && val <= prev_ptr[c + step + 1] &&
								val <= next_ptr[c] && val <= next_ptr[c - 1] && val <= next_ptr[c + 1] &&
								val <= next_ptr[c - step - 1] && val <= next_ptr[c - step] && val <= next_ptr[c - step + 1] &&
								val <= next_ptr[c + step - 1] && val <= next_ptr[c + step] && val <= next_ptr[c + step + 1])))
					{
						//++numKeys;
						//����������ʼ�кţ��кţ���ţ����ڲ��
						int octave = i, layer = j, r1 = r, c1 = c, nums = i * nOctaves + j;

						if (!adjust_local_extrema_2(dog_pyr, kpt, octave, layer, r1, c1,
							nOctaveLayers, (float)contrastThreshold,
							(float)edgeThreshold, (float)sigma))
						{
							continue;									//����ó�ʼ�㲻�����������򲻱���ĵ�
						}

						float scale = kpt.size / float(1 << octave);	//������������ڱ���ĳ߶�

						//�����ݶȷ��Ⱥ��ݶȷ���
						//amplit_orient(curr_img, amplit, orient, scale, nums);

						//max_histֵ��Ӧ�ķ���Ϊ������
						float max_hist = clac_orientation_hist(gauss_pyr[octave][layer], Point(c1, r1), scale, n, hist);
						//float max_hist = calc_orient_hist(amplit[nums], orient[nums], Point2f(c1, r1), scale, hist, n);

						//����mag_thrֵ��Ӧ�ķ���Ϊ��������
							//float mag_thr = max_hist * ORI_PEAK_RATIO;	//����ֵ 80% �ķ�����Ϊ��������

							//���Ӹ������������������������ݶȲ����³����
							float sum = 0.0;								//ֱ��ͼ��Ӧ�ķ�ֵ֮��
						float mag_thr = 0.0;							//�ж��Ƿ�Ϊ���������ֵ

						for (int i = 0; i < n; ++i)
						{
							sum += hist[i];
						}
						mag_thr = 0.5 * (1.0 / 36) * sum;


						//����ֱ��ͼ�е� 36 ��bin
						for (int i = 0; i < n; ++i)
						{
							int left = i > 0 ? i - 1 : n - 1;
							int right = i < n - 1 ? i + 1 : 0;

							//�����µ������㣬��������ֵ 80% �ķ��򣬸�ֵ���������㣬��Ϊһ���µ������㣻���ж�������㣬λ�á��߶���ͬ������ͬ
							if (hist[i] > hist[left] && hist[i] > hist[right] && hist[i] >= mag_thr)
							{
								float bin = i + 0.5f * (hist[left] - hist[right]) / (hist[left] + hist[right] - 2 * hist[i]);
								bin = bin < 0 ? n + bin : bin >= n ? bin - n : bin;


								//�޸ĵĵط�����������������޸�Ϊ��0-180�ȣ��൱�ڶԷ�������һ��ϸ��
								float angle = (360.f / n) * bin;
								if (angle >= 1 && angle <= 180)
								{
									kpt.angle = angle;
								}
								else if (angle > 180 && angle < 360)
								{
									kpt.angle = 360 - angle;
								}

								//kpt.angle = (360.f / n) * bin;			//ԭʼ SIFT ����ʹ�õ��������������0-360��
								keypoints.push_back(kpt);					//�����������

							}
						}
					}
				}
			}
		}
	}

	//cout << "��ʼ����Ҫ�������������: " << numKeys << endl;
}


/*�ú�������matlab�е�meshgrid����*/
/*x_range��ʾx����ķ�Χ
y_range��ʾy����ķ�Χ
X��ʾ���ɵ���x��仯������
Y��ʾ������y��任������
*/
static void meshgrid(const Range& x_range, const Range& y_range, Mat& X, Mat& Y)
{
	int x_start = x_range.start, x_end = x_range.end;
	int y_start = y_range.start, y_end = y_range.end;
	int width = x_end - x_start + 1;
	int height = y_end - y_start + 1;

	X.create(height, width, CV_32FC1);
	Y.create(height, width, CV_32FC1);

	for (int i = y_start; i <= y_end; i++)
	{
		float* ptr_1 = X.ptr<float>(i - y_start);
		for (int j = x_start; j <= x_end; ++j)
			ptr_1[j - x_start] = j * 1.0f;
	}

	for (int i = y_start; i <= y_end; i++)
	{
		float* ptr_2 = Y.ptr<float>(i - y_start);
		for (int j = x_start; j <= x_end; ++j)
			ptr_2[j - x_start] = i * 1.0f;
	}
}


/******************************����һ���������������***********************************/
/*gauss_image��ʾ���������ڵĸ�˹��
  main_angle��ʾ������������򣬽Ƕȷ�Χ��0-360��
  pt��ʾ�������ڸ�˹ͼ���ϵ����꣬����뱾�飬�����������ײ�
  scale��ʾ���������ڲ�ĳ߶ȣ�����ڱ��飬�����������ײ�
  d��ʾ����������������
  n��ʾÿ�������������ݶȽǶȵȷָ���
  descriptor��ʾ���ɵ��������������*/
static void calc_sift_descriptor(const Mat& gauss_image, float main_angle, Point2f pt,
	float scale, int d, int n, float* descriptor)
{
	Point ptxy(cvRound(pt.x), cvRound(pt.y));					//����ȡ��

	float cos_t = cosf(-main_angle * (float)(CV_PI / 180));		//�ѽǶ�ת��Ϊ���ȣ����������������
	float sin_t = sinf(-main_angle * (float)(CV_PI / 180));		//�ѽǶ�ת��Ϊ���ȣ����������������

	float bins_per_rad = n / 360.f;								// n = 8 ���ݶ�ֱ��ͼ��Ϊ 8 ������

	float exp_scale = -1.f / (d * d * 0.5f);					//Ȩ��ָ������

	float hist_width = DESCR_SCL_FCTR * scale;					//������������������߳���������ı߳�

	int radius = cvRound(hist_width * (d + 1) * sqrt(2) * 0.5f);//����������뾶(d+1)*(d+1)����������

	int rows = gauss_image.rows, cols = gauss_image.cols;		//��ǰ��˹���С�����Ϣ

	//����������뾶
	radius = min(radius, (int)sqrt((double)rows * rows + cols * cols));

	cos_t = cos_t / hist_width;
	sin_t = sin_t / hist_width;

	int len = (2 * radius + 1) * (2 * radius + 1);				//����������������Ϊ���涯̬�����ڴ�ʹ��

	int histlen = (d + 2) * (d + 2) * (n + 2);					//ֵΪ 360 

	AutoBuffer<float> buf(6 * len + histlen);

	//X����ˮƽ��֣�Y������ֱ��֣�Mag�����ݶȷ��ȣ�Angle���������㷽��, W�����˹Ȩ��
	float* X = buf, * Y = buf + len, * Mag = Y, * Angle = Y + len, * W = Angle + len;
	float* RBin = W + len, * CBin = RBin + len, * hist = CBin + len;

	//�������ֱ��ͼ����
	for (int i = 0; i < d + 2; ++i)				// i ��Ӧ row
	{
		for (int j = 0; j < d + 2; ++j)			// j ��Ӧ col
		{
			for (int k = 0; k < n + 2; ++k)

				hist[(i * (d + 2) + j) * (n + 2) + k] = 0.f;
		}
	}

	//�������ڵ����ط��䵽��Ӧ�������ڣ�������������ÿ�����ص��Ȩ��(������ d*d ��ÿһ��С����)
	int k = 0;

	//ʵ�������� 4 x 4 ���������� 16 �����ӵ㣬ÿ�����ӵ㶼��������������ģ�
	//ͨ�������Բ�ֵ�Բ�ͬ���ӵ������ص���м�Ȩ���õ���ͬ�����ӵ��ϻ��Ʒ���ֱ��ͼ

	for (int i = -radius; i < radius; ++i)						// i ��Ӧ y ������
	{
		for (int j = -radius; j < radius; ++j)					// j ��Ӧ x ������
		{
			float c_rot = j * cos_t - i * sin_t;				//��ת�������ڲ������ x ����
			float r_rot = j * sin_t + i * cos_t;				//��ת�������ڲ������ y ����

			//��ת�� 5 x 5 �������е��������ص㱻���䵽 4 x 4 ��������
			float cbin = c_rot + d / 2 - 0.5f;					//��ת��Ĳ���������������� x ����
			float rbin = r_rot + d / 2 - 0.5f;					//��ת��Ĳ���������������� y ����

			int r = ptxy.y + i, c = ptxy.x + j;					//ptxy�Ǹ�˹�������е�����

			//����rbin��cbin��Χ��(-1,d)
			if (rbin > -1 && rbin < d && cbin>-1 && cbin < d && r>0 && r < rows - 1 && c>0 && c < cols - 1)
			{
				float dx = gauss_image.at<float>(r, c + 1) - gauss_image.at<float>(r, c - 1);
				float dy = gauss_image.at<float>(r + 1, c) - gauss_image.at<float>(r - 1, c);

				X[k] = dx;												//�������������ص��ˮƽ���
				Y[k] = dy;												//�������������ص����ֱ���

				CBin[k] = cbin;											//���������в���������������� x ����
				RBin[k] = rbin;											//���������в���������������� y ����

				W[k] = (c_rot * c_rot + r_rot * r_rot) * exp_scale;		//��˹Ȩֵ��ָ������

				++k;
			}
		}
	}

	//�������������������������ݶȷ��ȣ��ݶȽǶȣ��͸�˹Ȩֵ
	len = k;

	cv::hal::exp(W, W, len);						//���������в�������������������صĸ�˹Ȩֵ
	cv::hal::fastAtan2(Y, X, Angle, len, true);		//���������в�������������������ص��ݶȷ��򣬽Ƕȷ�Χ��0-360��
	cv::hal::magnitude(X, Y, Mag, len);				//���������в�������������������ص��ݶȷ���

	//ʵ�������� 4 x 4 ���������� 16 �����ӵ㣬ÿ�����ӵ㶼��������������ģ�
	//ͨ�������Բ�ֵ�Բ�ͬ���ӵ������ص���м�Ȩ���õ���ͬ�����ӵ��ϻ��Ʒ���ֱ��ͼ

	//����ÿ�������������������
	for (k = 0; k < len; ++k)
	{
		float rbin = RBin[k], cbin = CBin[k];		//�����������ص����꣬rbin,cbin��Χ��(-1,d)

		//�Ľ��ĵط����Է��������һ��ϸ����Ҳ��Ϊ�����Ӷ��ݶȲ����³����
			//if (Angle[k] > 180 && Angle[k] < 360)
			//	Angle[k] = 360 - Angle[k];

			//�����������ص㴦���ķ���
			float temp = Angle[k] - main_angle;

		/*if (temp > 180 && temp < 360)
			temp = 360 - temp;*/

		float obin = temp * bins_per_rad;			//ָ����������������������ص��Ӧ�ķ���

		float mag = Mag[k] * W[k];					//�����������ص����Ȩֵ����ݶȷ�ֵ

		int r0 = cvFloor(rbin);						//roȡֵ������{-1��0��1��2��3}��û̫��Ϊʲô��
		int c0 = cvFloor(cbin);						//c0ȡֵ������{-1��0��1��2��3}
		int o0 = cvFloor(obin);

		rbin = rbin - r0;							//�����������ص������С�����֣��������Բ�ֵ���������ص������
		cbin = cbin - c0;
		obin = obin - o0;							//���������С������

		//���Ʒ�ΧΪ�ݶ�ֱ��ͼ������[0,n)��8 ������ֱ��ͼ
		if (o0 < 0)
			o0 = o0 + n;
		if (o0 >= n)
			o0 = o0 - n;

		//�����Բ�ֵ���ڼ�����������������֮������ض���������������ã���������䵽��Ӧ�������8��������
		//���ض�Ӧ����Ϣͨ����Ȩ���������Χ�����ӵ㣬������Ӧ������ݶ�ֵ�����ۼ�  

		float v_r1 = mag * rbin;					//�ڶ��з����ֵ
		float v_r0 = mag - v_r1;					//��һ�з����ֵ


		float v_rc11 = v_r1 * cbin;					//�ڶ��еڶ��з����ֵ�����½����ӵ�
		float v_rc10 = v_r1 - v_rc11;				//�ڶ��е�һ�з����ֵ�����½����ӵ�

		float v_rc01 = v_r0 * cbin;					//��һ�еڶ��з����ֵ�����Ͻ����ӵ�
		float v_rc00 = v_r0 - v_rc01;				//��һ�е�һ�з����ֵ�����Ͻ����ӵ�


		//һ�����ص�ķ���Ϊÿ�����ӵ������������������
		float v_rco111 = v_rc11 * obin;				//���½����ӵ�ڶ��������Ϸ����ֵ
		float v_rco110 = v_rc11 - v_rco111;			//���½����ӵ��һ�������Ϸ����ֵ

		float v_rco101 = v_rc10 * obin;
		float v_rco100 = v_rc10 - v_rco101;

		float v_rco011 = v_rc01 * obin;
		float v_rco010 = v_rc01 - v_rco011;

		float v_rco001 = v_rc00 * obin;
		float v_rco000 = v_rc00 - v_rco001;

		//�������������������
		int idx = ((r0 + 1) * (d + 2) + c0 + 1) * (n + 2) + o0;

		hist[idx] += v_rco000;
		hist[idx + 1] += v_rco001;
		hist[idx + n + 2] += v_rco010;
		hist[idx + n + 3] += v_rco011;
		hist[idx + (d + 2) * (n + 2)] += v_rco100;
		hist[idx + (d + 2) * (n + 2) + 1] += v_rco101;
		hist[idx + (d + 3) * (n + 2)] += v_rco110;
		hist[idx + (d + 3) * (n + 2) + 1] += v_rco111;
	}

	//����Բ��ѭ�������ԣ��Լ����Ժ����С�� 0 �Ȼ���� 360 �ȵ�ֵ���½��е�����ʹ
	//���� 0��360 ��֮��
	for (int i = 0; i < d; ++i)
	{
		for (int j = 0; j < d; ++j)
		{
			//������ hist[0][2][3] �� 0 �У��� 2 �У����ӵ�ֱ��ͼ�еĵ� 3 �� bin
			int idx = ((i + 1) * (d + 2) + (j + 1)) * (n + 2);

			hist[idx] += hist[idx + n];
			//hist[idx + 1] += hist[idx + n + 1];//opencvԴ������仰�Ƕ����,hist[idx + n + 1]��Զ��0.0

			for (k = 0; k < n; ++k)
				descriptor[(i * d + j) * n + k] = hist[idx + k];
		}
	}

	//�������ӽ��й�һ��
	int lenght = d * d * n;
	float norm = 0;

	//������������������ģֵ��ƽ��
	for (int i = 0; i < lenght; ++i)
	{
		norm = norm + descriptor[i] * descriptor[i];
	}
	norm = sqrt(norm);							//��������������ģֵ

	//�˴ι�һ����ȥ�����յ�Ӱ��
	for (int i = 0; i < lenght; ++i)
	{
		descriptor[i] = descriptor[i] / norm;
	}

	//��ֵ�ض�,ȥ���������������д��� 0.2 ��ֵ�������������Թ��յ�Ӱ��(������Ͷȶ�ĳЩ�ŵ��ݶ�Ӱ��ϴ󣬶Է����Ӱ���С)
	for (int i = 0; i < lenght; ++i)
	{
		descriptor[i] = min(descriptor[i], DESCR_MAG_THR);
	}

	//�ٴι�һ�����ܹ���������Ķ�����
	norm = 0;
	for (int i = 0; i < lenght; ++i)
	{
		norm = norm + descriptor[i] * descriptor[i];
	}
	norm = sqrt(norm);
	for (int i = 0; i < lenght; ++i)
	{
		descriptor[i] = descriptor[i] / norm;
	}
}


/*************************�ú�������ÿ�������������������*****************************/
/*amplit��ʾ���������ڲ���ݶȷ���ͼ��
  orient��ʾ���������ڲ���ݶȽǶ�ͼ��
  pt��ʾ�������λ��
  scale��ʾ���������ڲ�ĳ߶�
  main_ori��ʾ�������������0-360��
  d��ʾGLOH�Ƕȷ������������Ĭ����8��
  n��ʾÿ�������ڽǶ���0-360��֮��ȷָ�����nĬ����8
 */
static void calc_gloh_descriptor(const Mat& amplit, const Mat& orient, Point2f pt, float scale, float main_ori, int d, int n, float* ptr_des)
{
	Point point(cvRound(pt.x), cvRound(pt.y));

	//��������ת�������Һ�����
	float cos_t = cosf(-main_ori / 180.f * (float)CV_PI);
	float sin_t = sinf(-main_ori / 180.f * (float)CV_PI);

	int num_rows = amplit.rows;
	int num_cols = amplit.cols;
	int radius = cvRound(SAR_SIFT_RADIUS_DES * scale);
	radius = min(radius, min(num_rows / 2, num_cols / 2));//����������뾶

	int radius_x_left = point.x - radius;
	int radius_x_right = point.x + radius;
	int radius_y_up = point.y - radius;
	int radius_y_down = point.y + radius;

	//��ֹԽ��
	if (radius_x_left < 0)
		radius_x_left = 0;
	if (radius_x_right > num_cols - 1)
		radius_x_right = num_cols - 1;
	if (radius_y_up < 0)
		radius_y_up = 0;
	if (radius_y_down > num_rows - 1)
		radius_y_down = num_rows - 1;

	//��ʱ��������Χ��������������ģ�����ڸþ���
	int center_x = point.x - radius_x_left;
	int center_y = point.y - radius_y_up;

	//��������Χ�����������ݶȷ��ȣ��ݶȽǶ�
	Mat sub_amplit = amplit(Range(radius_y_up, radius_y_down + 1), Range(radius_x_left, radius_x_right + 1));
	Mat sub_orient = orient(Range(radius_y_up, radius_y_down + 1), Range(radius_x_left, radius_x_right + 1));


	//��center_x��center_yλ���ģ�������������������ת
	Range x_rng(-(point.x - radius_x_left), radius_x_right - point.x);
	Range y_rng(-(point.y - radius_y_up), radius_y_down - point.y);
	Mat X, Y;
	meshgrid(x_rng, y_rng, X, Y);
	Mat c_rot = X * cos_t - Y * sin_t;
	Mat r_rot = X * sin_t + Y * cos_t;
	Mat GLOH_angle, GLOH_amplit;
	phase(c_rot, r_rot, GLOH_angle, true);//�Ƕ���0-360��֮��
	GLOH_amplit = c_rot.mul(c_rot) + r_rot.mul(r_rot);//Ϊ�˼ӿ��ٶȣ�û�м��㿪��

	//����Բ�뾶ƽ��
	float R1_pow = (float)radius * radius;//��Բ�뾶ƽ��
	float R2_pow = pow(radius * SAR_SIFT_GLOH_RATIO_R1_R2, 2.f);//�м�Բ�뾶ƽ��
	float R3_pow = pow(radius * SAR_SIFT_GLOH_RATIO_R1_R3, 2.f);//��Բ�뾶ƽ��

	int sub_rows = sub_amplit.rows;
	int sub_cols = sub_amplit.cols;

	//��ʼ����������,�ڽǶȷ���������ӽ��в�ֵ
	int len = (d * 2 + 1) * n;
	AutoBuffer<float> hist(len);
	for (int i = 0; i < len; ++i)//����
		hist[i] = 0;

	for (int i = 0; i < sub_rows; ++i)
	{
		float* ptr_amplit = sub_amplit.ptr<float>(i);
		float* ptr_orient = sub_orient.ptr<float>(i);
		float* ptr_GLOH_amp = GLOH_amplit.ptr<float>(i);
		float* ptr_GLOH_ang = GLOH_angle.ptr<float>(i);
		for (int j = 0; j < sub_cols; ++j)
		{
			if (((i - center_y) * (i - center_y) + (j - center_x) * (j - center_x)) < radius * radius)
			{
				float pix_amplit = ptr_amplit[j];//�����ص��ݶȷ���
				float pix_orient = ptr_orient[j];//�����ص��ݶȷ���
				float pix_GLOH_amp = ptr_GLOH_amp[j];//��������GLOH�����еİ뾶λ��
				float pix_GLOH_ang = ptr_GLOH_ang[j];//��������GLOH�����е�λ�÷���

				int rbin, cbin, obin;
				rbin = pix_GLOH_amp < R3_pow ? 0 : (pix_GLOH_amp > R2_pow ? 2 : 1);//rbin={0,1,2}
				cbin = cvRound(pix_GLOH_ang * d / 360.f);
				cbin = cbin > d ? cbin - d : (cbin <= 0 ? cbin + d : cbin);//cbin=[1,d]

				obin = cvRound(pix_orient * n / 360.f);
				obin = obin > n ? obin - n : (obin <= 0 ? obin + n : obin);//obin=[1,n]

				if (rbin == 0)//��Բ
					hist[obin - 1] += pix_amplit;
				else
				{
					int idx = ((rbin - 1) * d + cbin - 1) * n + n + obin - 1;
					hist[idx] += pix_amplit;
				}
			}
		}
	}

	//�������ӽ��й�һ��
	float norm = 0;
	for (int i = 0; i < len; ++i)
	{
		norm = norm + hist[i] * hist[i];
	}
	norm = sqrt(norm);
	for (int i = 0; i < len; ++i)
	{
		hist[i] = hist[i] / norm;
	}

	//��ֵ�ض�
	for (int i = 0; i < len; ++i)
	{
		hist[i] = min(hist[i], DESCR_MAG_THR);
	}

	//�ٴι�һ��
	norm = 0;
	for (int i = 0; i < len; ++i)
	{
		norm = norm + hist[i] * hist[i];
	}
	norm = sqrt(norm);
	for (int i = 0; i < len; ++i)
	{
		ptr_des[i] = hist[i] / norm;
	}

}


/******************************����һ��������������ӡ��Ľ���***********************************/
static void improve_calc_sift_descriptor(const Mat& gauss_image, float main_angle, Point2f pt,
	float scale, int d, int n, float* descriptor)
{
	int n1 = 16, n2 = 6, n3 = 4;

	Point ptxy(cvRound(pt.x), cvRound(pt.y));					//����ȡ��

	float cos_t = cosf(-main_angle * (float)(CV_PI / 180));		//���������������
	float sin_t = sinf(-main_angle * (float)(CV_PI / 180));		//���������������

	float bin_per_rad_1 = n1 / 360.f;							//n=8
	float bin_per_rad_2 = n2 / 360.f;							//ԭ�������㲿����ֵ
	float bin_per_rad_3 = n3 / 360.f;							//ԭ�������㲿����ֵ

	float exp_scale = -1.f / (d * d * 0.5f);					//Ȩ��ָ������
	float hist_width = DESCR_SCL_FCTR * scale;					//������߳�������������Ҳ���������ص����

	int radius = cvRound(hist_width * (d + 1) * sqrt(2) * 0.5f);//����������뾶(d+1)*(d+1)

	int rows = gauss_image.rows, cols = gauss_image.cols;

	//����������뾶
	radius = min(radius, (int)sqrt((double)rows * rows + cols * cols));

	cos_t = cos_t / hist_width;
	sin_t = sin_t / hist_width;

	int len = (2 * radius + 1) * (2 * radius + 1);				//��������������
	int histlen = (d + 2) * (d + 2) * (n1 + 2);

	AutoBuffer<float> buf(6 * len + histlen);

	//X����ˮƽ��֣�Y������ֱ��֣�Mag�����ݶȷ��ȣ�Angle���������㷽��, W�����˹Ȩ��
	float* X = buf, * Y = buf + len, * Mag = Y, * Angle = Y + len, * W = Angle + len;
	float* X2 = buf, * Y2 = buf + len, * Mag2 = Y, * Angle2 = Y + len, * W2 = Angle + len;
	float* X3 = buf, * Y3 = buf + len, * Mag3 = Y, * Angle3 = Y + len, * W3 = Angle + len;

	float* RBin = W + len, * CBin = RBin + len, * hist = CBin + len;
	float* RBin2 = W + len, * CBin2 = RBin + len;
	float* RBin3 = W + len, * CBin3 = RBin + len;

	//�������ֱ��ͼ����
	for (int i = 0; i < d + 2; ++i)
	{
		for (int j = 0; j < d + 2; ++j)
		{
			for (int k = 0; k < n + 2; ++k)
				hist[(i * (d + 2) + j) * (n + 2) + k] = 0.f;
		}
	}

	//�������ڵ����ط��䵽��Ӧ�������ڣ�������������ÿ�����ص��Ȩ��(������ d*d ��ÿһ��С����)

	int k1 = 0, k2 = 0, k3 = 0;

	vector<int> v;									//������������ص��Ӧ�����

	for (int i = -radius; i < radius; ++i)
	{
		for (int j = -radius; j < radius; ++j)
		{
			float c_rot = j * cos_t - i * sin_t;	//��ת�������ڲ������ x ����
			float r_rot = j * sin_t + i * cos_t;	//��ת�������ڲ������ y ����

			float rbin = r_rot + d / 2 - 0.5f;		//��ת��Ĳ���������������� y ����
			float cbin = c_rot + d / 2 - 0.5f;		//��ת��Ĳ���������������� x ����

			int r = ptxy.y + i, c = ptxy.x + j;		//ptxy�Ǹ�˹�������е�����

			//�������ĵ���Ĳ��ֽ��в���
			if (abs(i) < (radius / 3) && abs(j) < (radius / 3))
			{
				//����rbin,cbin��Χ��(-1,d)
				if (rbin > -1 && rbin < d && cbin>-1 && cbin < d &&
					r>0 && r < rows - 1 && c>0 && c < cols - 1)
				{
					float dx = gauss_image.at<float>(r, c + 1) - gauss_image.at<float>(r, c - 1);
					float dy = gauss_image.at<float>(r + 1, c) - gauss_image.at<float>(r - 1, c);

					X[k1] = dx;								//�������������ص��ˮƽ���
					Y[k1] = dy;								//�������������ص����ֱ���

					RBin[k1] = rbin;						//���������в���������������� y ����
					CBin[k1] = cbin;						//���������в���������������� x ����

					//��˹Ȩֵ��ָ������
					W[k1] = (c_rot * c_rot + r_rot * r_rot) * exp_scale;

					++k1;
				}
			}
			//�������ĵ�Զ�Ĳ��ֽ��в���
			else if (abs(i) < (2 * radius / 3) && abs(i) > (radius / 3) && abs(j) < (2 * radius / 3) && abs(j) > (radius / 3))
			{
				//����rbin,cbin��Χ��(-1,d)
				if (rbin > -1 && rbin < d && cbin>-1 && cbin < d &&
					r>0 && r < rows - 1 && c>0 && c < cols - 1)
				{
					float dx = gauss_image.at<float>(r, c + 1) - gauss_image.at<float>(r, c - 1);
					float dy = gauss_image.at<float>(r + 1, c) - gauss_image.at<float>(r - 1, c);

					X2[k2] = dx;								//�������������ص��ˮƽ���
					Y2[k2] = dy;								//�������������ص����ֱ���

					RBin2[k2] = rbin;							//���������в���������������� y ����
					CBin2[k2] = cbin;							//���������в���������������� x ����

					//��˹Ȩֵ��ָ������
					W2[k2] = (c_rot * c_rot + r_rot * r_rot) * exp_scale;

					++k2;
				}
			}
			else
			{
				//����rbin,cbin��Χ��(-1,d)
				if (rbin > -1 && rbin < d && cbin>-1 && cbin < d &&
					r>0 && r < rows - 1 && c>0 && c < cols - 1)
				{
					float dx = gauss_image.at<float>(r, c + 1) - gauss_image.at<float>(r, c - 1);
					float dy = gauss_image.at<float>(r + 1, c) - gauss_image.at<float>(r - 1, c);

					X3[k3] = dx;								//�������������ص��ˮƽ���
					Y3[k3] = dy;								//�������������ص����ֱ���

					RBin3[k3] = rbin;							//���������в���������������� y ����
					CBin3[k3] = cbin;							//���������в���������������� x ����

					//��˹Ȩֵ��ָ������
					W3[k3] = (c_rot * c_rot + r_rot * r_rot) * exp_scale;

					++k3;
				}
			}
		}
	}

	//��������������ĺϲ�ƴ��
	for (int k = 0; k < k2; k++)
	{
		X[k1 + k] = X2[k];
		Y[k1 + k] = Y2[k];

		RBin[k1 + k] = RBin2[k];
		CBin[k1 + k] = CBin2[k];

		W[k1 + k] = W2[k];

	}

	for (int k = 0; k < k3; k++)
	{
		X[k1 + k2 + k] = X3[k];
		Y[k1 + k2 + k] = Y3[k];

		RBin[k1 + k2 + k] = RBin3[k];
		CBin[k1 + k2 + k] = CBin3[k];

		W[k1 + k2 + k] = W3[k];

	}

	//�������������������������ݶȷ��ȣ��ݶȽǶȣ��͸�˹Ȩֵ
	len = k1 + k2 + k3;

	cv::hal::exp(W, W, len);						//���������в�������������������صĸ�˹Ȩֵ
	cv::hal::fastAtan2(Y, X, Angle, len, true);		//���������в�������������������ص��ݶȷ��򣬽Ƕȷ�Χ��0-360��
	cv::hal::magnitude(X, Y, Mag, len);				//���������в�������������������ص��ݶȷ���

	//����ÿ�������������������
	for (int k = 0; k < len; ++k)
	{
		float rbin = RBin[k], cbin = CBin[k];		//�����������ص����꣬rbin,cbin��Χ��(-1,d)

		//���������������
		if (k < k1)
		{
			//�����������ص㴦���ķ���
			float obin = (Angle[k] - main_angle) * bin_per_rad_1;

			float mag = Mag[k] * W[k];					//�����������ص����Ȩֵ����ݶȷ�ֵ

			int r0 = cvFloor(rbin);						//roȡֵ������{-1,0,1,2��3}������ȡ��
			int c0 = cvFloor(cbin);						//c0ȡֵ������{-1��0��1��2��3}
			int o0 = cvFloor(obin);

			rbin = rbin - r0;							//�����������ص������С�����֣��������Բ�ֵ
			cbin = cbin - c0;
			obin = obin - o0;

			//���Ʒ�ΧΪ�ݶ�ֱ��ͼ������[0,n)
			if (o0 < 0)
				o0 = o0 + n1;
			if (o0 >= n1)
				o0 = o0 - n1;

			//�����Բ�ֵ���ڼ�����������������֮������ض���������������ã���������䵽��Ӧ�������8��������
			//ʹ�������Բ�ֵ(����ά)����������ֱ��ͼ
			float v_r1 = mag * rbin;					//�ڶ��з����ֵ
			float v_r0 = mag - v_r1;					//��һ�з����ֵ


			float v_rc11 = v_r1 * cbin;					//�ڶ��еڶ��з����ֵ
			float v_rc10 = v_r1 - v_rc11;				//�ڶ��е�һ�з����ֵ

			float v_rc01 = v_r0 * cbin;					//��һ�еڶ��з����ֵ
			float v_rc00 = v_r0 - v_rc01;


			float v_rco111 = v_rc11 * obin;				//�ڶ��еڶ��еڶ��������Ϸ����ֵ��ÿ��������ȥ�ڽ���������
			float v_rco110 = v_rc11 - v_rco111;			//�ڶ��еڶ��е�һ�������Ϸ����ֵ

			float v_rco101 = v_rc10 * obin;
			float v_rco100 = v_rc10 - v_rco101;

			float v_rco011 = v_rc01 * obin;
			float v_rco010 = v_rc01 - v_rco011;

			float v_rco001 = v_rc00 * obin;
			float v_rco000 = v_rc00 - v_rco001;

			//�������������������
			int idx = ((r0 + 1) * (d + 2) + c0 + 1) * (n1 + 2) + o0;

			hist[idx] += v_rco000;
			hist[idx + 1] += v_rco001;
			hist[idx + n1 + 2] += v_rco010;
			hist[idx + n1 + 3] += v_rco011;
			hist[idx + (d + 2) * (n1 + 2)] += v_rco100;
			hist[idx + (d + 2) * (n1 + 2) + 1] += v_rco101;
			hist[idx + (d + 3) * (n1 + 2)] += v_rco110;
			hist[idx + (d + 3) * (n1 + 2) + 1] += v_rco111;
		}

		//��������Զ������
		else if (k >= k1 && k < k2)
		{
			//�����������ص㴦���ķ���
			float obin = (Angle[k] - main_angle) * bin_per_rad_2;

			float mag = Mag[k] * W[k];					//�����������ص����Ȩֵ����ݶȷ�ֵ

			int r0 = cvFloor(rbin);						//roȡֵ������{-1,0,1,2��3}������ȡ��
			int c0 = cvFloor(cbin);						//c0ȡֵ������{-1��0��1��2��3}
			int o0 = cvFloor(obin);

			rbin = rbin - r0;							//�����������ص������С�����֣��������Բ�ֵ
			cbin = cbin - c0;
			obin = obin - o0;

			//���Ʒ�ΧΪ�ݶ�ֱ��ͼ������[0,n)
			if (o0 < 0)
				o0 = o0 + n2;
			if (o0 >= n1)
				o0 = o0 - n2;

			//�����Բ�ֵ���ڼ�����������������֮������ض���������������ã���������䵽��Ӧ�������8��������
			//ʹ�������Բ�ֵ(����ά)����������ֱ��ͼ
			float v_r1 = mag * rbin;					//�ڶ��з����ֵ
			float v_r0 = mag - v_r1;					//��һ�з����ֵ


			float v_rc11 = v_r1 * cbin;					//�ڶ��еڶ��з����ֵ
			float v_rc10 = v_r1 - v_rc11;				//�ڶ��е�һ�з����ֵ

			float v_rc01 = v_r0 * cbin;					//��һ�еڶ��з����ֵ
			float v_rc00 = v_r0 - v_rc01;


			float v_rco111 = v_rc11 * obin;				//�ڶ��еڶ��еڶ��������Ϸ����ֵ��ÿ��������ȥ�ڽ���������
			float v_rco110 = v_rc11 - v_rco111;			//�ڶ��еڶ��е�һ�������Ϸ����ֵ

			float v_rco101 = v_rc10 * obin;
			float v_rco100 = v_rc10 - v_rco101;

			float v_rco011 = v_rc01 * obin;
			float v_rco010 = v_rc01 - v_rco011;

			float v_rco001 = v_rc00 * obin;
			float v_rco000 = v_rc00 - v_rco001;

			//�������������������
			int idx = ((r0 + 1) * (d + 2) + c0 + 1) * (n2 + 2) + o0;

			hist[idx] += v_rco000;
			hist[idx + 1] += v_rco001;
			hist[idx + n2 + 2] += v_rco010;
			hist[idx + n2 + 3] += v_rco011;
			hist[idx + (d + 2) * (n2 + 2)] += v_rco100;
			hist[idx + (d + 2) * (n2 + 2) + 1] += v_rco101;
			hist[idx + (d + 3) * (n2 + 2)] += v_rco110;
			hist[idx + (d + 3) * (n2 + 2) + 1] += v_rco111;
		}
		else
		{
			//�����������ص㴦���ķ���
			float obin = (Angle[k] - main_angle) * bin_per_rad_3;

			float mag = Mag[k] * W[k];					//�����������ص����Ȩֵ����ݶȷ�ֵ

			int r0 = cvFloor(rbin);						//roȡֵ������{-1,0,1,2��3}������ȡ��
			int c0 = cvFloor(cbin);						//c0ȡֵ������{-1��0��1��2��3}
			int o0 = cvFloor(obin);

			rbin = rbin - r0;							//�����������ص������С�����֣��������Բ�ֵ
			cbin = cbin - c0;
			obin = obin - o0;

			//���Ʒ�ΧΪ�ݶ�ֱ��ͼ������[0,n)
			if (o0 < 0)
				o0 = o0 + n3;
			if (o0 >= n1)
				o0 = o0 - n3;

			//�����Բ�ֵ���ڼ�����������������֮������ض���������������ã���������䵽��Ӧ�������8��������
			//ʹ�������Բ�ֵ(����ά)����������ֱ��ͼ
			float v_r1 = mag * rbin;					//�ڶ��з����ֵ
			float v_r0 = mag - v_r1;					//��һ�з����ֵ


			float v_rc11 = v_r1 * cbin;					//�ڶ��еڶ��з����ֵ
			float v_rc10 = v_r1 - v_rc11;				//�ڶ��е�һ�з����ֵ

			float v_rc01 = v_r0 * cbin;					//��һ�еڶ��з����ֵ
			float v_rc00 = v_r0 - v_rc01;


			float v_rco111 = v_rc11 * obin;				//�ڶ��еڶ��еڶ��������Ϸ����ֵ��ÿ��������ȥ�ڽ���������
			float v_rco110 = v_rc11 - v_rco111;			//�ڶ��еڶ��е�һ�������Ϸ����ֵ

			float v_rco101 = v_rc10 * obin;
			float v_rco100 = v_rc10 - v_rco101;

			float v_rco011 = v_rc01 * obin;
			float v_rco010 = v_rc01 - v_rco011;

			float v_rco001 = v_rc00 * obin;
			float v_rco000 = v_rc00 - v_rco001;

			//�������������������
			int idx = ((r0 + 1) * (d + 2) + c0 + 1) * (n3 + 2) + o0;

			hist[idx] += v_rco000;
			hist[idx + 1] += v_rco001;
			hist[idx + n3 + 2] += v_rco010;
			hist[idx + n3 + 3] += v_rco011;
			hist[idx + (d + 2) * (n3 + 2)] += v_rco100;
			hist[idx + (d + 2) * (n3 + 2) + 1] += v_rco101;
			hist[idx + (d + 3) * (n3 + 2)] += v_rco110;
			hist[idx + (d + 3) * (n3 + 2) + 1] += v_rco111;
		}
	}

	//����Բ��ѭ�������ԣ��Լ����Ժ����С�� 0 �Ȼ���� 360 �ȵ�ֵ���½��е�����ʹ
	//���� 0��360 ��֮��
	for (int i = 0; i < d; ++i)
	{
		for (int j = 0; j < d; ++j)
		{
			int idx = ((i + 1) * (d + 2) + (j + 1)) * (n + 2);
			hist[idx] += hist[idx + n];
			//hist[idx + 1] += hist[idx + n + 1];//opencvԴ������仰�Ƕ����,hist[idx + n + 1]��Զ��0.0
			for (int k = 0; k < n; ++k)
				descriptor[(i * d + j) * n + k] = hist[idx + k];
		}
	}

	//�������ӽ��й�һ��
	int lenght = d * d * n;
	float norm = 0;

	//������������������ģֵ��ƽ��
	for (int i = 0; i < lenght; ++i)
	{
		norm = norm + descriptor[i] * descriptor[i];
	}
	norm = sqrt(norm);							//��������������ģֵ

	//�˴ι�һ����ȥ�����յ�Ӱ��
	for (int i = 0; i < lenght; ++i)
	{
		descriptor[i] = descriptor[i] / norm;
	}

	//��ֵ�ض�,ȥ���������������д��� 0.2 ��ֵ�������������Թ��յ�Ӱ��(������Ͷȶ�ĳЩ�ŵ��ݶ�Ӱ��ϴ󣬶Է����Ӱ���С)
	for (int i = 0; i < lenght; ++i)
	{
		descriptor[i] = min(descriptor[i], DESCR_MAG_THR);
	}

	//�ٴι�һ�����ܹ���������Ķ�����
	norm = 0;

	for (int i = 0; i < lenght; ++i)
	{
		norm = norm + descriptor[i] * descriptor[i];
	}
	norm = sqrt(norm);
	for (int i = 0; i < lenght; ++i)
	{
		descriptor[i] = descriptor[i] / norm;
	}
}


/********************************�ú���������������������������***************************/
/*gauss_pyr��ʾ��˹������
  keypoints��ʾ�����㡢
  descriptors��ʾ���ɵ��������������*/
void mySift::calc_sift_descriptors(const vector<vector<Mat>>& gauss_pyr, vector<KeyPoint>& keypoints,
	Mat& descriptors, const vector<Mat>& amplit, const vector<Mat>& orient)
{
	int d = DESCR_WIDTH;										//d=4,�������������������d x d
	int n = DESCR_HIST_BINS;									//n=8,ÿ�������������ݶȽǶȵȷ�Ϊ8������

	descriptors.create(keypoints.size(), d * d * n, CV_32FC1);	//����ռ�

	for (size_t i = 0; i < keypoints.size(); ++i)				//����ÿһ��������
	{
		int octaves, layer;

		//�õ����������ڵ���ţ����
		octaves = keypoints[i].octave & 255;
		layer = (keypoints[i].octave >> 8) & 255;

		//�õ�����������ڱ�������꣬������ײ�
		Point2f pt(keypoints[i].pt.x / (1 << octaves), keypoints[i].pt.y / (1 << octaves));

		float scale = keypoints[i].size / (1 << octaves);		//�õ�����������ڱ���ĳ߶�
		float main_angle = keypoints[i].angle;					//������������

		//����õ��������
		calc_sift_descriptor(gauss_pyr[octaves][layer], main_angle, pt, scale, d, n, descriptors.ptr<float>((int)i));
		//improve_calc_sift_descriptor(gauss_pyr[octaves][layer], main_angle, pt, scale, d, n, descriptors.ptr<float>((int)i));
		//calc_gloh_descriptor(amplit[octaves], orient[octaves], pt, scale, main_angle, d, n, descriptors.ptr<float>((int)i));

		if (double_size)//���ͼ��ߴ�����һ��
		{
			keypoints[i].pt.x = keypoints[i].pt.x / 2.f;
			keypoints[i].pt.y = keypoints[i].pt.y / 2.f;
		}
	}

}


/*************�ú�������SAR_SIFT�߶ȿռ�*****************/
/*image��ʾ�����ԭʼͼ��
 sar_harris_fun��ʾ�߶ȿռ��Sar_harris����
 amplit��ʾ�߶ȿռ����ص��ݶȷ���
 orient��ʾ�߶ȿռ����ص��ݶȷ���
 */
void mySift::build_sar_sift_space(const Mat& image, vector<Mat>& sar_harris_fun, vector<Mat>& amplit, vector<Mat>& orient)
{
	//ת������ͼ���ʽ
	Mat gray_image;
	if (image.channels() != 1)
		cvtColor(image, gray_image, CV_RGB2GRAY);
	else
		gray_image = image;

	//��ͼ��ת��Ϊ0-1֮��ĸ�������
	Mat float_image;

	double ratio = pow(2, 1.0 / 3.0);							//��������ĳ߶ȱ�,Ĭ����2^(1/3)

	//������ת��Ϊ0-1֮��ĸ������ݺ�ת��Ϊ0-255֮��ĸ������ݣ�Ч����һ����
	//gray_image.convertTo(float_image, CV_32FC1, 1.f / 255.f, 0.f);//ת��Ϊ0-1֮��

	gray_image.convertTo(float_image, CV_32FC1, 1, 0.f);		//ת��Ϊ0-255֮��ĸ�����

	//�����ڴ�
	sar_harris_fun.resize(Mmax);
	amplit.resize(Mmax);
	orient.resize(Mmax);

	for (int i = 0; i < Mmax; ++i)
	{
		float scale = (float)sigma * (float)pow(ratio, i);		//��õ�ǰ��ĳ߶�
		int radius = cvRound(2 * scale);
		Mat kernel;

		roewa_kernel(radius, scale, kernel);

		//�ĸ��˲�ģ������
		Mat W34 = Mat::zeros(2 * radius + 1, 2 * radius + 1, CV_32FC1);
		Mat W12 = Mat::zeros(2 * radius + 1, 2 * radius + 1, CV_32FC1);
		Mat W14 = Mat::zeros(2 * radius + 1, 2 * radius + 1, CV_32FC1);
		Mat W23 = Mat::zeros(2 * radius + 1, 2 * radius + 1, CV_32FC1);

		kernel(Range(radius + 1, 2 * radius + 1), Range::all()).copyTo(W34(Range(radius + 1, 2 * radius + 1), Range::all()));
		kernel(Range(0, radius), Range::all()).copyTo(W12(Range(0, radius), Range::all()));
		kernel(Range::all(), Range(radius + 1, 2 * radius + 1)).copyTo(W14(Range::all(), Range(radius + 1, 2 * radius + 1)));
		kernel(Range::all(), Range(0, radius)).copyTo(W23(Range::all(), Range(0, radius)));

		//�˲�
		Mat M34, M12, M14, M23;
		double eps = 0.00001;
		filter2D(float_image, M34, CV_32FC1, W34, Point(-1, -1), eps);
		filter2D(float_image, M12, CV_32FC1, W12, Point(-1, -1), eps);
		filter2D(float_image, M14, CV_32FC1, W14, Point(-1, -1), eps);
		filter2D(float_image, M23, CV_32FC1, W23, Point(-1, -1), eps);

		//����ˮƽ�ݶȺ���ֱ�ݶ�
		Mat Gx, Gy;
		log((M14) / (M23), Gx);
		log((M34) / (M12), Gy);

		//�����ݶȷ��Ⱥ��ݶȷ���
		magnitude(Gx, Gy, amplit[i]);
		phase(Gx, Gy, orient[i], true);

		//����sar-Harris����
		//Mat Csh_11 = log(scale)*log(scale)*Gx.mul(Gx);
		//Mat Csh_12 = log(scale)*log(scale)*Gx.mul(Gy);
		//Mat Csh_22 = log(scale)*log(scale)*Gy.mul(Gy);

		Mat Csh_11 = scale * scale * Gx.mul(Gx);
		Mat Csh_12 = scale * scale * Gx.mul(Gy);
		Mat Csh_22 = scale * scale * Gy.mul(Gy);//��ʱ��ֵΪ0.8

		//Mat Csh_11 = Gx.mul(Gx);
		//Mat Csh_12 = Gx.mul(Gy);
		//Mat Csh_22 = Gy.mul(Gy);//��ʱ��ֵΪ0.8/100

		//��˹Ȩ��
		float gauss_sigma = sqrt(2.f) * scale;
		int size = cvRound(3 * gauss_sigma);

		Size kern_size(2 * size + 1, 2 * size + 1);
		GaussianBlur(Csh_11, Csh_11, kern_size, gauss_sigma, gauss_sigma);
		GaussianBlur(Csh_12, Csh_12, kern_size, gauss_sigma, gauss_sigma);
		GaussianBlur(Csh_22, Csh_22, kern_size, gauss_sigma, gauss_sigma);

		/*Mat gauss_kernel;//�Զ���Բ�θ�˹��
		gauss_circle(size, gauss_sigma, gauss_kernel);
		filter2D(Csh_11, Csh_11, CV_32FC1, gauss_kernel);
		filter2D(Csh_12, Csh_12, CV_32FC1, gauss_kernel);
		filter2D(Csh_22, Csh_22, CV_32FC1, gauss_kernel);*/

		Mat Csh_21 = Csh_12;

		//����sar_harris����
		Mat temp_add = Csh_11 + Csh_22;

		double d = 0.04;					//sar_haiirs�������ʽ�е����������Ĭ����0.04

		sar_harris_fun[i] = Csh_11.mul(Csh_22) - Csh_21.mul(Csh_12) - (float)d * temp_add.mul(temp_add);
	}
}


/***************�ú��������������������������*************/
/*amplit��ʾ�߶ȿռ����ط���
 orient��ʾ�߶ȿռ������ݶȽǶ�
 keys��ʾ��⵽��������
 descriptors��ʾ��������������������M x N��,M��ʾ�����Ӹ�����N��ʾ������ά��
 */
void mySift::calc_gloh_descriptors(const vector<Mat>& amplit, const vector<Mat>& orient, const vector<KeyPoint>& keys, Mat& descriptors)
{
	int d = SAR_SIFT_GLOH_ANG_GRID;							//d=4����d=8
	int n = SAR_SIFT_DES_ANG_BINS;							//n=8Ĭ��

	int num_keys = (int)keys.size();
	int grids = 2 * d + 1;

	//descriptors.create(num_keys, grids * n, CV_32FC1);
	descriptors.create(num_keys, grids * n, CV_32FC1);

	for (int i = 0; i < num_keys; ++i)
	{
		int octaves = keys[i].octave & 255;					//���������ڲ�

		float* ptr_des = descriptors.ptr<float>(i);
		float scale = keys[i].size / (1 << octaves);		//�õ�����������ڱ���ĳ߶�;							//���������ڲ�ĳ߶�
		float main_ori = keys[i].angle;						//������������

		//�õ�����������ڱ�������꣬������ײ�
		Point2f point(keys[i].pt.x / (1 << octaves), keys[i].pt.y / (1 << octaves));

		cout << "layer=" << octaves << endl;
		cout << "scale=" << scale << endl;

		//����������������������
		calc_gloh_descriptor(amplit[octaves], orient[octaves], point, scale, main_ori, d, n, ptr_des);
	}
}


//������������������������� SIFT ���Ӷ�����������//

/******************************��������*********************************/
/*image��ʾ�����ͼ��
  gauss_pyr��ʾ���ɵĸ�˹������
  dog_pyr��ʾ���ɵĸ�˹���DOG������
  keypoints��ʾ��⵽��������
  vector<float>& cell_contrast ���ڴ��һ����Ԫ��������������ĶԱȶ�
  vector<float>& cell_contrasts���ڴ��һ���߶Ȳ������е�Ԫ����������ĶԱȶ�
  vector<vector<vector<float>>>& all_cell_contrasts���ڴ�����г߶Ȳ������е�Ԫ��ĶԱȶ�
  vector<vector<float>>& average_contrast���ڴ�����г߶Ȳ��ж��е�Ԫ���ƽ���Աȶ�*/
void mySift::detect(const Mat& image, vector<vector<Mat>>& gauss_pyr, vector<vector<Mat>>& dog_pyr, vector<KeyPoint>& keypoints,
	vector<vector<vector<float>>>& all_cell_contrasts,
	vector<vector<float>>& average_contrast, vector<vector<int>>& n_cells, vector<int>& num_cell, vector<vector<int>>& available_n,
	vector<int>& available_num, vector<KeyPoint>& final_keypoints,
	vector<KeyPoint>& Final_keypoints, vector<KeyPoint>& Final_Keypoints)
{
	if (image.empty() || image.depth() != CV_8U)
		CV_Error(CV_StsBadArg, "����ͼ��Ϊ�գ�����ͼ����Ȳ���CV_8U");


	//�����˹����������
	int nOctaves;
	nOctaves = num_octaves(image);

	//���ɸ�˹��������һ��ͼ��
	Mat init_gauss;
	create_initial_image(image, init_gauss);

	//���ɸ�˹�߶ȿռ�ͼ��
	build_gaussian_pyramid(init_gauss, gauss_pyr, nOctaves);

	//���ɸ�˹��ֽ�����(DOG��������or LOG������)
	build_dog_pyramid(dog_pyr, gauss_pyr);

	//��DOG�������ϼ��������
	find_scale_space_extrema1(dog_pyr, gauss_pyr, keypoints);			//ԭʼ SIFT ����
	// UR_SIFT,�����Ǽ�������㣬��δ�Բ�����ĵ����ɸѡ
/*UR_SIFT_find_scale_space_extrema(dog_pyr, gauss_pyr, keypoints, all_cell_contrasts,
average_contrast, n_cells, num_cell, available_n, available_num, final_keypoints, Final_keypoints, Final_Keypoints, N);*/

//���ָ�������������,�����趨������С��Ĭ�ϼ������������
	if (nfeatures != 0 && nfeatures < (int)keypoints.size())
	{
		//��������Ӧֵ(���Աȶȣ��Աȶ�ԽСԽ���ȶ�)�Ӵ�С����
		sort(keypoints.begin(), keypoints.end(), [](const KeyPoint& a, const KeyPoint& b) {return a.response > b.response; });

		//ɾ��������������
		keypoints.erase(keypoints.begin() + nfeatures, keypoints.end());
	}
}


/**********************����������*******************/
/*gauss_pyr��ʾ��˹������
 keypoints��ʾ�����㼯��
 descriptors��ʾ�������������*/
void mySift::comput_des(const vector<vector<Mat>>& gauss_pyr, vector<KeyPoint>& final_keypoints, const vector<Mat>& amplit,
	const vector<Mat>& orient, Mat& descriptors)
{
	calc_sift_descriptors(gauss_pyr, final_keypoints, descriptors, amplit, orient);
}

