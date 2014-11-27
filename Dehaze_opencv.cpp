/*参考：
http://www.cnblogs.com/Imageshop/p/3410279.html
*/

#include<iostream>
#include<cv.h>
#include<time.h>
#include<highgui.h>
#include <imgproc/imgproc.hpp>
using namespace std;
#include "DarkCh.h"
#include "Others.h"
#include "EqualizeHist.h"

static IplImage *src=NULL;
static IplImage *dst=NULL;

static unsigned char *H;		//输入有雾图像
static unsigned char *D;		//去雾后图像
	IplImage *dstM=NULL;
	IplImage *dstM_ave=NULL;
	IplImage *dst_L=NULL;

	unsigned char *M;			//三通道最小值
	unsigned char *M_ave;
	unsigned char *L;

/*******************************************************************
* HistoStretch函数：直方图拉伸：选取图像每个通道最大的值，
* 大于最大值的像素设为255，小于最大值的像素值乘以（255/max)
********************************************************************/
void HistoStretch(unsigned char* &res)
{
	int i,j;
	int max1=0,max2=0,max3=0;
  	for(i=0;i<src->height;i++) 
	{
		int tmp=i*src->widthStep;
		//unsigned char* ImgPt=H+i*src->widthStep;
		for(j=0;j<src ->width;j++)			//寻找RGB通道中max
		{	
			if (max1<res[tmp])   max1=res[tmp];
			if (max2<res[tmp+1]) max2=res[tmp+1];
			if (max3<res[tmp+2]) max3=res[tmp+2];
			tmp +=3;
		}
		//if (max1==255) break;
	}

	if (max1==255 && max2==255 && max3==255) return;
	
	double index1=255.0/max1;
	double index2=255.0/max2;
	double index3=255.0/max3;


	int value1[256];
	int value2[256];
	int value3[256];
	for ( i = 0; i < 256; i ++)
	{
		value1[i] = CheckValue((int)(i * index1));
		value2[i] = CheckValue((int)(i * index2));
		value3[i] = CheckValue((int)(i * index3));
	}
	for(  i=0;i<src->height;i++) /*求输入图像每个像素三通道的最小值，得到全局暗通道图像*/
	{
		int tmp=i*src->widthStep;
        for(int j=0;j<src->width;j++)
			{	//i*src->widthStep+3*j
				res[tmp] =value1[res[tmp]];
				res[tmp+1] =value2[res[tmp+1]];
				res[tmp+2] =value3[res[tmp+2]];
				tmp +=3;		
			}
	}
	/*for(  i=0;i<dst->height;i++) 
	{
		int tmp=i*dst->widthStep;
		for(int j=0;j<dst ->width;j++)
		{	
			if (max1<res[tmp])   res[tmp]=255;
			else	res[tmp]=255.0/max1*res[tmp];
			if (max2<res[tmp+1]) res[tmp+1]=255;
			else	res[tmp+1]=255.0/max2*res[tmp+1];
			if (max3<res[tmp+2]) res[tmp+2]=255;
			else	res[tmp+2]=255.0/max3*res[tmp+2];
			tmp +=3;
		}
		//if (max1==255) break;
	}*/
	
//cout << max1 <<' ' << max2 << ' ' << max3 << endl;
}
/*******************************************************************
* LineBright函数：亮度调整   rgb = RGB + RGB * (1 / (1 - value) - 1)
********************************************************************/
void LineBright(unsigned char* &imgBuf,int bright)
{
	int i,j;
	if (bright == 0 )
		return;

	double bv = bright <= -255? -1.0 : bright / 255.0;
	if (bright > 0 && bright < 255)
		bv = 1.0 / (1.0 - bv) - 1.0;

	int values[256];
	for (int i = 0; i < 256; i ++)
	{
		values[i] = CheckValue(i + (int)(i * bv+0.5 ));
	}
	for( i=0;i<src->height;i++) /*求输入图像每个像素三通道的最小值，得到全局暗通道图像*/
	{
		int tmp=i*src->widthStep;
        for(j=0;j<src->width;j++)
			{	//i*src->widthStep+3*j
				imgBuf[tmp] =values[imgBuf[tmp]];
				imgBuf[tmp+1] =values[imgBuf[tmp+1]];
				imgBuf[tmp+2] =values[imgBuf[tmp+2]];
				tmp +=3;			
			}
	}
}
/**************************************************************************
* Dehaze函数：去雾处理
@ H 输入,三通道
@ D 输出 
M暗通道
M_ave
L
内存需要：M+M_ave+L = width*height*3
***************************************************************************/
void Dehaze(unsigned char* H,unsigned char* M,unsigned char* M_ave,unsigned char* L,
			int nChannels,double p,int width,int height,unsigned char* D)	
