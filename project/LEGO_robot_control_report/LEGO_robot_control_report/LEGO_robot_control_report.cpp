#include "stdafx.h"
#include "fantom/iNXT.h"
#include "fantom/iNXTIterator.h"
#include "fantom/tStatus.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include <time.h>		// msleepを使うために必要
#include "msleep.h"		// n秒間動作を停止する関数を使うのに用いる
#include "choonnpa.h"	// 超音波センサーを用いるのに使う
#include "hikari.h"		// 光センサーを用いるのに使う

#define distance	240	// 壁（ボール）の手前までの超音波センサーの距離(255以下で均等に30分割するための最適距離)
#define gene_length 30	// 読み込む遺伝子長
#define Shot_Power  40	// 赤玉を打つパワー

#define MOTOR_A 0	// アームの番号
#define MOTOR_R 1	// 右モータの番号
#define MOTOR_L 2	// 左モータの番号


//---------------------------------------- プロトタイプ宣言 ----------------------------------------
void motor(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int mot_num, char motor_speed);
void file_input(char *filename, int speed[gene_length]);
//--------------------------------------------------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[]) {
	int speed[gene_length] = {};			// GAによる最適速度をファイルから読み込み格納する配列
	int Position[gene_length] = {};			// 0～255までの距離(位置情報)を格納するための配列

	/***** 1. speed配列の速度に対応する超音波センサの値（位置情報）の決定 *****/
	//------------------------------ 位置情報の計算 (17 ～ 249 まで) -------------------------------
	for (int i = 0; i < gene_length; i++) {
		// 配列の先頭から降順に、位置情報を格納する。
		// LightValue = 24 の時に停止 (speed[29] = 0)するように、
		// Position[29]に24が格納されるように計算する。
		// Position配列は、「8」ずつ値が変化していくように設定。

		Position[i] = (distance - distance * i / gene_length) + 9;

	}
	//---------------------------------------------------------------------------------------------

	/***** 2. エリート（最適速度）ファイルの読み込み*****/
	file_input("elite.txt", speed);

	// 各超音波センサーの値に対する、各speed配列内の最適速度の出力
	for (int i = 0; i < gene_length; i++) {
		printf("num: %d\t\tUSonicValue >= %d\tspeed: %d\n", i, Position[i], speed[i]);
	}


	//----------------------------------- 前処理 -----------------------------------
	// ライブラリ参照用変数
	nFANTOM100::iNXTIterator* nxtIteratorPtr = NULL;
	nFANTOM100::tStatus status;
	nFANTOM100::iNXT* nxtPtr = NULL;

	char exitcode = 0;

	// NXTへの接続．参照用ポインタの取得（USB or Bluetooth）
	nxtIteratorPtr = nFANTOM100::iNXT::createNXTIterator(
		true,
		5,
		status
	);

	// 接続に失敗した場合，終了
	if (status.isFatal()) {
		printf("接続に失敗しました。\n\n");
		return 0;
	}

	// 接続成功時の出力
	printf("Communication start with NXT...\n");

	// ライブラリ用ポインタの取得
	nxtPtr = nxtIteratorPtr->getNXT(status);

	// 参照用ポインタの破棄
	nFANTOM100::iNXT::destroyNXTIterator(nxtIteratorPtr);

	//各センサーの初期化
	setUSonicValue(nxtPtr, status, 3);		// 超音波センサ初期化　ポート番号4は3
	setLightValue(nxtPtr, status, 2);		// 光センサー初期化	   ポート番号3は2
  // --------------------------------------------------------------------------------


  /*********************************************************/
  /*************** 3. ロボットの自動制御開始 ***************/
  /*********************************************************/

	int num = 0;	// 超音波センサーの数値（距離）に対応する、speed配列のインデックス

	while (exitcode == 0) {			// exitcode = 1となるまで（赤玉を打つまで）無限ループ

		int LightValue = getLightValue(nxtPtr, status, 2);		// 光センサーの値
		int USonicValue = getUSonicValue(nxtPtr, status, 3);	// 超音波センサーの値

		/***** step1. 壁までGAでの最適速度で近づく *****/
		// 実際の距離を超音波センサーの数値に対応させ、壁まで走行。
		if (USonicValue) {
			for (int i = 0; i < gene_length; i++) {			// 超音波センサーの値を用いて現在の位置を把握
				if (USonicValue >= Position[i]) {			// 超音波センサーの値に対応したスピードにする
					num = i;
					break;		// 距離に応じた最適速度にした後、for文から抜ける。
				}
			}
		}
		motor(nxtPtr, status, MOTOR_R, speed[num]);		// 右車輪
		motor(nxtPtr, status, MOTOR_L, speed[num]);		// 左車輪

		// 超音波センサーの数値、超音波センサーの値に対応するPositionの値、GAの速度、光センサーの値
		printf("USonicValue=%5d\tPosition=%5d\tspeed=%5d\tLightValue=%5d\r", USonicValue, Position[num], speed[num], LightValue);

		/***** step2. 壁の間際(USonicValue = 24)まで移動した場合(speed配列の末尾まで速度データを読み込んだ場合) *****/
		if (num == gene_length - 1) {
			/***** step2. 目の前の対象物体の色を判定 *****/
			Msleep(1000);
			LightValue = getLightValue(nxtPtr, status, 2);
			Msleep(1000);
			
			/***--------------- step2-1. 光センサーの値が170以上、すなわち、赤玉以外の場合 ---------------***/
			if (LightValue > 170) {
				Msleep(1000);

				/*****  step3. 対象物の右側部分を探索 *****/
				// 3-1. 右側を向く
				motor(nxtPtr, status, MOTOR_R, -speed[27]);
				Msleep(1000);
				
				// 3-2. 少しだけ前進
				motor(nxtPtr, status, MOTOR_R, speed[27]);
				motor(nxtPtr, status, MOTOR_L, speed[27]);
				Msleep(800);

				// 3-3. 停止
				motor(nxtPtr, status, MOTOR_R, 0);
				motor(nxtPtr, status, MOTOR_L, 0);
				Msleep(1000);

				/***** step4. 目の前(右側前方)の対称物体の色を判定 *****/
				Msleep(1000);
				LightValue = getLightValue(nxtPtr, status, 2);
				Msleep(1000);

				/*****  step4-A. 対称物体が赤玉である場合 *****/
				// 対称物体との距離が近いため、LightValueの赤玉条件を165としている。
				if (LightValue < 165) {
					// A-1. アームを時計回りにShot_Powerだけ回す
					motor(nxtPtr, status, MOTOR_A, Shot_Power);
					Msleep(180);

					// A-2. アームを停止
					motor(nxtPtr, status, MOTOR_A, 0);
					Msleep(20);

					// A-3. アームを反時計回りにShot_Powerだけ回す
					motor(nxtPtr, status, MOTOR_A, -(Shot_Power));
					Msleep(200);

					// A-4. アームを停止
					motor(nxtPtr, status, MOTOR_A, 0);

					break;	// 無限ループから抜ける(プログラム終了)
					// exitcode = 1;とすると、step5を実行してしまう。（exitcodeを評価するのは、先頭のwhile文）
				}

				/***** step4-B. 対象物が赤玉以外の場合 *****/
				// step3.の、対象物の右側探索直前の状態（向き）に戻す
				else {
					// B-1. 少しだけ後退
					motor(nxtPtr, status, MOTOR_R, -speed[27]);
					motor(nxtPtr, status, MOTOR_L, -speed[27]);
					Msleep(1200);

					// B-2. 左側を向く
					motor(nxtPtr, status, MOTOR_R, speed[27]-8);
					Msleep(800);

					// B-3. 停止
					motor(nxtPtr, status, MOTOR_R, 0);
					motor(nxtPtr, status, MOTOR_L, 0);
					Msleep(1000);
				}

				/***** step5. 未探索な右側領域を探索できるように後退する *****/
				// 5-1. 右斜め後ろに後退
				motor(nxtPtr, status, MOTOR_R, -speed[24]);
				motor(nxtPtr, status, MOTOR_L, -speed[24] - 10);
				Msleep(2000);

				// 5-2. 左斜め後ろに後退し、斜体の向きをまっすぐに調整
				motor(nxtPtr, status, MOTOR_R, -speed[24] - 10);
				motor(nxtPtr, status, MOTOR_L, -speed[24]);
				Msleep(2000);

				// 5-3. 一度止まる
				motor(nxtPtr, status, MOTOR_R, 0);
				motor(nxtPtr, status, MOTOR_L, 0);
				Msleep(1000);

			}

			/***--------------- step2-2. 対象物が赤玉の場合 ---------------***/
			else {
				// 2-2-1. アームを時計回りにShot_Powerだけ回す
				motor(nxtPtr, status, MOTOR_A, Shot_Power);
				Msleep(180);
				
				// 2-2-2. アームを停止
				motor(nxtPtr, status, MOTOR_A, 0);
				Msleep(20);
				
				// 2-2-3. アームを反時計回りにShot_Powerだけ回す
				motor(nxtPtr, status, MOTOR_A, -(Shot_Power) );
				Msleep(200);
				
				// 2-2-4. アームを停止
				motor(nxtPtr, status, MOTOR_A, 0);

				exitcode = 1;	// 無限ループ終了(プログラム終了)
			}

			num = 0;	// speed配列のインデックスの初期化（壁から離れた状態にする）
		}	// 最初の光センサーの値によるif文終了

	}	// 無限ループ(while文)終了

	/*********************************************************/
	/*************** 3. ロボットの自動制御終了 ***************/
	/*********************************************************/



	//------------------------------ 後処理 ------------------------------
	// 参照用ポインタの破棄
	nFANTOM100::iNXT::destroyNXT(nxtPtr);
	return 0;
	// -------------------------------------------------------------------

}


