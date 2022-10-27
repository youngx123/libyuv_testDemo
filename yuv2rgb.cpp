#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "libyuv.h"
#include<chrono>
#include <opencv2/opencv.hpp>	

/*
使用libyuv库对yuyv(yuy2)格式进行转化,主要包括：
yuy2 -> ARGB

ARGB -> I420

I420 - I420Scale
*/


void yuyv2RGB(uint8_t * yuyv, cv::Mat &rgbMat, int width, int height)
{

        libyuv::YUY2ToARGB( 
                                                yuyv,
                                                width*2, 

                                                rgbMat.data, 
                                                width*4,

                                                width,
                                                height
                                        );
        
        cv::Mat tmp = rgbMat.clone();
        cv::Mat BGRA_mat = cv::Mat(height, width, CV_8UC4, cv::Scalar::all(0));
        cv::cvtColor(tmp, BGRA_mat, cv::COLOR_BGRA2BGR);
	cv::imwrite("./I422_bgra.jpg", BGRA_mat);
}


void rgb2I420_Scale(cv::Mat &rgbMat, int width, int height, float scale_value)
{
	uint8_t* yuvI420 = new uint8_t[width * height * 3 / 2];

        uint8_t* dst_y = yuvI420;
	int dst_y_stride = width;

	uint8_t* dst_u = yuvI420 + width * height;
	int dst_u_stride = (width + 1) / 2;

	uint8_t* dst_v = (yuvI420 + width * height) + dst_u_stride * ((height + 1) / 2);
	int dst_v_stride = (width + 1) / 2;

        libyuv::ARGBToI420(
                                        rgbMat.data, 
                                        width * 4, 
                                        
                                        dst_y,
                                        dst_y_stride, 
                                        
                                        dst_u, 
                                        dst_u_stride, 
                                        
                                        dst_v, 
                                        dst_v_stride, 
                                        
                                        width,                                          
                                        height
                                );

        int dstWidth = (int)width*scale_value;    
        int dstHeight= (int)height*scale_value;  

       std::cout<<"----- I420  scale to width  :  "<< dstWidth << "       height   :  " <<dstHeight<<std::endl;

        uint8_t *yuv420_Scale = new uint8_t[dstWidth *dstHeight*3 / 2];

        uint8_t* scale_dst_y = yuv420_Scale;
	int scale_dst_y_stride = dstWidth;

	uint8_t* scale_dst_u = yuv420_Scale + dstWidth*dstHeight;
	int scale_dst_u_stride = (dstWidth+1)/2;

	uint8_t* scale_dst_v = (yuv420_Scale + dstWidth*dstHeight) + scale_dst_u_stride * ((dstHeight + 1) / 2);
	int scale_dst_v_stride =  (dstWidth+1)/2;

        libyuv::I420Scale(
                                        dst_y, 
                                        dst_y_stride,
                                        
                                         dst_u, 
                                         dst_u_stride, 
                                         
                                         dst_v, 
                                         dst_v_stride,
                                         
                                         width, 
                                         height,
                                        
                                        scale_dst_y,
                                        scale_dst_y_stride,

                                        scale_dst_u,
                                        scale_dst_u_stride,

                                       scale_dst_v,
                                       scale_dst_v_stride,

                                        dstWidth,
                                        dstHeight,

                                        libyuv::kFilterBilinear
                                );

        // I420  scale 
        cv::Mat  matI420_scale = cv::Mat(dstHeight, dstWidth, CV_8UC4, cv::Scalar::all(0));
	libyuv::I420ToARGB(
                                                scale_dst_y,
                                                 scale_dst_y_stride, 
                                                 
                                                 scale_dst_u, 
                                                 scale_dst_u_stride, 

                                                scale_dst_v, 
                                                scale_dst_v_stride,

						matI420_scale.data, 
                                                dstWidth * 4, 
                                                
                                                dstWidth, 
                                                dstHeight
                                        );
	cv::imwrite("./yuyv2I420_bgra_scale.jpg", matI420_scale);
        
}


int main() 
{
        int width = 1280, height = 720;
        float scale_value = 0.25;
        int dstWidth = (int)width*scale_value;    
        int dstHeight= (int)height*scale_value;  

        int framesize = width * height * 2;

        uint8_t *yuv422 = new uint8_t[framesize];

        std::cout<<"next read\n";
        FILE *yuv422File = fopen("./00316_3.yuyv", "rb");
        fread(yuv422, framesize * sizeof(uint8_t), 1, yuv422File);
        fclose(yuv422File);
   

        // yuy2 convert to ARGB and save image
        cv::Mat argb = cv::Mat(height, width, CV_8UC4, cv::Scalar::all(0));
        yuyv2RGB(yuv422, argb, width, height);

        rgb2I420_Scale(argb,  width,  height,  scale_value);


        std::cout<<"----- END \n";
        return 0;
}
