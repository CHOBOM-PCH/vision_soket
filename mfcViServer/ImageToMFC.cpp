#include "stdafx.h"
#include "ImageToMFC.h"

#include <crtdbg.h>//메모리누수 체크
#ifndef _DEBUG//메모리 누스체크

#define new new(_CLIENT_BLOCK,__FILE__,__LINE)

#endif



void ImagePrintInMFC(CImage *image_mfc, CStatic* Print_window, InputArray _src)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);//메모리누수 체크

	RECT r;//출력할 윈도우의 크기 측정
	(*Print_window).GetClientRect(&r);//출력할 윈도우의 크기 측정
	cv::Size winSize(r.right, r.bottom);
	cv::Mat Print_image = _src.getMat();// 출력할 이미지를 가져옴
	if (image_mfc){// 만약 이미 이미지가 잡혀있다면
		image_mfc->ReleaseDC();// 한번 화면에 방출하고
		delete image_mfc;// 지움
		image_mfc = nullptr;// 널포인터로 초기화도 시켜줌
	}

	image_mfc = new CImage();
	image_mfc->Create(winSize.width, winSize.height, 24);
	// 위에서 정했던 사이즈대로 이미지 크기를 정하고 24bpp짜리로 만듬.
	
	// 비트맵 인포 설정
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 24;
	bitInfo.bmiHeader.biWidth = Print_image.cols;
	bitInfo.bmiHeader.biHeight =- Print_image.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;
	///////////////
	if (Print_image.cols == winSize.width  && Print_image.rows == winSize.height){
		// source and destination have same size원본과 목적이미지가 같은 크기라면
		// transfer memory block메모리를 대입해줌
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width
		//테두리는 여기 표시됨 최대 3픽셀까지
		SetDIBitsToDevice(image_mfc->GetDC(),
			//destination rectangle
			0, 0, winSize.width, winSize.height,
			0, 0, 0, Print_image.rows,
			Print_image.data, &bitInfo, DIB_RGB_COLORS);
	}
	else {// 원본 이미지와 목적 이미지의 사이즈가 다르다면
		// destination rectangle
		int destx = 0, desty = 0;
		int destw = winSize.width;
		int desth = winSize.height;
		 // 원본 영상의 사이즈를 따와서
		// rectangle defined on source bitmap
		// using imgWidth instead of mat_temp.cols will ignore the padding border
		int imgx = 0, imgy = 0;
		int imgWidth = Print_image.cols; //- border;
		int imgHeight = Print_image.rows;

		StretchDIBits(image_mfc->GetDC(),
			destx, desty, destw, desth,
			imgx, imgy, imgWidth, imgHeight,
			Print_image.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	//image_mfc->BitBlt(::GetDC((*Print_window).m_hWnd), 0, 0);//메모리릭
	HDC hDC;
	hDC = ::GetDC((*Print_window).m_hWnd);
	image_mfc->BitBlt( hDC, 0, 0);
	::ReleaseDC((*Print_window).m_hWnd, hDC);

	if (image_mfc){
		image_mfc->ReleaseDC();
		delete image_mfc; 
		image_mfc = nullptr;
	}
}
