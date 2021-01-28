#include <stdio.h>						// �W���w�b�_�[
#include <windows.h>					// Windows API�p�w�b�_�[
#include "stdafx.h"
#include "futaba_rs.h"

#define SERVO_ID		1				// �T�[�{ID�̎w��u1�v�ɐݒ�
#define COM_PORT		(char *)"COM1"	// �ʐM�|�[�g�̎w��

// �T�[�{�̓���
void futaba_rs::sv_move(int id, short sPos, unsigned short sTime) {
	
	unsigned char	sendbuf[28];
	unsigned char	sum;
	int				i;

	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)0xFA;						// �w�b�_�[1
	sendbuf[1] = (unsigned char)0xAF;						// �w�b�_�[2
	sendbuf[2] = (unsigned char)id;							// �T�[�{ID
	sendbuf[3] = (unsigned char)0x00;						// �t���O
	sendbuf[4] = (unsigned char)0x1E;						// �A�h���X(0x1E=30)
	sendbuf[5] = (unsigned char)0x04;						// ����(4byte)
	sendbuf[6] = (unsigned char)0x01;						// ��
	sendbuf[7] = (unsigned char)(sPos & 0x00FF);			// �ʒu
	sendbuf[8] = (unsigned char)((sPos & 0xFF00) >> 8);		// �ʒu
	sendbuf[9] = (unsigned char)(sTime & 0x00FF);			// ����
	sendbuf[10] = (unsigned char)((sTime & 0xFF00) >> 8);	// ����
															// �`�F�b�N�T���̌v�Z
	sum = sendbuf[2];
	for (i = 3; i < 11; i++) {
		sum = (unsigned char)(sum ^ sendbuf[i]);
	}
	sendbuf[11] = sum;								// �`�F�b�N�T��

	send((char *)sendbuf, 12);

}

// �T�[�{ID�ƃg���N�̃I���I�t
void futaba_rs::sv_torque(int id, int torque) {

	unsigned char	sendbuf[28];
	unsigned char	sum;
	int				i;

	short sPos = 0;
	unsigned short sTime = 0;

	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)0xFA;			// �w�b�_�[1
	sendbuf[1] = (unsigned char)0xAF;			// �w�b�_�[2
	sendbuf[2] = (unsigned char)id;				// �T�[�{ID
	sendbuf[3] = (unsigned char)0x00;			// �t���O
	sendbuf[4] = (unsigned char)0x24;			// �A�h���X(0x1E=30)
	sendbuf[5] = (unsigned char)0x01;			// ����(1byte)
	sendbuf[6] = (unsigned char)0x01;			// ��
	sendbuf[7] = (unsigned char)torque;		    // �g���N

												// �`�F�b�N�T���̌v�Z
	sum = sendbuf[2];
	for (i = 3; i < 8; i++) {
		sum = (unsigned char)(sum ^ sendbuf[i]);
	}
	sendbuf[8] = sum;								// �`�F�b�N�T��

	send((char *)sendbuf, 9);

}

