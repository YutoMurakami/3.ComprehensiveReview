#include "End.h"

End::End()
{
	hHintFont = HMakeFont(12, 24, TEXT("ＭＳ ゴシック"));

	spark.ReadBmp("spark.bmp");

	for (int i = 0; i < 10; i++)
	{
		sparks[i].x = rand() % WND_W;
		sparks[i].y = rand() % 400;
		sparks[i].life = rand() % 60;
	}

	ESimg[0].ReadBmp("End1.bmp");
	ESimg[1].ReadBmp("End2.bmp");
	ESimg[2].ReadBmp("End3.bmp");
	ESimg[3].ReadBmp("End4.bmp");
	ESimg[4].ReadBmp("End5.bmp");
}

End::~End()
{
	DeleteObject(hHintFont);
}

// 操作関数(キーの追加 Mode確認用 ゲーム終了用ハンドル)
void End::Ctrl(const Key& key, Stage& stage, HWND& hWnd)
{
	// エンドロール中（スクロール完了前）は上下選択を無効にする
	if (stage.GetMode() != Mode::End || scrollEnd)
	{
		if (key.Pressed(VK_UP) || key.Pressed('W') || key.POVPressed(0) || key.StickUpPressed())
		{
			seRequest = AnimSE::sentaku;
			if (sel > 0) sel--;
			else         sel = 1;
		}

		if (key.Pressed(VK_DOWN) || key.Pressed('S') || key.POVPressed(18000) || key.StickDownPressed())
		{
			seRequest = AnimSE::sentaku;
			if (sel < 1) sel++;
			else         sel = 0;
		}
	}

	// ゲームクリア画面の操作内容
	if (stage.GetMode() == Mode::Clear)
	{
		clearTimer++;

		if (clearTimer <= 120) return;
		switch (sel) {
		case 0:
			// タイトル画面に戻る
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
			{
				seRequest = AnimSE::ketei;
				stage.RequestMode(Mode::End);	// エンドロールへ
				sel = 0;
				clearTimer = 0;
				return;
			}
			break;
		case 1:
			// タイトルに戻る
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
			{
				seRequest = AnimSE::ketei;
				stage.RequestMode(Mode::Title);	// エンドロールをスキップ
				sel = 0;
				clearTimer = 0;
				return;
			}
			break;
		}
	}

	// エンドロール画面の操作内容
	if (stage.GetMode() == Mode::End)
	{
		// Space長押しスキップ（60フレーム = 約1秒）
		if (!IsScrollEnd())
		{
			if (key.Down(VK_SPACE) || key.JoyDown(7))
			{
				skipHoldTimer++;
				if (skipHoldTimer >= 60)
				{
					scrollEnd = true;
					creditY = -(60 * 37 + 100);
					skipHoldTimer = 0;
				}
			}
			else
			{
				skipHoldTimer = 0;
			}
		}

		if (IsScrollEnd())
		{
			switch (sel) {
			case 0:
				// タイトル画面に戻る
				if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
				{
					seRequest = AnimSE::ketei;
					stage.RequestMode(Mode::Title);
					sel = 0;
					creditY = WND_H;
					scrollEnd = false;
					return;
				}
				break;
			case 1:
				// ゲームを辞める
				if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
				{
					PostMessage(hWnd, WM_CLOSE, 0, 0);	// アプリを閉じるメッセージを送信
				}
				break;
			}
		}
	}

	// ゲームオーバー画面の操作
	if (stage.GetMode() == Mode::Over)
	{
		switch (sel) {
		case 0:
			// タイトル画面に戻る
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
			{
				seRequest = AnimSE::ketei;
				stage.RequestMode(Mode::Title);
				sel = 0;
				return;
			}
			break;
		case 1:
			// ゲームを辞める
			if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);	// アプリを閉じるメッセージを送信
			}
			break;
		}
	}
}

