
// ROBOT_CTLDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "ROBOT_CTL.h"
#include "ROBOT_CTLDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "futaba_rs.h"
#include "ODriveArduino.h"

#include <time.h>     // for clock()
#include <mmsystem.h>	// timeGetTime()
#pragma comment( lib, "winmm.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//ini�t�@�C������擾����V���A���ݒ�
#define SETFILE _T( "./setting.ini")

// �X���b�h�̓N���X�O����Ăяo�����߁A�����Ŏg�p����ϐ��́A�O���[�o���ϐ��Ƃ��Ĉ������j

futaba_rs Futaba_RS;	// �T�[�{����p�N���X
ODriveArduino ODrive;

float pos_m0 = 0.0f;

char get_port[10];
int get_baud;

int sv_count = 1;		// �T�[�{�̌� (���Ƃ�setting����I�[�o�[���C�h����)
int sv_ctl_f = 0;		// �T�[�{�R���g���[���t���O

sv_r Data[100];			// �Ƃ肠����ID:0����ID:99�܂Ŋm��(�T�[�{�̃f�[�^)

clock_t Processing_time, Processing_time1, Processing_time2, Processing_time3;	// �������Ԃ̓��ꕨ


CString filetime;		// CSV��ۑ�����Ƃ��̃t�@�C����
int flag_csv = 0;		// CSV�t���O
FILE* csv_fp;			// CSV�̃t�@�C���|�C���^

int sv_angle_rw_all = 0;

int pointA;
int pointB;
int timeAB;
int timeBA;
int time_A;
int time_B;

int f_testmove = 0;
int f_testmove_do = 0;
int f_testmove2 = 0;

int sv_diff[100];

BOOL sv_torque_w[100];
BOOL sv_reverse_w[100];

// �֐�
void RSGetDataALL(void);	// �S�T�[�{��M
void thread_motor(void);	// �T�[�{����p�X���b�h
void thread_testmove(void);	    // �e�X�g����p�X���b�h
void thread_testmove2(void);	    // �e�X�g����p�X���b�h

// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CROBOT_CTLDlg �_�C�A���O


IMPLEMENT_DYNAMIC(CROBOT_CTLDlg, CDialogEx);

CROBOT_CTLDlg::CROBOT_CTLDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ROBOT_CTL_DIALOG, pParent)
	, com_number(_T(""))
	, com_baudrate(_T(""))
	, com_openclode(_T(""))

	, temp0001(_T(""))
	, flip_radio(0)
	, csvlist(_T(""))
	, current_filename(_T(""))
	, select_move(_T(""))
	, motion_file_select(_T(""))
	, motion_name(_T(""))

	, Current_lim(_T("0"))
	, Vel_limit(_T("0"))

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CROBOT_CTLDlg::~CROBOT_CTLDlg()
{
	// ���̃_�C�A���O�p�̃I�[�g���[�V���� �v���L�V������ꍇ�́A���̃_�C�A���O
	//  �ւ̃|�C���^�[�� NULL �ɖ߂��܂��A����ɂ���ă_�C�A���O���폜���ꂽ����
	//  ���킩��܂��B
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CROBOT_CTLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, com_number);
	DDX_Text(pDX, IDC_EDIT3, com_baudrate);
	DDX_Text(pDX, IDC_EDIT5, com_openclode);

	DDX_Text(pDX, IDC_ANGLE_61, sv_angle_r[1]);
	DDX_Text(pDX, IDC_SPEED_61, sv_speed_r[1]);
	DDX_Text(pDX, IDC_LOAD_61, sv_load_r[1]);


	DDX_Text(pDX, IDC_ANGLE_62, sv_angle_r[2]);
	DDX_Text(pDX, IDC_SPEED_62, sv_speed_r[2]);
	DDX_Text(pDX, IDC_LOAD_62, sv_load_r[2]);



	DDX_Slider(pDX, IDC_ANGLE_SL61, sv_angle_rw[1]);
	DDX_Slider(pDX, IDC_ANGLE_SL62, sv_angle_rw[2]);


	DDX_Slider(pDX, IDC_ANGLE_SALL, sv_angle_rw_all);



	DDX_Text(pDX, IDC_ANGLE_VALL, sv_angle_rw_v_all);

	DDX_Control(pDX, IDC_ANGLE_SL61, sv_angle_sl_ctl[1]);
	DDX_Control(pDX, IDC_ANGLE_SL62, sv_angle_sl_ctl[2]);


	DDX_Control(pDX, IDC_ANGLE_SALL, sv_angle_sl_ctl_all);


	DDX_Control(pDX, IDC_SV_OPEN, sv_opne_ctlf);
	DDX_Control(pDX, IDC_SV_CLOSE, sv_close_ctlf);
	DDX_Text(pDX, IDC_TEMP0001, temp0001);
	DDX_Control(pDX, IDC_BUTTON_CSV, sv_csv_cap);

	DDX_Control(pDX, IDC_PLAY, test_play);
	DDX_Control(pDX, IDC_PLAY2, test_play2);
	DDX_Control(pDX, IDC_STOP, test_stop);

	
	//DDX_Text(pDX, IDC_sv_cnt, sv_count_s);

	DDX_Text(pDX, IDC_pointA, pointA);
	DDX_Text(pDX, IDC_pointB, pointB);
	DDX_Text(pDX, IDC_time_A, time_A);
	DDX_Text(pDX, IDC_time_B, time_B);
	DDX_Text(pDX, IDC_timeAB, timeAB);
	DDX_Text(pDX, IDC_timeBA, timeBA);


	// ODrve �R���t�B�O
	DDX_Text(pDX, IDC_vel_lim, Vel_limit);
	DDX_Text(pDX, IDC_current_lim, Current_lim);

}

