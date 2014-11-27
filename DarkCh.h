#ifndef DARKCHANNEL__H
#define	DARKCHANNEL__H
#include <stdlib.h>
#include <time.h>
/*******************************************************************
* 确保值在0-255
********************************************************************/
inline int CheckValue(int value)
{
	return (value & ~0xff) == 0? value : value > 255? 255 : 0;
}
/*******************************************************************
* FastBlur函数：快速均值滤波
********************************************************************/
static void FastBlur(unsigned char* &pix,unsigned char* &out,int w, int h, int radius)
{
	//if (radius<1) return;
	int wm=w-1;
	int hm=h-1;
	int wh=w*h;
	int div=radius*2+1;		//radius*2+1
	unsigned char *r=new unsigned char[wh];
	int rsum,x,y,i,p,p1,p2,yp,yi,yw;
	int *vMIN = new int[MAX(w,h)];
	int *vMAX = new int[MAX(w,h)];

	unsigned char *dv=new unsigned char[256*div];
	for (i=0;i<256*div;i++) dv[i]=(i/div);
	yw=yi=0;
	for (y=0;y<h;y++)
	{
		rsum=0;
		for(i=-radius;i<=radius;i++)
		{
			p = (yi + MIN(wm, MAX(i,0))) ;
			rsum += pix[p];
		}
		for (x=0;x<w;x++)
		{
			r[yi]=dv[rsum];
			if(y==0)
			{
				vMIN[x]=MIN(x+radius+1,wm);
				vMAX[x]=MAX(x-radius,0);
			}
			p1 = (yw+vMIN[x]);//
			p2 = (yw+vMAX[x]);//

			rsum += pix[p1] - pix[p2];
			yi++;
		}
		yw+=w;
	}

	for (x=0;x<w;x++)
	{
		rsum=0;
		yp=-radius*w;
		for(i=-radius;i<=radius;i++)
		{
			yi=MAX(0,yp)+x;
			rsum+=r[yi];
			yp+=w;
		}
		yi=x;
		for(y=0;y<h;y++)
		{
			out[yi] = dv[rsum];
			if(x==0)
			{
				vMIN[y]=MIN(y+radius+1,hm)*w;
				vMAX[y]=MAX(y-radius,0)*w;
			}
			p1=x+vMIN[y];
			p2=x+vMAX[y];

			rsum+=r[p1]-r[p2];
			yi+=w;
		}
	}

	delete r;
	delete vMIN;
	delete vMAX;
	delete dv;
}

static void meanfilter32(unsigned char *imSrc,const int windows,const int width,const int height,int *Acol,unsigned char *imDst)
{
	int i,j,k,r,n,sum;
	r=(windows-1)/2;

	for(i=0; i<height; i++)
	{
		if(i==0)
		{
			for(k=0; k<width; k++)				
			{
				Acol[k]=imSrc[k];
				for(n=1; n<=r; n++)
					Acol[k]+=imSrc[n*width+k]*2;
			}
		}
		else									
		{
			if(i>0 && i<=r)
			{
				for(k=0; k<width; k++)
					Acol[k]=Acol[k]-imSrc[(r+1-i)*width+k]+imSrc[(i+r)*width+k];
			}
			else if(i>r && i<height-r)
			{
				for(k=0; k<width; k++)
					Acol[k]=Acol[k]-imSrc[(i-r-1)*width+k]+imSrc[(i+r)*width+k];
			}
			else
			{	
				for(k=0; k<width; k++)
					Acol[k]=Acol[k]-imSrc[(i-r-1)*width+k]+imSrc[(2*height-i-r-1)*width+k];
			}
		}
		sum=Acol[0];						
		for(n=1; n<=r; n++)
			sum+=2*Acol[n];
		imDst[i*width]=sum/(windows*windows);

		for(j=1; j<width; j++)				
		{
			if(j>=1 && j<r+1)
			{
				sum=sum-Acol[r+1-j]+Acol[j+r];
				imDst[i*width+j]=sum/(windows*windows);
			}
			else if(j>=r+1 && j<width-r)
			{
				sum=sum-Acol[j-1-r]+Acol[j+r];
				imDst[i*width+j]=sum/(windows*windows);
			}
			else
			{
				sum=sum-Acol[j-1-r]+Acol[2*width-j-r-1];
				imDst[i*width+j]=sum/(windows*windows);
			}
		}
	}
}



#endif