//void Dehaze(unsigned char* H,
//			int nChannels,double p,int width,int height,unsigned char* D)	
{	
	/****求取M(x)，M(x)=min(H_c(x)) 即求输入图像每个像素三通道的最小值,产生最小值矩阵M，若H为灰度图像则M=H******/
	int sum=0;
	double m_av=0;
	int min=0;
	int i,j;
	double A=0;
	if(nChannels==3)
	{
		for(i=0;i<height;i++) 
		{
			int tmp=i*width*3;	//3 ch
			int tmp1=i*width;	//1 channel

			for(j=0;j<width;j++)
			{	
				min=H[tmp];
				if (min>H[tmp+1]) min=H[tmp+1];
				if (min>H[tmp+2]) min=H[tmp+2];
				M[tmp1]=min;
				sum += min;
				tmp +=3;
				tmp1++;
			}
		}
	}else
		M = H;	//灰度图像，第一通道
	
	double time;
	clock_t start, finish;
	start = clock();
	//FastBlur(M,M_ave,width,height,25);

	int *Acol = new int [width];meanfilter32(M,25,width,height,Acol,M_ave);//Acol为一行大小的辅助空间
	finish = clock();
	time = (double)(finish - start) / CLK_TCK;
	printf( "快速均值模糊：%f seconds\n", time );
	m_av=(double)sum/(width*height*255);//M所有元素的均值
	double min_p=(p*m_av) < 0.9 ? (p*m_av) : 0.9;
		/***求出环境光：L。（x）***/
	for(i=0;i<height;i++) 
	{
		int tmp1=i*width;
		for(j=0;j<width;j++)
		{	
			unsigned int temp;
			unsigned int temp_ave;
			temp=M[tmp1];
			temp_ave=M_ave[tmp1];
			int temp_value=min_p*temp_ave;
			min=temp_value < temp ? temp_value : temp;
			L[tmp1]=min;
			tmp1++;
		}
	}
	/*求出A*/
	/*
	int max1=0;
  	for(i=0;i<height;i++) 
	{
		int tmp=i*width*3;
		for(j=0;j<width;j++)
		{	
			if (max1<H[tmp])   max1=H[tmp];
			if (max1<H[tmp+1]) max1=H[tmp+1];
			if (max1<H[tmp+2]) max1=H[tmp+2]; 
			tmp +=3;
		}
		if (max1==255) break;
	}

	int max2=0;
	for(i=0;i<height;i++) 
	{
		int tmp1=i*width;
		//unsigned char* ImgPt1=M_ave+i*dstM_ave->widthStep;
		for(j=0;j<width;j++)
		{	
			if (max2<M_ave[tmp1])   max2=M_ave[tmp1];
			tmp1++;
		}
	}
	A=(max1+max2)/2;
	*/
	A=255;
	/*finish = clock();
	time = (double)(finish - start) / CLK_TCK;
	printf( "%f seconds\n", time );*/
	//A = 255;
//cout <<"max1,max2,A="<<A << endl;
	start = clock();

	/**计算去雾图像**/
	unsigned char * Table = (unsigned char*)malloc(256*256);//(unsigned char  *) malloc (256*256*sizeof(unsigned char));
	double InvA = 1/A;
	double Value=0;
	int Index=0;
	//建立查找表
	for (int Y = 0; Y < 256; Y++)
	{
		//Index=Y<<8;
		for (int X = 0; X < 256; X++)
		{
			Value = (Y-X) /(1-X*InvA);
			if (Value > 255)
				Value = 255;
			else if (Value < 0)
				Value = 0;
			Table[Index++]= Value;
		}
	}

	unsigned int r,g,b;
	unsigned int temp_L;
	for( i=0;i<height;i++) 
	{
		int tmp=i*width*3;
		int tmp1=i*width;		//L为单通道
		for(j=0;j<width;j++)
		{	
			r=H[tmp];
			g=H[tmp+1];
			b=H[tmp+2];

			temp_L=L[tmp1+j];

			//查表
			D[tmp]=Table[(r<<8)+temp_L];
			D[tmp+1]=Table[(g<<8)+temp_L];
			D[tmp+2]=Table[(b<<8)+temp_L];
			tmp +=3;
		}
	}
	free(Table);	
	finish = clock();
	time = (double)(finish - start) / CLK_TCK;
	printf( "generate result:%f seconds\n", time );
}
int main()
{
	double time;
	clock_t start, finish;

	cvNamedWindow( "src", 0 );
	cvNamedWindow( "M", 0 );
	cvNamedWindow( "M_ave", 0 );
	cvNamedWindow( "L", 0 );
	//cvMoveWindow("src", 0, 0); 
	cvNamedWindow( "dst", 0 );

	src=cvLoadImage("nupt.jpg");//3
//#define HIST
#ifdef HIST
	cvNamedWindow("gray",0);
	cvNamedWindow("EqualHist",0);
	IplImage *src2,*grayRet;
	src2 = cvLoadImage("nupt.jpg",0);
	grayRet = cvCreateImage(cvSize(src2->width,src2->height),IPL_DEPTH_8U,1);
	unsigned char *pgray = (unsigned char*)src2->imageData;
	unsigned char *pgrayRet=(unsigned char*)grayRet->imageData;
	start = clock();
	EqualizeHist(pgray,pgrayRet,src2->width,src2->height);
	finish = clock();
	time = (double)(finish - start) / CLK_TCK;
	printf( "EqualizeHist %f seconds\n", time );
	if(src == NULL|| src2==NULL){
		cout<<"read picture failed!\n";
		return -1;
	}
#endif


	dstM=cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);
	dstM_ave=cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);
	dst_L=cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);

	dst=cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,3);
	
	H= (unsigned char*)src->imageData;			//源图像数据3
	D=(unsigned char*)dst->imageData;			//目的图像数据3
		M= (unsigned char*)dstM->imageData;		//单通道
	M_ave= (unsigned char*)dstM_ave->imageData;	//单通道
	L= (unsigned char*)dst_L->imageData;		//单通道


	start = clock();

	Dehaze(H,M,M_ave,L,3,1.25,src->width,src->height,D );	
	
		//HistoStretch(D);
		//LineBrightAndContrast(D, 70, 0, 120);
		//LineBright(D,20);		//可选的加强
			
		finish = clock();
		time = (double)(finish - start) / CLK_TCK;
		printf( "%f seconds\n", time );
		//}
		cvSaveImage("dehaze.bmp",dst);
		//cout<<"write frame"<<endl;
		//cvWriteFrame( m_pVideoWriter, dst );
		cvShowImage("src",src);
		//dst->origin=1;
		cvShowImage("dst",dst);
		cvShowImage("M",dstM);
		cvShowImage("M_ave",dstM_ave);
		cvShowImage("L",dst_L);
#ifdef HIST
		cvShowImage("gray",src2);
		cvShowImage("EqualHist",grayRet);
#endif
		//cvWaitKey(40);
		//src = cvQueryFrame(capture);
	//}
	cvWaitKey(-1);
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&dstM);
	cvReleaseImage(&dstM_ave);
	cvReleaseImage(&dst_L);
}

