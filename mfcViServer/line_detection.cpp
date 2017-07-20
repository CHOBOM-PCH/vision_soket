#include "stdafx.h"
#include "line_detection.h"

void Line_Detect(InputArray _src, OutputArray _dst, int* distance, double* degree, int command)
{
	using namespace cv;
	Mat input_image = _src.getMat();
	Mat gray_img;
	Mat blur_img;
	Mat threshOutput_img;
	Mat threshOutput2_img;
	Mat edge_img;
	vector<Vec4i>lines;
	int length = 0;
		
	Mat output_img;
	output_img = input_image.clone();
	_dst.getMatRef() = output_img;
	//imshow("image",input_image);
	/////pre processing
	cvtColor(input_image, gray_img, CV_BGR2GRAY);
	GaussianBlur(gray_img, blur_img, cv::Size(5,5), 10);
	adaptiveThreshold(blur_img, threshOutput_img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 15, 2);
	threshold(blur_img,threshOutput2_img, 100, 255, THRESH_OTSU);
	Canny(threshOutput2_img, edge_img, 10, 100, 3);
	//imshow("image", threshOutput_image);
	imshow("image", edge_img);
	//line detect
	//HoughLinesP(threshOutput_img, lines, 1, (PI / 45), 100, 200, 5);
	HoughLinesP(edge_img, lines, 1, (PI / 180), 30, 50, 3);
	//save line
	Vec4d params,eparams;
	int x1, y1, x2, y2;
	int xe1 = 0, ye1 = 0, xe2 = 0, ye2 = 0;
	int e1 = 0, e2 = 0;
	int kk = 1;
	Point ct1(0,0), ct2(0,0);
	for (int k = 0; k < lines.size(); k++){
		params = lines[k];
		x1 = params[0];
		y1 = params[1];
		x2 = params[2];
		y2 = params[3];
		*degree = fabs(atan2f(((float)(y1 - y2)), abs((float)(x2 - x1))) * 180/PI);
		if(command == 1){
			if(*degree < 10 || *degree >170){//MVsavex
				eparams[0] = eparams[0] + params[0];
				eparams[1] = eparams[1] + params[1];
				eparams[2] = eparams[2] + params[2];
				eparams[3] = eparams[3] + params[3];
				xe1 = (eparams[0]) / (kk);
				ye1 = (eparams[1]) / (kk);
				xe2 = (eparams[2]) / (kk);
				ye2 = (eparams[3]) / (kk);
				kk++;
				Point pt1(x1, y1),pt2(x2, y2);
				
				//line(output_img, pt1, pt2, Scalar(0, 0, 255), 1);
				//printf("선들의 좌표값 시작 x:%d y:%d 끝 x:%d y:%d \n",x1, y1, x2, y2);
			}
			ct1.x = 0, ct1.y = input_image.rows / 2, ct2.x = input_image.cols, ct2.y = input_image.rows / 2;
			length = (ye1 + ye2) / 2 - (input_image.rows / 2);

		} else if(command == 2){//MVsavey
			if(*degree < 100 && *degree >80){
				eparams[0] = eparams[0] + params[0];
				eparams[1] = eparams[1] + params[1];
				eparams[2] = eparams[2] + params[2];
				eparams[3] = eparams[3] + params[3];
				xe1 = (eparams[0]) / (kk);
				ye1 = (eparams[1]) / (kk);
				xe2 = (eparams[2]) / (kk);
				ye2 = (eparams[3]) / (kk);
				kk++;
				Point pt1(x1, y1),pt2(x2, y2);
				
				//line(output_img, pt1, pt2, Scalar(0, 0, 255), 1);
				//printf("선들의 좌표값 시작 x:%d y:%d 끝 x:%d y:%d \n",x1, y1, x2, y2);
			}
			ct1.x = input_image.cols / 2, ct1.y = 0, ct2.x = input_image.cols / 2, ct2.y = input_image.rows;
			length =  (xe1 + xe2) / 2 - (input_image.cols / 2);
		}
	}
	//print line and distance
	if (xe1 == 0){
		xe1 = e1, xe2 = e2;
	}
	else {
		e1 = xe1, e2 = xe2;
		Point ept1(xe1,ye1),ept2(xe2,ye2);
		line(output_img, ept1, ept2, Scalar(255, 255, 0), 2);//평균선
		line(output_img, ct1, ct2, Scalar(0, 255, 255), 2);//중심선

		
		*degree = (atan2f(((float)(ye1 - ye2)), abs((float)(xe2 - xe1))) * 180/PI);
		*distance = length;
		Point dp1((xe1 + xe2) / 2, 300),dp2(((input_image.cols) / 2), 300);
		//line(output_image,dp1,dp2, Scalar(255,0,255),1);
		//string te;
		//std::stringstream ste;
		//ste << "distance:" << *distance << "pixel";
		//te=ste.str();
		//putText(output_image,te,dp1,3,1.2,Scalar(0,255,0));
	}
	//imshow ("출력",output_image);
	return ;
}