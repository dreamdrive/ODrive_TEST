
// ROBOT_CTLDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "ROBOT_CTL.h"
#include "ROBOT_CTLDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
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

// ODrive����p�N���X
ODriveArduino ODrive;

// ODrive�̃p�����[�^(���M�p)
float pos_m[2] = { 0.0f ,0.0f };

// ODrive�̃p�����[�^(��M�p)
float r_pos_m[2] = { 0.0f ,0.0f };
float r_vel_m[2] = { 0.0f ,0.0f };
float r_current_m[2] = { 0.0f ,0.0f };

// �V���A���|�[�g�֘A
char get_port[10];
int get_baud;

// �T�[�{�R���g���[���t���O(����X���b�h�̃t���O)
int sv_ctl_f = 0;		

// �������Ԍv���p�̕ϐ�
clock_t Processing_time;
clock_t Processing_time1;
clock_t Processing_time2;
clock_t Processing_time3;

// CSV�֘A
CString filetime;		// CSV��ۑ�����Ƃ��̃t�@�C����
int flag_csv = 0;		// CSV�t���O
FILE* csv_fp;			// CSV�̃t�@�C���|�C���^

// �w���l(�X���C�_)
int sv_angle_rw_all = 0;
int sv_angle_rw_all1 = 0;
int goal_angle[2] = {0,0};

// �`�F�b�N�{�b�N�X
BOOL chk_axis[2] = {FALSE,FALSE };

// ���׃e�X�g�̃p�����[�^
int pointA;
int pointB;
int timeAB;
int timeBA;
int time_A;
int time_B;

// ���׃e�X�g�̓���Ɏg���t���O
int f_testmove = 0;			// �e�X�g����J�n�t���O
int f_testmove_do = 0;		// �e�X�g������s���t���O

// �֐�
void thread_motor(void);		// �T�[�{����p�X���b�h
void thread_testmove(void);	    // �e�X�g����p�X���b�h

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

	// �V���A���|�[�g����֘A
	DDX_Text(pDX, IDC_EDIT2, com_number);						// COM�ԍ�
	DDX_Text(pDX, IDC_EDIT3, com_baudrate);						// �{�[���[�g
	DDX_Text(pDX, IDC_EDIT5, com_openclode);					// �X�e�[�^�X
	DDX_Control(pDX, IDC_SV_OPEN, sv_opne_ctlf);				// OPEN
	DDX_Control(pDX, IDC_SV_CLOSE, sv_close_ctlf);				// CLOSE
	DDX_Text(pDX, IDC_TEMP0001, temp0001);						// ���䎞�ԕ\���p

	// ODrive ��M�\�� axis0
	DDX_Text(pDX, IDC_ANGLE_00, sv_angle_r[0]);
	DDX_Text(pDX, IDC_SPEED_00, sv_speed_r[0]);
	DDX_Text(pDX, IDC_LOAD_00, sv_load_r[0]);
	DDX_Slider(pDX, IDC_ANGLE_SL00, sv_angle_rw[0]);			// �l
	DDX_Control(pDX, IDC_ANGLE_SL00, sv_angle_sl_ctl[0]);		// �X���C�_�̃R���g���[��

	// ODrive ��M�\�� axis1
	DDX_Text(pDX, IDC_ANGLE_01, sv_angle_r[1]);
	DDX_Text(pDX, IDC_SPEED_01, sv_speed_r[1]);
	DDX_Text(pDX, IDC_LOAD_01, sv_load_r[1]);
	DDX_Slider(pDX, IDC_ANGLE_SL01, sv_angle_rw[1]);			// �l
	DDX_Control(pDX, IDC_ANGLE_SL01, sv_angle_sl_ctl[1]);		// �X���C�_�̃R���g���[��

	// �ʒu���� ���M�p axis0
	DDX_Text(pDX, IDC_ANGLE_VALL, goal_angle_str[0]);			// �p�x�̒l(���l�\��(����))
	DDX_Slider(pDX, IDC_ANGLE_SALL, goal_angle[0]);				// �X���C�_�̒l
	DDX_Control(pDX, IDC_ANGLE_SALL, goal_angle_ctl[0]);		// �X���C�_�̃R���g���[���[(����E�����̐ݒ�p)

	// �ʒu���� ���M�p axis1
	DDX_Text(pDX, IDC_ANGLE_VALL1, goal_angle_str[1]);			// �p�x�̒l(���l�\��(����))
	DDX_Slider(pDX, IDC_ANGLE_SALL1, goal_angle[1]);			// �X���C�_�̒l
	DDX_Control(pDX, IDC_ANGLE_SALL1, goal_angle_ctl[1]);		// �X���C�_�̃R���g���[���[(����E�����̐ݒ�p)

	// CSV�o�̓{�^��
	DDX_Control(pDX, IDC_BUTTON_CSV, sv_csv_cap);

	// ODrve �R���t�B�Oaxis0
	DDX_Text(pDX, IDC_vel_lim, Vel_limit[0]);
	DDX_Text(pDX, IDC_current_lim, Current_lim[0]);
	DDX_Text(pDX, IDC_pos_gain, Pos_gain[0]);
	DDX_Text(pDX, IDC_vel_gain, Vel_gain[0]);
	DDX_Text(pDX, IDC_vel_integrator_gain, Vel_integrator_gain[0]);

	// ODrve �R���t�B�Oaxis1
	DDX_Text(pDX, IDC_vel_lim1, Vel_limit[1]);
	DDX_Text(pDX, IDC_current_lim1, Current_lim[1]);
	DDX_Text(pDX, IDC_pos_gain1, Pos_gain[1]);
	DDX_Text(pDX, IDC_vel_gain1, Vel_gain[1]);
	DDX_Text(pDX, IDC_vel_integrator_gain1, Vel_integrator_gain[1]);

	// ���׃e�X�g�J�n�E��~�{�^���R���g���[��
	DDX_Control(pDX, IDC_PLAY, test_play);
	DDX_Control(pDX, IDC_STOP, test_stop);

	// ���׃e�X�g�p�����[�^�[
	DDX_Text(pDX, IDC_pointA, pointA);
	DDX_Text(pDX, IDC_pointB, pointB);
	DDX_Text(pDX, IDC_time_A, time_A);
	DDX_Text(pDX, IDC_time_B, time_B);
	DDX_Text(pDX, IDC_timeAB, timeAB);
	DDX_Text(pDX, IDC_timeBA, timeBA);

	// axis0,1�g�p�`�F�b�N
	DDX_Check(pDX, IDC_CHECK00, chk_axis[0]);
	DDX_Check(pDX, IDC_CHECK01, chk_axis[1]);

}

