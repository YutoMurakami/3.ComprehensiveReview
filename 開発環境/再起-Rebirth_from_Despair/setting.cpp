#include "Setting.h"

Setting::Setting()
{
}

// 操作関数(キーの追加)
void Setting::Ctrl(const Key& key, AudioManager& audio, Stage& stage, HWND& hWnd)
{
	// selの初期化
	if (before != stage.GetMode())
	{
		before = stage.GetMode();
		sel = 0;
	}

	// 設定画面の操作内容
	if (stage.GetMode() == Mode::Setting)
	{
		if (key.Pressed(VK_UP) || key.Pressed('W') || key.POVPressed(0) || key.StickUpPressed())
		{
			seRequest = AnimSE::sentaku;
			if (sel > 0) sel--;
			else         sel = 3;
		}

		if (key.Pressed(VK_DOWN) || key.Pressed('S') || key.POVPressed(18000) || key.StickDownPressed())
		{
			seRequest = AnimSE::sentaku;
			if (sel < 3) sel++;
			else         sel = 0;
		}

		switch (sel) {

			// BGM
		case 0:
			if (key.Pressed(VK_RIGHT) || key.Pressed('D') || key.POVPressed(9000) || key.StickRightPressed())
			{
				seRequest = AnimSE::sentaku;
				if (audio.bgmVolume < 100) audio.SetBGMVolume(audio.bgmVolume + 5);
			}

			if (key.Pressed(VK_LEFT) || key.Pressed('A') || key.POVPressed(27000) || key.StickLeftPressed())
			{
				seRequest = AnimSE::sentaku;
				if (audio.bgmVolume > 0) audio.SetBGMVolume(audio.bgmVolume - 5);
			}
			break;

			// SE
		case 1:
			if (key.Pressed(VK_RIGHT) || key.Pressed('D') || key.POVPressed(9000) || key.StickRightPressed())
			{
				seRequest = AnimSE::sentaku;
				if (audio.seVolume < 100) audio.SetSEVolume(audio.seVolume + 5);
			}
			if (key.Pressed(VK_LEFT) || key.Pressed('A') || key.POVPressed(27000) || key.StickLeftPressed())
			{
				seRequest = AnimSE::sentaku;
				if (audio.seVolume > 0) audio.SetSEVolume(audio.seVolume - 5);
			}
			break;

			// Titleに戻る
		case 2:
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
			{
				seRequest = AnimSE::ketei;
				stage.RequestMode(Mode::Title);
				sel = 0;
			}
			break;

			// ゲームを終了
		case 3:
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;
		}
	}
}

// 選択表示関数
void Setting::DrawChoice(HDC hdc)
{
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkColor(hdc, RGB(0, 0, 0));
	if (sel == 0) SetBkMode(hdc, OPAQUE);
	TextOut(hdc, 70, 80, TEXT("BGM音量調整"), lstrlen(TEXT("BGM音量調整")));
	SetBkMode(hdc, TRANSPARENT);

	if (sel == 1) SetBkMode(hdc, OPAQUE);
	TextOut(hdc, 70, 180, TEXT("SE音量調整"), lstrlen(TEXT("SE音量調整")));
	SetBkMode(hdc, TRANSPARENT);

	if (sel == 2) SetBkMode(hdc, OPAQUE);
	TextOut(hdc, 70, 460, TEXT("タイトルに戻る"), lstrlen(TEXT("タイトルに戻る")));
	SetBkMode(hdc, TRANSPARENT);

}

void Setting::Draw(HDC hdc)    // 表示関数
{
	SetTextColor(hdc, RGB(255, 0, 0));
	SetBkColor(hdc, RGB(0, 0, 0));
	if (sel == 3) SetBkMode(hdc, OPAQUE);
	TextOut(hdc, WND_W / 2 - 20 * 8, 600, TEXT("ゲームを辞める"), lstrlen(TEXT("ゲームを辞める")));
	SetBkMode(hdc, TRANSPARENT);

	SetTextColor(hdc, RGB(0, 0, 0));
	TextOut(hdc, 600, 300, TEXT("A or ← 音量を下げる"), lstrlen(TEXT("A or ← 音量を下げる")));
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, 600, 380, TEXT("D or → 音量を上げる"), lstrlen(TEXT("D or → 音量を上げる")));
	SetBkMode(hdc, TRANSPARENT);
}

// SE要求
AnimSE Setting::GetSERequest() const
{
	return seRequest;
}
// SE要求解除
void Setting::ClearSERequest()
{
	seRequest = AnimSE::None;
}