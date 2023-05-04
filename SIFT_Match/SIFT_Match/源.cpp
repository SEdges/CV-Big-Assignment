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
        // 检测关键点
        
        detector->detect(img1, keypoints1);
        detector->detect(img2, keypoints2);

        // 计算描述符
        Mat descriptors1, descriptors2;
        detector->compute(img1, keypoints1, descriptors1);
        detector->compute(img2, keypoints2, descriptors2);

        // 保存特征
        saveFeatures("features1.yml", descriptors1);
        saveFeatures("features2.yml", descriptors2);

        // 匹配描述符
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
//    // 读取图像
//    Mat img1 = imread("VGG_Oxford_graf01.jpg", IMREAD_GRAYSCALE);
//    Mat img2 = imread("VGG_Oxford_graf03.jpg", IMREAD_GRAYSCALE);
//    
//    // 创建SiftMatcher对象
//    SiftMatcher matcher;
//
//    // 匹配两个图像之间的特征
//    std::vector<DMatch> matches;
//    matcher.match(img1, img2, matches);
//
//    // 绘制匹配结果
//    Mat img_matches;
//    drawMatches(img1, matcher.keypoints1, img2, matcher.keypoints2, matches, img_matches);
//
//    // 保存匹配结果
//    imwrite("matches.jpg", img_matches);
//
//    imshow("Matches", img_matches);
//
//    // 获取窗口的大小
//    int width = getWindowImageRect("Matches").width;
//    int height = getWindowImageRect("Matches").height;
//
//    // 调整图像的大小
//    Mat img_matches_resized;
//    resize(img_matches, img_matches_resized, Size(width, height));
//
//    // 显示匹配结果
//    imshow("Matches", img_matches_resized);
//
//    waitKey(0);
//
//    return 0;
//}