BEGIN_MESSAGE_MAP(CROBOT_CTLDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()

	ON_BN_CLICKED(IDOK, &CROBOT_CTLDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CROBOT_CTLDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CROBOT_CTLDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_SV_OPEN, &CROBOT_CTLDlg::OnBnClickedSvOpen)
	ON_BN_CLICKED(IDC_SV_CLOSE, &CROBOT_CTLDlg::OnBnClickedSvClose)

	ON_BN_CLICKED(IDC_BUTTON_CSV, &CROBOT_CTLDlg::OnBnClickedButtonCsv)
	ON_BN_CLICKED(IDC_BUTTON_ZERO, &CROBOT_CTLDlg::OnBnClickedButtonZero)
	ON_BN_CLICKED(IDC_BUTTON_DIFF, &CROBOT_CTLDlg::OnBnClickedButtonDiff)
	ON_BN_CLICKED(IDC_PLAY, &CROBOT_CTLDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_STOP, &CROBOT_CTLDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_PLAY2, &CROBOT_CTLDlg::OnBnClickedPlay2)
	ON_BN_CLICKED(IDC_BUTTON4, &CROBOT_CTLDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_READ_CONF, &CROBOT_CTLDlg::OnBnClickedReadConf)
	ON_BN_CLICKED(IDC_WRITE_CONF, &CROBOT_CTLDlg::OnBnClickedWriteConf)
	ON_BN_CLICKED(IDC_SAVE_ODRIVE, &CROBOT_CTLDlg::OnBnClickedSaveOdrive)
	ON_BN_CLICKED(IDC_BUTTON_aaa, &CROBOT_CTLDlg::OnBnClickedButtonaaa)
END_MESSAGE_MAP()


// CROBOT_CTLDlg ���b�Z�[�W �n���h���[

BOOL CROBOT_CTLDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
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

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B ��������������������������������������������������������������������������������

	int i;

	UpdateData(TRUE);	// GUI���l


	//�S�T�[�{���쑬�x��0msec�ɐݒ�
	for (i = 0; i < 100; i++) {
		Data[i].g_time = 0;
	}
	
	// �X���C�_�̐ݒ�p�̈ꎞ�ϐ��ő�E�ŏ��E�f�t�H���g�^���O
	int max, min;
	char part_name[10];


	for (i = 1; i < 3; i++) {
		//////////////////�X���C�_�[�͈̔͂�ݒ肷��

		sprintf_s(part_name, "SURVO_%d", i);

		min = GetPrivateProfileInt("SURVO_MIN", part_name, -1800, SETFILE);
		max = GetPrivateProfileInt("SURVO_MAX", part_name, 1800, SETFILE);
		

		//�X���C�_�[�ݒ�
		sv_angle_sl_ctl[i].SetRange(min, max);		// �ő�E�ŏ�
		sv_angle_rw[i] = 50;						// �f�t�H���g�l
		if (i != 1) sv_reverse_w[i] = GetPrivateProfileInt("SURVO_REVERSE", part_name, 0, SETFILE);

		//////////////////�X���C�_�[�͈̔͂�ݒ肷��
	}

	sv_angle_sl_ctl_all.SetRange(-1800,1800);
	sv_angle_rw_all = 100;
	UpdateData(FALSE);	// �l��GUI
	sv_angle_rw_all = 0;
	UpdateData(FALSE);	// �l��GUI

	// INI�t�@�C������擾
	GetPrivateProfileString("serial", "COM", "INI ERROR", get_port, sizeof(get_port), SETFILE);
	get_baud = GetPrivateProfileInt("serial", "BAUD", 0, SETFILE);

	com_number = get_port;
	com_baudrate.Format(_T("%d bps"), get_baud);
	com_openclode = "CLOSE";

	sv_count = GetPrivateProfileInt("SURVO", "COUNT", 0, SETFILE);
	sv_count_s.Format(_T("%d ��"), sv_count);


	pointA = 200;
	pointB = -200;
	time_A = 500;
	time_B = 500;
	timeAB = 500;
	timeBA = 500;


	
	UpdateData(FALSE);	// �l��GUI

	

	//�\���X�V�p�̃^�C�}�[��16ms�ŋN�� (�^�C�}�[���N�����Ȃ���΁A�ݒ胂�[�h�ɓ����)
	SetTimer(5678, 16, NULL);

	

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CROBOT_CTLDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CROBOT_CTLDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{

		CPaintDC paintDC(this);

		// �����ŕ`�悷��
		if (m_bmp.GetSafeHandle() == NULL)
		{
			// �K���Ƀr�b�g�}�b�v���쐬
			// �r�b�g�}�b�v�����[�h
			m_bmp.LoadBitmap(IDB_BITMAP1);
		}
		// �r�b�g�}�b�v�̑傫�����擾
		BITMAP bitmap;
		m_bmp.GetBitmap(&bitmap);
		CSize sz(bitmap.bmWidth, bitmap.bmHeight);

		CDC dc;
		dc.CreateCompatibleDC(&paintDC);
		// �f�B�o�C�X�R���e�L�X�g�őI��
		CBitmap* pOld = dc.SelectObject(&m_bmp);
		// ���Ƃ��Ƃ̃f�B�o�C�X�R���e�L�X�g��
		// �r�b�g�}�b�v�𓧉߂œ]��
		if (::TransparentBlt(paintDC, 690, 15, sz.cx, sz.cy,
			dc, 0, 0, sz.cx, sz.cy, (UINT)RGB(255, 0, 255))
			== FALSE)
		{
			// �G���[�ԍ����擾
			DWORD dwErr = ::GetLastError();
			// �����ɃG���[���̏������L�q���邱��
		}

		// �r�b�g�}�b�v�I������
		dc.SelectObject(pOld);


		//�����l����
		UpdateData(FALSE);


		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CROBOT_CTLDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// �R���g���[���[���I�u�W�F�N�g�� 1 ���܂��ێ����Ă���ꍇ�A
//  �I�[�g���[�V�����T�[�o�[�̓��[�U�[�� UI �����ۂɏI����
//  ���܂���B�����̃��b�Z�[�W �n���h���[�̓v���L�V���܂��g�p��
//  ���ǂ������m�F���A���ꂩ�� UI ����\���ɂȂ�܂����_�C�A��
//  �O�͂��ꂪ�����ꂽ�ꍇ���̏ꏊ�Ɏc��܂��B

void CROBOT_CTLDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}


BOOL CROBOT_CTLDlg::CanExit()
{
	// �v���L�V �I�u�W�F�N�g���܂��c���Ă���ꍇ�A�I�[�g���[�V����
	//  �R���g���[���[�͂��̃A�v���P�[�V�������܂��ێ����Ă��܂��B
	//  �_�C�A���O�̎��͎͂c���܂��� UI �͔�\���ɂȂ�܂��B
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}



void CROBOT_CTLDlg::OnBnClickedOk()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CDialogEx::OnOK();
}


