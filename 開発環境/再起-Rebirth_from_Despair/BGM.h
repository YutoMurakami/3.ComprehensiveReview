#pragma once
#include "global.h"

#pragma comment(lib, "winmm.lib")	//音関係ﾗｲﾌﾞﾗﾘ

// サウンドクラス
class BGM
{
public:
	HWND hWnd;		// ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ

	BGM() = default;		// ｺﾝｽﾄﾗｸﾀ {} で処理が何もないことを意味する
	~BGM() = default;	// ﾃﾞｽﾄﾗｸﾀ

	//Sound関数
	void BGM_Open(const char* fileName);	// ｵｰﾌﾟﾝ
	void BGM_SetVolume(int volume);			// 音量設定
	void BGM_Play();	//再生
	void BGM_Close();	//ｸﾛｰｽﾞ
	void BGM_Loop();	//ﾙｰﾌﾟ

private:
};