%用RANSAC求解单应性矩阵
%最大迭代次数在主函数中已经设定（1000）
function [H,num_inlier,point1,point2,in_res] = RANSAC(D1,D2,threshold,num,iternations)
D1(3,:)=1;
D2(3,:)=1;
n=size(D1,2);
best_n_inlier=0;
best_H=[];
for i=1:iternations
    s=randsample(n,num);
    x=D1(1,s);
    y=D1(2,s);
    xp=D2(1,s);
    yp=D2(2,s);
    P=[
        x(1),y(1),1,0,0,0,-x(1)*xp(1),-y(1)*xp(1),-xp(1);
        0,0,0,x(1),y(1),1,-x(1)*yp(1),-y(1)*yp(1),-yp(1);
        x(2),y(2),1,0,0,0,-x(2)*xp(2),-y(2)*xp(2),-xp(2);
        0,0,0,x(2),y(2),1,-x(2)*yp(2),-y(2)*yp(2),-yp(2);
        x(3),y(3),1,0,0,0,-x(3)*xp(3),-y(3)*xp(3),-xp(3);
        0,0,0,x(3),y(3),1,-x(3)*yp(3),-y(3)*yp(3),-yp(3);
        x(4),y(4),1,0,0,0,-x(4)*xp(4),-y(4)*xp(4),-xp(4);
        0,0,0,x(4),y(4),1,-x(4)*yp(4),-y(4)*yp(4),-yp(4);
        ];
    [~,~,V]=svd(P);
    h=V(:,9);
    H=[h(1),h(2),h(3);h(4),h(5),h(6);h(7),h(8),h(9)];

    D2_new=H*D1;
    D2_new(1,:)=D2_new(1,:)./D2_new(3,:);
    D2_new(2,:)=D2_new(2,:)./D2_new(3,:);
    D2_new(3,:)=D2_new(3,:)./D2_new(3,:);
    SD=sum((D2_new-D2).^2);
    inliers=find(SD<threshold);
    num_inlier=length(inliers);
    %更新最好的单应性矩阵和内点
    if(num_inlier>best_n_inlier)
        best_H=H;
        best_n_inlier=num_inlier;
        point1=D1(1:2,inliers);
        point2=D2(1:2,inliers);
        in_res=SD(inliers);
    end
end
H=best_H;
num_inlier=best_n_inlier;

end