BEGIN_MESSAGE_MAP(CROBOT_CTLDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()

	ON_BN_CLICKED(IDOK, &CROBOT_CTLDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CROBOT_CTLDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_SV_OPEN, &CROBOT_CTLDlg::OnBnClickedSvOpen)
	ON_BN_CLICKED(IDC_SV_CLOSE, &CROBOT_CTLDlg::OnBnClickedSvClose)
	ON_BN_CLICKED(IDC_BUTTON_CSV, &CROBOT_CTLDlg::OnBnClickedButtonCsv)
	ON_BN_CLICKED(IDC_PLAY, &CROBOT_CTLDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_STOP, &CROBOT_CTLDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_BUTTON4, &CROBOT_CTLDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_WRITE_CONF, &CROBOT_CTLDlg::OnBnClickedWriteConf)
	ON_BN_CLICKED(IDC_SAVE_ODRIVE, &CROBOT_CTLDlg::OnBnClickedSaveOdrive)
	ON_BN_CLICKED(IDC_BUTTON3, &CROBOT_CTLDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CROBOT_CTLDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON20, &CROBOT_CTLDlg::OnBnClickedButton20)
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

	// ���M�p�X���C�_
	goal_angle_ctl[0].SetRange(-1800, 1800);	// �ő�E�ŏ�180�x ��0.1�x�����݂ɏo����悤1800
	goal_angle_ctl[1].SetRange(-1800, 1800);	// �ő�E�ŏ�180�x ��0.1�x�����݂ɏo����悤1800

	// ��M�p�X���C�_
	sv_angle_sl_ctl[0].SetRange(-180, 180);		// �ő�E�ŏ�180�x
	sv_angle_sl_ctl[1].SetRange(-180, 180);		// �ő�E�ŏ�180�x
	
	// ��U�K���Ȑ��l�ɂ��ĉ�ʍX�V(�ő�ŏ��̔��f�̂���)
	goal_angle[0] = 100;
	goal_angle[1] = 100;
	sv_angle_rw[0] = 100;
	sv_angle_rw[1] = 100;
	UpdateData(FALSE);	// �l��GUI

	// 0�ʒu�Ń��Z�b�g����ʍX�V
	goal_angle[0] = 0;
	goal_angle[1] = 0;
	sv_angle_rw[0] = 0;
	sv_angle_rw[1] = 0;
	UpdateData(FALSE);	// �l��GUI

	// INI�t�@�C������擾
	GetPrivateProfileString("serial", "COM", "INI ERROR", get_port, sizeof(get_port), SETFILE);
	get_baud = GetPrivateProfileInt("serial", "BAUD", 0, SETFILE);

	com_number = get_port;
	com_baudrate.Format(_T("%d bps"), get_baud);
	com_openclode = "CLOSE";


	// A-B ���� ��{�p�����[�^
	pointA = 0;
	pointB = 20;
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


void CROBOT_CTLDlg::OnBnClickedButton1()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};

	// ���Z�b�g�R�}���h
	ODrive.reset();

	// �V���A���|�[�g�N���[�Y
	ODrive.close();

}

