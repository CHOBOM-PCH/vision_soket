
// mfcViServerDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "line_detection.h"
#include "ImageToMFC.h"

#include <conio.h>
#include <windows.h>
#include <vector>
#include <iostream>
#include <fstream>

#define MAX_USER_COUNT 100
#define PI 3.1415926

using namespace cv;
using namespace std;

struct UserData {
	SOCKET h_socket;// -1�̸� �Ⱦ��� ���� -1�� �ƴϸ� ���� ����
	char ip_address[16];//255.255.255.255 15�ڸ�+ /0-> 16��
};

// CmfcViServerDlg ��ȭ ����
class CmfcViServerDlg : public CDialogEx
{
	// �����Դϴ�.
public:
	CmfcViServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MFCVISERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


	// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void CmfcViServerDlg::AddEventString(const char *ap_string);
	void CmfcViServerDlg::SendFrameData(SOCKET ah_socket, char a_message_id, unsigned short int a_body_size, char* ap_send_data);
	afx_msg LRESULT On25001(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On25002(WPARAM wParam, LPARAM lParam);
private:
	CListBox m_event_list;
	CListBox m_x_dist_list;
	CListBox m_y_dist_list;
	SOCKET mh_listen_socket;
	UserData m_user_list[MAX_USER_COUNT]; 
public:
	afx_msg void OnDestroy();
	CStatic mRealImg;
	CStatic mCapImg;
	//�������� ����////
	VideoCapture* capture;
	Mat O_img;
	Mat gray_img;
	Mat blur_img;
	Mat thresh_img;
	Mat H_img;
	CImage *image_mfc;
	CString str;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnBnClickedCaptureBtn();
	afx_msg void OnBnClickedCommandBtn();
};