void CROBOT_CTLDlg::OnBnClickedButton1()	// �S�g�g���N�I���I
{
	
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B




	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};


	ODrive.reset();

	ODrive.close();	// �V���A���|�[�g�N���[�Y


}

void CROBOT_CTLDlg::OnBnClickedButton2()	// �S�g�E�́I
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	int i;

	for (i = 1; i < 9; i++) {
		sv_torque_w[i] = 0;
	}

	UpdateData(FALSE);
}


void CROBOT_CTLDlg::OnBnClickedSvOpen()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	com_openclode = "OPEN";
	UpdateData(FALSE);
	
	//	sv_open();
	sv_ctl_f = 1;	//�X���b�h�R���g���[���p�̃t���O (�X���b�h�𗧂��グ��O��1�ɂ��āA�����0�ɂ���ƃX���b�h�̖������[�v���I���)
	
	// �Ƃ肠�����S�T�[�{�g���N�I�t
	OnBnClickedButton2();	// �E�̓{�^�����Ăяo��

	////////////�X���b�h�p�p�����[�^
	HANDLE handle;

	////////////�X���b�h�̐���
	handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_motor, NULL, 0, NULL);

	sv_opne_ctlf.EnableWindow(FALSE);
	sv_close_ctlf.EnableWindow(TRUE);

	// 0.1�b�҂��č������擾
	Sleep(100);
	OnBnClickedButtonDiff();

	sv_angle_rw_all = Data[1].angle;
	UpdateData(FALSE);

}



