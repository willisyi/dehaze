//#include <math.h>
typedef unsigned char UInt8;
void EqualizeHist( unsigned char* src, unsigned char* dst,int width,int height )
{
    int x,y;
   //int hist_sz = 256;
    int hist[256];
    memset(hist, 0, sizeof(hist));
    int size = width*height;
//统计每个灰度值出现的次数
    for(x=0;x<size;x++)
            hist[*(src+x)]++;

    
    float scale = 255.f/(width*height);
    int sum = 0;
    UInt8 lut[257];

    for(x = 0; x < 256; x++ )
    {
        sum += hist[x];
        //int val = round(sum*scale);
	int val = sum*scale+0.5;
	//int val0 = val< 16 ? 16: val > 235? 235 : val;
        lut[x] = val;
    }

    lut[0] = 0;
    for( y = 0; y < size; y++ )
    {
            *(dst+y) = lut[*(src+y)];
    }
}
