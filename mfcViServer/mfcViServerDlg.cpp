
// mfcViServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "mfcViServer.h"
#include "mfcViServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int dist = 0; //거리측정용 변수선언
double degree = 0;
int k = 0;//이미지 파일 저장용 순서변수
int cmd = 5;//코맨드 관련 변수
int ind = 0;
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CmfcViServerDlg 대화 상자




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


// CmfcViServerDlg 메시지 처리기

BOOL CmfcViServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//////소켓통신용 초기화///////////////////////////////////////////////////////////////
	mh_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    //AF_INET : 인터넷 기본 TCP 주소, 0은 2번째 인자 버전과 자동으로 프로토콜을 연결해줌
    sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//ip는 할당된 ip에 따른다: cmd->ipconfig
    srv_addr.sin_port = htons(18000);//htonl=host byte order to network order, s=unsigned short int
 
    bind(mh_listen_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));
    //소켓을 네트워크 카드에 연결
    AddEventString("서비스를 시작합니다.");//리스트박스에 출력
    listen(mh_listen_socket, 1);
    //몇 개 까지 받을거냐(동시에 처리하는 것이 아님,)빠르게 만들려면 소켓을 여러개 만들어 리슨도 여러개 만듬
     for (int i = 0; i < MAX_USER_COUNT; i++) {//비어있는 소켓을 찾는다
        m_user_list[i].h_socket = INVALID_SOCKET;
    }
    WSAAsyncSelect(mh_listen_socket, m_hWnd, 25001, FD_ACCEPT);//비동기함수
    //accept를 사용하면 실제 사용자가 접속할 때까지 프로그램이 응답상태에 빠지는 단점이있음
    //h_socket에서 FD_ACCEPT 이벤트가 발생하면 25001메세지를 이 대화상자에 보내라는 것->응답없음 상태에 빠지지않음
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
	////////////////영상재생 초기화/////////////////////////////////////////////////
		capture = new VideoCapture(1);
	if (!capture->isOpened()){
		MessageBox(_T("비디오파일이 없습니다."));

	}
	image_mfc = NULL;
	////////////////////////////////////////////////////////////////////////////////

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CmfcViServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CmfcViServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CmfcViServerDlg::OnDestroy()
{//윈도우 파괴직전 생기는 메세지
    CDialogEx::OnDestroy();

	//vision 끄기
	capture->release();
	KillTimer(123);

	///소켓통신용////
    //창 닫기전 쓰고 있는 소켓이 있다면 모두 클로즈
    for (int i = 0; i < MAX_USER_COUNT; i++) {
        if (m_user_list[i].h_socket != INVALID_SOCKET) {
            closesocket(m_user_list[i].h_socket);
        }
    }
	/////////////////
}
///////////////소켓통신 서버용 함수//////////////////////

void CmfcViServerDlg::AddEventString(const char *ap_string)
{//리스트 박스에 문자 출력 및 커서 자동이동하는 함수
    while (m_event_list.GetCount() > 3000) {
        m_event_list.DeleteString(0);
    }
    int index = m_event_list.InsertString(-1, ap_string);
    m_event_list.SetCurSel(index);
}

void CmfcViServerDlg::SendFrameData(SOCKET ah_socket, char a_message_id, 
    unsigned short int a_body_size, char* ap_send_data)//헤더와 데이터의 content를 보내는 함수
{
    char *p_send_data = new char[4 + a_body_size];//헤더 4 byte
    *p_send_data = 27;//key의 고유값을 27로 통일
    *(p_send_data + 1) = a_message_id;//message_id
    *(unsigned short *)(p_send_data + 2) = a_body_size;
    //일시적으로 char*를 short*에 맞게 변위를 조절
    
    memcpy(p_send_data + 4, ap_send_data, a_body_size);//데이터를 복사
    send(ah_socket, p_send_data, a_body_size + 4, 0);// 클라이언트에게 전송
                        //send(동기화함수->다 진행될때까지 벗어나지 못함)
    delete[] p_send_data;//전송후 삭제
}