void CROBOT_CTLDlg::OnBnClickedSvClose()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	sv_ctl_f = 0;
	//Sleep(1000);

	com_openclode = "CLOSE";
	UpdateData(FALSE);

	sv_opne_ctlf.EnableWindow(TRUE);
	sv_close_ctlf.EnableWindow(FALSE);

}



//----------------------------�T�[�{���[�^�[�̊֘A�֐� -------------------------


void RSGetDataALL(void)
{
	int i;
	sv_r rData[100];		// �Ƃ肠����ID:0����ID:99�܂Ŋm��(�T�[�{�̃f�[�^)

	for (i = 1; i < sv_count+1; i++) {		// �����̐��l��ϐ��ɁI��������������������������������������������������������
		
		rData[i] = Futaba_RS.sv_read2(i);
		
		Data[i].angle		= rData[i].angle;
		Data[i].load		= rData[i].load;
		Data[i].speed		= rData[i].speed;
		Data[i].temperature = rData[i].temperature;
		Data[i].time		= rData[i].time;
		Data[i].error		= rData[i].error;
	}

}



//���[�^�[����p�̃X���b�h
void thread_motor(void)
{

	int i;
	SYSTEMTIME st;
	clock_t now_time = 0,old_time = 0;	// ���Ԍv���p


	//if (Futaba_RS.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
	//											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
	//	while (1);
	//};


	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
												//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};





	// �Ƃ肠�����S�T�[�{�g���N�I�t
	//for (i = 0; i < 100; i++) {
	//	Futaba_RS.sv_torque(i, 0);
	//}

	while (sv_ctl_f) {
		
		// �������Ԃ̎擾
		now_time = clock();						// ���ݎ��Ԃ̍X�V
		Processing_time = now_time - old_time;	// �������Ԃ̎Z�o
		old_time = now_time;					// �ߋ����Ԃ̍X�V

		// �T�[�{�̒l���擾
		
		// ���x�擾
		Data[1].speed = ODrive.GetVelocity(0) *1000 ;	//
		Data[1].angle = ODrive.GetPosition(0) * 3.6f;
		Data[1].load = ODrive.GetCurrent(0) * 1000;
		

		Processing_time1 = clock() - now_time;




		Processing_time2 = clock() - Processing_time1 - now_time;




		pos_m0 = (float)sv_angle_rw_all/36.0f;
		ODrive.SetPosition(0, pos_m0);









		Processing_time3 = clock() - Processing_time2 - now_time - Processing_time1;




		if (flag_csv) {	// CSV�ɋL�^

			// ���Ԃ̎擾
			GetSystemTime(&st);
			// wHour���X���ԑ����āA���{���Ԃɂ���
			fprintf_s(csv_fp, "\"%04d/%02d/%02d_%02d:%02d:%02d.%03d\",",
				st.wYear, st.wMonth, st.wDay,
				st.wHour + 9, st.wMinute, st.wSecond, st.wMilliseconds);

			for (i = 1; i < 3; i++) {
				//�g���N�I�t�Ȃ�擾�l�@�I���Ȃ�w���p g_angle	
				fprintf_s(csv_fp, "%d,", Data[i].g_angle); // �ڕW�p�x
				fprintf_s(csv_fp, "%d,", Data[i].angle);   // �����p�x
				fprintf_s(csv_fp, "%d,", Data[i].speed);   // �����p���x
				fprintf_s(csv_fp, "%d,", Data[i].load);    // �����d��
				fprintf_s(csv_fp, "%d,", Data[i].temperature); // �������x
				fprintf_s(csv_fp, "0x%04x,", Data[i].error);   // �G���[
			}

			fprintf_s(csv_fp, "\n");

		}




	}

	//Futaba_RS.close();	// �V���A���|�[�g�N���[�Y


	ODrive.close();	// �V���A���|�[�g�N���[�Y
}


