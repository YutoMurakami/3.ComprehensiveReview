#include "BGM.h"

//ｵｰﾌﾟﾝ
void BGM::BGM_Open(const char* fileName)
{
	char str[128];

	strcpy_s(str, "open BGM/");
	strcat_s(str, fileName);
	strcat_s(str, " alias BGM");

	mciSendString(str, NULL, 0, hWnd);
}

// 音量設定
void BGM::BGM_SetVolume(int volume)
{
	char command[128];
	sprintf_s(command, 128, "setaudio BGM volume to %d", volume * 5);
	mciSendString(command, NULL, 0, NULL);
}

//再生
void BGM::BGM_Play()
{
	mciSendString("play BGM notify", NULL, 0, hWnd);
}

//ｸﾛｰｽﾞ
void BGM::BGM_Close()
{
	mciSendString("close BGM", NULL, 0, hWnd);
}

//ﾙｰﾌﾟ
void BGM::BGM_Loop()
{
	char statusMsg[256];	//ｽﾃｰﾀｽﾒｯｾｰｼﾞの取得用

	//BGMの状態を取得する
	mciSendString("status BGM mode",
		statusMsg, 255, hWnd);

	//BGMが停止しているか？
	if (strcmp(statusMsg, "stopped") == 0)
	{
		//BGMの先頭に戻す
		mciSendString("seek BGM to start",
			NULL, 0, hWnd);
		//BGMの再生
		BGM_Play();
	}
}