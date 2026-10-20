#include "AudioManager.h"

void AudioManager::SetBGMVolume(int v)
{
	bgmVolume = v;
}

void AudioManager::SetSEVolume(int v)
{
	seVolume = v;
}

int AudioManager::GetBGMVolume() const
{
	return bgmVolume;
}

int AudioManager::GetSEVolume() const
{
	return seVolume;
}