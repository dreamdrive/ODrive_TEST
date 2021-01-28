
// DlgProxy.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "ROBOT_CTL.h"
#include "DlgProxy.h"
#include "ROBOT_CTLDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CROBOT_CTLDlgAutoProxy

IMPLEMENT_DYNCREATE(CROBOT_CTLDlgAutoProxy, CCmdTarget)

CROBOT_CTLDlgAutoProxy::CROBOT_CTLDlgAutoProxy()
{
	EnableAutomation();
	
	// �I�[�g���[�V���� �I�u�W�F�N�g���A�N�e�B�u�ł������A�A�v���P�[�V������ 
	//	���s��Ԃɂ��Ă��������A�R���X�g���N�^�[�� AfxOleLockApp ���Ăяo���܂��B
	AfxOleLockApp();

	// �A�v���P�[�V�����̃��C�� �E�B���h�E �|�C���^�[���Ƃ����ă_�C�A���O
	//  �փA�N�Z�X���܂��B�v���L�V�̓����|�C���^�[����_�C�A���O�ւ̃|�C
	//  ���^��ݒ肵�A�_�C�A���O�̖߂�|�C���^�[�����̃v���L�V�֐ݒ肵��
	//  ���B
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CROBOT_CTLDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CROBOT_CTLDlg)))
		{
			m_pDialog = reinterpret_cast<CROBOT_CTLDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CROBOT_CTLDlgAutoProxy::~CROBOT_CTLDlgAutoProxy()
{
	// ���ׂẴI�u�W�F�N�g���I�[�g���[�V�����ō쐬���ꂽ�ꍇ�ɃA�v���P�[�V����
	//	���I�����邽�߂ɁA�f�X�g���N�^�[�� AfxOleUnlockApp ���Ăяo���܂��B
	//  ���̏����̊ԂɁA���C�� �_�C�A���O��j�󂵂܂��B
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CROBOT_CTLDlgAutoProxy::OnFinalRelease()
{
	// �I�[�g���[�V���� �I�u�W�F�N�g�ɑ΂���Ō�̎Q�Ƃ���������Ƃ���
	// OnFinalRelease ���Ăяo����܂��B���N���X�͎����I�ɃI�u�W�F�N�g��
	// �폜���܂��B�ǉ��̃N���[���A�b�v���K�v�ł�
	// �ʂȌ㏈����ǉ����Ă��������B

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CROBOT_CTLDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CROBOT_CTLDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// ����: VBA ����^�C�v �Z�[�t�ȃo�C���h���T�|�[�g���邽�߂ɁAIID_IROBOT_CTL �̃T�|�[�g��ǉ����܂��B
//  ���� IID �́A.IDL �t�@�C���̃f�B�X�p�b�` �C���^�[�t�F�C�X�փA�^�b�`����� 
//  GUID �ƈ�v���Ȃ���΂Ȃ�܂���B

// {7247C1E0-D0DC-46E5-9EAC-AB57596BF892}
static const IID IID_IROBOT_CTL =
{ 0x7247C1E0, 0xD0DC, 0x46E5, { 0x9E, 0xAC, 0xAB, 0x57, 0x59, 0x6B, 0xF8, 0x92 } };

BEGIN_INTERFACE_MAP(CROBOT_CTLDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CROBOT_CTLDlgAutoProxy, IID_IROBOT_CTL, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 �}�N�����A���̃v���W�F�N�g�� StdAfx.h �Œ�`����܂��B
// {F971C2A3-EE25-4C5F-A5E2-86C3A36DC739}
IMPLEMENT_OLECREATE2(CROBOT_CTLDlgAutoProxy, "ROBOT_CTL.Application", 0xf971c2a3, 0xee25, 0x4c5f, 0xa5, 0xe2, 0x86, 0xc3, 0xa3, 0x6d, 0xc7, 0x39)


// CROBOT_CTLDlgAutoProxy ���b�Z�[�W �n���h���[
