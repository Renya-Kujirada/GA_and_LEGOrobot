#include "stdafx.h"
#include "fantom/iNXT.h"
#include "fantom/iNXTIterator.h"
#include "fantom/tStatus.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include <time.h>		// msleep���g�����߂ɕK�v
#include "msleep.h"		// n�b�ԓ�����~����֐����g���̂ɗp����
#include "choonnpa.h"	// �����g�Z���T�[��p����̂Ɏg��
#include "hikari.h"		// ���Z���T�[��p����̂Ɏg��

#define distance	240	// �ǁi�{�[���j�̎�O�܂ł̒����g�Z���T�[�̋���(255�ȉ��ŋϓ���30�������邽�߂̍œK����)
#define gene_length 30	// �ǂݍ��ވ�`�q��
#define Shot_Power  40	// �ԋʂ�łp���[

#define MOTOR_A 0	// �A�[���̔ԍ�
#define MOTOR_R 1	// �E���[�^�̔ԍ�
#define MOTOR_L 2	// �����[�^�̔ԍ�


//---------------------------------------- �v���g�^�C�v�錾 ----------------------------------------
void motor(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int mot_num, char motor_speed);
void file_input(char *filename, int speed[gene_length]);
//--------------------------------------------------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[]) {
	int speed[gene_length] = {};			// GA�ɂ��œK���x���t�@�C������ǂݍ��݊i�[����z��
	int Position[gene_length] = {};			// 0�`255�܂ł̋���(�ʒu���)���i�[���邽�߂̔z��

	/***** 1. speed�z��̑��x�ɑΉ����钴���g�Z���T�̒l�i�ʒu���j�̌��� *****/
	//------------------------------ �ʒu���̌v�Z (17 �` 249 �܂�) -------------------------------
	for (int i = 0; i < gene_length; i++) {
		// �z��̐擪����~���ɁA�ʒu�����i�[����B
		// LightValue = 24 �̎��ɒ�~ (speed[29] = 0)����悤�ɁA
		// Position[29]��24���i�[�����悤�Ɍv�Z����B
		// Position�z��́A�u8�v���l���ω����Ă����悤�ɐݒ�B

		Position[i] = (distance - distance * i / gene_length) + 9;

	}
	//---------------------------------------------------------------------------------------------

	/***** 2. �G���[�g�i�œK���x�j�t�@�C���̓ǂݍ���*****/
	file_input("elite.txt", speed);

	// �e�����g�Z���T�[�̒l�ɑ΂���A�espeed�z����̍œK���x�̏o��
	for (int i = 0; i < gene_length; i++) {
		printf("num: %d\t\tUSonicValue >= %d\tspeed: %d\n", i, Position[i], speed[i]);
	}


	//----------------------------------- �O���� -----------------------------------
	// ���C�u�����Q�Ɨp�ϐ�
	nFANTOM100::iNXTIterator* nxtIteratorPtr = NULL;
	nFANTOM100::tStatus status;
	nFANTOM100::iNXT* nxtPtr = NULL;

	char exitcode = 0;

	// NXT�ւ̐ڑ��D�Q�Ɨp�|�C���^�̎擾�iUSB or Bluetooth�j
	nxtIteratorPtr = nFANTOM100::iNXT::createNXTIterator(
		true,
		5,
		status
	);

	// �ڑ��Ɏ��s�����ꍇ�C�I��
	if (status.isFatal()) {
		printf("�ڑ��Ɏ��s���܂����B\n\n");
		return 0;
	}

	// �ڑ��������̏o��
	printf("Communication start with NXT...\n");

	// ���C�u�����p�|�C���^�̎擾
	nxtPtr = nxtIteratorPtr->getNXT(status);

	// �Q�Ɨp�|�C���^�̔j��
	nFANTOM100::iNXT::destroyNXTIterator(nxtIteratorPtr);

	//�e�Z���T�[�̏�����
	setUSonicValue(nxtPtr, status, 3);		// �����g�Z���T�������@�|�[�g�ԍ�4��3
	setLightValue(nxtPtr, status, 2);		// ���Z���T�[������	   �|�[�g�ԍ�3��2
  // --------------------------------------------------------------------------------


  /*********************************************************/
  /*************** 3. ���{�b�g�̎�������J�n ***************/
  /*********************************************************/

	int num = 0;	// �����g�Z���T�[�̐��l�i�����j�ɑΉ�����Aspeed�z��̃C���f�b�N�X

	while (exitcode == 0) {			// exitcode = 1�ƂȂ�܂Łi�ԋʂ�ł܂Łj�������[�v

		int LightValue = getLightValue(nxtPtr, status, 2);		// ���Z���T�[�̒l
		int USonicValue = getUSonicValue(nxtPtr, status, 3);	// �����g�Z���T�[�̒l

		/***** step1. �ǂ܂�GA�ł̍œK���x�ŋ߂Â� *****/
		// ���ۂ̋����𒴉��g�Z���T�[�̐��l�ɑΉ������A�ǂ܂ő��s�B
		if (USonicValue) {
			for (int i = 0; i < gene_length; i++) {			// �����g�Z���T�[�̒l��p���Č��݂̈ʒu��c��
				if (USonicValue >= Position[i]) {			// �����g�Z���T�[�̒l�ɑΉ������X�s�[�h�ɂ���
					num = i;
					break;		// �����ɉ������œK���x�ɂ�����Afor�����甲����B
				}
			}
		}
		motor(nxtPtr, status, MOTOR_R, speed[num]);		// �E�ԗ�
		motor(nxtPtr, status, MOTOR_L, speed[num]);		// ���ԗ�

		// �����g�Z���T�[�̐��l�A�����g�Z���T�[�̒l�ɑΉ�����Position�̒l�AGA�̑��x�A���Z���T�[�̒l
		printf("USonicValue=%5d\tPosition=%5d\tspeed=%5d\tLightValue=%5d\r", USonicValue, Position[num], speed[num], LightValue);

		/***** step2. �ǂ̊ԍ�(USonicValue = 24)�܂ňړ������ꍇ(speed�z��̖����܂ő��x�f�[�^��ǂݍ��񂾏ꍇ) *****/
		if (num == gene_length - 1) {
			/***** step2. �ڂ̑O�̑Ώە��̂̐F�𔻒� *****/
			Msleep(1000);
			LightValue = getLightValue(nxtPtr, status, 2);
			Msleep(1000);
			
			/***--------------- step2-1. ���Z���T�[�̒l��170�ȏ�A���Ȃ킿�A�ԋʈȊO�̏ꍇ ---------------***/
			if (LightValue > 170) {
				Msleep(1000);

				/*****  step3. �Ώە��̉E��������T�� *****/
				// 3-1. �E��������
				motor(nxtPtr, status, MOTOR_R, -speed[27]);
				Msleep(1000);
				
				// 3-2. ���������O�i
				motor(nxtPtr, status, MOTOR_R, speed[27]);
				motor(nxtPtr, status, MOTOR_L, speed[27]);
				Msleep(800);

				// 3-3. ��~
				motor(nxtPtr, status, MOTOR_R, 0);
				motor(nxtPtr, status, MOTOR_L, 0);
				Msleep(1000);

				/***** step4. �ڂ̑O(�E���O��)�̑Ώ̕��̂̐F�𔻒� *****/
				Msleep(1000);
				LightValue = getLightValue(nxtPtr, status, 2);
				Msleep(1000);

				/*****  step4-A. �Ώ̕��̂��ԋʂł���ꍇ *****/
				// �Ώ̕��̂Ƃ̋������߂����߁ALightValue�̐ԋʏ�����165�Ƃ��Ă���B
				if (LightValue < 165) {
					// A-1. �A�[�������v����Shot_Power������
					motor(nxtPtr, status, MOTOR_A, Shot_Power);
					Msleep(180);

					// A-2. �A�[�����~
					motor(nxtPtr, status, MOTOR_A, 0);
					Msleep(20);

					// A-3. �A�[���𔽎��v����Shot_Power������
					motor(nxtPtr, status, MOTOR_A, -(Shot_Power));
					Msleep(200);

					// A-4. �A�[�����~
					motor(nxtPtr, status, MOTOR_A, 0);

					break;	// �������[�v���甲����(�v���O�����I��)
					// exitcode = 1;�Ƃ���ƁAstep5�����s���Ă��܂��B�iexitcode��]������̂́A�擪��while���j
				}

				/***** step4-B. �Ώە����ԋʈȊO�̏ꍇ *****/
				// step3.�́A�Ώە��̉E���T�����O�̏�ԁi�����j�ɖ߂�
				else {
					// B-1. �����������
					motor(nxtPtr, status, MOTOR_R, -speed[27]);
					motor(nxtPtr, status, MOTOR_L, -speed[27]);
					Msleep(1200);

					// B-2. ����������
					motor(nxtPtr, status, MOTOR_R, speed[27]-8);
					Msleep(800);

					// B-3. ��~
					motor(nxtPtr, status, MOTOR_R, 0);
					motor(nxtPtr, status, MOTOR_L, 0);
					Msleep(1000);
				}

				/***** step5. ���T���ȉE���̈��T���ł���悤�Ɍ�ނ��� *****/
				// 5-1. �E�΂ߌ��Ɍ��
				motor(nxtPtr, status, MOTOR_R, -speed[24]);
				motor(nxtPtr, status, MOTOR_L, -speed[24] - 10);
				Msleep(2000);

				// 5-2. ���΂ߌ��Ɍ�ނ��A�Α̂̌������܂������ɒ���
				motor(nxtPtr, status, MOTOR_R, -speed[24] - 10);
				motor(nxtPtr, status, MOTOR_L, -speed[24]);
				Msleep(2000);

				// 5-3. ��x�~�܂�
				motor(nxtPtr, status, MOTOR_R, 0);
				motor(nxtPtr, status, MOTOR_L, 0);
				Msleep(1000);

			}

			/***--------------- step2-2. �Ώە����ԋʂ̏ꍇ ---------------***/
			else {
				// 2-2-1. �A�[�������v����Shot_Power������
				motor(nxtPtr, status, MOTOR_A, Shot_Power);
				Msleep(180);
				
				// 2-2-2. �A�[�����~
				motor(nxtPtr, status, MOTOR_A, 0);
				Msleep(20);
				
				// 2-2-3. �A�[���𔽎��v����Shot_Power������
				motor(nxtPtr, status, MOTOR_A, -(Shot_Power) );
				Msleep(200);
				
				// 2-2-4. �A�[�����~
				motor(nxtPtr, status, MOTOR_A, 0);

				exitcode = 1;	// �������[�v�I��(�v���O�����I��)
			}

			num = 0;	// speed�z��̃C���f�b�N�X�̏������i�ǂ��痣�ꂽ��Ԃɂ���j
		}	// �ŏ��̌��Z���T�[�̒l�ɂ��if���I��

	}	// �������[�v(while��)�I��

	/*********************************************************/
	/*************** 3. ���{�b�g�̎�������I�� ***************/
	/*********************************************************/



	//------------------------------ �㏈�� ------------------------------
	// �Q�Ɨp�|�C���^�̔j��
	nFANTOM100::iNXT::destroyNXT(nxtPtr);
	return 0;
	// -------------------------------------------------------------------

}


