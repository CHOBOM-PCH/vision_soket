
// mfcViServerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "mfcViServer.h"
#include "mfcViServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int dist = 0; //�Ÿ������� ��������
double degree = 0;
int k = 0;//�̹��� ���� ����� ��������
int cmd = 5;//�ڸǵ� ���� ����
int ind = 0;
// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CmfcViServerDlg ��ȭ ����




CmfcViServerDlg::CmfcViServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CmfcViServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmfcViServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENT_LIST, m_event_list);
	DDX_Control(pDX, IDC_REAL_IMG, mRealImg);
	DDX_Control(pDX, IDC_CAPTURE_IMG, mCapImg);
	DDX_Control(pDX, IDC_X_DIST_LIST, m_x_dist_list);
	DDX_Control(pDX, IDC_Y_DIST_LIST, m_y_dist_list);
}

BEGIN_MESSAGE_MAP(CmfcViServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(25001, &CmfcViServerDlg::On25001)
	ON_MESSAGE(25002, &CmfcViServerDlg::On25002)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_Start_BTN, &CmfcViServerDlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(IDC_CAPTURE_BTN, &CmfcViServerDlg::OnBnClickedCaptureBtn)
	ON_BN_CLICKED(IDC_COMMAND_BTN, &CmfcViServerDlg::OnBnClickedCommandBtn)
END_MESSAGE_MAP()


// CmfcViServerDlg �޽��� ó����

BOOL CmfcViServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	//////������ſ� �ʱ�ȭ///////////////////////////////////////////////////////////////
	mh_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    //AF_INET : ���ͳ� �⺻ TCP �ּ�, 0�� 2��° ���� ������ �ڵ����� ���������� ��������
    sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//ip�� �Ҵ�� ip�� ������: cmd->ipconfig
    srv_addr.sin_port = htons(18000);//htonl=host byte order to network order, s=unsigned short int
 
    bind(mh_listen_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));
    //������ ��Ʈ��ũ ī�忡 ����
    AddEventString("���񽺸� �����մϴ�.");//����Ʈ�ڽ��� ���
    listen(mh_listen_socket, 1);
    //�� �� ���� �����ų�(���ÿ� ó���ϴ� ���� �ƴ�,)������ ������� ������ ������ ����� ������ ������ ����
     for (int i = 0; i < MAX_USER_COUNT; i++) {//����ִ� ������ ã�´�
        m_user_list[i].h_socket = INVALID_SOCKET;
    }
    WSAAsyncSelect(mh_listen_socket, m_hWnd, 25001, FD_ACCEPT);//�񵿱��Լ�
    //accept�� ����ϸ� ���� ����ڰ� ������ ������ ���α׷��� ������¿� ������ ����������
    //h_socket���� FD_ACCEPT �̺�Ʈ�� �߻��ϸ� 25001�޼����� �� ��ȭ���ڿ� ������� ��->������� ���¿� ����������
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
	/////////////////////////////////////////////////////////////////////////////////
	////////////////������� �ʱ�ȭ/////////////////////////////////////////////////
		capture = new VideoCapture(1);
	if (!capture->isOpened()){
		MessageBox(_T("���������� �����ϴ�."));

	}
	image_mfc = NULL;
	////////////////////////////////////////////////////////////////////////////////

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CmfcViServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CmfcViServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CmfcViServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CmfcViServerDlg::OnDestroy()
{//������ �ı����� ����� �޼���
    CDialogEx::OnDestroy();

	//vision ����
	capture->release();
	KillTimer(123);

	///������ſ�////
    //â �ݱ��� ���� �ִ� ������ �ִٸ� ��� Ŭ����
    for (int i = 0; i < MAX_USER_COUNT; i++) {
        if (m_user_list[i].h_socket != INVALID_SOCKET) {
            closesocket(m_user_list[i].h_socket);
        }
    }
	/////////////////
}
///////////////������� ������ �Լ�//////////////////////

void CmfcViServerDlg::AddEventString(const char *ap_string)
{//����Ʈ �ڽ��� ���� ��� �� Ŀ�� �ڵ��̵��ϴ� �Լ�
    while (m_event_list.GetCount() > 3000) {
        m_event_list.DeleteString(0);
    }
    int index = m_event_list.InsertString(-1, ap_string);
    m_event_list.SetCurSel(index);
}

