// opencvtest.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"


#include<iostream>
#include<cmath>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//加载的图片的地址
string addr="C:\\Users\\liuzhaochao\\Desktop\\Opencv.png";

RNG rng(12345);

//获取轮廓的中心点
Point Center_cal(vector<vector<Point> > contours,int i)
{
      int centerx=0,centery=0,n=contours[i].size();
      //在提取的小正方形的边界上每隔周长个像素提取一个点的坐标，
      //求所提取四个点的平均坐标（即为小正方形的大致中心）
      centerx = (contours[i][n/4].x + contours[i][n*2/4].x + contours[i][3*n/4].x + contours[i][n-1].x)/4;
      centery = (contours[i][n/4].y + contours[i][n*2/4].y + contours[i][3*n/4].y + contours[i][n-1].y)/4;
      Point point1=Point(centerx,centery);
      return point1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Mat img=imread(addr);//读取图片
	
	if(img.empty()){
		cout<<"未找到该文件\n";
		return -1;
	}

	Mat src_all=img.clone();
	namedWindow("opencvTest",CV_WINDOW_NORMAL);//创建指定名称的窗口
	imshow("opencvTest",img);//将图片在指定窗口上显示出来

	Mat gray,binary;//灰度矩阵，二进制矩阵
	cvtColor(img,gray,COLOR_BGR2GRAY);
	namedWindow("gray",CV_WINDOW_NORMAL);//创建指定名称的窗口
	imshow("gray",gray);//将图片在指定窗口上显示出来

	blur(gray, gray,Size(3,3));//模糊，去除毛刺
	threshold(gray, binary,100, 255,THRESH_OTSU);
	namedWindow("binary",CV_WINDOW_NORMAL);//创建指定名称的窗口
	imshow("binary",binary);//将图片在指定窗口上显示出来
	/****************************************\
			对得到的二进制图像binary进行处理
	\****************************************/
	
	//vector<vector<Point> > contours,contours2;
    //vector<Vec4i> hierarchy;
	
	
	Scalar color = Scalar(1,1,255 );
    Mat threshold_output;
    vector<vector<Point> > contours,contours2;
    vector<Vec4i> hierarchy;
    Mat drawing = Mat::zeros( img.size(), CV_8UC3 );
    Mat drawing2 = Mat::zeros( img.size(), CV_8UC3 );
    Mat drawingAllContours = Mat::zeros( img.size(), CV_8UC3 );

	Mat copy_binary=binary.clone();
	findContours(copy_binary,contours,hierarchy,CV_RETR_TREE,CHAIN_APPROX_NONE,Point(0,0));
	 int c=0,ic=0,k=0,area=0;
 
    //通过黑色定位角作为父轮廓，有两个子轮廓的特点，筛选出三个定位角
    int parentIdx=-1;
    for( int i = 0; i< contours.size(); i++ )
    {
        //画出所以轮廓图
        drawContours( drawingAllContours, contours, parentIdx,  CV_RGB(255,255,255) , 1, 8);
        if (hierarchy[i][2] != -1 && ic==0)
        {
            parentIdx = i;
            ic++;
        }
        else if (hierarchy[i][2] != -1)
        {
            ic++;
        }
        else if(hierarchy[i][2] == -1)
        {
            ic = 0;
            parentIdx = -1;
        }
 
        //有两个子轮廓
        if ( ic >= 2)
        {
            //保存找到的三个黑色定位角
            contours2.push_back(contours[parentIdx]);
            //画出三个黑色定位角的轮廓
            drawContours( drawing, contours, parentIdx,  CV_RGB(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255)) , 1, 8);
            ic = 0;
            parentIdx = -1;
        }
    }
 
    //填充的方式画出三个黑色定位角的轮廓
    for(int i=0; i<contours2.size(); i++)
        drawContours( drawing2, contours2, i,  CV_RGB(rng.uniform(100,255),rng.uniform(100,255),rng.uniform(100,255)) , -1, 4, hierarchy[k][2], 0, Point() );
 
    //获取三个定位角的中心坐标
    Point point[3];
    for(int i=0; i<contours2.size(); i++)
    {
        point[i] = Center_cal( contours2, i );
    }
 
    //计算轮廓的面积，计算定位角的面积，从而计算出边长
    area = contourArea(contours2[1]);
    int area_side = cvRound (sqrt (double(area)));
    for(int i=0; i<contours2.size(); i++)
    {
        //画出三个定位角的中心连线
        line(drawing2,point[i%contours2.size()],point[(i+1)%contours2.size()],color,area_side/2,8);
    }
 
    namedWindow("DrawingAllContours",CV_WINDOW_NORMAL);
    imshow( "DrawingAllContours", drawingAllContours );
 
    namedWindow("Drawing2",CV_WINDOW_NORMAL);
    imshow( "Drawing2", drawing2 );
 
    namedWindow("Drawing",CV_WINDOW_NORMAL);
    imshow( "Drawing", drawing );
 
 
    //接下来要框出这整个二维码
    Mat gray_all,threshold_output_all;
    vector<vector<Point> > contours_all;
    vector<Vec4i> hierarchy_all;
    cvtColor( drawing2, gray_all, CV_BGR2GRAY );
 
 
    threshold( gray_all, threshold_output_all, 45, 255, THRESH_BINARY );
    findContours( threshold_output_all, contours_all, hierarchy_all,  RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0) );//RETR_EXTERNAL表示只寻找最外层轮廓
 
 
    Point2f fourPoint2f[4];
    //求最小包围矩形
    RotatedRect rectPoint = minAreaRect(contours_all[0]);
 
    //将rectPoint变量中存储的坐标值放到 fourPoint的数组中
    rectPoint.points(fourPoint2f);
 
 
    for (int i = 0; i < 4; i++)
    {
        line(src_all, fourPoint2f[i%4], fourPoint2f[(i + 1)%4]
            , Scalar(20,21,237), 3);
    }
 
    namedWindow("Src_all",CV_WINDOW_NORMAL);
    imshow( "Src_all", src_all );
 
    //框出二维码后，就可以提取出二维码，然后使用解码库zxing，解出码的信息。
    //或者研究二维码的排布规则，自己写解码部分


	waitKey(0);
	//waitKey(6000);//等待6000ms自动关闭
	getchar();
	return 0;
}