void CROBOT_CTLDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B


	if (nIDEvent == 5678) {

		int i;


		if (f_testmove_do == 0) UpdateData(TRUE);		// �X���C�_�̈ʒu�E�{�b�N�X�̒l��ϐ��ɑ�� (�Đ����łȂ����)

		//for (i = 1; i < sv_count + 1; i++) {
		//	sv_angle_r[i].Format(_T("%.1f deg"), ((float)Data[i].angle / 10));	//��M�����l[�p�x]���i�[
		//	sv_time_r[i].Format(_T("%.2f sec"), ((float)Data[i].time / 100));	//��M�����l[����]���i�[
		//	sv_speed_r[i].Format(_T("%d d/s"), Data[i].speed);					//��M�����l[���x]���i�[
		//	sv_load_r[i].Format(_T("%d mA"), Data[i].load);						//��M�����l[����]���i�[
		//	sv_temperature_r[i].Format(_T("%d��"), Data[i].temperature);		//��M�����l[���x]���i�[

		//	sv_error_r[i].Format(_T("%o"), Data[i].error);						// �G���[���i�[
		//	if (Data[i].error != 0) {
		//		sv_angle_r[i].Format(_T("XXXXX"));			// �G���[�̂Ƃ���[XXXXX]��\��
		//		sv_time_r[i].Format(_T("XXXXX"));			// �G���[�̂Ƃ���[XXXXX]��\��
		//		sv_speed_r[i].Format(_T("XXXXX"));			// �G���[�̂Ƃ���[XXXXX]��\��
		//		sv_load_r[i].Format(_T("XXXXX"));			// �G���[�̂Ƃ���[XXXXX]��\��
		//		sv_temperature_r[i].Format(_T("XXXXX"));	// �G���[�̂Ƃ���[XXXXX]��\��
		//	}

		//	sv_diff_w[i].Format(_T("%d"), sv_diff[i]);							// �������i�[


		//	Data[i].torque = sv_torque_w[i];									// �g���N�`�F�b�N�{�b�N�X���g���N�̒l�ɔ��f

		//	if (Data[i].torque == 0) {			// �g���N���I�t�̏ꍇ
		//		sv_angle_rw[i] = Data[i].angle;							// �p�x���X���C�_�[
		//		sv_angle_rw_v[i].Format(_T("%d"), Data[i].angle);		// �p�x���l�{�b�N�X
		//		sv_angle_rw_all = Data[1].angle;

		//	}
		//	else {								// �g���N���I���̏ꍇ

		//		//if (sv_reverse_w[i] == TRUE) {	// ���o�[�X���I���̎�
		//		//	Data[i].g_angle = -(do_sv_angle_rw_all + sv_diff[i]);
		//		//}
		//		//else {							// ���o�[�X���I�t�̎�
		//		//	Data[i].g_angle = do_sv_angle_rw_all + sv_diff[i];
		//		//}
		//		sv_angle_rw[i] = Data[i].g_angle;						// �p�x �� �X���C�_�[
		//		sv_angle_rw_v[i].Format(_T("%d"), Data[i].g_angle);		// �p�x �� �l�{�b�N�X
		//	}

		//}

		sv_speed_r[1].Format(_T("%.2f"), (float)Data[1].speed / 1000);					//��M�����l[���x]���i�[
		sv_angle_r[1].Format(_T("%.2f"), (float)Data[1].angle / 100);	//��M�����l[�p�x]���i�[
		sv_load_r[1].Format(_T("%d"), Data[1].load);						//��M�����l[����]���i�[
		sv_angle_rw[1] = Data[1].angle;
		
		sv_angle_rw_v_all.Format(_T("%d"), sv_angle_rw_all);		// �p�x���l�{�b�N�X
		
	


		// ���䎞�Ԃ̕\��
		temp0001.Format(_T("��������: %d msec (%d /%d /%d)"), Processing_time, Processing_time1, Processing_time2, Processing_time3);

		// ���W�I�{�^���̂��낢��

		UpdateData(FALSE);	//�ϐ��̒l���X���C�_�̈ʒu�E�{�b�N�X�̒l�ɔ��f

	}

	
	CDialogEx::OnTimer(nIDEvent);
	
}





