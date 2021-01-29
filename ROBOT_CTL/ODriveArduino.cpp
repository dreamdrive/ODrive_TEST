//#include "Arduino.h"
#include <stdio.h>						// �W���w�b�_�[
#include <windows.h>					// Windows API�p�w�b�_�[
#include "stdafx.h"
#include "ODriveArduino.h"
#include <string>

#define COM_PORT		(char *)"COM1"	// �ʐM�|�[�g�̎w��


int sjlen(const char* str);

static const int kMotorOffsetFloat = 2;
static const int kMotorStrideFloat = 28;
static const int kMotorOffsetInt32 = 0;
static const int kMotorStrideInt32 = 4;
static const int kMotorOffsetBool = 0;
static const int kMotorStrideBool = 4;
static const int kMotorOffsetUint16 = 0;
static const int kMotorStrideUint16 = 2;

// Print with stream operator
//template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
//template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

//ODriveArduino::ODriveArduino(Stream& serial)
//    : serial_(serial) {}

void ODriveArduino::SetPosition(int motor_number, float position) {
    SetPosition(motor_number, position, 0.0f, 0.0f);
}

void ODriveArduino::SetPosition(int motor_number, float position, float velocity_feedforward) {
    SetPosition(motor_number, position, velocity_feedforward, 0.0f);
}

void ODriveArduino::SetPosition(int motor_number, float position, float velocity_feedforward, float current_feedforward) {

	char sendbuf[100];
	// ���M�o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	//serial_ << "p " << motor_number  << " " << position << " " << velocity_feedforward << " " << current_feedforward << "\n";
	sprintf_s(sendbuf, "p %d %f %f %f\n", motor_number, position, velocity_feedforward, current_feedforward);


	send((char*)sendbuf, sjlen((char*)sendbuf));
}

void ODriveArduino::SetVelocity(int motor_number, float velocity) {
    SetVelocity(motor_number, velocity, 0.0f);
}

void ODriveArduino::SetVelocity(int motor_number, float velocity, float current_feedforward) {
    


	char sendbuf[100];
	// ���M�o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	//serial_ << "v " << motor_number  << " " << velocity << " " << current_feedforward << "\n";
	sprintf_s(sendbuf, "v %d %f %f\n", motor_number, velocity, current_feedforward);


	send((char*)sendbuf, sjlen((char*)sendbuf));


}

void ODriveArduino::SetCurrent(int motor_number, float current) {



	char sendbuf[100];
	// ���M�o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	//serial_ << "c " << motor_number << " " << current << "\n";
	sprintf_s(sendbuf, "c %d %f\n", motor_number, current);


	send((char*)sendbuf, sjlen((char*)sendbuf));



}

void ODriveArduino::TrapezoidalMove(int motor_number, float position){



	char sendbuf[100];
	// ���M�o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	//serial_ << "t " << motor_number << " " << position << "\n";
	sprintf_s(sendbuf, "t %d %f\n", motor_number, position);


	send((char*)sendbuf, sjlen((char*)sendbuf));




}

void ODriveArduino::ODriveINIT() {

	char sendbuf1[100];
	char sendbuf2[100];
	char sendbuf3[100];
	char sendbuf4[100];

	int requested_state;


	// �ǂݍ��݃o�b�t�@�N���A
	memset(sendbuf1, 0x00, sizeof(sendbuf1));
	// �ǂݍ��݃o�b�t�@�N���A
	memset(sendbuf2, 0x00, sizeof(sendbuf2));
	// �ǂݍ��݃o�b�t�@�N���A
	memset(sendbuf3, 0x00, sizeof(sendbuf3));
	// �ǂݍ��݃o�b�t�@�N���A
	memset(sendbuf4, 0x00, sizeof(sendbuf4));


	sprintf_s(sendbuf1, "w axis0.controller.config.vel_limit %f\n", 10.0f);
//	send((char*)sendbuf1, sizeof(sendbuf1));
	send((char*)sendbuf1, sjlen((char*)sendbuf1));

	sprintf_s(sendbuf2, "w axis0.motor.config.current_lim %f\n", 11.0f);
//	send((char*)sendbuf2, sizeof(sendbuf2));
	send((char*)sendbuf2, sjlen((char*)sendbuf2));

	sprintf_s(sendbuf3, "w axis1.controller.config.vel_limit %f\n", 10.0f);
//	send((char*)sendbuf3, sizeof(sendbuf3));
	send((char*)sendbuf3, sjlen((char*)sendbuf3));

	sprintf_s(sendbuf4, "w axis1.motor.config.current_lim %f\n", 11.0f);
//	send((char*)sendbuf4, sizeof(sendbuf4));
	send((char*)sendbuf4, sjlen((char*)sendbuf4));



	requested_state = AXIS_STATE_MOTOR_CALIBRATION;
	//sprintf_s(sendbuf5, "Axis0: Requesting state %d\n", requested_state);
	//send((char*)sendbuf5, sizeof(sendbuf5));
	if (!run_state(0, requested_state, true)) return;



	requested_state = AXIS_STATE_ENCODER_OFFSET_CALIBRATION;
	//sprintf_s(sendbuf6, "Axis0: Requesting state %d\n", requested_state);
	//send((char*)sendbuf6, sizeof(sendbuf6));
	if (!run_state(0, requested_state, true, 25.0f)) return;





	requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL;
	//sprintf_s(sendbuf7, "Axis0: Requesting state %d\n", requested_state);
	//send((char*)sendbuf7, sizeof(sendbuf7));
	if (!run_state(0, requested_state, false /*don't wait*/)) return;



}





