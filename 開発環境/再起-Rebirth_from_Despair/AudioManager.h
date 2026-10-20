#pragma once
#include "BGM.h"
#include "SE.h"

// オーディオ設定クラス
class AudioManager
{
public:
	void SetBGMVolume(int v);
	void SetSEVolume(int v);

	int GetBGMVolume() const;
	int GetSEVolume() const;

	int bgmVolume = 50;
	int seVolume = 50;

private:
};