// opencvtest.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"


#include<iostream>
#include<cmath>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//���ص�ͼƬ�ĵ�ַ
string addr="C:\\Users\\liuzhaochao\\Desktop\\Opencv.png";

RNG rng(12345);

//��ȡ���������ĵ�
Point Center_cal(vector<vector<Point> > contours,int i)
{
      int centerx=0,centery=0,n=contours[i].size();
      //����ȡ��С�����εı߽���ÿ���ܳ���������ȡһ��������꣬
      //������ȡ�ĸ����ƽ�����꣨��ΪС�����εĴ������ģ�
      centerx = (contours[i][n/4].x + contours[i][n*2/4].x + contours[i][3*n/4].x + contours[i][n-1].x)/4;
      centery = (contours[i][n/4].y + contours[i][n*2/4].y + contours[i][3*n/4].y + contours[i][n-1].y)/4;
      Point point1=Point(centerx,centery);
      return point1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Mat img=imread(addr);//��ȡͼƬ
	
	if(img.empty()){
		cout<<"δ�ҵ����ļ�\n";
		return -1;
	}

	Mat src_all=img.clone();
	namedWindow("opencvTest",CV_WINDOW_NORMAL);//����ָ�����ƵĴ���
	imshow("opencvTest",img);//��ͼƬ��ָ����������ʾ����

	Mat gray,binary;//�ҶȾ��󣬶����ƾ���
	cvtColor(img,gray,COLOR_BGR2GRAY);
	namedWindow("gray",CV_WINDOW_NORMAL);//����ָ�����ƵĴ���
	imshow("gray",gray);//��ͼƬ��ָ����������ʾ����

	blur(gray, gray,Size(3,3));//ģ����ȥ��ë��
	threshold(gray, binary,100, 255,THRESH_OTSU);
	namedWindow("binary",CV_WINDOW_NORMAL);//����ָ�����ƵĴ���
	imshow("binary",binary);//��ͼƬ��ָ����������ʾ����
	/****************************************\
			�Եõ��Ķ�����ͼ��binary���д���
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
 
    //ͨ����ɫ��λ����Ϊ�����������������������ص㣬ɸѡ��������λ��
    int parentIdx=-1;
    for( int i = 0; i< contours.size(); i++ )
    {
        //������������ͼ
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
 
        //������������
        if ( ic >= 2)
        {
            //�����ҵ���������ɫ��λ��
            contours2.push_back(contours[parentIdx]);
            //����������ɫ��λ�ǵ�����
            drawContours( drawing, contours, parentIdx,  CV_RGB(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255)) , 1, 8);
            ic = 0;
            parentIdx = -1;
        }
    }
 
    //���ķ�ʽ����������ɫ��λ�ǵ�����
    for(int i=0; i<contours2.size(); i++)
        drawContours( drawing2, contours2, i,  CV_RGB(rng.uniform(100,255),rng.uniform(100,255),rng.uniform(100,255)) , -1, 4, hierarchy[k][2], 0, Point() );
 
    //��ȡ������λ�ǵ���������
    Point point[3];
    for(int i=0; i<contours2.size(); i++)
    {
        point[i] = Center_cal( contours2, i );
    }
 
    //������������������㶨λ�ǵ�������Ӷ�������߳�
    area = contourArea(contours2[1]);
    int area_side = cvRound (sqrt (double(area)));
    for(int i=0; i<contours2.size(); i++)
    {
        //����������λ�ǵ���������
        line(drawing2,point[i%contours2.size()],point[(i+1)%contours2.size()],color,area_side/2,8);
    }
 
    namedWindow("DrawingAllContours",CV_WINDOW_NORMAL);
    imshow( "DrawingAllContours", drawingAllContours );
 
    namedWindow("Drawing2",CV_WINDOW_NORMAL);
    imshow( "Drawing2", drawing2 );
 
    namedWindow("Drawing",CV_WINDOW_NORMAL);
    imshow( "Drawing", drawing );
 
 
    //������Ҫ�����������ά��
    Mat gray_all,threshold_output_all;
    vector<vector<Point> > contours_all;
    vector<Vec4i> hierarchy_all;
    cvtColor( drawing2, gray_all, CV_BGR2GRAY );
 
 
    threshold( gray_all, threshold_output_all, 45, 255, THRESH_BINARY );
    findContours( threshold_output_all, contours_all, hierarchy_all,  RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0) );//RETR_EXTERNAL��ʾֻѰ�����������
 
 
    Point2f fourPoint2f[4];
    //����С��Χ����
    RotatedRect rectPoint = minAreaRect(contours_all[0]);
 
    //��rectPoint�����д洢������ֵ�ŵ� fourPoint��������
    rectPoint.points(fourPoint2f);
 
 
    for (int i = 0; i < 4; i++)
    {
        line(src_all, fourPoint2f[i%4], fourPoint2f[(i + 1)%4]
            , Scalar(20,21,237), 3);
    }
 
    namedWindow("Src_all",CV_WINDOW_NORMAL);
    imshow( "Src_all", src_all );
 
    //�����ά��󣬾Ϳ�����ȡ����ά�룬Ȼ��ʹ�ý����zxing����������Ϣ��
    //�����о���ά����Ų������Լ�д���벿��


	waitKey(0);
	//waitKey(6000);//�ȴ�6000ms�Զ��ر�
	getchar();
	return 0;
}

