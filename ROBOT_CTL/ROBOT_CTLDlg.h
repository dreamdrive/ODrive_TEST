
// ROBOT_CTLDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"




class CROBOT_CTLDlgAutoProxy;


// CROBOT_CTLDlg �_�C�A���O
class CROBOT_CTLDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CROBOT_CTLDlg);
	friend class CROBOT_CTLDlgAutoProxy;

// �R���X�g���N�V����
public:
	CROBOT_CTLDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[
	virtual ~CROBOT_CTLDlg();

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROBOT_CTL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	CROBOT_CTLDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedSvOpen();
	afx_msg void OnBnClickedSvClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonCsv();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedPlay2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedWriteConf();
	afx_msg void OnBnClickedSaveOdrive();
	afx_msg void OnBnClickedButton3();

	// �V���A���|�[�g�֘A
	CEdit com_num;
	CString com_number;
	CString com_baudrate;
	CString com_openclode;

	// ��M�p�����[�^(�̃e�L�X�g�\���p)
	CString sv_angle_r[2];
	CString sv_speed_r[2];
	CString sv_load_r[2];


	CBitmap m_bmp;  // �\������r�b�g�}�b�v




	// ODrive�̃p�����[�^�ݒ�p
	CString Current_lim[2];
	CString Vel_limit[2];
	CString Pos_gain[2];
	CString Vel_gain[2];
	CString Vel_integrator_gain[2];


	int sv_angle_rw[2];
	CString sv_angle_rw_v[2];



	CString sv_angle_rw_v_all;
	CString sv_angle_rw_v_all1;

	CSliderCtrl sv_angle_sl_ctl[2];
	CSliderCtrl sv_angle_sl_ctl_all;
	CSliderCtrl sv_angle_sl_ctl_all1;


	CButton sv_opne_ctlf;
	CButton sv_close_ctlf;
	CButton sv_csv_cap;

	CButton test_play;
	CButton test_play2;
	CButton test_stop;

	CString temp0001;
	int flip_radio;



	CString csvlist;
	CListBox csv_list;

	CString current_filename;

	CListBox move_list;
	CString select_move;

	CListBox motion_file_list;
	CString motion_file_select;
	CString motion_name;

	int saisei_kaishi;

	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton20();
};
