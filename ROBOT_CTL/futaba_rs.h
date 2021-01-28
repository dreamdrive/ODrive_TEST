

// �G���[�R�[�h
#define TEMPERATURE_ERROR 0x80		// ���x�G���[(�g���N�I�t)
#define TEMPERATURE_ALARM 0x20		// ���x�A���[�g
#define FLASH_WRITE_ERROR 0x08		// �t���b�V���������݃G���[
#define PROCESSING_ERROR  0x02		// ��M�p�P�b�g�����G���[
#define TIMEOUT_ERROR     0x0100	// �^�C���A�E�g�G���[
#define CHECKSUM_ERROR    0x0200	// �`�F�b�N�T���G���[
#define NO_ERROR_RS       0x0000	// �G���[�Ȃ�

struct sv_r {
	int				enable;
	short			angle;
	short			time;
	short			speed;
	short			load;
	short			temperature;
	int				torque;
	int				old_torque;
	int				error;
	short			g_angle;
	short			old_g_angle;
	unsigned short	g_time;
};

// �T�[�{�̃������}�b�v�̑���M�Ɏg�p����\���̂̒�`
struct frs_t {
	int 			id;				// �T�[�{ID

	short			angle;			// ���݂̊p�x (�T�[�{�����M�����l)
	short			time;			// �ړ����� (�T�[�{�����M�����l)
	short			speed;			// �p���x (�T�[�{�����M�����l)
	short			load;			// ���� (�T�[�{�����M�����l)
	short			temperature;	// ���x (�T�[�{�����M�����l)
	char			torque;			// �g���NON/OFF (�T�[�{�����M�����l)

	short			g_angle;		// �ڕW�p�x (�T�[�{�ɑ��M����ڕW�l)
	unsigned short	g_time;			// �ڕW�ւ̈ړ����� (�T�[�{�ɑ��M����ڕW�l)

	unsigned short	error;			// �G���[
};

class futaba_rs
{
	//HANDLE hcom;
	DWORD mask;
	COMMTIMEOUTS ctmo;
	OVERLAPPED o;
	COMMPROP cmp;


private:

	HANDLE		hComm = NULL;		// �ʐM�p�n���h��
	int			ret = 0;			// ���^�[���p
	int			c;					// ���͑҂��p

	int receive(char* buf_ptr, int size);//�󂯎��o�b�t�@�̏ꏊ�ƃT�C�Y
	int send(char* buf_ptr, int size);//����o�b�t�@�̏ꏊ�ƃT�C�Y

	int CommClose();


public:

	char flag_opened;	//com�|�[�g���J����Ă��邩�ǂ���
	char comport[16];	//com�|�[�g��
	int baudrate;		//�{�[���[�g�������ɏo��

	bool init(char* comport_in, int baudrate);
	bool close(void);

	void sv_move(int id, short sPos, unsigned short sTime);
	void sv_move_long(sv_r sendDATA[100]);

	void sv_torque(int id, int torque);		// �g���N�̃I���I�t
	int sv_read_torque(int id);

	sv_r sv_read(int id);
	sv_r sv_read2(int id);

};

