#include "stdafx.h"
#include "ImageToMFC.h"

#include <crtdbg.h>//�޸𸮴��� üũ
#ifndef _DEBUG//�޸� ����üũ

#define new new(_CLIENT_BLOCK,__FILE__,__LINE)

#endif



void ImagePrintInMFC(CImage *image_mfc, CStatic* Print_window, InputArray _src)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);//�޸𸮴��� üũ

	RECT r;//����� �������� ũ�� ����
	(*Print_window).GetClientRect(&r);//����� �������� ũ�� ����
	cv::Size winSize(r.right, r.bottom);
	cv::Mat Print_image = _src.getMat();// ����� �̹����� ������
	if (image_mfc){// ���� �̹� �̹����� �����ִٸ�
		image_mfc->ReleaseDC();// �ѹ� ȭ�鿡 �����ϰ�
		delete image_mfc;// ����
		image_mfc = nullptr;// �������ͷ� �ʱ�ȭ�� ������
	}

	image_mfc = new CImage();
	image_mfc->Create(winSize.width, winSize.height, 24);
	// ������ ���ߴ� �������� �̹��� ũ�⸦ ���ϰ� 24bpp¥���� ����.
	
	// ��Ʈ�� ���� ����
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
		// source and destination have same size������ �����̹����� ���� ũ����
		// transfer memory block�޸𸮸� ��������
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width
		//�׵θ��� ���� ǥ�õ� �ִ� 3�ȼ�����
		SetDIBitsToDevice(image_mfc->GetDC(),
			//destination rectangle
			0, 0, winSize.width, winSize.height,
			0, 0, 0, Print_image.rows,
			Print_image.data, &bitInfo, DIB_RGB_COLORS);
	}
	else {// ���� �̹����� ���� �̹����� ����� �ٸ��ٸ�
		// destination rectangle
		int destx = 0, desty = 0;
		int destw = winSize.width;
		int desth = winSize.height;
		 // ���� ������ ����� ���ͼ�
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
	//image_mfc->BitBlt(::GetDC((*Print_window).m_hWnd), 0, 0);//�޸𸮸�
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
