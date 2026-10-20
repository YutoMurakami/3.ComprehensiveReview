#include "Title.h"

Title::Title()
{
}

// 操作関数(キーの追加)
void Title::Ctrl(const Key& key, Stage& stage, HWND& hWnd)
{
	// selの初期化
	if (before != stage.GetMode())
	{
		before = stage.GetMode();
		sel = 0;
	}

	// タイトル画面の操作内容
	if (stage.GetMode() == Mode::Title)
	{
		// 上選択（十字上 / スティック上 / W / ↑）
		if (key.Pressed(VK_UP) || key.Pressed('W') || key.POVPressed(0) || key.StickUpPressed()) {
			seRequest = AnimSE::sentaku;
			if (sel > 0) sel--;
			else         sel = 2;
		}

		// 下選択（十字下 / スティック下 / S / ↓）
		if (key.Pressed(VK_DOWN) || key.Pressed('S') || key.POVPressed(18000) || key.StickDownPressed()) {
			seRequest = AnimSE::sentaku;
			if (sel < 2) sel++;
			else         sel = 0;
		}

		switch (sel) {
			// ゲームスタート
		case 0:
			// 決定 (エンター/Aボタン)
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0)) {
				seRequest = AnimSE::ketei;
				stage.RequestMode(Mode::Tutorial);
			}
			break;

			// 設定
		case 1:
			// 決定 (エンター/Aボタン)
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0)) {
				seRequest = AnimSE::ketei;
				stage.RequestMode(Mode::Setting);
			}
			break;

			// ゲームを終了
		case 2:
			// 決定 (エンター/Aボタン)
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0)) {
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;
		}
	}
}

void Title::Draw(HDC hdc)    // 表示関数
{
	static int animFrame = 0;
	animFrame++;

	double rad = animFrame * 0.08;   // 少し速め
	int pulse = (int)(200 + sin(rad) * 50); // 150〜250くらい

	SetBkMode(hdc, TRANSPARENT);

	// ───── ゲームを始める ─────
	if (sel == 0)
		SetTextColor(hdc, RGB(pulse, pulse, pulse));  // 呼吸
	else
		SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定

	TextOut(hdc, WND_W / 2 - 20 * 7, 500,
		TEXT("ゲームを始める"),
		lstrlen(TEXT("ゲームを始める")));


	// ───── 設定 ─────
	if (sel == 1)
		SetTextColor(hdc, RGB(pulse, pulse, pulse));
	else
		SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定

	TextOut(hdc, WND_W / 2 - 20 * 4, 560,
		TEXT("設定画面"),
		lstrlen(TEXT("設定画面")));


	// ───── 終了 ─────
	if (sel == 2)
		SetTextColor(hdc, RGB(pulse, pulse, pulse));
	else
		SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定

	TextOut(hdc, WND_W / 2 - 20 * 7, 620,
		TEXT("ゲームを辞める"),
		lstrlen(TEXT("ゲームを辞める")));
}

// SE要求
AnimSE Title::GetSERequest() const
{
	return seRequest;
}

// SE要求解除
void Title::ClearSERequest()
{
	seRequest = AnimSE::None;
}