void CROBOT_CTLDlg::OnBnClickedButtonCsv()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	int i;

	//���ݎ��Ԃ̎擾
	CTime cTime = CTime::GetCurrentTime();           // ���ݎ���

	//���ݎ��Ԃ��t�@�C������
	filetime = cTime.Format("FB_TEST_%Y%m%d_%H%M%S.csv");   // "YYYY/mm/dd HH:MM:SS"�`���̎�����������擾	// �t�@�C���� RTT ���A���^�C���e�B�[�`


	if (flag_csv == 0){


		fopen_s(&csv_fp, filetime, "w");	// �t�@�C���I�[�v��

		sv_csv_cap.SetWindowTextA("�� CSV�o�͒�...");

		// �^�C�g���o��
		fprintf_s(csv_fp, "�o�͎���,");
		for (i = 1; i < 3; i++) {
			//�g���N�I�t�Ȃ�擾�l�@�I���Ȃ�w���p g_angle	
			fprintf_s(csv_fp, "�ڕW�p�x(ID:%d),", i); // �ڕW�p�x
			fprintf_s(csv_fp, "�����p�x(ID:%d),", i);   // �����p�x
			fprintf_s(csv_fp, "�����p���x(ID:%d),", i);   // �����p���x
			fprintf_s(csv_fp, "�����d��(ID:%d),", i);    // �����d��
			fprintf_s(csv_fp, "�������x(ID:%d),", i); // �������x
			fprintf_s(csv_fp, "�G���[(ID:%d),", i);   // �G���[
		}

		fprintf_s(csv_fp, "\n");


		flag_csv = 1;
	}
	else {
		flag_csv = 0;
		Sleep(100);
		fclose(csv_fp); // �t�@�C���N���[�Y
		sv_csv_cap.SetWindowTextA("CSV�o��");
	}

	UpdateData(FALSE);

}


void CROBOT_CTLDlg::OnBnClickedButtonZero()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B


	sv_angle_rw_all = 0;

	UpdateData(FALSE);	//�ϐ��̒l���X���C�_�̈ʒu�E�{�b�N�X�̒l�ɔ��f
}


