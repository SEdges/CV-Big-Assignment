%计算并画出核线
function epipolar(F2,inliers,matchedPoints3,matchedPoints4,img_3Gray)
    plot(matchedPoints3.Location(inliers,1),matchedPoints4.Location(inliers,2),'g+');
    epiLines = epipolarLine(F2',matchedPoints4.Location(inliers,:));
    points = lineToBorderPoints(epiLines,size(img_3Gray));
    line(points(:,[1,3])',points(:,[2,4])');