void CROBOT_CTLDlg::OnBnClickedSvOpen()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	com_openclode = "OPEN";
	UpdateData(FALSE);
	
	//	sv_open();
	sv_ctl_f = 1;	//�X���b�h�R���g���[���p�̃t���O (�X���b�h�𗧂��グ��O��1�ɂ��āA�����0�ɂ���ƃX���b�h�̖������[�v���I���)

	////////////�X���b�h�p�p�����[�^
	HANDLE handle;

	////////////�X���b�h�̐���
	handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_motor, NULL, 0, NULL);


	// �\���̕ύX
	sv_opne_ctlf.EnableWindow(FALSE);
	sv_close_ctlf.EnableWindow(TRUE);

	UpdateData(FALSE);

}

void CROBOT_CTLDlg::OnBnClickedSvClose()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	sv_ctl_f = 0;

	com_openclode = "CLOSE";
	UpdateData(FALSE);

	sv_opne_ctlf.EnableWindow(TRUE);
	sv_close_ctlf.EnableWindow(FALSE);

}

//----------------------------�T�[�{���[�^�[�̊֘A�֐� -------------------------

//���[�^�[����p�̃X���b�h
void thread_motor(void)
{

	int i;
	SYSTEMTIME st;
	clock_t now_time = 0,old_time = 0;	// ���Ԍv���p

	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
												//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};

	while (sv_ctl_f) {
		
		// �������Ԃ̎擾
		now_time = clock();						// ���ݎ��Ԃ̍X�V
		Processing_time = now_time - old_time;	// �������Ԃ̎Z�o
		old_time = now_time;					// �ߋ����Ԃ̍X�V

		
		// ODrive�p�����[�^�擾(axis0)
		if (chk_axis[0]){
			r_vel_m[0] = ODrive.GetVelocity(0);
			r_pos_m[0] = ODrive.GetPosition(0);
			r_current_m[0] = ODrive.GetCurrent(0);
		}

		// ODrive�p�����[�^�擾(axis1)
		if (chk_axis[1]) {
			r_vel_m[1] = ODrive.GetVelocity(1);
			r_pos_m[1] = ODrive.GetPosition(1);
			r_current_m[1] = ODrive.GetCurrent(1);
		}

		// ���Ԍv��
		Processing_time1 = clock() - now_time;

		// ODrive�p�����[�^���M(axis0)
		if (chk_axis[0]) {
			pos_m[0] = (float)goal_angle[0] / 36.0f;
			ODrive.SetPosition(0, pos_m[0]);
		}
		// ODrive�p�����[�^���M(axis1)
		if (chk_axis[1]) {
			pos_m[1] = (float)goal_angle[1] / 36.0f;
			ODrive.SetPosition(1, pos_m[1]);
		}

		// ���Ԍv��
		Processing_time2 = clock() - Processing_time1 - now_time;


		if (flag_csv) {	// CSV�ɋL�^

			// ���Ԃ̎擾
			GetSystemTime(&st);
			// wHour���X���ԑ����āA���{���Ԃɂ���
			fprintf_s(csv_fp, "\"%04d/%02d/%02d_%02d:%02d:%02d.%03d\",",
				st.wYear, st.wMonth, st.wDay,
				st.wHour + 9, st.wMinute, st.wSecond, st.wMilliseconds);

			for (i = 0; i < 2; i++) {
				//�g���N�I�t�Ȃ�擾�l�@�I���Ȃ�w���p g_angle	
				fprintf_s(csv_fp, "%f,", pos_m[i] * 3.6f); // �ڕW�p�x
				fprintf_s(csv_fp, "%f,", r_pos_m[i] * 3.6f);   // �����p�x
				fprintf_s(csv_fp, "%f,", r_vel_m[i] * 3.6f);   // �����p���x
				fprintf_s(csv_fp, "%f,", r_current_m[i] * 3.6f);    // �����d��
			}
			fprintf_s(csv_fp, "\n");
		}

		// ���Ԍv��
		Processing_time3 = clock() - Processing_time2 - now_time - Processing_time1;
	}

	ODrive.close();	// �V���A���|�[�g�N���[�Y

}