void CROBOT_CTLDlg::OnBnClickedButtonDiff()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	int i;
	int max = 0, min = 0;

	sv_reverse_w[1] = FALSE;

	for (i = 1; i < sv_count + 1; i++) {

		sv_diff[i] = Data[i].angle - Data[1].angle;
		
		if (sv_reverse_w[i] == FALSE) {		// ���]�̂Ƃ�
			sv_diff[i] = Data[i].angle - Data[1].angle;
		}
		else {								// �t�]�̂Ƃ�
			sv_diff[i] = -Data[i].angle - Data[1].angle;
		}

		if (max < sv_diff[i]) max = sv_diff[i];
		if (min > sv_diff[i]) min = sv_diff[i];

	}

	sv_angle_sl_ctl_all.SetRange(-1800 - min, 1800 - max);

}


void thread_testmove(void) {


	// ���݈ʒu��A��B��A��

	int i;
	int temp_point;

	f_testmove_do = 1;

	temp_point = sv_angle_rw_all;	// ���ݒn��ۑ�

	timeBeginPeriod(1);// �^�C�}�[�̍ŏ����x��1msec�ɂ���

	while (f_testmove) {

		// ���݈ʒu����A
		for (i = 0; i <= (timeBA/10); i++) {
			sv_angle_rw_all = temp_point + (int)(((double)i / (double)(timeBA / 10)) * (pointA - temp_point));			// �L���X�g�̃g���u��
			Sleep(10);	// 10�~���b
		}

		// A�n�_�ŐÎ~
		Sleep(time_A);

		// A�n�_����B
		for (i = 0; i <= (timeAB/10); i++) {
			sv_angle_rw_all = pointA + (int)(((double)i / (double)(timeAB / 10)) * (pointB - pointA));			// �L���X�g�̃g���u��
			Sleep(10);	// 10�~���b
		}

		// A�n�_�ŐÎ~
		Sleep(time_A);

		temp_point = pointB;	// ���ݒn��B��ۑ�

	}

	timeEndPeriod(1);// �^�C�}�[�̍ŏ����x��߂�
	// B�n�_�ŐÎ~
	f_testmove_do = 0;

}


void thread_testmove2(void) {


	// ���݈ʒu��A��B��A��

	int i=0;
	int temp_point;

	int do_sv_angle_rw_all;

	temp_point = sv_angle_rw_all;	// ���ݒn��ۑ�

	sv_ctl_f = 0;	// �{���̐���X���b�h�̏I��
	Sleep(1000);		// 1�b�҂�

	if (Futaba_RS.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};


	while (f_testmove2) {

		// ���݈ʒu����A
		do_sv_angle_rw_all = pointA + (int)(((double)i / (double)timeAB) * (pointB - pointA));			// �L���X�g�̃g���u��
		for (i = 1; i < sv_count + 1; i++) {
			if (sv_reverse_w[i] == TRUE) {	// ���o�[�X���I���̎�
				Data[i].g_angle = -(do_sv_angle_rw_all + sv_diff[i]);
			}
			else {							// ���o�[�X���I�t�̎�
				Data[i].g_angle = do_sv_angle_rw_all + sv_diff[i];
			}
			//�S�T�[�{���쑬�x��[timeBA]msec�ɐݒ�
			Data[i].g_time = timeBA / 10;
		}
		// �S�T�[�{�ɖڕW�p�x�ƖڕW���x�𑗐M�I
		Futaba_RS.sv_move_long(Data);
		Sleep(timeBA);



		// A�n�_�ŐÎ~
		Sleep(time_A);

		// A�n�_����B
		do_sv_angle_rw_all = temp_point + (int)(((double)i / (double)timeBA) * (pointA - temp_point));			// �L���X�g�̃g���u��	
		for (i = 1; i < sv_count + 1; i++) {
			if (sv_reverse_w[i] == TRUE) {	// ���o�[�X���I���̎�
				Data[i].g_angle = -(do_sv_angle_rw_all + sv_diff[i]);
			}
			else {							// ���o�[�X���I�t�̎�
				Data[i].g_angle = do_sv_angle_rw_all + sv_diff[i];
			}
			//�S�T�[�{���쑬�x��[timeBA]msec�ɐݒ�
			Data[i].g_time = timeAB / 10;
		}
		// �S�T�[�{�ɖڕW�p�x�ƖڕW���x�𑗐M�I
		Futaba_RS.sv_move_long(Data);
		Sleep(timeAB);

		// A�n�_�ŐÎ~
		Sleep(time_A);

		temp_point = pointB;	// ���ݒn��B��ۑ�

	}
	// B�n�_�ŐÎ~

	//�S�T�[�{���쑬�x��0msec�ɐݒ�
	for (i = 0; i < 100; i++) {
		Data[i].g_time = 0;
	}


	Futaba_RS.close();	// �V���A���|�[�g�N���[�Y
	Sleep(1000);

	sv_ctl_f = 1;	// �{���̐���X���b�h�̏I��

	////////////�X���b�h�p�p�����[�^
	HANDLE handle;

	////////////�X���b�h�̐���
	handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_motor, NULL, 0, NULL);

	// �Ƃ肠�����S�T�[�{�g���N�I�t
	for (i = 1; i < 9; i++) {
		sv_torque_w[i] = 0;
	}

}





