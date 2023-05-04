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
	/********************** 1���������� **********************/

	//-----------------�����һ��ͼƬ-----------------------------------------
	OPENFILENAME ofn;			// �����Ի���ṹ
	TCHAR szFile1[MAX_PATH];		// �����ȡ�ļ����ƵĻ�����   
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile1;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile1);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0Image\0*.PNG;*.JPG\0"; //���˹���
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = L"C:\\Program Files";	//ָ��Ĭ��·��
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	std::string sFolder1 = "";
	if (GetOpenFileName(&ofn))
	{
		//��ʾѡ����ļ��� 
		sFolder1 = UnicodeToANSI(ofn.lpstrFile);
	}
	else
	{
		return false;
	}

	//-----------------����ڶ���ͼƬ-----------------------------------------
	/*OPENFILENAME ofn;	*/		// �����Ի���ṹ
	TCHAR szFile2[MAX_PATH];		// �����ȡ�ļ����ƵĻ�����   
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile2;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile2);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0Image\0*.PNG;*.JPG\0"; //���˹���
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = L"C:\\Program Files";	//ָ��Ĭ��·��
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	std::string sFolder2 = "";
	if (GetOpenFileName(&ofn))
	{
		//��ʾѡ����ļ��� 
		sFolder2 = UnicodeToANSI(ofn.lpstrFile);
	}
	else
	{
		return false;
	}
	//--------����ļ������Ե�ַ����string sFolder--------------------

	Mat image_1 = imread(sFolder1, -1);                             //flag=-1: 8bit original channel
	Mat image_2 = imread(sFolder2, -1);
	
	string change_model = "perspective";							//affineΪ����任����ʼΪperspective

	string folderPath = ".\\image_save";
	_mkdir(folderPath.c_str());										//�ڵ�ǰĿ¼�´����ļ������ڱ�������ͼ��

	double total_count_beg = (double)getTickCount();				//�㷨������ʱ�俪ʼ��ʱ

	mySift sift_1(0, 3, 0.04, 10, 1.6, true);						//�����

	/********************** 1���ο�ͼ��������������� **********************/

	vector<vector<Mat>> gauss_pyr_1, dog_pyr_1;						//��˹�������͸�˹��ֽ�����
	vector<KeyPoint> keypoints_1;									//������
	vector<vector<vector<float>>> all_cell_contrasts_1;				//���г߶Ȳ������е�Ԫ��ĶԱȶ�
	vector<vector<float>> average_contrast_1;						//���г߶Ȳ��ж��е�Ԫ���ƽ���Աȶ�
	vector<vector<int>> n_cells_1;									//���г߶Ȳ㣬ÿһ�߶Ȳ������е�Ԫ������������
	vector<int> num_cell_1;											//��ǰ�߶Ȳ������е�Ԫ���е�����������
	vector<vector<int>> available_n_1;								//���г߶Ȳ㣬ÿһ�߶Ȳ������е�Ԫ��ɵõ�������
	vector<int> available_num_1;									//һ���߶Ȳ������е�Ԫ���п�����������
	vector<KeyPoint> final_keypoints1;								//��һ��ɸѡ���
	vector<KeyPoint> Final_keypoints1;								//�ڶ���ɸѡ���
	vector<KeyPoint> Final_Keypoints1;								//������ɸѡ���

	Mat descriptors_1;												//����������

	double detect_1 = (double)getTickCount();						//������������ʱ�俪ʼ��ʱ


	sift_1.detect(image_1, gauss_pyr_1, dog_pyr_1, keypoints_1, all_cell_contrasts_1,
		average_contrast_1, n_cells_1, num_cell_1, available_n_1, available_num_1, final_keypoints1, Final_keypoints1, Final_Keypoints1);			//��������

	cout << "�ο�ͼ��������������� =" << keypoints_1.size() << endl;

	//getTickFrequency() ����ֵΪһ��ļ�ʱ�����������߱�ֵΪ��������ʱ��
	double detect_time_1 = ((double)getTickCount() - detect_1) / getTickFrequency();

	cout << "�ο�ͼ����������ʱ���ǣ� " << detect_time_1 << "s" << endl;

	double comput_1 = (double)getTickCount();

	vector<Mat> sar_harris_fun_1;
	vector<Mat> amplit_1;
	vector<Mat> orient_1;

	sift_1.comput_des(gauss_pyr_1, keypoints_1, amplit_1, orient_1, descriptors_1);

	double comput_time_1 = ((double)getTickCount() - comput_1) / getTickFrequency();

	cout << "�ο�ͼ������������ʱ���ǣ� " << comput_time_1 << "s" << endl;

	/********************** 1������׼ͼ��������������� **********************/

	vector<vector<Mat>> gauss_pyr_2, dog_pyr_2;
	vector<KeyPoint> keypoints_2;
	vector<vector<vector<float>>> all_cell_contrasts_2;				//���г߶Ȳ������е�Ԫ��ĶԱȶ�
	vector<vector<float>> average_contrast_2;						//���г߶Ȳ��ж��е�Ԫ���ƽ���Աȶ�
	vector<vector<int>> n_cells_2;									//���г߶Ȳ㣬ÿһ�߶Ȳ������е�Ԫ������������
	vector<int> num_cell_2;											//��ǰ�߶Ȳ������е�Ԫ���е�����������
	vector<vector<int>> available_n_2;								//���г߶Ȳ㣬ÿһ�߶Ȳ��е�һ����Ԫ��ɵõ�������
	vector<int> available_num_2;									//һ���߶Ȳ������е�Ԫ���п�����������
	vector<KeyPoint> final_keypoints2;								//��һ��ɸѡ���
	vector<KeyPoint> Final_keypoints2;								//�ڶ���ɸѡ���
	vector<KeyPoint> Final_Keypoints2;								//������ɸѡ���

	Mat descriptors_2;

	double detect_2 = (double)getTickCount();

	sift_1.detect(image_2, gauss_pyr_2, dog_pyr_2, keypoints_2, all_cell_contrasts_2,
		average_contrast_2, n_cells_2, num_cell_2, available_n_2, available_num_2, final_keypoints2, Final_keypoints2, Final_Keypoints2);

	cout << "����׼ͼ��������������� =" << keypoints_2.size() << endl;

	double detect_time_2 = ((double)getTickCount() - detect_2) / getTickFrequency();

	cout << "����׼ͼ����������ʱ���ǣ� " << detect_time_2 << "s" << endl;

	double comput_2 = (double)getTickCount();

	vector<Mat> sar_harris_fun_2;
	vector<Mat> amplit_2;
	vector<Mat> orient_2;

	sift_1.comput_des(gauss_pyr_2, keypoints_2, amplit_2, orient_2, descriptors_2);

	double comput_time_2 = ((double)getTickCount() - comput_2) / getTickFrequency();

	cout << "����׼����������ʱ���ǣ� " << comput_time_2 << "s" << endl;

	/********************** 1���������ν��ھ����ƥ�䣬����Ӱ�������׼ **********************/

	myMatch mymatch;

	double match_time = (double)getTickCount();						//Ӱ����׼��ʱ��ʼ

	//knnMatch������DescriptorMatcher��ĳ�Ա������FlannBasedMatcher��DescriptorMatcher������
	Ptr<DescriptorMatcher> matcher1 = new FlannBasedMatcher;
	Ptr<DescriptorMatcher> matcher2 = new FlannBasedMatcher;

	vector<vector<DMatch>> dmatchs;									//vector<DMatch>�д�ŵ���һ�������ӿ���ƥ��ĺ�ѡ��
	vector<vector<DMatch>> dmatch1;
	vector<vector<DMatch>> dmatch2;

	matcher1->knnMatch(descriptors_1, descriptors_2, dmatchs, 2);

	cout << "�����֮ǰ��ʼƥ���Ը����ǣ�" << dmatchs.size() << endl;

	Mat matched_lines;												//ͬ��������
	vector<DMatch> init_matchs, right_matchs;						//���ڴ����ȷƥ��ĵ�

	//�ú������ص��ǿռ�ӳ��ģ�Ͳ���
	Mat homography = mymatch.match(image_1, image_2, dmatchs, keypoints_1, keypoints_2, change_model, right_matchs, matched_lines, init_matchs);


	double match_time_2 = ((double)getTickCount() - match_time) / getTickFrequency();

	cout << "������ƥ�仨��ʱ���ǣ� " << match_time_2 << "s" << endl;
	cout << change_model << "�任�����ǣ�" << endl;
	cout << homography << endl;

	/********************** 1������ȷƥ�������д���ļ��� **********************/

	ofstream ofile;
	ofile.open(".\\position.txt");									//�����ļ�
	for (size_t i = 0; i < right_matchs.size(); ++i)
	{
		ofile << keypoints_1[right_matchs[i].queryIdx].pt << "   "
			<< keypoints_2[right_matchs[i].trainIdx].pt << endl;
	}

	/********************** 1��ͼ���ں� **********************/

	double fusion_beg = (double)getTickCount();

	Mat fusion_image, mosaic_image, regist_image;
	mymatch.image_fusion(image_1, image_2, homography, fusion_image, regist_image);

	imwrite(".\\image_save\\�ںϺ��ͼ��.jpg", fusion_image);

	double fusion_time = ((double)getTickCount() - fusion_beg) / getTickFrequency();
	cout << "ͼ���ںϻ���ʱ���ǣ� " << fusion_time << "s" << endl;

	double total_time = ((double)getTickCount() - total_count_beg) / getTickFrequency();
	cout << "�ܻ���ʱ���ǣ� " << total_time << "s" << endl;

	/********************** 1����������ͼ����ʾ��׼��� **********************/

	//������������ͼ��
	Mat chessboard_1, chessboard_2, mosaic_images;

	Mat image1 = imread(".\\image_save\\��׼��Ĳο�ͼ��.jpg", -1);
	Mat image2 = imread(".\\image_save\\��׼��Ĵ���׼ͼ��.jpg", -1);

	mymatch.mosaic_map(image1, image2, chessboard_1, chessboard_2, mosaic_images, 50);

	imwrite(".\\image_save\\�ο�ͼ������ͼ��.jpg", chessboard_1);
	imwrite(".\\image_save\\����׼ͼ������ͼ��.jpg", chessboard_2);
	imwrite(".\\image_save\\��������ͼ�����.jpg", mosaic_images);

	return 0;
}