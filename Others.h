#ifndef OTHERS__H
#define OTHERS__H



// 线性亮度/对比度调整
//void LineBrightAndContrast(unsigned char * &imgBuf, int bright, int contrast, int threshold)
//{
//	int i,j;
//	if (bright == 0 && contrast == 0)
//		return;
//
//	float bv = bright <= -255? -1.0f : bright / 255.0f;
//	if (bright > 0 && bright < 255)
//		bv = 1.0f / (1.0f - bv) - 1.0f;
//
//	float cv = contrast <= -255? -1.0f : contrast / 255.0f;
//	if (contrast > 0 && contrast < 255)
//		cv = 1.0f / (1.0f - cv) - 1.0f;
//
//	int values[256];
//	for (i = 0; i < 256; i ++)
//	{
//		int v = contrast > 0? CheckValue(i + (int)(i * bv + 0.5f)) : i;
//		if (contrast >= 255)
//			v = v >= threshold? 255 : 0;
//		else
//			v = CheckValue(v + (int)((v - threshold) * cv + 0.5f));
//		values[i] = contrast <= 0? CheckValue(v + (int)(v * bv + 0.5f)) : v;
//	}
//	for( i=0;i<src->height;i++) /*求输入图像每个像素三通道的最小值，得到全局暗通道图像*/
//	{
//		    int tmp=i*src->widthStep;
//            for(j=0;j<src->width;j++)
//				{	//i*src->widthStep+3*j
//					imgBuf[tmp] =values[imgBuf[tmp]];
//					imgBuf[tmp+1] =values[imgBuf[tmp+1]];
//					imgBuf[tmp+2] =values[imgBuf[tmp+2]];
//					tmp +=3;
//						
//				}
//		}
//}


#endif