// 表示関数
void End::DrawOver(HDC hdc)
{
	static int animFrame = 0;
	animFrame++;

	double rad = animFrame * 0.08;   // 少し速め
	int pulse = (int)(200 + sin(rad) * 50); // 150〜250くらい

	SetBkMode(hdc, TRANSPARENT);

	// ゲームオーバー画面の表示内容

	// ───── タイトルに戻る ─────
	if (sel == 0)
		SetTextColor(hdc, RGB(pulse, pulse, pulse));  // 呼吸
	else
		SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定

	TextOut(hdc, WND_W / 2 - 20 * 7, 580, TEXT("タイトルに戻る"), lstrlen(TEXT("タイトルに戻る")));

	// ───── ゲームを辞め ─────
	if (sel == 1)
		SetTextColor(hdc, RGB(pulse, pulse, pulse));  // 呼吸
	else
		SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定

	TextOut(hdc, WND_W / 2 - 20 * 7, 650, TEXT("ゲームを辞める"), lstrlen(TEXT("ゲームを辞める")));
}
void End::DrawClear(HDC hdc)
{
	static int animFrame = 0;
	animFrame++;

	double rad = animFrame * 0.08;   // 少し速め
	int pulse = (int)(200 + sin(rad) * 50); // 150〜250くらい

	// キラキラ演出制御
	for (int i = 0; i < 10; i++)
	{
		sparks[i].life--;

		if (sparks[i].life <= 0)
		{
			sparks[i].x = rand() % WND_W;
			sparks[i].y = rand() % 550;
			sparks[i].life = 60;
		}
	}

	if (clearTimer < 60)
	{
		int alpha = 255 - clearTimer * 8;

		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
		RECT rc = { 0,0,WND_W,WND_H };

		SetBkMode(hdc, TRANSPARENT);
		FillRect(hdc, &rc, brush);

		DeleteObject(brush);
	}
	if (clearTimer >= 60)
	{
		for (int i = 0; i < 10; i++)
		{
			spark.hdc = hdc;
			spark.TransBltWH(sparks[i].x, sparks[i].y, 0, 0, 16, 16);
		}
	}
	if (clearTimer >= 120) {
		// ゲームクリア画面の表示内容
		if (sel == 0)
			SetTextColor(hdc, RGB(pulse, pulse, pulse));  // 呼吸
		else
			SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定
		TextOut(hdc, WND_W / 2 - 20 * 6, 580, TEXT("エンドロール"), lstrlen(TEXT("エンドロール")));

		if (sel == 1)
			SetTextColor(hdc, RGB(pulse, pulse, pulse));  // 呼吸
		else
			SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定
		TextOut(hdc, WND_W / 2 - 20 * 7, 650, TEXT("タイトルに戻る"), lstrlen(TEXT("タイトルに戻る")));
	}
}
void End::DrawScrollEnd(HDC hdc)
{
	static int animFrame = 0;
	animFrame++;

	double rad = animFrame * 0.08;   // 少し速め
	int pulse = (int)(200 + sin(rad) * 50); // 150〜250くらい

	// 装飾用カウント
	static float frame = 0;
	frame+= 0.5;

	// レインボー縁取り
	int r = (sin(frame * 0.05) * 127) + 128;
	int g = (sin(frame * 0.07) * 127) + 128;
	int b = (sin(frame * 0.09) * 127) + 128;

	HPEN pen = CreatePen(PS_SOLID, 6, RGB(r, g, b));
	HPEN old = (HPEN)SelectObject(hdc, pen);

	// エンドロール画像
	for (int i = 0; i < 5; i++)	ESimg[i].hdc = hdc;

	//エンドロール画面の表示内容
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);

	int y = creditY;								// スクロール用

	// 1. 開始地点
	int scrollStart = -WND_H/3;

	// 2. 終了地点：25行目が画面上端に来た時 (creditY が -1500)
	int scrollEndPos = -(60 * 25);

	// 3. 画像を表示させる総距離 (0 - (-1500) = 1500)
	int totalDistance = scrollStart - scrollEndPos;

	// 4. 1枚あたりの表示期間 (1500 / 5 = 300ピクセル)
	int interval = totalDistance / 5;

	// 5. インデックス計算
	// creditY が 0 より大きい間は progress がマイナスになり、index もマイナスになる
	int progress = scrollStart - creditY;
	int imgIndex = progress / interval;

	// タイトル
	TextOut(hdc, textX, y, TEXT("タイトル"), 9);
	TextOut(hdc, textX, y + 60 * 1, TEXT("再起 Rebirth from Despair"), 26);

	// プログラム
	TextOut(hdc, textX, y + 60 * 3, TEXT("プログラム"), 11);
	TextOut(hdc, textX, y + 60 * 4, TEXT("村上 憂都"), 10);
	// 補助 検索等
	TextOut(hdc, textX, y + 60 * 6, TEXT("補助 検索等"), 12);
	TextOut(hdc, textX, y + 60 * 7, TEXT("Claude"), 7);
	TextOut(hdc, textX, y + 60 * 8, TEXT("ChatGPT"), 8);
	TextOut(hdc, textX, y + 60 * 9, TEXT("Gemini"), 7);

	// イラスト
	TextOut(hdc, textX, y + 60 * 11, TEXT("イラスト"), 9);
	TextOut(hdc, textX, y + 60 * 12, TEXT("村上 憂都"), 10);
	// ツール
	TextOut(hdc, textX, y + 60 * 14, TEXT("使用ツール"), 11);
	TextOut(hdc, textX, y + 60 * 15, TEXT("FireAlpaca"), 11);
	// 補助 検索等
	TextOut(hdc, textX, y + 60 * 17, TEXT("補助 検索等"), 12);
	TextOut(hdc, textX, y + 60 * 18, TEXT("ChatGPT"), 8);
	TextOut(hdc, textX, y + 60 * 19, TEXT("Gemini"), 7);

	// 音楽
	TextOut(hdc, textX, y + 60 * 21, TEXT("音楽"), 5);
	TextOut(hdc, textX, y + 60 * 22, TEXT("SUNO AI"), 8);
	TextOut(hdc, textX, y + 60 * 23, TEXT("MUREKA AI"), 10);
	TextOut(hdc, textX, y + 60 * 24, TEXT("魔王魂 https://maou.audio/rule/"), 32);
	TextOut(hdc, textX, y + 60 * 25, TEXT("@oyu_bgm"), 9);
	TextOut(hdc, textX, y + 60 * 26, TEXT("BGM Kew - 0123 in my room"), 26);
	TextOut(hdc, textX, y + 60 * 27, TEXT("Otakara BGM"), 12);
	TextOut(hdc, textX, y + 60 * 28, TEXT("効果音ラボ"), 11);
	TextOut(hdc, textX, y + 60 * 29, TEXT("hotaru:BIT STAR"), 16);

	// 協力
	TextOut(hdc, WND_W / 2 - 25 * 2, y + 60 * 33, TEXT("協力"), 5);
	TextOut(hdc, WND_W / 2 - 25 * 12, y + 60 * 34, TEXT("バンタンゲームアカデミー"), 25);

	// 最後
	TextOut(hdc, WND_W / 2 - 25 * 10.5, y + 60 * 41, TEXT("Thank you for playing"), 22);

	// スキップヒント（右上に常に表示）
	HFONT hOldFont = (HFONT)SelectObject(hdc, hHintFont);
	SetTextColor(hdc, RGB(pulse, pulse, pulse));  // 呼吸
	TextOut(hdc, 850, 10, TEXT("Space / Startボタン長押しでスキップ"), lstrlen(TEXT("Space / Startボタン長押しでスキップ")));
	SelectObject(hdc, hOldFont);
	SetTextColor(hdc, RGB(255, 255, 255));


	// スクリーンショット表示
	if (imgIndex >= 0 && imgIndex < 5) {
		// 縁取り描画
		Rectangle(hdc, 850, 300, 1170, 480);
		SelectObject(hdc, old);
		ESimg[imgIndex].BBltWH(850, 300, 0, 0, 320, 180);
	}
	DeleteObject(pen);

	if (!scrollEnd)
		creditY -= 0.75; // スクロール

	// スクロール終わり（"Thank you for playing" が画面上端を越えたら）
	if (creditY < -(60 * 41 + 100))
		scrollEnd = true;
}

void End::DrawEnd(HDC hdc)
{
	static int animFrame = 0;
	animFrame++;

	double rad = animFrame * 0.08;   // 少し速め
	int pulse = (int)(200 + sin(rad) * 50); // 150〜250くらい

	// エンドロール後に表示
	if (sel == 0)
		SetTextColor(hdc, RGB(pulse, pulse, pulse));  // 呼吸
	else
		SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定
	TextOut(hdc, WND_W / 2 - 60 * 7, WND_H / 2 - 150, TEXT("タイトルに戻る"), lstrlen(TEXT("タイトルに戻る")));

	if (sel == 1)
		SetTextColor(hdc, RGB(pulse, pulse, pulse));  // 呼吸
	else
		SetTextColor(hdc, RGB(100, 100, 100));        // 薄グレー固定
	TextOut(hdc, WND_W / 2 - 60 * 7, WND_H / 2 + 100, TEXT("ゲームを辞める"), lstrlen(TEXT("ゲームを辞める")));
}

// SE要求
AnimSE End::GetSERequest() const
{
	return seRequest;
}

// SE要求解除
void End::ClearSERequest()
{
	seRequest = AnimSE::None;
}