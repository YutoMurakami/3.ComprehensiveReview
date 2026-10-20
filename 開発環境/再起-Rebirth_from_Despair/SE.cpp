#include "SE.h"

// 効果音(wav)
// --- 細かいSE設定 ---
// SE オープン
void SE::SE_Open(const char* fileName)
{
	char str[128];
	strcpy_s(str, "open SE/");
	strcat_s(str, fileName);
	strcat_s(str, " alias SE");  // BGMの代わりにSEを使用
	mciSendString(str, NULL, 0, hWnd);
}

// SE 再生
void SE::SE_Play_MCI()
{
	mciSendString("play SE", NULL, 0, hWnd);  // notifyなしで一度だけ再生
}

// SE クローズ
void SE::SE_Close()
{
	mciSendString("close SE", NULL, 0, hWnd);
}

// --- 簡略化設定 ---
void SE::SE_Play(const char* fileName, bool isLoop)
{
	char str[128];
	char command[256];

	if (strlen(fileName) > 0)
	{
		strcpy_s(str, "SE/");
		strcat_s(str, fileName);
	}
	else
	{
		// ファイル名が無しなのでループの停止
		PlaySound(NULL, NULL, 0);
		return;
	}
	if (isLoop == true)
	{
		// ループあり
		PlaySound(str, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	}
	else
	{
		// ループなし
		PlaySound(str, NULL, SND_ASYNC | SND_FILENAME);
	}
}

// --- マルチ音声再生 ---
void SE::SE_Play_Multi(const char* filename, int volume) {
	static int seCounter = 0;
	char aliasName[64];
	char command[256];
	char fullPath[128];

	if (volume <= 0) return;

	// ファイルパスを構築（既存のSE_Openと同じ形式）
	strcpy_s(fullPath, "SE/");
	strcat_s(fullPath, filename);

	// ユニークなエイリアス名を作成
	sprintf_s(aliasName, 64, "SE%d", seCounter++);

	// ファイルを開く
	sprintf_s(command, 256, "open %s alias %s", fullPath, aliasName);
	mciSendString(command, NULL, 0, hWnd);

	// 音量設定
	sprintf_s(command, 256, "setaudio %s volume to %d", aliasName, volume * 5);
	mciSendString(command, NULL, 0, NULL);

	// 再生
	sprintf_s(command, 256, "play %s", aliasName);
	mciSendString(command, NULL, 0, hWnd);

	// 再生後にクローズ（別スレッドで少し遅延させる必要があるかも）
	sprintf_s(command, 256, "close %s", aliasName);
	// すぐにcloseすると音が出ないので、タイマーで後でクローズするか
	// 単純にcloseしないでおく（メモリ使用量は増えるが動作する）
}

// 死亡音を一度だけ再生
void SE::PlayDeadSoundOnce(int seVolume)
{
	static DWORD lastDeadSoundTime = 0;
	DWORD now = timeGetTime();
	const DWORD DEAD_SOUND_COOLDOWN_MS = 350; // 350ms のクールダウン
	if (now - lastDeadSoundTime > DEAD_SOUND_COOLDOWN_MS) {
		SE_Play_Multi("Dead.mp3", seVolume);
		lastDeadSoundTime = now;
	}
}

// 一度だけ再生
void SE::SE_Play_Once(const char* filename, int volume) {
	static char lastPlayed[128] = "";
	static DWORD lastTime = 0;

	if (volume <= 0) return;

	// 直前に同じ音を鳴らしてから 3 秒以内なら再生しない
	if (strcmp(lastPlayed, filename) == 0 && timeGetTime() - lastTime < 3000) {
		return;
	}

	// 実際の再生処理
	SE_Play_Multi(filename, volume);

	strcpy_s(lastPlayed, filename);
	lastTime = timeGetTime();
}