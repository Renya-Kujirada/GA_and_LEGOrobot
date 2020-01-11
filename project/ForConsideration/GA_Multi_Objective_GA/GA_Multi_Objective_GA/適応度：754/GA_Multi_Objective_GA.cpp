// GA_Multi_Objective_GA_20180113_�l�@�p�����v���O����_MOGA(�p���[�g�Ȑ������߂邽�߂̃v���O����_)
// elite���x��elite.txt�ɁA�S����ɂ�����G���[�g�̂̓K���xMAX_fitness��fitness.txt�ɏ������ރv���O����

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>	// msleep���g�����߂ɕK�v

//�p�����[�^�錾
#define IND_NUM     251                 // �̐��i�̔ԍ��j
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

	int pareto[IND_NUM][gene_length] = {};	// �p���[�g���ł����(�e����Ŏ�����Ɏc���p���[�g��)
	int PARETO_NUM = 0;						// �e����ɂ�����p���[�g���̌�(�z��pareto�̃C���f�b�N�X)
	int Pareto_speed_sum[IND_NUM] = {};		// �p���[�g���̖ړI�֐��l�i���x�ω��̍��v�j
	int Pareto_speed_change[IND_NUM] = {};	// �p���[�g���̖ړI�֐��l�i���x�ω��̓��̍��v�j
	int inferiority_flag = 0;					// �򓙉��ł���ꍇ�A1�𗧂Ă�t���O

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
				individual[i][j] = rand() % 100;	// 1 �` 100�܂ł̗���
			}
		}
	}


	/******************************************************/
	/*************** ��`�I�A���S���Y���J�n ***************/
	/******************************************************/
	for (g = 0; g < generation; g++) {	// generation��i������

		printf("generation : %d����\t", g + 1);		// ����̕\��
		printf("�Z MAX_fitness:%7d\r", MAX_fitness);	// ������̓K���x

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
			//fitness[i] = speed_sum[i] - 2 * speed_change[i];
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


		/************************************************************************************/
		/***** �p���[�g���̒T�� *****/
		PARETO_NUM = 0;
//		int Pareto_fitness[IND_NUM] = {};
		inferiority_flag = 0;

		for (i = 0; i < IND_NUM; i++) {
			// i�Ԗڂ̌̂ƑS�Ă�(j�Ԗڂ�)�̖̂ړI�֐��l���r
			for (j = 0; j < IND_NUM; j++) {
				if (i != j && speed_sum[i] <= speed_sum[j] && -speed_change[i] <= -speed_change[j]) {
					inferiority_flag = 1;
					break;
				}
			}
			if (inferiority_flag == 0) {
				// �p���[�g���̈�`�q����ۑ�
				for (j = 0; j < gene_length; j++) {
					pareto[PARETO_NUM][j] = individual[i][j];	// ��`�q���ۑ�
				}

				//printf("%d", PARETO_NUM);
				//getchar();

				// �p���[�g���̖ړI�֐��l��ۑ�
//				Pareto_speed_sum[PARETO_NUM] = speed_sum[i];
//				Pareto_speed_change[PARETO_NUM] = speed_change[i];

				// �p���[�g���̓K���x��ۑ�
//				Pareto_fitness[PARETO_NUM] = fitness[i];	//-----------------------------------------���炭�p���[�g���̓K���x�͕s�v???

				PARETO_NUM++;		// �p���[�g���̌��X�V
			}
			inferiority_flag = 0;	// �򓙉��̃t���O��������
		}

		/************************************************************************************/


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
			rnd2 = rand() % EVOL_NUM;		// ����������2�g������

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
			else if (i <= PARETO_NUM) {
				for (j = 0; j < gene_length; j++) {
					individual[i][j] = pareto[i - 1][j];	// individual ��1�`PARETO_NUM�ԖڂɃp���[�g�����i�[
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


	/***** �G���[�g�̂̏��o�� *****/

	// �ŏI�I�ȃG���[�g�̈�`�q����\��
	printf("\n\n�Zlast elite:\n");
	for (j = 0; j < gene_length; j++) {
		printf("%d\t", elite[j]);	// individual[0][j]
	}

	// �ŏI�I�ȃG���[�g�̓K���x��\��
	printf("\n�Zlast MAX_fitness:\t%d\n\n", MAX_fitness);
	
	// �ŏI�I�ȃG���[�g�̑��x�̍��v�l��\��
	printf("\n�Zlast speed_sum:\t%d\n\n", speed_sum[MAX_IND_NUM]);

	// �ŏI�I�ȃG���[�g�̑��x�ω��̓��̍��v�l��\��
	printf("\n�Zlast speed_change:\t%d\n\n", speed_change[MAX_IND_NUM]);


	/************************************************************************************/
	/***** �p���[�g���̒T�� *****/
	PARETO_NUM = 0;
	Pareto_speed_sum[IND_NUM] = {};
	Pareto_speed_change[IND_NUM] = {};
	inferiority_flag = 0;

	for (i = 0; i < IND_NUM; i++) {
		for (j = 0; j < IND_NUM; j++) {
			if (i != j && speed_sum[i] <= speed_sum[j] && -speed_change[i] <= -speed_change[j]) {
				inferiority_flag = 1;
				break;
			}
		}
		if (inferiority_flag == 0) {
			Pareto_speed_sum[PARETO_NUM] = speed_sum[i];
			Pareto_speed_change[PARETO_NUM] = speed_change[i];
			PARETO_NUM++;
		}
		inferiority_flag = 0;
	}
	/************************************************************************************/


	// ��`�I�A���S���Y���ɂ��ŏI�I�ȍœK���x��elite.txt�ɏ�������
	FILE *f_elite;
	fopen_s(&f_elite, "elite.txt", "w");
	for (j = 0; j < gene_length; j++) {
		fprintf(f_elite, "%d\n", individual[0][j]);
	}
	fclose(f_elite);

	// ��`�I�A���S���Y���ɂ��e����ł̓K���x��fitness.txt�ɏ�������
	FILE *f_fitness;
	fopen_s(&f_fitness, "fitness.txt", "w");
	for (g = 0; g < generation; g++) {
		fprintf(f_fitness, "%d\n", all_fitness[g]);
	}
	fclose(f_fitness);



	/***** �ŏI����̊e�ړI�֐��̒l *****/
	// �ŏI����̓K���x��last_generation_fitness.txt�ɏ�������
	FILE *f_last_fitness;
	fopen_s(&f_last_fitness, "last_generation_fitness.txt", "w");
	for (i = 0; i < IND_NUM; i++) {
		fprintf(f_last_fitness, "%d\n", fitness[i]);
	}
	fclose(f_last_fitness);
	
	// �ŏI����̊e�̂̑��x���v��speed_sum.txt�ɏ�������
	FILE *f_speed_sum;
	fopen_s(&f_speed_sum, "speed_sum.txt", "w");
	for (i = 0; i < IND_NUM; i++) {
		fprintf(f_speed_sum, "%d\n", speed_sum[i]);
	}
	fclose(f_speed_sum);

	// �ŏI����̊e�̂̑��x�ω��̓��̍��v��speed_change.txt�ɏ�������
	FILE *f_speed_change;
	fopen_s(&f_speed_change, "speed_change.txt", "w");
	for (i = 0; i < IND_NUM; i++) {
		fprintf(f_speed_change, "%d\n", -speed_change[i]);
	}
	fclose(f_speed_change);



	/***** �p���[�g�œK���̊e�ړI�֐��̒l *****/
	// �p���[�g�œK���̌̂̑��x���v��Pareto_speed_sum.txt�ɏ�������
	FILE *f_Pareto_speed_sum;
	fopen_s(&f_Pareto_speed_sum, "Pareto_speed_sum.txt", "w");
	for (i = 0; i < PARETO_NUM; i++) {
		fprintf(f_Pareto_speed_sum, "%d\n", Pareto_speed_sum[i]);
	}
	fclose(f_Pareto_speed_sum);

	// �p���[�g�œK���̌̂̑��x�ω��̓��̍��v��Pareto_speed_change.txt�ɏ�������
	FILE *f_Pareto_speed_change;
	fopen_s(&f_Pareto_speed_change, "Pareto_speed_change.txt", "w");
	for (i = 0; i < PARETO_NUM; i++) {
		fprintf(f_Pareto_speed_change, "%d\n", -Pareto_speed_change[i]);
	}
	fclose(f_Pareto_speed_change);

	return 0;
}
