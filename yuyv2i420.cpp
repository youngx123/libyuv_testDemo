#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "libyuv.h"
#include<chrono>
#include <opencv2/opencv.hpp>	

/*
使用libyuv 和Opencv对yuyv(yuy2)格式进行转化,主要包括：
opencv : yuy2   --> cv::Mat  -->  RGB -- > resize 

libyuv   : yuy2   --> I420  -->  I420Scale -- > ARGB 
*/

void opencvConvert(std::string yuvfile, int width, int height,  float scale_value=1)
{

	int framesize = width * height *2;
	FILE *yuv422File = fopen(yuvfile.c_str(), "rb");

	unsigned char *yuv422 = new uint8_t[framesize];
	fread(yuv422, framesize * sizeof( unsigned char), 1, yuv422File);
	fclose(yuv422File);


	int resizeWidth =  (int)width*scale_value;     // 360
	int resizeHeight=  (int)height*scale_value;   //180

	auto cv_start= std::chrono::steady_clock::now();

	cv::Mat yuvImg, rgbImg, rgbImg_scale;
	yuvImg.create(height, width, CV_8UC2);
	memcpy(yuvImg.data, yuv422, framesize * sizeof( unsigned char)) ;

	cv::cvtColor(yuvImg, rgbImg, cv::COLOR_YUV2BGR_YUYV);
        
	auto cvbgr_start= std::chrono::steady_clock::now();
	
	
	if (resizeHeight ==height){
			std::chrono::duration<double> rgbconvert_spent = cvbgr_start - cv_start;
			std::cout << "yuyv   convert  to  BGR :  " << rgbconvert_spent.count() * 1000 << "    ms\n\n ";
	}
	else {
		cv::resize(rgbImg, rgbImg_scale, cv::Size(resizeWidth, resizeHeight));
		auto cv_end= std::chrono::steady_clock::now();


		std::chrono::duration<double> rgbconvert_spent = cvbgr_start - cv_start;
		std::chrono::duration<double> rgbSclae_spent = cv_end - cvbgr_start;
		std::cout << "yuyv   convert  to  BGR :  " << rgbconvert_spent.count() * 1000 << "    ms\n ";
		std::cout << "BGR   scale  time :  " << rgbSclae_spent.count() * 1000 << "    ms\n ";
		std::cout << "yuyv   -> BGR  -> Scale :  " << (rgbSclae_spent.count() + rgbconvert_spent.count()) * 1000 << "    ms\n\n ";
	}

	if (resizeHeight !=height)
	{
		cv::imwrite("./yuyv2_rgb_cv_scale.jpg", rgbImg_scale);
		cv::imwrite("./yuyv2_rgb_cv.jpg", rgbImg);
}
	else{
		cv::imwrite("./yuyv2_rgb_cv.jpg", rgbImg);
	}
}