void CROBOT_CTLDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	if (nIDEvent == 5678) {

		if (f_testmove_do == 0) UpdateData(TRUE);		// �X���C�_�̈ʒu�E�{�b�N�X�̒l��ϐ��ɑ�� (�Đ����łȂ����)

		sv_speed_r[0].Format(_T("%.1f"), r_vel_m[0] * 3.6f);			//��M�����l[���x]���i�[ (�������deg/s�ɕϊ�)
		sv_angle_r[0].Format(_T("%.1f"), r_pos_m[0] * 3.6f);			//��M�����l[�p�x]���i�[ (�������deg�ɕϊ�)
		sv_angle_rw[0] = r_pos_m[0] * 3.6f;
		sv_load_r[0].Format(_T("%.0f"), r_current_m[0] * 1000);			//��M�����l[����]���i�[ (mA�ɕϊ�)

		goal_angle_str[0].Format(_T("%.1f"), goal_angle[0]/10.0f);			// �p�x���l�{�b�N�X
		
		sv_speed_r[1].Format(_T("%.1f"), r_vel_m[1] * 3.6f);			//��M�����l[���x]���i�[ (�������deg/s�ɕϊ�)
		sv_angle_r[1].Format(_T("%.1f"), r_pos_m[1] * 3.6f);			//��M�����l[�p�x]���i�[ (�������deg�ɕϊ�)
		sv_angle_rw[1] = r_pos_m[1] * 3.6f;
		sv_load_r[1].Format(_T("%.0f"), r_current_m[1] * 1000);			//��M�����l[����]���i�[ (mA�ɕϊ�)
						
		goal_angle_str[1].Format(_T("%.1f"), goal_angle[1]/10.0f);			// �p�x���l�{�b�N�X

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
		for (i = 0; i < 2; i++) {
			//�g���N�I�t�Ȃ�擾�l�@�I���Ȃ�w���p g_angle	
			fprintf_s(csv_fp, "�ڕW�p�x(deg)[axis:%d],", i); // �ڕW�p�x
			fprintf_s(csv_fp, "�����p�x(deg)[axis:%d],", i);   // �����p�x
			fprintf_s(csv_fp, "�����p���x(deg/s)[axis:%d],", i);   // �����p���x
			fprintf_s(csv_fp, "�����d��(mA)[axis:%d],", i);    // �����d��
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

void thread_testmove(void) {

	// ���݈ʒu��A��B��A��

	int i;
	int temp_point;

	// �e�X�g������s���t���O
	f_testmove_do = 1;

	temp_point = goal_angle[0];	// ���ݒn��ۑ�

	timeBeginPeriod(1);// �^�C�}�[�̍ŏ����x��1msec�ɂ���

	while (f_testmove) {

		// ���݈ʒu����A
		for (i = 0; i <= (timeBA/10); i++) {
			goal_angle[0] = temp_point + (int)(((double)i / (double)(timeBA / 10)) * ((pointA*10) - temp_point));			// �L���X�g�̃g���u��
			Sleep(10);	// 10�~���b
		}

		// A�n�_�ŐÎ~
		Sleep(time_A);

		// A�n�_����B
		for (i = 0; i <= (timeAB/10); i++) {
			goal_angle[0] = (pointA * 10) + (int)(((double)i / (double)(timeAB / 10)) * ((pointB * 10) - (pointA * 10)));			// �L���X�g�̃g���u��
			Sleep(10);	// 10�~���b
		}

		// A�n�_�ŐÎ~
		Sleep(time_A);

		temp_point = (pointB * 10);	// ���ݒn��B��ۑ�

	}

	timeEndPeriod(1);// �^�C�}�[�̍ŏ����x��߂�
	// B�n�_�ŐÎ~

	// �e�X�g������s���t���O
	f_testmove_do = 0;
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

	UpdateData(FALSE);

}

void CROBOT_CTLDlg::OnBnClickedStop()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	f_testmove = 0; 

	test_play.EnableWindow(TRUE);
	test_stop.EnableWindow(FALSE);

	UpdateData(FALSE);
}


void CROBOT_CTLDlg::OnBnClickedButton4()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
												//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};

	if (chk_axis[0]) ODrive.ODriveINIT(0);	// �g�p����Ȃ珉����
	if (chk_axis[1]) ODrive.ODriveINIT(1);	// �g�p����Ȃ珉����

	ODrive.close();	// �V���A���|�[�g�N���[�Y
}