sv_r futaba_rs::sv_read(int id)
{
	unsigned char	sendbuf[32];
	unsigned char	readbuf[128];
	unsigned char	sum;
	int				i;
	sv_r rDATA;

	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)0xFA;				// �w�b�_�[1
	sendbuf[1] = (unsigned char)0xAF;				// �w�b�_�[2
	sendbuf[2] = (unsigned char)id;					// �T�[�{ID
	sendbuf[3] = (unsigned char)0x09;				// �t���O(0x01 | 0x04<<1)
	sendbuf[4] = (unsigned char)0x00;				// �A�h���X(0x00)
	sendbuf[5] = (unsigned char)0x00;				// ����(0byte)
	sendbuf[6] = (unsigned char)0x01;				// ��
													// �`�F�b�N�T���̌v�Z
	sum = sendbuf[2];
	for (i = 3; i < 7; i++) {
		sum = (unsigned char)(sum ^ sendbuf[i]);
	}
	sendbuf[7] = sum;								// �`�F�b�N�T��


	send((char *)sendbuf, 8);


	// �ǂݍ��݃o�b�t�@�N���A
	memset(readbuf, 0x00, sizeof(readbuf));

	// ��M�I
	receive((char *)readbuf, 26);

	// ��M�f�[�^�̊m�F
	sum = readbuf[2];
	for (i = 3; i < 26; i++) {
		sum = sum ^ readbuf[i];
	}

	if (sum) {
		// �`�F�b�N�T���G���[
		rDATA.error = CHECKSUM_ERROR;
		return rDATA;
	}

	rDATA.angle = ((readbuf[8] << 8) & 0x0000FF00) | (readbuf[7] & 0x000000FF);
	rDATA.time = ((readbuf[10] << 8) & 0x0000FF00) | (readbuf[9] & 0x000000FF);
	rDATA.speed = ((readbuf[12] << 8) & 0x0000FF00) | (readbuf[11] & 0x000000FF);
	rDATA.load = ((readbuf[14] << 8) & 0x0000FF00) | (readbuf[13] & 0x000000FF);
	rDATA.temperature = ((readbuf[16] << 8) & 0x0000FF00) | (readbuf[15] & 0x000000FF);

	rDATA.error = NO_ERROR;	// �G���[�Ȃ���
	if (readbuf[3] != NO_ERROR)	rDATA.error = readbuf[3];

	return rDATA;
}


sv_r futaba_rs::sv_read2(int id)
{
	unsigned char	sendbuf[32];
	unsigned char	readbuf[128];
	unsigned char	sum;
	int				i;
	sv_r rDATA;

	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)0xFA;				// �w�b�_�[1
	sendbuf[1] = (unsigned char)0xAF;				// �w�b�_�[2
	sendbuf[2] = (unsigned char)id;					// �T�[�{ID
	sendbuf[3] = (unsigned char)0x0F;				// �t���O(0x0F) �w��A�h���X����̎w��̒�����ԓ�
	sendbuf[4] = (unsigned char)0x2A;				// �A�h���X(0x2A) ���݈ʒu
	sendbuf[5] = (unsigned char)0x0A;				// ����(10byte)
	sendbuf[6] = (unsigned char)0x00;				// �� (�C�ӃA�h���X���^�[���̏ꍇ��cnt=0x00)

													// �`�F�b�N�T���̌v�Z
	sum = sendbuf[2];
	for (i = 3; i < 7; i++) {
		sum = (unsigned char)(sum ^ sendbuf[i]);
	}
	sendbuf[7] = sum;								// �`�F�b�N�T��


	send((char *)sendbuf, 8);


	// �ǂݍ��݃o�b�t�@�N���A
	memset(readbuf, 0x00, sizeof(readbuf));

	// ��M�I
	receive((char *)readbuf, 18);

	// ��M�f�[�^�̊m�F
	sum = readbuf[2];
	for (i = 3; i < 18; i++) {
		sum = sum ^ readbuf[i];
	}


	if (sum) {
		// �`�F�b�N�T���G���[
		rDATA.error = CHECKSUM_ERROR;
		return rDATA;
	}

	rDATA.angle = ((readbuf[8] << 8) & 0x0000FF00) | (readbuf[7] & 0x000000FF);
	rDATA.time = ((readbuf[10] << 8) & 0x0000FF00) | (readbuf[9] & 0x000000FF);
	rDATA.speed = ((readbuf[12] << 8) & 0x0000FF00) | (readbuf[11] & 0x000000FF);
	rDATA.load = ((readbuf[14] << 8) & 0x0000FF00) | (readbuf[13] & 0x000000FF);
	rDATA.temperature = ((readbuf[16] << 8) & 0x0000FF00) | (readbuf[15] & 0x000000FF);

	rDATA.error = NO_ERROR;	// �G���[�Ȃ���
	if (readbuf[3] != NO_ERROR)	rDATA.error = readbuf[3];

	return rDATA;
}