float ODriveArduino::readFloat(void) {
	//return readString().toFloat();
	return atof(readString());  
}

float ODriveArduino::GetVelocity(int motor_number){

	char sendbuf[100];
	// ���M�o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));
	//serial_<< "r axis" << motor_number << ".encoder.vel_estimate\n";
	sprintf_s(sendbuf, "r axis%d.encoder.vel_estimate\n", motor_number);


	send((char*)sendbuf, sjlen((char*)sendbuf));

	return readFloat();


}

int ODriveArduino::readInt() {
    //return readString().toInt();
	return atoi(readString());
}

bool ODriveArduino::run_state(int axis, int requested_state, bool wait_for_idle, float timeout) {
	
	char sendbuf[100];
	int timeout_ctr = (int)(timeout * 10.0f);



	// ���M�o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));
    //serial_ << "w axis" << axis << ".requested_state " << requested_state << '\n';
	sprintf_s(sendbuf, "w axis%d.requested_state %d\n", axis,requested_state);
	send((char*)sendbuf, sjlen((char*)sendbuf));

    if (wait_for_idle) {
        do {
            //delay(100);
			Sleep(100);
			// ���M�o�b�t�@�N���A
			memset(sendbuf, 0x00, sizeof(sendbuf));
            //serial_ << "r axis" << axis << ".current_state\n";
			sprintf_s(sendbuf, "r axis%d.current_state\n", axis);
			send((char*)sendbuf, sjlen((char*)sendbuf));

        } while (readInt() != AXIS_STATE_IDLE && --timeout_ctr > 0);
    }

    return timeout_ctr > 0;
}

CString ODriveArduino::readString() {
    CString str = "";
	char cbuf;
	
	//static const unsigned long timeout = 1000;
    //unsigned long timeout_start = millis();
    for (;;) {
    //    while (!serial_.available()) {
    //        if (millis() - timeout_start >= timeout) {
    //            return str;
    //        }
    //    }
		receive(&cbuf, 1);
        if (cbuf == '\n')
            break;
        str += cbuf;
    }
    

	
	return str;
}



//---------------------------------------------------------------------------
// Futaba��sample����q��
bool ODriveArduino::init(char* comport_in, int baudrate)
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
bool ODriveArduino::close(void)
{
	if (flag_opened != 1)return false;

	if (hComm) {
		CloseHandle(hComm);
	}

	return 1;
	//return CloseHandle(hComm);

}
//---------------------------------------------------------------------------
int ODriveArduino::receive(char* buf_ptr, int size)
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
int ODriveArduino::send(char* buf_ptr, int size)
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
int ODriveArduino::CommClose()
{
	if (hComm) {
		CloseHandle(hComm);
	}

	return 1;
}


int sjlen(const char* str)
{
	int count = 0;  // �������̃J�E���g�p
	int skip = 0;  // skip=1�̏ꍇ�͕����J�E���g���X�L�b�v����
	while (*str != '\0') {
		if (skip) {  // 2�o�C�g������2�o�C�g�ڂ̏ꍇ�̓J�E���g���Ȃ�
			skip = 0;
		}
		else {
			if ((*str & 0xE0) == 0x80 || (*str & 0xE0) == 0xE0) { skip = 1; } //2�o�C�g�����ɊY������ꍇ
			count++;
		}
		str++;
	}
	return count;
}
