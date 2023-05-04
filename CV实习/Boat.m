%% Question One：SIFT特征提取
clear;
clc;
%读取两幅图片
img_1 = imread('VGG_Oxford_boat01.jpg');
img_2 = imread('VGG_Oxford_boat03.jpg');

%将图像转换为灰度图像并提取SIFT特征点
img_1=rgb2gray(img_1);
points_1 = detectSIFTFeatures(img_1);
img_2=rgb2gray(img_2);
points_2 = detectSIFTFeatures(img_2);

% 在图像中显示特征点
imshow(img_1);
hold on;
plot(points_1.selectStrongest(200),'showOrientation',true);
%绘出特征最强的200个点，若为"true"，绘制特征点方向
[features1, validPts1]=extractFeatures(img_1,points_1);

figure;
imshow(img_2);
hold on;
plot(points_2.selectStrongest(200),'showOrientation',true);
[features2, validPts2]=extractFeatures(img_2,points_2);

% 提取前200个最强的特征点信息
%n_features = 200;
%points_info = table(points_1.Location(1:n_features, :), ...
    %points_1.Scale(1:n_features), points_1.Orientation(1:n_features), ...
    %features1(1:n_features, :));

% 创建一个空的table
points_info1 = table();
points_info2 = table();

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

    % 将特征点信息添加到points_info表格中
    row1 = table(loc1(1), loc1(2), scale1, orient1, descriptor1);
    points_info1 = [points_info1; row1];

    row2 = table(loc2(1), loc2(2), scale2, orient2, descriptor2);
    points_info2 = [points_info2; row2];
end

% 将特征点信息写入Excel表格
filename = 'Boat.sift_features1.xlsx';
writetable(points_info1, filename);

filename = 'Boat.sift_features2.xlsx';
writetable(points_info2, filename);


%% Question Two：SIFT特征匹配
%匹配两幅图像中的特征点
indexPairs=matchFeatures(features1, features2, 'MaxRatio', 0.03, 'MatchThreshold', 50);
%'MaxRatio=0.03'：当最佳匹配和次优匹配之间的比率小于0.03时，才将匹配视为有效匹配
                 %这个值比较小，意味着较高的匹配准确性，但也会导致匹配点的数量减少
%'MatchThreshold=50'：当两个特征点之间的距离小于50时，它们才会被视为有效匹配
                    %该值越大，匹配点越多
matchedPoints1 = validPts1(indexPairs(:,1));
matchedPoints2 = validPts2(indexPairs(:,2));

%展示匹配图像
figure; 
showMatchedFeatures(img_1,img_2,matchedPoints1,matchedPoints2, ...
    'montage',PlotOptions={'ro','bo','g-'});
%第一幅图中的特征点用红色圆圈（ro）表示，第二幅图中的特征点用蓝色圆圈（bo）表示，连线用绿色直线（g-）表示