void futaba_rs::sv_move_long(sv_r sendDATA[100]) {

	unsigned char	sendbuf[200];
	unsigned char	sum;
	int				i,j;

	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)0xFA;				    // �w�b�_�[1
	sendbuf[1] = (unsigned char)0xAF;				    // �w�b�_�[2
	sendbuf[2] = (unsigned char)0x00;				    // ID
	sendbuf[3] = (unsigned char)0x00;				    // �t���O
	sendbuf[4] = (unsigned char)0x1E;				    // �A�h���X(0x1E=30)
	sendbuf[5] = (unsigned char)0x05;				    // ����(5byte)
	sendbuf[6] = (unsigned char)0x08;				    // ��8��(0x08=0)

	j = 7;

	for (i = 1; i < 9; i++) {

		sendbuf[j] = i;															// ID

		sendbuf[j + 1] = (unsigned char)(sendDATA[i].g_angle & 0x00FF);			// �ʒu
		sendbuf[j + 2] = (unsigned char)((sendDATA[i].g_angle & 0xFF00) >> 8);	// �ʒu
		
		sendbuf[j + 3] = (unsigned char)(sendDATA[i].g_time & 0x00FF);			// ���x
		sendbuf[j + 4] = (unsigned char)((sendDATA[i].g_time & 0xFF00) >> 8);	// ���x

		j = j + 5;
	}


													// �`�F�b�N�T���̌v�Z
	sum = sendbuf[2];
	for (i = 3; i < j; i++) {
		sum = (unsigned char)(sum ^ sendbuf[i]);
	}
	sendbuf[j] = sum;								// �`�F�b�N�T��

	j++;
	send((char *)sendbuf, j);						// ���M

}



int futaba_rs::sv_read_torque(int id)
{
	unsigned char	sendbuf[32];
	unsigned char	readbuf[128];
	unsigned char	sum;
	int				i;
	//	sv_r rDATA;

	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)0xFA;				// �w�b�_�[1
	sendbuf[1] = (unsigned char)0xAF;				// �w�b�_�[2
	sendbuf[2] = (unsigned char)id;					// �T�[�{ID

	sendbuf[3] = (unsigned char)0x0F;				// �t���O(0x0F) �w��A�h���X����̎w��̒�����ԓ�

	sendbuf[4] = (unsigned char)0x24;				// �A�h���X(0x07) �ԐM�x������
	sendbuf[5] = (unsigned char)0x01;				// ����(1byte)
	sendbuf[6] = (unsigned char)0x00;				// �� (�C�ӃA�h���X���^�[���̏ꍇ��cnt=0x00)
													// �`�F�b�N�T���̌v�Z
	sum = sendbuf[2];
	for (i = 3; i < 7; i++) {
		sum = (unsigned char)(sum ^ sendbuf[i]);
	}
	sendbuf[7] = sum;								// �`�F�b�N�T��


	send((char *)sendbuf, 8);


	// �ǂݍ��݃o�b�t�@�N���A
	memset(readbuf, 0x00, sizeof(readbuf));

	// ��M�I
	receive((char *)readbuf, 9);

	// ��M�f�[�^�̊m�F
	sum = readbuf[2];
	for (i = 3; i < 9; i++) {
		sum = sum ^ readbuf[i];
	}

	if (sum) {
		// �`�F�b�N�T���G���[
		return -3;
	}

	return readbuf[7];
}



