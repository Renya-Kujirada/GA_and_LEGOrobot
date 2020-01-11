
/* 光センサーをセット	　　					*
 * 引数１：nxtPtr  引数２：入力ポート番号		*
 * タイプはLIGHT_ACTIVE、モードはraw	　　	*/
void setLightValue(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int port)
{
	ViUInt8 directCommandBuffer[4];

		directCommandBuffer[0] =0x05,	//[+1]コマンドコード - SETINPUTMODE
		directCommandBuffer[1] =0x00,	//[+2]入力ポート(0-3)
		directCommandBuffer[2] =0x05;	//[+3]センサータイプ - LIGHT_ACTIVE * * * *
		directCommandBuffer[3] =0x00,	//[+4]センサーモード - RAWMODE

		directCommandBuffer[1] = port;

	//NXTへダイレクトコマンド送信
	nxtPtr->sendDirectCommand(
		false,reinterpret_cast< ViByte* >(directCommandBuffer),
		sizeof(directCommandBuffer),NULL,0,status);
}

// 光センサ値の読み取り
int getLightValue(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int port)
{
	ViUInt8 directCommandBuffer[2];

	directCommandBuffer[0] = 0x07;  //[+1]コマンドコード -GETOUTPUTSTATE-
	directCommandBuffer[1] = port;  //[+2]ポート番号

	ViUInt8 resBuffer[63];

	//NXTへダイレクトコマンド送信
	nxtPtr->sendDirectCommand(
		true,reinterpret_cast<ViByte*>(directCommandBuffer),
		sizeof(directCommandBuffer),resBuffer,15,status);

		return resBuffer[7];
}
