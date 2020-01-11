// GA_Multi_Objective_GA_20180113_考察用実験プログラム_MOGA(パレート曲線を求めるためのプログラム_)
// elite速度をelite.txtに、全世代におけるエリート個体の適合度MAX_fitnessをfitness.txtに書き込むプログラム

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>	// msleepを使うために必要

//パラメータ宣言
#define IND_NUM     251                 // 個体数（個体番号）
#define gene_length 30	                // 遺伝子長（遺伝子番号）
#define EVOL_NUM	(IND_NUM - 1)		// エリートを除いた残りの進化個体数（トーナメント選択・交叉を行う個体数）（偶数）

#define generation  10000               // 世代数
#define CROSS_RATE	0.5					// 交叉率
#define MUT_RATE    0.01                // 突然変異率

#define First_Speed 20                  // 初期速度固定
#define Last_Speed  0                   // 最終速度固定


int main(void)
{
	srand((unsigned)time(NULL));

	int individual[IND_NUM][gene_length];	// 各個体の遺伝子情報（遺伝子[個体番号][遺伝子番号]）
	int fitness[IND_NUM] = {};				// 各個体の適合度
	int all_fitness[generation] = {};		// 全世代における各エリート個体の適合度
	int speed_sum[IND_NUM] = {};			// 遺伝子の値の合計
	int speed_change[IND_NUM] = {};			// 速度変化の2乗の合計

	int elite[gene_length] = {};		// エリート個体用配列
	int elite_number = 0;				// エリート個体の遺伝子番号（0に初期化）
	int MAX_fitness = 0;				// 個体の適応度の最大値
	int MAX_IND_NUM = 0;				// 0番目の個体の遺伝子がエリートであると仮定

	int pareto[IND_NUM][gene_length] = {};	// パレート解である個体(各世代で次世代に残すパレート解)
	int PARETO_NUM = 0;						// 各世代におけるパレート解の個数(配列paretoのインデックス)
	int Pareto_speed_sum[IND_NUM] = {};		// パレート解の目的関数値（速度変化の合計）
	int Pareto_speed_change[IND_NUM] = {};	// パレート解の目的関数値（速度変化の二乗の合計）
	int inferiority_flag = 0;					// 劣等解である場合、1を立てるフラグ

	int tournament[EVOL_NUM][gene_length];	// individual[][]から、トーナメント方式で選択した個体
	int cross[EVOL_NUM][gene_length];		// トーナメント選択した個体間で交叉させた新たな個体
	int temp = 0;							// 交叉用データ一時保管変数
	int mutation[EVOL_NUM][gene_length];	// 交叉した個体を突然変異させた個体

	int rnd1 = 0;	// 乱数格納変数
	int rnd2 = 0;

	int i, j, g;	// for文カウンタ


	/*************** 1. 初期個体群生成 ***************/

	for (i = 0; i < IND_NUM; i++) {
		for (j = 0; j < gene_length; j++) {
			if (j == 0) {
				individual[i][j] = First_Speed;
			}
			else if (j == gene_length - 1) {
				individual[i][j] = Last_Speed;
			}
			else {
				individual[i][j] = rand() % 100;	// 1 ～ 100までの乱数
			}
		}
	}


	/******************************************************/
	/*************** 遺伝的アルゴリズム開始 ***************/
	/******************************************************/
	for (g = 0; g < generation; g++) {	// generation回進化する

		printf("generation : %d世代\t", g + 1);		// 世代の表示
		printf("〇 MAX_fitness:%7d\r", MAX_fitness);	// 現世代の適合度

		/*************** 2. 評価 ***************/
		// 初期化
		for (i = 0; i < IND_NUM; i++) {
			speed_sum[i] = 0;
			speed_change[i] = 0;
		}

		// 遺伝子の値の合計
		for (i = 0; i < IND_NUM; i++) {
			for (j = 0; j < gene_length; j++) {
				speed_sum[i] += individual[i][j];
			}
		}
		
		// 速度変化の二乗の合計
		for (i = 0; i < IND_NUM; i++) {
			for (j = 0; j < gene_length - 1; j++) {
				speed_change[i] += (int)pow((individual[i][j + 1] - individual[i][j]), 2.0);
			}
		}

		// 適合度の計算
		for (i = 0; i < IND_NUM; i++) {
			fitness[i] = speed_sum[i] - speed_change[i];
			//fitness[i] = speed_sum[i] - 2 * speed_change[i];
		}


		/*************** 3.選択 ***************/
		/***** 3-1. エリート選択 *****/
		// 初期化
		MAX_fitness = fitness[0];	// 0番目の個体の適応度を最大値に設定
		MAX_IND_NUM = 0;			// 0番目の個体の遺伝子をエリートに設定

		// 適応度の最大値を求める
		for (i = 1; i < IND_NUM; i++) {			// 0番目をエリートに初期設定しているので、i = 1からスタート
			if (MAX_fitness < fitness[i]) {
				MAX_fitness = fitness[i];
				MAX_IND_NUM = i;
			}
		}

		// エリート個体の遺伝子情報・適応度を保存
		for (j = 0; j < gene_length; j++) {
			elite[j] = individual[MAX_IND_NUM][j];	// 遺伝子情報保存
		}
		all_fitness[g] = MAX_fitness;		// 適合度保存


		/************************************************************************************/
		/***** パレート解の探索 *****/
		PARETO_NUM = 0;
//		int Pareto_fitness[IND_NUM] = {};
		inferiority_flag = 0;

		for (i = 0; i < IND_NUM; i++) {
			// i番目の個体と全ての(j番目の)個体の目的関数値を比較
			for (j = 0; j < IND_NUM; j++) {
				if (i != j && speed_sum[i] <= speed_sum[j] && -speed_change[i] <= -speed_change[j]) {
					inferiority_flag = 1;
					break;
				}
			}
			if (inferiority_flag == 0) {
				// パレート解の遺伝子情報を保存
				for (j = 0; j < gene_length; j++) {
					pareto[PARETO_NUM][j] = individual[i][j];	// 遺伝子情報保存
				}

				//printf("%d", PARETO_NUM);
				//getchar();

				// パレート解の目的関数値を保存
//				Pareto_speed_sum[PARETO_NUM] = speed_sum[i];
//				Pareto_speed_change[PARETO_NUM] = speed_change[i];

				// パレート解の適応度を保存
//				Pareto_fitness[PARETO_NUM] = fitness[i];	//-----------------------------------------恐らくパレート解の適応度は不要???

				PARETO_NUM++;		// パレート解の個数更新
			}
			inferiority_flag = 0;	// 劣等解のフラグを初期化
		}

		/************************************************************************************/


		/***** 3-2. トーナメント選択 *****/
		// エリートを含む計 IND_NUM個(51個)の個体individual[][]から、TEMP_NUM個（IND_NUM - 1個）(50個)をトーナメント方式で選択する
		for (i = 0; i < EVOL_NUM; i++) {	// EVOL_NUM 回トーナメントを行う
			rnd1 = rand() % IND_NUM;	// ランダムに2ペア選ぶ(indivisualの個体番号)（エリートを含む）
			rnd2 = rand() % IND_NUM;

			if (fitness[rnd1] >= fitness[rnd2]) {	// rnd1の個体番号の適応度が大きい、もしくは適応度が等しいまたは rnd1 = rnd2の場合
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

		
		/*************** 4. 交叉 ***************/
		for (i = 0; i < EVOL_NUM; i += 2) {	// 交叉対象のEVOL_NUM 個（偶数個）の個体間で、EVOL_NUM / 2 回交叉させる
			rnd1 = rand() % EVOL_NUM;
			rnd2 = rand() % EVOL_NUM;		// 交叉させる2組を決定

			for (j = 0; j < gene_length; j++) {
				cross[i][j] = tournament[rnd1][j];		// 交叉後の配列に交叉前の遺伝子をコピー
				cross[i + 1][j] = tournament[rnd2][j];

				if ( (double)rand() / RAND_MAX < CROSS_RATE ) {			// 0.0～1.0の乱数が交叉率より大きいか
					temp = cross[i][j];					// 交叉させる
					cross[i][j] = cross[i + 1][j];
					cross[i + 1][j] = temp;
				}
			}			

		}


		/*************** 5. 突然変異 ***************/
		for (i = 0; i < EVOL_NUM; i++) {			// 全ての交叉済みの個体に対して突然変異を行う
			for (j = 1; j < gene_length - 1; j++) {				// 初期速度と最終速度は変化させない
				if ( (double)rand() / RAND_MAX < MUT_RATE ) {
					cross[i][j] = rand() % 100 + 1;		// 突然変異させる
				}
			}
		}
		for (i = 0; i < EVOL_NUM; i++) {
			for (j = 0; j < gene_length; j++) {
				mutation[i][j] = cross[i][j];	// 突然変異後のcrossを突然変異済み配列に格納
			}
		}


		/*************** 6. 最終的な子世代の作成 ***************/
		for (i = 0; i < IND_NUM; i++) {
			if (i == 0) {
				for (j = 0; j < gene_length; j++) {
					individual[i][j] = elite[j];		// individual の先頭にエリートを格納
				}
			}
			else if (i <= PARETO_NUM) {
				for (j = 0; j < gene_length; j++) {
					individual[i][j] = pareto[i - 1][j];	// individual の1～PARETO_NUM番目にパレート解を格納
				}
			}
			else {
				for (j = 0; j < gene_length; j++) {
					individual[i][j] = mutation[i - 1][j];	// 残りは進化後の個体に更新
				}
			}

		}

	}
	/******************************************************/
	/*************** 遺伝的アルゴリズム終了 ***************/
	/******************************************************/


	/***** エリート個体の情報出力 *****/

	// 最終的なエリートの遺伝子情報を表示
	printf("\n\n〇last elite:\n");
	for (j = 0; j < gene_length; j++) {
		printf("%d\t", elite[j]);	// individual[0][j]
	}

	// 最終的なエリートの適応度を表示
	printf("\n〇last MAX_fitness:\t%d\n\n", MAX_fitness);
	
	// 最終的なエリートの速度の合計値を表示
	printf("\n〇last speed_sum:\t%d\n\n", speed_sum[MAX_IND_NUM]);

	// 最終的なエリートの速度変化の二乗の合計値を表示
	printf("\n〇last speed_change:\t%d\n\n", speed_change[MAX_IND_NUM]);


	/************************************************************************************/
	/***** パレート解の探索 *****/
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


	// 遺伝的アルゴリズムによる最終的な最適速度をelite.txtに書き込み
	FILE *f_elite;
	fopen_s(&f_elite, "elite.txt", "w");
	for (j = 0; j < gene_length; j++) {
		fprintf(f_elite, "%d\n", individual[0][j]);
	}
	fclose(f_elite);

	// 遺伝的アルゴリズムによる各世代での適応度をfitness.txtに書き込み
	FILE *f_fitness;
	fopen_s(&f_fitness, "fitness.txt", "w");
	for (g = 0; g < generation; g++) {
		fprintf(f_fitness, "%d\n", all_fitness[g]);
	}
	fclose(f_fitness);



	/***** 最終世代の各目的関数の値 *****/
	// 最終世代の適応度をlast_generation_fitness.txtに書き込み
	FILE *f_last_fitness;
	fopen_s(&f_last_fitness, "last_generation_fitness.txt", "w");
	for (i = 0; i < IND_NUM; i++) {
		fprintf(f_last_fitness, "%d\n", fitness[i]);
	}
	fclose(f_last_fitness);
	
	// 最終世代の各個体の速度合計をspeed_sum.txtに書き込み
	FILE *f_speed_sum;
	fopen_s(&f_speed_sum, "speed_sum.txt", "w");
	for (i = 0; i < IND_NUM; i++) {
		fprintf(f_speed_sum, "%d\n", speed_sum[i]);
	}
	fclose(f_speed_sum);

	// 最終世代の各個体の速度変化の二乗の合計をspeed_change.txtに書き込み
	FILE *f_speed_change;
	fopen_s(&f_speed_change, "speed_change.txt", "w");
	for (i = 0; i < IND_NUM; i++) {
		fprintf(f_speed_change, "%d\n", -speed_change[i]);
	}
	fclose(f_speed_change);



	/***** パレート最適解の各目的関数の値 *****/
	// パレート最適解の個体の速度合計をPareto_speed_sum.txtに書き込み
	FILE *f_Pareto_speed_sum;
	fopen_s(&f_Pareto_speed_sum, "Pareto_speed_sum.txt", "w");
	for (i = 0; i < PARETO_NUM; i++) {
		fprintf(f_Pareto_speed_sum, "%d\n", Pareto_speed_sum[i]);
	}
	fclose(f_Pareto_speed_sum);

	// パレート最適解の個体の速度変化の二乗の合計をPareto_speed_change.txtに書き込み
	FILE *f_Pareto_speed_change;
	fopen_s(&f_Pareto_speed_change, "Pareto_speed_change.txt", "w");
	for (i = 0; i < PARETO_NUM; i++) {
		fprintf(f_Pareto_speed_change, "%d\n", -Pareto_speed_change[i]);
	}
	fclose(f_Pareto_speed_change);

	return 0;
}