/***** ファイル読み込み関数 *****/
void file_input(char *filename, int speed[gene_length]) {
	FILE *fp;
	int i = 0;
	int temp = 0;

	if ((fopen_s(&fp, filename, "r")) != 0) {
		printf("ファイルがありません:%s\n", filename);
		exit(1);
	}

	// speed配列にeliteファイルのデータを格納
	for (i = 0; i < gene_length; i++) {
		fscanf_s(fp, "%d", &speed[i]);
	}
}
/***** ここまで*****/


/***** モーター制御関数（アーム、右車輪、左車輪）*****/
void motor(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int mot_num, char motor_speed) {	// 関数の実行結果を格紊する変数
	ViUInt8 directCommandBuffer[] = {

	4, // コマンド．「SETOUTPUTSTATE《を意味する4が入る．

	0, // 出力ポート番号．0：「A《ポート，1：「B《ポート，2：「C《ポート．

	0, // モーターのパワー．値の範囲：-100～+100．0：モーター停止．

	7, // モーターのモード．1：モーターオン，2：ブレーキオン，4：制御オン．

	1, // 制御モード．0：制御なし，1：スピードの制御，2：複数モーターの同期

	0, // 回転レート．値の範囲：-100～+100．

	0x20, // 動作状態．0x10：徐々に加速，0x20：通常動作，0x30：徐々に減速．

	0,0,0,0 // 制御する回転量．0：回転継続．

	};


	// モーターの左右・速度指定（アーム: mot_num = 0, 右車輪：mot_num = 1, 左車輪：mot_num = 2モーター）
	directCommandBuffer[1] = mot_num;		// 動かす部位の指定
	directCommandBuffer[2] = motor_speed;	// 速度の指定


	// ダイレクトコマンドの送信

	nxtPtr->sendDirectCommand(

		false,

		reinterpret_cast<ViByte*>(directCommandBuffer),

		sizeof(directCommandBuffer),

		NULL,

		0,

		status

	);

}
/***** ここまで ****/
