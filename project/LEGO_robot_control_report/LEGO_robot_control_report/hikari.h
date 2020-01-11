
/* ���Z���T�[���Z�b�g	�@�@					*
 * �����P�FnxtPtr  �����Q�F���̓|�[�g�ԍ�		*
 * �^�C�v��LIGHT_ACTIVE�A���[�h��raw	�@�@	*/
void setLightValue(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int port)
{
	ViUInt8 directCommandBuffer[4];

		directCommandBuffer[0] =0x05,	//[+1]�R�}���h�R�[�h - SETINPUTMODE
		directCommandBuffer[1] =0x00,	//[+2]���̓|�[�g(0-3)
		directCommandBuffer[2] =0x05;	//[+3]�Z���T�[�^�C�v - LIGHT_ACTIVE * * * *
		directCommandBuffer[3] =0x00,	//[+4]�Z���T�[���[�h - RAWMODE

		directCommandBuffer[1] = port;

	//NXT�փ_�C���N�g�R�}���h���M
	nxtPtr->sendDirectCommand(
		false,reinterpret_cast< ViByte* >(directCommandBuffer),
		sizeof(directCommandBuffer),NULL,0,status);
}

// ���Z���T�l�̓ǂݎ��
int getLightValue(nFANTOM100::iNXT* nxtPtr, nFANTOM100::tStatus status, int port)
{
	ViUInt8 directCommandBuffer[2];

	directCommandBuffer[0] = 0x07;  //[+1]�R�}���h�R�[�h -GETOUTPUTSTATE-
	directCommandBuffer[1] = port;  //[+2]�|�[�g�ԍ�

	ViUInt8 resBuffer[63];

	//NXT�փ_�C���N�g�R�}���h���M
	nxtPtr->sendDirectCommand(
		true,reinterpret_cast<ViByte*>(directCommandBuffer),
		sizeof(directCommandBuffer),resBuffer,15,status);

		return resBuffer[7];
}