afx_msg LRESULT CmfcViServerDlg::On25001(WPARAM wParam, LPARAM lParam)
{//wParam -> 이벤트가 발생한 소켓의 핸들이 넘어옴, lParam에 소켓에 발생한 메세지가 옴FD_ACCEPT 
    sockaddr_in client_addr;
    int sockaddr_in_size = sizeof(sockaddr_in);
    SOCKET h_socket = accept(mh_listen_socket, (LPSOCKADDR)&client_addr, &sockaddr_in_size);
    //소켓 자리에 wParam을 적어도 됨
    //2번 째 인자는 접속하는 client의 ip를 알 수 있게하는 역할(발신자 표시)
    //h_socket은 클론 소켓으로, listen 소켓을 복제함. listen 소켓은 사용되지않고 accept 소켓만 사용됨
    
    int i;//for문 밖에서도 i를 사용하기 위해서
    for (i = 0; i < MAX_USER_COUNT; i++) {//비어있는 소켓을 찾는다
        if (m_user_list[i].h_socket == INVALID_SOCKET)break;
    }
 
    if (i < MAX_USER_COUNT) {//빈 방이 있는 상태
        m_user_list[i].h_socket = h_socket;//해당 소켓을 저장
        strcpy(m_user_list[i].ip_address, inet_ntoa(client_addr.sin_addr));
        //client address 복사
        WSAAsyncSelect(m_user_list[i].h_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
        //FD_WRITE는 서버에서 데이터 보낼떄 이 메세지에는 비동기를 걸지 않음-> 보내는 시점을 서버가 알기 때문
        //FD_READ 상대편이 데이터를 보낼 때 발생되는 메세지
        //FD_CLOSE 상대편이 끊을 때
        // | 비트 연산자를 사용해야함, or를 하면 오버라이트 되서 마지막에 쓰인 메세지만 받게됨
        CString str;
        str.Format(_T("%s에서 접속했습니다."), m_user_list[i].ip_address);
 
    }
    else {//꽉 찬 상태
        AddEventString("관리 최대 인원 초과!\n");
        closesocket(h_socket);//accept용 소켓 클로즈->필요가 없으므로
        //끊어진 client 다시 접속 시도, -> 끊고,, 악순환 반복 따라서 리슨 소켓을 닫으면 아예 못들어옴.
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
            
            //새로 추가된 부분
            if (message_id == 1){
                int i;
                for (i = 0; i < MAX_USER_COUNT; i++) {//어느 소켓에서 보냈는지 찾는과정
                    if (m_user_list[i].h_socket == wParam)break;
                }
                //i번째 소켓에서 보낸 것을 확인함
 
                CString str2;//i번째 소켓의 주소와 받은 데이터를 str2에 연결함
                str2.Format(_T("%s"), /*m_user_list[i].ip_address, */p_body_data);
                AddEventString(str2);//서버 리스트 박스에 출력
 
                //접속한 모든 클라이언트에서 보냄(broad casting)
                for (i = 0; i < MAX_USER_COUNT; i++) {
                    if (m_user_list[i].h_socket != INVALID_SOCKET) {//유효하지않은 소켓이 아니라면
                        SendFrameData(m_user_list[i].h_socket, 1, str2.GetLength()+1, (char *)(const char*)str2);
                        //바디 사이즈는 NULL공간을 포함해야하므로 +1
                        //CString의 char*캐스팅은 (char *)(const char *)로 가능
                        //여기서 i는 위의 i의 관계없음
                    }                    
                }
            }
            //여기까지 기존에서 새로 추가된 부분
 
            if (p_body_data != NULL)delete[] p_body_data;
            WSAAsyncSelect(wParam, m_hWnd, 25002, FD_READ | FD_CLOSE);
        }
        
    }
    else {//FD_CLOSE
        closesocket(wParam);
        for (int i = 0; i < MAX_USER_COUNT; i++) {
            if (m_user_list[i].h_socket == wParam) {
                m_user_list[i].h_socket = INVALID_SOCKET;
                str.Format("사용자가 종료했습니다.: %s", m_user_list[i].ip_address);
                AddEventString(str);
                break;
            }
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////

/////////비젼 관련 내용//////////////////////////////////


void CmfcViServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nIDEvent == 123){
		capture->read(O_img);
		
		if(O_img.empty()){//재생이 끝나고 나가기
			KillTimer(123);
			capture->release();
			MessageBox(_T("비디오가 끝남"));
		}

		else{
			
			//image 화면에 맞게 출력
			//Line_Detect(O_img, H_img, &dist, &degree, cmd);
			ImagePrintInMFC(image_mfc,&mRealImg,O_img);
			image_mfc = NULL;
			
			int inde = m_event_list.GetCurSel();
			CString index;
			CString command;
			
			if (inde != LB_ERR)//ListBox가 에러가 아니면..
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetTimer(123,20,NULL);
}


void CmfcViServerDlg::OnBnClickedCaptureBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}