void yuvLibConvert(std::string yuvfile, int width, int height,  float scale_value=1.)
{

	int framesize = width * height ;
	FILE *yuv422File = fopen(yuvfile.c_str(), "rb");

	unsigned char *yuv422 = new uint8_t[framesize* 2];
	fread(yuv422, framesize * 2 * sizeof( unsigned char), 1, yuv422File);
	fclose(yuv422File);

    auto yuy2_start= std::chrono::steady_clock::now();

	uchar* yuvI420 = new uchar[framesize *3 /2 ];
	memset(yuvI420, 0, sizeof(uchar) * (framesize *3  / 2 ) );

	uint8_t* dst_y = yuvI420;
	int dst_y_stride = width;

	uchar* dst_u = yuvI420 + width * height;
	int dst_u_stride = (width + 1) / 2;

	uchar* dst_v = (yuvI420 + width * height) + dst_u_stride * ((height + 1) / 2);
	int dst_v_stride = (width + 1) / 2;

	libyuv::YUY2ToI420(
					yuv422, 
					width * 2, 
					
					dst_y, 
					dst_y_stride, 
					
					dst_u, 
					dst_u_stride, 
					
					dst_v, 
					dst_v_stride, 
					
					width, 
					height
			);
	auto yuy2_end= std::chrono::steady_clock::now();
	std::chrono::duration<double> yuyv_spent = yuy2_end - yuy2_start;
	// std::cout << "yuyv To yuv422  convert   time :  " << yuyv_spent.count() * 1000 << "    ms \n ";

	// I420 convert to BGR  and save BGR  image to disk
	cv::Mat  matI420 = cv::Mat(height, width, CV_8UC4, cv::Scalar::all(0));

	auto rgbCovnert_start = std::chrono::steady_clock::now();

	libyuv::I420ToARGB(
					dst_y, 
					dst_y_stride, 
					
					dst_u, 
					dst_u_stride, 

					dst_v, 
					dst_v_stride,

					matI420.data, 
					width * 4, 
					
					width, 
					height
			);

	auto rgbCovnert_end= std::chrono::steady_clock::now();
	std::chrono::duration<double> rgbCovnert_spent = rgbCovnert_end - rgbCovnert_start;
	cv::imwrite("./yuyv2i420_origin_size.jpg", matI420);


	int dstWidth =  (int)width*scale_value;    // 360
	int dstHeight= (int)height*scale_value;   //180
	if (dstWidth ==width){
			std::cout << "yuy2 -> yuv420   time :  " <<  yuyv_spent.count() * 1000 << "    ms \n ";
			std::cout << "yuv420 -> BGR  convert   time :  " <<  rgbCovnert_spent.count() * 1000 << "    ms \n ";
			std::cout << "yuy2 -> yuv420 -> BGR  convert   time :  " << ( yuyv_spent.count() + rgbCovnert_spent.count() )* 1000 << "    ms \n \n";
	}
	else
	{
	   std::cout<<"----- I420  scale to width  :  "<< dstWidth << "       height   :  " <<dstHeight<<"\n\n";
		
		auto yuv422_scale_start= std::chrono::steady_clock::now();


		uint8_t *yuv420_Scale = new uint8_t[dstWidth * dstHeight* 3 / 2];

		uint8_t* scale_dst_y = yuv420_Scale;
		int scale_dst_y_stride = dstWidth;

		uint8_t* scale_dst_u = yuv420_Scale + dstWidth*dstHeight;
		int scale_dst_u_stride = (dstWidth+1)/2;

		uint8_t* scale_dst_v = (yuv420_Scale + dstWidth*dstHeight) + scale_dst_u_stride * ((dstHeight + 1) / 2);
		int scale_dst_v_stride =  (dstWidth+1)/2;

		//std::cout<<"----- I420  scale  :  "<<std::endl;
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
		auto yuv422_scale_end= std::chrono::steady_clock::now();
		std::chrono::duration<double> yuv422Scale_spent = yuv422_scale_end - yuv422_scale_start;


		cv::Mat  matI420_scale = cv::Mat(dstHeight, dstWidth, CV_8UC4, cv::Scalar::all(0));

		auto rgbCovnert_scale_start= std::chrono::steady_clock::now();
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
						
		auto rgbCovnert_scale_end= std::chrono::steady_clock::now();
		std::chrono::duration<double> rgbCovnert_scale_spent = rgbCovnert_scale_end - rgbCovnert_scale_start;

		cv::imwrite("./yuyv2i420_scale.jpg", matI420_scale);


		std::cout << "yuy2 -> yuv420   time :  " <<  yuyv_spent.count() * 1000 << "    ms \n ";
		std::cout << "yuv420 -> BGR  convert   time :  " <<  rgbCovnert_spent.count() * 1000 << "    ms \n \n";
		std::cout << "yuv422  Scale    time :  " << yuv422Scale_spent.count() * 1000 << "    ms  \n ";
		std::cout << "yuv422  Scale  -> BGR  time :  " << rgbCovnert_scale_spent.count() * 1000 << "    ms  \n\n ";

		std::cout << "yuy2 -> yuv420 -> yuv422  Scale   time :  " << ( yuyv_spent.count() + yuv422Scale_spent.count())* 1000 << "    ms \n ";

		std::cout << "yuy2 -> yuv420 -> yuv422  Scale  -> BGR  convert    time :  " <<
															 ( yuyv_spent.count() +  yuv422Scale_spent.count() + rgbCovnert_scale_spent.count())* 1000 << "    ms \n\n ";

	}

}

int main(int argc, char *argv[]) {
        int width = 1280, height = 720;
        float scale_value ;
        if (argc ==1)
        {
			scale_value = 1;
        }
        else{
			scale_value = std::stof(argv[1]);
        }

        std::string yuvfile = "./00316_3.yuyv";

        //ust opencv convert  test      
        std::cout << " \n******************* opencv *******************" << std::endl;
         opencvConvert(yuvfile, width, height, scale_value);
        
        std::cout << "******************* libyuv *******************" << std::endl;
        yuvLibConvert(yuvfile, width, height, scale_value);
 
        return 0;
}
