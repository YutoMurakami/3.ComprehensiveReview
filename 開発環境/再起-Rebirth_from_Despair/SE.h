#pragma once
#include "global.h"

#pragma comment(lib, "winmm.lib")	//音関係ﾗｲﾌﾞﾗﾘ

// SEクラス
class SE
{
public:
	HWND hWnd;		// ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ

	SE() = default;		// ｺﾝｽﾄﾗｸﾀ {} で処理が何もないことを意味する
	~SE() {};	// ﾃﾞｽﾄﾗｸﾀ

	void SE_Play(const char* fileName, bool isLoop);		// 効果音(wav)
	void SE_Open(const char* fileName);						// SE オープン
	void SE_Close();										// SE クローズ
	void SE_Play_MCI();										// SE 再生
	void SE_Play_Multi(const char* filename, int volume);	// マルチ音声再生
	void PlayDeadSoundOnce(int seVolume);					// 死亡音を一度だけ再生
	void SE_Play_Once(const char* filename, int volume);	// 一度だけ再生
};