
// DlgProxy.h: �w�b�_�[ �t�@�C��
//

#pragma once

class CROBOT_CTLDlg;


// CROBOT_CTLDlgAutoProxy �R�}���h �^�[�Q�b�g

class CROBOT_CTLDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CROBOT_CTLDlgAutoProxy)

	CROBOT_CTLDlgAutoProxy();           // ���I�����Ŏg�p����� protected �R���X�g���N�^�[

// ����
public:
	CROBOT_CTLDlg* m_pDialog;

// ����
public:

// �I�[�o�[���C�h
	public:
	virtual void OnFinalRelease();

// ����
protected:
	virtual ~CROBOT_CTLDlgAutoProxy();

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CROBOT_CTLDlgAutoProxy)

	// �������ꂽ OLE �f�B�X�p�b�`���蓖�Ċ֐�

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