void CmfcViServerDlg::SendFrameData(SOCKET ah_socket, char a_message_id, 
    unsigned short int a_body_size, char* ap_send_data)//����� �������� content�� ������ �Լ�
{
    char *p_send_data = new char[4 + a_body_size];//��� 4 byte
    *p_send_data = 27;//key�� �������� 27�� ����
    *(p_send_data + 1) = a_message_id;//message_id
    *(unsigned short *)(p_send_data + 2) = a_body_size;
    //�Ͻ������� char*�� short*�� �°� ������ ����
    
    memcpy(p_send_data + 4, ap_send_data, a_body_size);//�����͸� ����
    send(ah_socket, p_send_data, a_body_size + 4, 0);// Ŭ���̾�Ʈ���� ����
                        //send(����ȭ�Լ�->�� ����ɶ����� ����� ����)
    delete[] p_send_data;//������ ����
}


afx_msg LRESULT CmfcViServerDlg::On25001(WPARAM wParam, LPARAM lParam)
{//wParam -> �̺�Ʈ�� �߻��� ������ �ڵ��� �Ѿ��, lParam�� ���Ͽ� �߻��� �޼����� ��FD_ACCEPT 
    sockaddr_in client_addr;
    int sockaddr_in_size = sizeof(sockaddr_in);
    SOCKET h_socket = accept(mh_listen_socket, (LPSOCKADDR)&client_addr, &sockaddr_in_size);
    //���� �ڸ��� wParam�� ��� ��
    //2�� ° ���ڴ� �����ϴ� client�� ip�� �� �� �ְ��ϴ� ����(�߽��� ǥ��)
    //h_socket�� Ŭ�� ��������, listen ������ ������. listen ������ �������ʰ� accept ���ϸ� ����
    
    int i;//for�� �ۿ����� i�� ����ϱ� ���ؼ�
    for (i = 0; i < MAX_USER_COUNT; i++) {//����ִ� ������ ã�´�
        if (m_user_list[i].h_socket == INVALID_SOCKET)break;
    }
 
    if (i < MAX_USER_COUNT) {//�� ���� �ִ� ����
        m_user_list[i].h_socket = h_socket;//�ش� ������ ����
        strcpy(m_user_list[i].ip_address, inet_ntoa(client_addr.sin_addr));
        //client address ����
        WSAAsyncSelect(m_user_list[i].h_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
        //FD_WRITE�� �������� ������ ������ �� �޼������� �񵿱⸦ ���� ����-> ������ ������ ������ �˱� ����
        //FD_READ ������� �����͸� ���� �� �߻��Ǵ� �޼���
        //FD_CLOSE ������� ���� ��
        // | ��Ʈ �����ڸ� ����ؾ���, or�� �ϸ� ��������Ʈ �Ǽ� �������� ���� �޼����� �ްԵ�
        CString str;
        str.Format(_T("%s���� �����߽��ϴ�."), m_user_list[i].ip_address);
 
    }
    else {//�� �� ����
        AddEventString("���� �ִ� �ο� �ʰ�!\n");
        closesocket(h_socket);//accept�� ���� Ŭ����->�ʿ䰡 �����Ƿ�
        //������ client �ٽ� ���� �õ�, -> ����,, �Ǽ�ȯ �ݺ� ���� ���� ������ ������ �ƿ� ������.
    }
    return 0;
}



afx_msg LRESULT CmfcViServerDlg::On25002(WPARAM wParam, LPARAM lParam)
{
    CString str;
    if (WSAGETSELECTEVENT(lParam) == FD_READ) {
        WSAAsyncSelect(wParam, m_hWnd, 25002, FD_CLOSE);
        char key;        
        recv(wParam, &key, 1, 0);
        if (key == 27) {
            char message_id;            
            recv(wParam, &message_id, 1, 0);
 
            unsigned short body_size;
            recv(wParam, (char *)&body_size, 2, 0);
 
            char* p_body_data = NULL;
            if (body_size > 0) {
                p_body_data = new char[body_size];
                int total = 0, x, retry = 0;
                while (total < body_size) {
                    x = recv(wParam, p_body_data + total, body_size - total, 0);
                    if (x == SOCKET_ERROR)break;
                    total = total + x;
                    if (total < body_size) {
                        Sleep(50);
                        retry++;
                        if (retry > 5)break;
                    }
                }
            }
            
            //���� �߰��� �κ�
            if (message_id == 1){
                int i;
                for (i = 0; i < MAX_USER_COUNT; i++) {//��� ���Ͽ��� ���´��� ã�°���
                    if (m_user_list[i].h_socket == wParam)break;
                }
                //i��° ���Ͽ��� ���� ���� Ȯ����
 
                CString str2;//i��° ������ �ּҿ� ���� �����͸� str2�� ������
                str2.Format(_T("%s"), /*m_user_list[i].ip_address, */p_body_data);
                AddEventString(str2);//���� ����Ʈ �ڽ��� ���
 
                //������ ��� Ŭ���̾�Ʈ���� ����(broad casting)
                for (i = 0; i < MAX_USER_COUNT; i++) {
                    if (m_user_list[i].h_socket != INVALID_SOCKET) {//��ȿ�������� ������ �ƴ϶��
                        SendFrameData(m_user_list[i].h_socket, 1, str2.GetLength()+1, (char *)(const char*)str2);
                        //�ٵ� ������� NULL������ �����ؾ��ϹǷ� +1
                        //CString�� char*ĳ������ (char *)(const char *)�� ����
                        //���⼭ i�� ���� i�� �������
                    }                    
                }
            }
            //������� �������� ���� �߰��� �κ�
 
            if (p_body_data != NULL)delete[] p_body_data;
            WSAAsyncSelect(wParam, m_hWnd, 25002, FD_READ | FD_CLOSE);
        }
        
    }
    else {//FD_CLOSE
        closesocket(wParam);
        for (int i = 0; i < MAX_USER_COUNT; i++) {
            if (m_user_list[i].h_socket == wParam) {
                m_user_list[i].h_socket = INVALID_SOCKET;
                str.Format("����ڰ� �����߽��ϴ�.: %s", m_user_list[i].ip_address);
                AddEventString(str);
                break;
            }
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////

/////////���� ���� ����//////////////////////////////////


void CmfcViServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if(nIDEvent == 123){
		capture->read(O_img);
		
		if(O_img.empty()){//����� ������ ������
			KillTimer(123);
			capture->release();
			MessageBox(_T("������ ����"));
		}

		else{
			
			//image ȭ�鿡 �°� ���
			//Line_Detect(O_img, H_img, &dist, &degree, cmd);
			ImagePrintInMFC(image_mfc,&mRealImg,O_img);
			image_mfc = NULL;
			
			int inde = m_event_list.GetCurSel();
			CString index;
			CString command;
			
			if (inde != LB_ERR)//ListBox�� ������ �ƴϸ�..
			{
				m_event_list.GetText(inde, str);
				AfxExtractSubString(command, str, 0, ' ');
				AfxExtractSubString(index, str, 1, ' ');
				ind = _ttoi(index);
				//m_x_dist_list.InsertString(-1, str);
				//cmd = LPSTR(LPCTSTR(str));
				if (command == "MVsavex")
					cmd = 1;
				else if (command == "MVsavey")
					cmd = 2;
				else if (command == "MVloadx")
					cmd = 3;
				else if (command == "MVloady")
					cmd = 4;
				else if (command == "MVinit")
					cmd = 0;


			}
		}

	}

	CDialogEx::OnTimer(nIDEvent);
}


void CmfcViServerDlg::OnBnClickedStartBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetTimer(123,20,NULL);
}


void CmfcViServerDlg::OnBnClickedCaptureBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	k = k + 1;
	int tt = 0;
		CString distance;

		
	std::string da;// = "D:\\kka\\image_1.jpg";
	std::stringstream ss;
	ss << "D:\\kka\\image_" << k <<".jpg";
	std::cout << ss.str();
	da = ss.str();
	if (cmd == 0){
		m_x_dist_list.ResetContent();
		m_y_dist_list.ResetContent();

		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		m_x_dist_list.InsertString(-1, _T("0"));
		
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		m_y_dist_list.InsertString(-1, _T("0"));
		
	}
	else{
	Line_Detect(O_img,H_img,&dist,&degree,cmd);
	ImagePrintInMFC(image_mfc,&mCapImg,H_img);
	imwrite(da,H_img);
	distance.Format(_T("%d"), dist);
	if (cmd == 1){	
		m_x_dist_list.DeleteString(ind);
		m_x_dist_list.InsertString(ind, distance );
	}else if (cmd == 2){
		m_y_dist_list.DeleteString(ind);
		m_y_dist_list.InsertString(ind, distance );
	}

	}

}


void CmfcViServerDlg::OnBnClickedCommandBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

}
