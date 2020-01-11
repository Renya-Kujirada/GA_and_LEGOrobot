#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>	// msleep���g�����߂ɕK�v

//�p�����[�^�錾
#define IND_NUM     51                  // �̐��i�̔ԍ��j
#define gene_length 30	                // ��`�q���i��`�q�ԍ��j
#define EVOL_NUM	(IND_NUM - 1)		// �G���[�g���������c��̐i���̐��i�g�[�i�����g�I���E�������s���̐��j�i�����j

#define generation  10000               // ���㐔
#define CROSS_RATE	0.5					// ������
#define MUT_RATE    0.01                // �ˑR�ψٗ�

#define First_Speed 20                  // �������x�Œ�
#define Last_Speed  0                   // �ŏI���x�Œ�


int main(void)
{
	srand((unsigned)time(NULL));

	int individual[IND_NUM][gene_length];	// �e�̂̈�`�q���i��`�q[�̔ԍ�][��`�q�ԍ�]�j
	int fitness[IND_NUM] = {};				// �e�̂̓K���x
	int all_fitness[generation] = {};		// �S����ɂ�����e�G���[�g�̂̓K���x
	int speed_sum[IND_NUM] = {};			// ��`�q�̒l�̍��v
	int speed_change[IND_NUM] = {};			// ���x�ω���2��̍��v

	int elite[gene_length] = {};		// �G���[�g�̗p�z��
	int elite_number = 0;				// �G���[�g�̂̈�`�q�ԍ��i0�ɏ������j
	int MAX_fitness = 0;				// �̂̓K���x�̍ő�l
	int MAX_IND_NUM = 0;				// 0�Ԗڂ̌̂̈�`�q���G���[�g�ł���Ɖ���

	int tournament[EVOL_NUM][gene_length];	// individual[][]����A�g�[�i�����g�����őI��������
	int cross[EVOL_NUM][gene_length];		// �g�[�i�����g�I�������̊ԂŌ����������V���Ȍ�
	int temp = 0;							// �����p�f�[�^�ꎞ�ۊǕϐ�
	int mutation[EVOL_NUM][gene_length];	// ���������̂�ˑR�ψق�������

	int rnd1 = 0;	// �����i�[�ϐ�
	int rnd2 = 0;

	int i, j, g;	// for���J�E���^


	/*************** 1. �����̌Q���� ***************/
	for (i = 0; i < IND_NUM; i++) {
		for (j = 0; j < gene_length; j++) {
			if (j == 0) {
				individual[i][j] = First_Speed;
			}
			else if (j == gene_length - 1) {
				individual[i][j] = Last_Speed;
			}
			else {
				individual[i][j] = rand() % 100 + 1;	// 1 �` 100�܂ł̗���
			}
		}
	}


	/******************************************************/
	/*************** ��`�I�A���S���Y���J�n ***************/
	/******************************************************/
	for (g = 0; g < generation; g++) {	// generation��i������

		printf("generation : %d����\r", g + 1);		// ����̕\��

		/*************** 2. �]�� ***************/
		// ������
		for (i = 0; i < IND_NUM; i++) {
			speed_sum[i] = 0;
			speed_change[i] = 0;
		}

		// ��`�q�̒l�̍��v
		for (i = 0; i < IND_NUM; i++) {
			for (j = 0; j < gene_length; j++) {
				speed_sum[i] += individual[i][j];
			}
		}

		// ���x�ω��̓��̍��v
		for (i = 0; i < IND_NUM; i++) {
			for (j = 0; j < gene_length - 1; j++) {
				speed_change[i] += (int)pow((individual[i][j + 1] - individual[i][j]), 2.0);
			}
		}

		// �K���x�̌v�Z
		for (i = 0; i < IND_NUM; i++) {
			fitness[i] = speed_sum[i] - speed_change[i];
		}


		/*************** 3.�I�� ***************/
		/***** 3-1. �G���[�g�I�� *****/
		// ������
		MAX_fitness = fitness[0];	// 0�Ԗڂ̌̂̓K���x���ő�l�ɐݒ�
		MAX_IND_NUM = 0;			// 0�Ԗڂ̌̂̈�`�q���G���[�g�ɐݒ�

		// �K���x�̍ő�l�����߂�
		for (i = 1; i < IND_NUM; i++) {			// 0�Ԗڂ��G���[�g�ɏ����ݒ肵�Ă���̂ŁAi = 1����X�^�[�g
			if (MAX_fitness < fitness[i]) {
				MAX_fitness = fitness[i];
				MAX_IND_NUM = i;
			}
		}

		// �G���[�g�̂̈�`�q���E�K���x��ۑ�
		for (j = 0; j < gene_length; j++) {
			elite[j] = individual[MAX_IND_NUM][j];	// ��`�q���ۑ�
		}
		all_fitness[g] = MAX_fitness;		// �K���x�ۑ�


		/***** 3-2. �g�[�i�����g�I�� *****/
		// �G���[�g���܂ތv IND_NUM��(51��)�̌�individual[][]����ATEMP_NUM�iIND_NUM - 1�j(50��)���g�[�i�����g�����őI������
		for (i = 0; i < EVOL_NUM; i++) {	// EVOL_NUM ��g�[�i�����g���s��
			rnd1 = rand() % IND_NUM;	// �����_����2�y�A�I��(indivisual�̌̔ԍ�)�i�G���[�g���܂ށj
			rnd2 = rand() % IND_NUM;

			if (fitness[rnd1] >= fitness[rnd2]) {	// rnd1�̌̔ԍ��̓K���x���傫���A�������͓K���x���������܂��� rnd1 = rnd2�̏ꍇ
				for (j = 0; j < gene_length; j++) {
					tournament[i][j] = individual[rnd1][j];
				}
			}
			else if (fitness[rnd1] < fitness[rnd2]) {
				for (j = 0; j < gene_length; j++) {
					tournament[i][j] = individual[rnd2][j];
				}
			}
		}

		
		/*************** 4. ���� ***************/
		for (i = 0; i < EVOL_NUM; i += 2) {	// �����Ώۂ�EVOL_NUM �i�����j�̌̊ԂŁAEVOL_NUM / 2 �����������
			rnd1 = rand() % EVOL_NUM;
			rnd2 = rand() % EVOL_NUM;		// �����_���Ɍ���������2�g������

			for (j = 0; j < gene_length; j++) {
				cross[i][j] = tournament[rnd1][j];		// ������̔z��Ɍ����O�̈�`�q���R�s�[
				cross[i + 1][j] = tournament[rnd2][j];

				if ( (double)rand() / RAND_MAX < CROSS_RATE ) {			// 0.0�`1.0�̗��������������傫����
					temp = cross[i][j];					// ����������
					cross[i][j] = cross[i + 1][j];
					cross[i + 1][j] = temp;
				}
			}			

		}


		/*************** 5. �ˑR�ψ� ***************/
		for (i = 0; i < EVOL_NUM; i++) {			// �S�Ă̌����ς݂̌̂ɑ΂��ēˑR�ψق��s��
			for (j = 1; j < gene_length - 1; j++) {				// �������x�ƍŏI���x�͕ω������Ȃ�
				if ( (double)rand() / RAND_MAX < MUT_RATE ) {
					cross[i][j] = rand() % 100 + 1;		// �ˑR�ψق�����
				}
			}
		}
		for (i = 0; i < EVOL_NUM; i++) {
			for (j = 0; j < gene_length; j++) {
				mutation[i][j] = cross[i][j];	// �ˑR�ψٌ��cross��ˑR�ψٍςݔz��Ɋi�[
			}
		}


		/*************** 6. �ŏI�I�Ȏq����̍쐬 ***************/
		for (i = 0; i < IND_NUM; i++) {
			if (i == 0) {
				for (j = 0; j < gene_length; j++) {
					individual[i][j] = elite[j];		// individual �̐擪�ɃG���[�g���i�[
				}
			}
			else {
				for (j = 0; j < gene_length; j++) {
					individual[i][j] = mutation[i - 1][j];	// �c��͐i����̌̂ɍX�V
				}
			}

		}

	}
	/******************************************************/
	/*************** ��`�I�A���S���Y���I�� ***************/
	/******************************************************/


	// �ŏI�I�ȃG���[�g�̈�`�q����\��
	printf("\n\n�Zlast elite:\n");
	for (j = 0; j < gene_length; j++) {
		printf("%d\t", elite[j]);	// elite[j] = individual[0][j]
	}

	// �ŏI�I�ȃG���[�g�̓K���x��\��
	printf("\n�Zlast MAX_fitness:\t%d\n\n", MAX_fitness);
	
	// ��`�I�A���S���Y���ɂ��œK���x��elite.txt�ɏ�������
	FILE *f_elite;
	fopen_s(&f_elite, "elite.txt", "w");
	for (j = 0; j < gene_length; j++) {
		fprintf(f_elite, "%d\n", elite[j]);
	}
	fclose(f_elite);

	// ��`�I�A���S���Y���ɂ��e����̓K���x��fitness.txt�ɏ�������
	FILE *f_fitness;
	fopen_s(&f_fitness, "fitness.txt", "w");
	for (g = 0; g < generation; g++) {
		fprintf(f_fitness, "%d\n", all_fitness[g]);
	}
	fclose(f_fitness);

	return 0;
}
