/* 超音波センサーをセット	　　				*
 * 引数１：nxtPtr  引数２：入力ポート番号		*
 * タイプはLIGHT_ACTIVE、モードはraw	　　	*/
void setUSonicValue(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int port)
{
	ViUInt8 directCommandBuffer[4];

		directCommandBuffer[0] =0x05,	//[+1]コマンドコード - SETINPUTMODE
		directCommandBuffer[1] =0x00,	//[+2]入力ポート(0-3)
		directCommandBuffer[2] =0x0B;	//[+3]センサータイプ - LIGHT_ACTIVE * * * *
		directCommandBuffer[3] =0x00,	//[+4]センサーモード - RAWMODE

		directCommandBuffer[1] = port;

	//NXTへダイレクトコマンド送信
	nxtPtr->sendDirectCommand(
		false,reinterpret_cast< ViByte* >(directCommandBuffer),
		sizeof(directCommandBuffer),NULL,0,status);
}

// 超音波センサ値の読み取り
int getUSonicValue(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int port)
{
	ViUInt8 directCommandBuffer1[] = {
		0x0F,	//[+1]command code - LSWRITE
		0x00,	//[+2]Port(0-3)
		0x02,	//number of bytes TX
		0x01,	//number of bytes RX
		0x02,	//0x02, 0x42 - doa one time reading (command 2 bytes long - address & action)
		0x42	
	};
	directCommandBuffer1[1] = port;

	ViUInt8 res[2] = {0};

	//Send direct command to the NXT.
	nxtPtr->sendDirectCommand(
		true,reinterpret_cast< ViByte* >( directCommandBuffer1 ),
		sizeof(directCommandBuffer1),res,1,status		);
	
	//デジタルセンサーの値取得は時間がかかるため、少し待つ
	//Msleep(100);

	ViUInt8 directCommandBuffer2[] = {
		0x0E,	//[+1]command code - LSGETSTATUS
		0x00,	//[+2]input port(0-3)
	};
	directCommandBuffer2[1] = port;
	
	//Send the direct command to the NXT.
	nxtPtr->sendDirectCommand(
		false,reinterpret_cast< ViByte* >( directCommandBuffer2 ),
		sizeof( directCommandBuffer2 ),NULL,0,status
	);

	//デジタルセンサーの値取得は時間がかかるため、少し待つ
	//Msleep(100);

	ViUInt8 directCommandBuffer3[] = {
		0x10,	//[+1]command code - LSREAD
		0x00,	//[+2]input port
	};
	directCommandBuffer3[1] = port;

	ViUInt8 resBuffer[63];

	//Send the direct command to the NXT.
	nxtPtr->sendDirectCommand(
		true,reinterpret_cast< ViByte* >(directCommandBuffer3),
		sizeof(directCommandBuffer3),resBuffer,20,status);
	
	//デジタルセンサーの値取得は時間がかかるため、少し待つ
	//Msleep(100);
	
//	printresponse(resBuffer, 20);

	return ( (int)resBuffer[4] );
}