//---------------------------------------------------------------------------
// Futaba��sample����q��
bool futaba_rs::init(char* comport_in, int baudrate)
{
	bool flag = true;


	//HANDLE			hComm;		// �ʐM�p�n���h��
	DCB				cDcb;		// �ʐM�ݒ�p
	COMMTIMEOUTS	cTimeouts;	// �ʐM�|�[�g�^�C���A�E�g�p

	sprintf_s(comport, comport_in);


	// �ʐM�|�[�g���J��
	hComm = CreateFileA(comport,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	// �n���h���̊m�F
	if (hComm == INVALID_HANDLE_VALUE) {
		hComm = NULL;
		goto FuncEnd;
	}


	// �ʐM�ݒ�
	if (!GetCommState(hComm, &cDcb)) {
		// �ʐM�ݒ�G���[�̏ꍇ
		printf("ERROR:GetCommState error\n");
		CommClose();
		hComm = NULL;
		goto FuncEnd;
	}
	cDcb.BaudRate = baudrate;				// �ʐM���x
	cDcb.ByteSize = 8;					// �f�[�^�r�b�g��
	cDcb.fParity = TRUE;				// �p���e�B�`�F�b�N
	cDcb.Parity = NOPARITY;			// �m�[�p���e�B
	cDcb.StopBits = ONESTOPBIT;			// 1�X�g�b�v�r�b�g

	if (!SetCommState(hComm, &cDcb)) {
		// �ʐM�ݒ�G���[�̏ꍇ
		printf("ERROR:GetCommState error\n");
		CommClose();
		hComm = NULL;
		goto FuncEnd;
	}


	// �ʐM�ݒ�(�ʐM�^�C���A�E�g�ݒ�)
	// �����̓ǂݍ��ݑ҂�����(ms)
	cTimeouts.ReadIntervalTimeout = 50;
	// �ǂݍ��݂̂P����������̎���(ms)
	cTimeouts.ReadTotalTimeoutMultiplier = 50;
	// �ǂݍ��݂̒萔����(ms)
	cTimeouts.ReadTotalTimeoutConstant = 50;
	// �������݂̂P����������̎���(ms)
	cTimeouts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(hComm, &cTimeouts)) {
		// �ʐM�ݒ�G���[�̏ꍇ
		printf("ERROR:SetCommTimeouts error\n");
		CommClose();
		hComm = NULL;
		goto FuncEnd;
	}


	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);


FuncEnd:
	//return hComm;
	flag_opened = 1;
	return(flag);

}

//---------------------------------------------------------------------------
bool futaba_rs::close(void)
{
	if (flag_opened != 1)return false;

	if (hComm) {
		CloseHandle(hComm);
	}

	return 1;
	//return CloseHandle(hComm);

}
//---------------------------------------------------------------------------
int futaba_rs::receive(char* buf_ptr, int size)
{
	//�󂯎�����o�C�g������Ԃ�->�ǂ��Ȃ��DEOF�𒴂����Ƃ��ɂ͂܂�
	//ReadFile�̕Ԃ�l��true�ŁCbyte��0�̂Ƃ�
	//->�t�@�C���|�C���^��EOF�𒴂��Ă��܂����C�Ƃ����Ӗ��炵��
	unsigned long byte, event;
	byte = 0;
	bool flag = false;


	while (!flag)
	{
		//��������M����܂ő҂�
		WaitCommEvent(hComm, &event, NULL);
		if (event | EV_RXCHAR)//��������M�������H
		{
			if (ReadFile(hComm, buf_ptr, size, &byte, NULL))
				flag = true;
			break;
		}
		byte = 0;//������
	}


	return byte;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int futaba_rs::send(char* buf_ptr, int size)
{
	//�o�b�t�@�̓��e�𑗂�
	unsigned long byte;
	if (flag_opened != 1)
	{
		printf("send() error. port Not opend\n");//debug
		printf("flag_opened=%d\n", flag_opened);//debug
		return -1;
	}


	if (WriteFile(hComm, buf_ptr, size, &byte, NULL))
		//��buffer����ǂݏo����hcom(�ʐM�|�[�g)�֏�������
	{
		return byte;
	}
	else return -1;

}
//---------------------------------------------------------------------------
/*----------------------------------------------------------------------------*/
/*
 *	�T�v�F�ʐM�|�[�g�����
 *
 *	�֐��FHANDLE CommClose( HANDLE hComm )
 *	�����F
 *		HANDLE			hComm		�ʐM�|�[�g�̃n���h��
 *	�߂�l�F
 *		1				����
 *
 */
int futaba_rs::CommClose()
{
	if( hComm ){
		CloseHandle( hComm );
	}

	return 1;
}