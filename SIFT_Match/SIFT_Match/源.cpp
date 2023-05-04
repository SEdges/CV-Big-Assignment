#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace cv;
using namespace cv::xfeatures2d;

class SiftMatcher
{
public:
    std::vector<KeyPoint> keypoints1, keypoints2;

    SiftMatcher() : detector(SIFT::create(120)) {}

    void match(const Mat& img1, const Mat& img2, std::vector<DMatch>& matches)
    {
        // ���ؼ���
        
        detector->detect(img1, keypoints1);
        detector->detect(img2, keypoints2);

        // ����������
        Mat descriptors1, descriptors2;
        detector->compute(img1, keypoints1, descriptors1);
        detector->compute(img2, keypoints2, descriptors2);

        // ��������
        saveFeatures("features1.yml", descriptors1);
        saveFeatures("features2.yml", descriptors2);

        // ƥ��������
        BFMatcher matcher;
        matcher.match(descriptors1, descriptors2, matches);
    }
    
    void saveFeatures(const std::string& filename, const cv::Mat& descriptors)
    {
        cv::FileStorage fs(filename, cv::FileStorage::WRITE);
        fs << "descriptors" << descriptors;
        fs.release();
    }

private:
    Ptr<SIFT> detector;
};

//int main()
//{
//    // ��ȡͼ��
//    Mat img1 = imread("VGG_Oxford_graf01.jpg", IMREAD_GRAYSCALE);
//    Mat img2 = imread("VGG_Oxford_graf03.jpg", IMREAD_GRAYSCALE);
//    
//    // ����SiftMatcher����
//    SiftMatcher matcher;
//
//    // ƥ������ͼ��֮�������
//    std::vector<DMatch> matches;
//    matcher.match(img1, img2, matches);
//
//    // ����ƥ����
//    Mat img_matches;
//    drawMatches(img1, matcher.keypoints1, img2, matcher.keypoints2, matches, img_matches);
//
//    // ����ƥ����
//    imwrite("matches.jpg", img_matches);
//
//    imshow("Matches", img_matches);
//
//    // ��ȡ���ڵĴ�С
//    int width = getWindowImageRect("Matches").width;
//    int height = getWindowImageRect("Matches").height;
//
//    // ����ͼ��Ĵ�С
//    Mat img_matches_resized;
//    resize(img_matches, img_matches_resized, Size(width, height));
//
//    // ��ʾƥ����
//    imshow("Matches", img_matches_resized);
//
//    waitKey(0);
//
//    return 0;
//}


