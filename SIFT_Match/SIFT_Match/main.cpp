#include"mySift.h"
#include"myDisplay.h"
#include"myMatch.h"
#include<direct.h>

#include<opencv2\highgui\highgui.hpp>
#include<opencv2\calib3d\calib3d.hpp>
#include<opencv2\imgproc\imgproc.hpp>

#include<fstream>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>

#include <Commdlg.h>
#include <tchar.h>

#define MAX_PATH  10000
#define _CRT_SECURE_NO_WARNINGS

using namespace cv;

//std::string UnicodeToANSI(const std::wstring& wstr)
//{
//	std::string ret;
//	std::mbstate_t state = {};
//	const wchar_t* src = wstr.data();
//	size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
//	if (static_cast<size_t>(-1) != len) {
//		std::unique_ptr< char[] > buff(new char[len + 1]);
//		len = std::wcsrtombs(buff.get(), &src, len, &state);
//		if (static_cast<size_t>(-1) != len) {
//			ret.assign(buff.get(), len);
//		}
//	}
//	return ret;
//}


int main(int argc, char* argv[])
{
	/********************** 1、读入数据 **********************/

	//-----------------浏览第一张图片-----------------------------------------
	OPENFILENAME ofn;			// 公共对话框结构
	TCHAR szFile1[MAX_PATH];		// 保存获取文件名称的缓冲区   
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile1;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile1);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0Image\0*.PNG;*.JPG\0"; //过滤规则
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = L"C:\\Program Files";	//指定默认路径
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	std::string sFolder1 = "";
	if (GetOpenFileName(&ofn))
	{
		//显示选择的文件。 
		sFolder1 = UnicodeToANSI(ofn.lpstrFile);
	}
	else
	{
		return false;
	}

	//-----------------浏览第二张图片-----------------------------------------
	/*OPENFILENAME ofn;	*/		// 公共对话框结构
	TCHAR szFile2[MAX_PATH];		// 保存获取文件名称的缓冲区   
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile2;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile2);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0Image\0*.PNG;*.JPG\0"; //过滤规则
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = L"C:\\Program Files";	//指定默认路径
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	std::string sFolder2 = "";
	if (GetOpenFileName(&ofn))
	{
		//显示选择的文件。 
		sFolder2 = UnicodeToANSI(ofn.lpstrFile);
	}
	else
	{
		return false;
	}
	//--------浏览文件，绝对地址读入string sFolder--------------------

	Mat image_1 = imread(sFolder1, -1);                             //flag=-1: 8bit original channel
	Mat image_2 = imread(sFolder2, -1);
	
	string change_model = "perspective";							//affine为仿射变换，初始为perspective

	string folderPath = ".\\image_save";
	_mkdir(folderPath.c_str());										//在当前目录下创建文件夹用于保存结果的图像

	double total_count_beg = (double)getTickCount();				//算法运行总时间开始计时

	mySift sift_1(0, 3, 0.04, 10, 1.6, true);						//类对象

	/********************** 1、参考图像特征点检测和描述 **********************/

	vector<vector<Mat>> gauss_pyr_1, dog_pyr_1;						//高斯金字塔和高斯差分金字塔
	vector<KeyPoint> keypoints_1;									//特征点
	vector<vector<vector<float>>> all_cell_contrasts_1;				//所有尺度层中所有单元格的对比度
	vector<vector<float>> average_contrast_1;						//所有尺度层中多有单元格的平均对比度
	vector<vector<int>> n_cells_1;									//所有尺度层，每一尺度层中所有单元格所需特征数
	vector<int> num_cell_1;											//当前尺度层中所有单元格中的所需特征数
	vector<vector<int>> available_n_1;								//所有尺度层，每一尺度层中所有单元格可得到特征数
	vector<int> available_num_1;									//一个尺度层中所有单元格中可用特征数量
	vector<KeyPoint> final_keypoints1;								//第一次筛选结果
	vector<KeyPoint> Final_keypoints1;								//第二次筛选结果
	vector<KeyPoint> Final_Keypoints1;								//第三次筛选结果

	Mat descriptors_1;												//特征描述子

	double detect_1 = (double)getTickCount();						//特征点检测运行时间开始计时


	sift_1.detect(image_1, gauss_pyr_1, dog_pyr_1, keypoints_1, all_cell_contrasts_1,
		average_contrast_1, n_cells_1, num_cell_1, available_n_1, available_num_1, final_keypoints1, Final_keypoints1, Final_Keypoints1);			//特征点检测

	cout << "参考图像检测出的总特征数 =" << keypoints_1.size() << endl;

	//getTickFrequency() 返回值为一秒的计时周期数，二者比值为特征点检测时间
	double detect_time_1 = ((double)getTickCount() - detect_1) / getTickFrequency();

	cout << "参考图像特征点检测时间是： " << detect_time_1 << "s" << endl;

	double comput_1 = (double)getTickCount();

	vector<Mat> sar_harris_fun_1;
	vector<Mat> amplit_1;
	vector<Mat> orient_1;

	sift_1.comput_des(gauss_pyr_1, keypoints_1, amplit_1, orient_1, descriptors_1);

	double comput_time_1 = ((double)getTickCount() - comput_1) / getTickFrequency();

	cout << "参考图像特征点描述时间是： " << comput_time_1 << "s" << endl;

	/********************** 1、待配准图像特征点检测和描述 **********************/

	vector<vector<Mat>> gauss_pyr_2, dog_pyr_2;
	vector<KeyPoint> keypoints_2;
	vector<vector<vector<float>>> all_cell_contrasts_2;				//所有尺度层中所有单元格的对比度
	vector<vector<float>> average_contrast_2;						//所有尺度层中多有单元格的平均对比度
	vector<vector<int>> n_cells_2;									//所有尺度层，每一尺度层中所有单元格所需特征数
	vector<int> num_cell_2;											//当前尺度层中所有单元格中的所需特征数
	vector<vector<int>> available_n_2;								//所有尺度层，每一尺度层中的一个单元格可得到特征数
	vector<int> available_num_2;									//一个尺度层中所有单元格中可用特征数量
	vector<KeyPoint> final_keypoints2;								//第一次筛选结果
	vector<KeyPoint> Final_keypoints2;								//第二次筛选结果
	vector<KeyPoint> Final_Keypoints2;								//第三次筛选结果

	Mat descriptors_2;

	double detect_2 = (double)getTickCount();

	sift_1.detect(image_2, gauss_pyr_2, dog_pyr_2, keypoints_2, all_cell_contrasts_2,
		average_contrast_2, n_cells_2, num_cell_2, available_n_2, available_num_2, final_keypoints2, Final_keypoints2, Final_Keypoints2);

	cout << "待配准图像检测出的总特征数 =" << keypoints_2.size() << endl;

	double detect_time_2 = ((double)getTickCount() - detect_2) / getTickFrequency();

	cout << "待配准图像特征点检测时间是： " << detect_time_2 << "s" << endl;

	double comput_2 = (double)getTickCount();

	vector<Mat> sar_harris_fun_2;
	vector<Mat> amplit_2;
	vector<Mat> orient_2;

	sift_1.comput_des(gauss_pyr_2, keypoints_2, amplit_2, orient_2, descriptors_2);

	double comput_time_2 = ((double)getTickCount() - comput_2) / getTickFrequency();

	cout << "待配准特征点描述时间是： " << comput_time_2 << "s" << endl;

	/********************** 1、最近邻与次近邻距离比匹配，两幅影像进行配准 **********************/

	myMatch mymatch;

	double match_time = (double)getTickCount();						//影像配准计时开始

	//knnMatch函数是DescriptorMatcher类的成员函数，FlannBasedMatcher是DescriptorMatcher的子类
	Ptr<DescriptorMatcher> matcher1 = new FlannBasedMatcher;
	Ptr<DescriptorMatcher> matcher2 = new FlannBasedMatcher;

	vector<vector<DMatch>> dmatchs;									//vector<DMatch>中存放的是一个描述子可能匹配的候选集
	vector<vector<DMatch>> dmatch1;
	vector<vector<DMatch>> dmatch2;

	matcher1->knnMatch(descriptors_1, descriptors_2, dmatchs, 2);

	cout << "距离比之前初始匹配点对个数是：" << dmatchs.size() << endl;

	Mat matched_lines;												//同名点连线
	vector<DMatch> init_matchs, right_matchs;						//用于存放正确匹配的点

	//该函数返回的是空间映射模型参数
	Mat homography = mymatch.match(image_1, image_2, dmatchs, keypoints_1, keypoints_2, change_model, right_matchs, matched_lines, init_matchs);


	double match_time_2 = ((double)getTickCount() - match_time) / getTickFrequency();

	cout << "特征点匹配花费时间是： " << match_time_2 << "s" << endl;
	cout << change_model << "变换矩阵是：" << endl;
	cout << homography << endl;

	/********************** 1、把正确匹配点坐标写入文件中 **********************/

	ofstream ofile;
	ofile.open(".\\position.txt");									//创建文件
	for (size_t i = 0; i < right_matchs.size(); ++i)
	{
		ofile << keypoints_1[right_matchs[i].queryIdx].pt << "   "
			<< keypoints_2[right_matchs[i].trainIdx].pt << endl;
	}

	/********************** 1、图像融合 **********************/

	double fusion_beg = (double)getTickCount();

	Mat fusion_image, mosaic_image, regist_image;
	mymatch.image_fusion(image_1, image_2, homography, fusion_image, regist_image);

	imwrite(".\\image_save\\融合后的图像.jpg", fusion_image);

	double fusion_time = ((double)getTickCount() - fusion_beg) / getTickFrequency();
	cout << "图像融合花费时间是： " << fusion_time << "s" << endl;

	double total_time = ((double)getTickCount() - total_count_beg) / getTickFrequency();
	cout << "总花费时间是： " << total_time << "s" << endl;

	/********************** 1、生成棋盘图，显示配准结果 **********************/

	//生成棋盘网格图像
	Mat chessboard_1, chessboard_2, mosaic_images;

	Mat image1 = imread(".\\image_save\\配准后的参考图像.jpg", -1);
	Mat image2 = imread(".\\image_save\\配准后的待配准图像.jpg", -1);

	mymatch.mosaic_map(image1, image2, chessboard_1, chessboard_2, mosaic_images, 50);

	imwrite(".\\image_save\\参考图像棋盘图像.jpg", chessboard_1);
	imwrite(".\\image_save\\待配准图像棋盘图像.jpg", chessboard_2);
	imwrite(".\\image_save\\两幅棋盘图像叠加.jpg", mosaic_images);

	return 0;
}