
// ROBOT_CTL.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��


// CROBOT_CTLApp:
// ���̃N���X�̎����ɂ��ẮAROBOT_CTL.cpp ���Q�Ƃ��Ă��������B
//

class CROBOT_CTLApp : public CWinApp
{
public:
	CROBOT_CTLApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CROBOT_CTLApp theApp;