void CROBOT_CTLDlg::OnBnClickedPlay()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	f_testmove = 1;

	////////////�X���b�h�p�p�����[�^
	HANDLE handle;

	////////////�X���b�h�̐���
	handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_testmove, NULL, 0, NULL);



	test_play.EnableWindow(FALSE);
	test_stop.EnableWindow(TRUE);
	test_play2.EnableWindow(FALSE);

	UpdateData(FALSE);


}


void CROBOT_CTLDlg::OnBnClickedStop()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	int i;

	f_testmove = 0; 
	f_testmove2 = 0;

	test_play.EnableWindow(TRUE);
	test_play2.EnableWindow(TRUE);
	test_stop.EnableWindow(FALSE);


	UpdateData(FALSE);

}


void CROBOT_CTLDlg::OnBnClickedPlay2()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	f_testmove2 = 1;

	////////////�X���b�h�p�p�����[�^
	HANDLE handle;

	////////////�X���b�h�̐���
	handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_testmove2, NULL, 0, NULL);

	test_play.EnableWindow(FALSE);
	test_stop.EnableWindow(TRUE);
	test_play2.EnableWindow(FALSE);

}


void CROBOT_CTLDlg::OnBnClickedButton4()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B


	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
												//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};



	ODrive.ODriveINIT();


	ODrive.close();	// �V���A���|�[�g�N���[�Y

}


void CROBOT_CTLDlg::OnBnClickedReadConf()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B





	float a=0, b=0;
	
	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};


	a = ODrive.Get_Vel_limit(0);
	Vel_limit.Format(_T("%f []"), a);
	

	b = ODrive.Get_Current_lim(0);
	Current_lim.Format(_T("%f []"), b);


	UpdateData(FALSE);	// �l��GUI


	ODrive.close();	// �V���A���|�[�g�N���[�Y
}


void CROBOT_CTLDlg::OnBnClickedWriteConf()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
										//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};


	ODrive.Set_Vel_limit(0, 10000.0f);
	ODrive.Set_Current_lim(0, 11.0f);


	ODrive.close();	// �V���A���|�[�g�N���[�Y


}


void CROBOT_CTLDlg::OnBnClickedSaveOdrive()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B



	//Current_lim.Format(_T("sssss"));



	//UpdateData(FALSE);	// �l��GUI


	float a = 0, b = 0;

	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};


	a = ODrive.Get_Vel_limit(0);
	Vel_limit.Format(_T("%f []"), a);


	b = ODrive.Get_Current_lim(0);
	Current_lim.Format(_T("%f []"), b);


	UpdateData(FALSE);	// �l��GUI


	ODrive.close();	// �V���A���|�[�g�N���[�Y

}


void CROBOT_CTLDlg::OnBnClickedButtonaaa()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	float a = 0, b = 0;

	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};


	a = ODrive.Get_Vel_limit(0);
	b = ODrive.Get_Current_lim(0);

	Vel_limit.Format(_T("%f"), a);


	Current_lim.Format(_T("%f"), b);



	ODrive.close();	// �V���A���|�[�g�N���[�Y

	UpdateData(FALSE);	// �l��GUI
}
