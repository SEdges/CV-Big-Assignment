%% Question One：SIFT特征提取
%清除工作区和命令行窗口
clear;
clc;

%读取四幅图片
img_1 = imread('homography_1.png');
img_2 = imread('homography_2.png');
img_3 = imread('fundamental_3.png');
img_4 = imread('fundamental_4.png');

%将图像转换为灰度图像并提取SIFT特征点
img_1Gray=rgb2gray(img_1);
points_1 = detectSIFTFeatures(img_1Gray);
img_2Gray=rgb2gray(img_2);
points_2 = detectSIFTFeatures(img_2Gray);
img_3Gray=rgb2gray(img_3);
points_3 = detectSIFTFeatures(img_3Gray);
img_4Gray=rgb2gray(img_4);
points_4 = detectSIFTFeatures(img_4Gray);

% 在图像中显示特征点
imshow(img_1);
hold on;
plot(points_1.selectStrongest(200),'showOrientation',true);
%绘出特征最强的200个点，若为"true"，绘制特征点方向
[features1, validPts1]=extractFeatures(img_1Gray,points_1);

figure;
imshow(img_2);
hold on;
plot(points_2.selectStrongest(200),'showOrientation',true);
[features2, validPts2]=extractFeatures(img_2Gray,points_2);

figure;
imshow(img_3);
hold on;
plot(points_3.selectStrongest(200),'showOrientation',true);
[features3, validPts3]=extractFeatures(img_3Gray,points_3);

figure;
imshow(img_4);
hold on;
plot(points_4.selectStrongest(200),'showOrientation',true);
[features4, validPts4]=extractFeatures(img_4Gray,points_4);

% 创建一个空的table
points_info1 = table();
points_info2 = table();
points_info3 = table();
points_info4 = table();


% 提取前200个最强的特征点信息
n_features = 200;
for i = 1:n_features
    % 从points_1中获取第i个特征点的信息
    loc1 = points_1.Location(i, :);
    scale1 = points_1.Scale(i);
    orient1 = points_1.Metric(i);
    %不使用points_1.Orientation：features1中已经包含了每个特征点的方向信息，
    %使用points_1.Orientation会使Excel表格中方向都变为0
    descriptor1 = features1(i, :);
    
    loc2 = points_2.Location(i, :);
    scale2 = points_2.Scale(i);
    orient2 = points_2.Metric(i);
    descriptor2 = features2(i, :);

    loc3 = points_3.Location(i, :);
    scale3 = points_3.Scale(i);
    orient3 = points_3.Metric(i);
    descriptor3 = features3(i, :);

    loc4 = points_4.Location(i, :);
    scale4 = points_4.Scale(i);
    orient4 = points_4.Metric(i);
    descriptor4 = features4(i, :);

    % 将特征点信息添加到points_info表格中
    row1 = table(loc1(1), loc1(2), scale1, orient1, descriptor1);
    points_info1 = [points_info1; row1];

    row2 = table(loc2(1), loc2(2), scale2, orient2, descriptor2);
    points_info2 = [points_info2; row2];

    row3 = table(loc3(1), loc3(2), scale3, orient3, descriptor3);
    points_info3 = [points_info3; row3];

    row4 = table(loc4(1), loc4(2), scale4, orient4, descriptor4);
    points_info4 = [points_info4; row4];


end

% 将特征点信息写入Excel表格
filename = 'H&F.sift_features1.xlsx';
writetable(points_info1, filename);

filename = 'H&F.sift_features2.xlsx';
writetable(points_info2, filename);

filename = 'H&F.sift_features3.xlsx';
writetable(points_info3, filename);

filename = 'H&F.sift_features4.xlsx';
writetable(points_info4, filename);

%% Question Two：SIFT特征匹配
%匹配两幅图像中的特征点
indexPairs1=matchFeatures(features1, features2, ...
    'MaxRatio', 0.1, 'MatchThreshold', 50);
indexPairs2=matchFeatures(features3, features4, ...
    'MaxRatio', 0.2, 'MatchThreshold', 50);
%为了保证匹配的图像有较好的视觉效果，两组图片最终的MaxRatio的值都是通过多次尝试得出来的

matchedPoints1 = validPts1(indexPairs1(:,1));
matchedPoints2 = validPts2(indexPairs1(:,2));
matchedPoints3 = validPts3(indexPairs2(:,1));
matchedPoints4 = validPts4(indexPairs2(:,2));

%展示匹配图像
figure; 
showMatchedFeatures(img_1,img_2,matchedPoints1,matchedPoints2, ...
    'montage',PlotOptions={'ro','bo','g-'});

figure; 
showMatchedFeatures(img_3,img_4,matchedPoints3,matchedPoints4, ...
    'montage',PlotOptions={'ro','bo','g-'});

%% Question Three：自动估计约束
%先计算H矩阵，算出标准点比例
[H1,inliers1,p1,p2,in_res1]=RANSAC(matchedPoints1.Location',matchedPoints2.Location',1,4,1000);
[H2,inliers2,p3,p4,in_res2]=RANSAC(matchedPoints3.Location',matchedPoints4.Location',1,4,1000);
r1=inliers1/matchedPoints1.Count;
r2=inliers2/matchedPoints3.Count;
%根据标准点比例，判断是否是单应性约束
if (r1<0.5)%小于0.5是基础矩阵，大于0.5是单应性矩阵
    load stereoPointPairs
    [F1,inliers_]=estimateFundamentalMatrix(matchedPoints1,matchedPoints2,Method="RANSAC",NumTrials=1000);
    %计算F矩阵
    figure;
    imshow(img_1);
    hold on;
    epipolar(F1,inliers_,matchedPoints1,matchedPoints2,img_1Gray);

    figure;
    imshow(img_2);
    hold on;
    epipolar(F1,inliers_,matchedPoints1,matchedPoints2,img_2Gray);
else
    %此处不再进行任何操作，因为若是H约束，之前就已经处理过了
end

if (r2<0.5)
    load stereoPointPairs
     [F2,inliers_]=estimateFundamentalMatrix(matchedPoints3,matchedPoints4,Method="RANSAC",NumTrials=1000);

     figure;
     imshow(img_3);
     hold on;
     epipolar(F2,inliers_,matchedPoints3,matchedPoints4,img_3Gray);

     figure;
     imshow(img_4);
     hold on;
     epipolar(F2,inliers_,matchedPoints3,matchedPoints4,img_4Gray);
else
end












