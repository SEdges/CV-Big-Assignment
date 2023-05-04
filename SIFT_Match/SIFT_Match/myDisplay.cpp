#include "myDisplay.h"

#include<opencv2\highgui\highgui.hpp>
#include<vector>
#include<sstream>

using namespace cv;

/***************************�ú����ѽ���������һ��ͼ����(�����˹��������DOG������)******************/
/*pyramid��ʾ��˹����������DOG������
 pyramid_image��ʾ���ɵĽ�����ͼ��
 nOctaveLayers��ʾÿ���м������Ĭ����3
 str��ʾ�Ǹ�˹����������DOG������
*/

std::string UnicodeToANSI(const std::wstring& wstr)
{
	std::string ret;
	std::mbstate_t state = {};
	const wchar_t* src = wstr.data();
	size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< char[] > buff(new char[len + 1]);
		len = std::wcsrtombs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	return ret;
}

void  myDisplay::mosaic_pyramid(const vector<vector<Mat>>& pyramid, Mat& pyramid_image, int nOctaceLayers, string str)
{
	//���ÿ�������ͼ���С
	vector<vector<int>> temp_size;
	for (auto beg = pyramid.cbegin(); beg != pyramid.cend(); ++beg)
	{
		vector<int> temp_vec;
		int rows = (*beg)[0].rows;
		int cols = (*beg)[0].cols;
		temp_vec.push_back(rows);
		temp_vec.push_back(cols);
		temp_size.push_back(temp_vec);
	}

	//����������ɵĽ�����ͼ��pyramid_image�Ĵ�С
	int total_rows = 0, total_cols = 0;
	for (auto beg = temp_size.begin(); beg != temp_size.end(); ++beg)
	{
		total_rows = total_rows + (*beg)[0];//��ȡ�д�С
		if (beg == temp_size.begin()) {
			if (str == string("��˹������"))
				total_cols = (nOctaceLayers + 3) * ((*beg)[1]);//��ȡ�д�С
			else if (str == string("DOG������"))
				total_cols = (nOctaceLayers + 2) * ((*beg)[1]);//��ȡ�д�С
		}
	}

	pyramid_image.create(total_rows, total_cols, CV_8UC1);
	int i = 0, accumulate_row = 0;
	for (auto beg = pyramid.cbegin(); beg != pyramid.cend(); ++beg, ++i)
	{
		int accumulate_col = 0;
		accumulate_row = accumulate_row + temp_size[i][0];
		for (auto it = (*beg).cbegin(); it != (*beg).cend(); ++it)
		{
			accumulate_col = accumulate_col + temp_size[i][1];
			Mat temp(pyramid_image, Rect(accumulate_col - temp_size[i][1], accumulate_row - temp_size[i][0], it->cols, it->rows));
			Mat temp_it;
			normalize(*it, temp_it, 0, 255, NORM_MINMAX, CV_32FC1);
			convertScaleAbs(temp_it, temp_it, 1, 0);
			temp_it.copyTo(temp);
		}
	}
}

/**************************�ú�������ƴ�Ӻ�ĸ�˹��������DOG������ͼ��**************************/
/*gauss_pyr_1��ʾ�ο���˹������
 dog_pyr_1��ʾ�ο�DOG������
 gauss_pyr_2��ʾ����׼��˹������
 dog_pyr_2��ʾ����׼DOG������
 nOctaveLayers��ʾ������ÿ���м����
 */
void myDisplay::write_mosaic_pyramid(const vector<vector<Mat>>& gauss_pyr_1, const vector<vector<Mat>>& dog_pyr_1,
	const vector<vector<Mat>>& gauss_pyr_2, const vector<vector<Mat>>& dog_pyr_2, int nOctaveLayers)
{

	//��ʾ�ο��ʹ���׼��˹������ͼ��
	Mat gauss_image_1, gauss_image_2;
	mosaic_pyramid(gauss_pyr_1, gauss_image_1, nOctaveLayers, string("��˹������"));
	mosaic_pyramid(gauss_pyr_2, gauss_image_2, nOctaveLayers, string("��˹������"));
	imwrite(".\\image_save\\�ο�ͼ���˹������.jpg", gauss_image_1);
	imwrite(".\\image_save\\����׼ͼ���˹������.jpg", gauss_image_2);

	//��ʾ�ο��ʹ���׼DOG������ͼ��
	Mat dog_image_1, dog_image_2;
	mosaic_pyramid(dog_pyr_1, dog_image_1, nOctaveLayers, string("DOG������"));
	mosaic_pyramid(dog_pyr_2, dog_image_2, nOctaveLayers, string("DOG������"));
	imwrite(".\\image_save\\�ο�ͼ��DOG������.jpg", dog_image_1);
	imwrite(".\\image_save\\����׼ͼ��DOG������.jpg", dog_image_2);
}