void CROBOT_CTLDlg::OnBnClickedWriteConf()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
										//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};

	UpdateData(TRUE);	// �l <- GUI

	ODrive.Set_Vel_limit(0, atof(Vel_limit[0]));
	ODrive.Set_Current_lim(0, atof(Current_lim[0]));
	ODrive.Set_pos_gain(0, atof(Pos_gain[0]));
	ODrive.Set_vel_gain(0, atof(Vel_gain[0]));
	ODrive.Set_vel_integrator_gain(0, atof(Vel_integrator_gain[0]));

	ODrive.Set_Vel_limit(1, atof(Vel_limit[1]));
	ODrive.Set_Current_lim(1, atof(Current_lim[1]));
	ODrive.Set_pos_gain(1, atof(Pos_gain[1]));
	ODrive.Set_vel_gain(1, atof(Vel_gain[1]));
	ODrive.Set_vel_integrator_gain(1, atof(Vel_integrator_gain[1]));

	ODrive.close();	// �V���A���|�[�g�N���[�Y

}

void CROBOT_CTLDlg::OnBnClickedSaveOdrive()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B


	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};

	// �R���t�B�O�Z�[�u
	ODrive.save_conf();

	// �V���A���|�[�g�N���[�Y
	ODrive.close();	

}

void CROBOT_CTLDlg::OnBnClickedButton3()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	float temp = 0;

	if (ODrive.init(get_port, get_baud) != true) {		//	COM�|�[�g RS232C�̏�����
											//	printf("�|�[�g(%s)���I�[�v���o���܂���ł����B\n",OPN_COM);
		while (1);
	};

	temp = ODrive.Get_Vel_limit(0);
	Vel_limit[0].Format(_T("%.2f"), temp);

	temp = ODrive.Get_Current_lim(0);
	Current_lim[0].Format(_T("%.2f"), temp);

	temp = ODrive.Get_pos_gain(0);
	Pos_gain[0].Format(_T("%.2f"), temp);

	temp = ODrive.Get_vel_gain(0);
	Vel_gain[0].Format(_T("%.2f"), temp);

	temp = ODrive.Get_integrator_gain(0);
	Vel_integrator_gain[0].Format(_T("%.2f"), temp);


	temp = ODrive.Get_Vel_limit(1);
	Vel_limit[1].Format(_T("%.2f"), temp);

	temp = ODrive.Get_Current_lim(1);
	Current_lim[1].Format(_T("%.2f"), temp);

	temp = ODrive.Get_pos_gain(1);
	Pos_gain[1].Format(_T("%.2f"), temp);

	temp = ODrive.Get_vel_gain(1);
	Vel_gain[1].Format(_T("%.2f"), temp);

	temp = ODrive.Get_integrator_gain(1);
	Vel_integrator_gain[1].Format(_T("%.2f"), temp);

	UpdateData(FALSE);	// �l��GUI

	ODrive.close();	// �V���A���|�[�g�N���[�Y
}


void CROBOT_CTLDlg::OnBnClickedButton2()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	goal_angle[0] = 0;

	UpdateData(FALSE);	// �l��GUI

}


void CROBOT_CTLDlg::OnBnClickedButton20()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B


	goal_angle[1] = 0;

	UpdateData(FALSE);	// �l��GUI
}