/***** �t�@�C���ǂݍ��݊֐� *****/
void file_input(char *filename, int speed[gene_length]) {
	FILE *fp;
	int i = 0;
	int temp = 0;

	if ((fopen_s(&fp, filename, "r")) != 0) {
		printf("�t�@�C��������܂���:%s\n", filename);
		exit(1);
	}

	// speed�z���elite�t�@�C���̃f�[�^���i�[
	for (i = 0; i < gene_length; i++) {
		fscanf_s(fp, "%d", &speed[i]);
	}
}
/***** �����܂�*****/


/***** ���[�^�[����֐��i�A�[���A�E�ԗցA���ԗցj*****/
void motor(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int mot_num, char motor_speed) {	// �֐��̎��s���ʂ��i�C����ϐ�
	ViUInt8 directCommandBuffer[] = {

	4, // �R�}���h�D�uSETOUTPUTSTATE�s���Ӗ�����4������D

	0, // �o�̓|�[�g�ԍ��D0�F�uA�s�|�[�g�C1�F�uB�s�|�[�g�C2�F�uC�s�|�[�g�D

	0, // ���[�^�[�̃p���[�D�l�͈̔́F-100�`+100�D0�F���[�^�[��~�D

	7, // ���[�^�[�̃��[�h�D1�F���[�^�[�I���C2�F�u���[�L�I���C4�F����I���D

	1, // ���䃂�[�h�D0�F����Ȃ��C1�F�X�s�[�h�̐���C2�F�������[�^�[�̓���

	0, // ��]���[�g�D�l�͈̔́F-100�`+100�D

	0x20, // �����ԁD0x10�F���X�ɉ����C0x20�F�ʏ퓮��C0x30�F���X�Ɍ����D

	0,0,0,0 // ���䂷���]�ʁD0�F��]�p���D

	};


	// ���[�^�[�̍��E�E���x�w��i�A�[��: mot_num = 0, �E�ԗցFmot_num = 1, ���ԗցFmot_num = 2���[�^�[�j
	directCommandBuffer[1] = mot_num;		// ���������ʂ̎w��
	directCommandBuffer[2] = motor_speed;	// ���x�̎w��


	// �_�C���N�g�R�}���h�̑��M

	nxtPtr->sendDirectCommand(

		false,

		reinterpret_cast<ViByte*>(directCommandBuffer),

		sizeof(directCommandBuffer),

		NULL,

		0,

		status

	);

}
/***** �����܂� ****/
