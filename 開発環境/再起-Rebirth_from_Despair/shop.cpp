#include "shop.h"

// 初期化処理
void Shop::Init()
{
}

// 操作関数
void Shop::Ctrl(const Key& key, Stage& stage, int& coin,
	bool& unlockAttack3, bool& unlockSpin, int& maxStamina, int playerHP)
{
	// モード切り替わり時に選択をリセット
	if (before != stage.GetMode())
	{
		before = stage.GetMode();
		sel = 0;
	}

	if (stage.GetMode() != Mode::shop) return;

	// 上下選択（0〜4 の 5 項目）
	if (key.Pressed(VK_UP) || key.Pressed('W') || key.POVPressed(0) || key.StickUpPressed())
	{
		if (sel > 0)
			sel = sel - 1;
		else
			sel = 4;
		seRequest = AnimSE::sentaku;
	}
	if (key.Pressed(VK_DOWN) || key.Pressed('S') || key.POVPressed(18000) || key.StickDownPressed())
	{
		if (sel < 4)
			sel = sel + 1;
		else
			sel = 0;
		seRequest = AnimSE::sentaku;
	}

	// 決定（Enter / A ボタン）
	if (key.Pressed(VK_RETURN) || key.JoyPressed(0))
	{
		switch (sel)
		{
		case 0: // B連撃(Attack3)解放 - 1コイン
			if (!unlockAttack3 && coin >= 1)
			{
				coin -= 1;
				unlockAttack3 = true;
				seRequest = AnimSE::ketei;
			}
			break;

		case 1: // R回転斬り(Spin)解放 - 5コイン
			if (!unlockSpin && coin >= 5)
			{
				coin -= 5;
				unlockSpin = true;
				seRequest = AnimSE::ketei;
			}
			break;

		case 2: // スタミナ増強(+10) - コストは現在値に応じて増加
		{
			int cost = (maxStamina - 90) / 10;
			if (coin >= cost)
			{
				coin -= cost;
				maxStamina += 10;
				seRequest = AnimSE::ketei;
			}
			break;
		}

		case 3: // HP回復 +20 - 3コイン（HP満タン時は購入不可）
			if (coin >= 3 && playerHP < 100)
			{
				coin -= 3;
				healRequest = true;
				seRequest = AnimSE::ketei;
			}
			break;

		case 4: // 閉じる
			stage.RequestPrev();
			seRequest = AnimSE::ketei;
			break;
		}
	}
}

// 表示関数
void Shop::Draw(HDC hdc, int coin, bool unlockAttack3, bool unlockSpin, int maxStamina, int playerHP)
{
	SetHDC(hdc);
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);

	// タイトル
	TextDraw(WND_W / 2 - 25 * 5, 60, "== SHOP ==");

	// 所持コイン
	char coinStr[64];
	wsprintf(coinStr, "所持コイン: %d", coin);
	TextDraw(WND_W / 2 - 25 * 6, 120, coinStr);

	// 各選択肢
	char buf[256];
	const char* markers[5] = { "  ", "  ", "  ", "  ", "  " };
	markers[sel] = "> ";

	// sel=0: B連撃(Attack3)解放
	const char* attack3Status;
	if (unlockAttack3)
		attack3Status = "  [解放済]";
	else
		attack3Status = "";
	wsprintf(buf, "%sB(XBOX A)連撃解放    (1コイン)%s",
		markers[0], attack3Status);
	TextDraw(WND_W / 10, 200, buf);

	// sel=1: R回転斬り(Spin)解放
	const char* spinStatus;
	if (unlockSpin)
		spinStatus = "  [解放済]";
	else
		spinStatus = "";
	wsprintf(buf, "%sR(XBOX B)回転斬り解放(5コイン)%s",
		markers[1], spinStatus);
	TextDraw(WND_W / 10, 270, buf);

	// sel=2: スタミナ増強
	int cost = (maxStamina - 90) / 10;
	wsprintf(buf, "%sスタミナ増強 +10     (%dコイン)  [現在: %d]",
		markers[2], cost, maxStamina);
	TextDraw(WND_W / 10, 340, buf);

	// sel=3: HP回復 +20（HP満タン時はロック表示）
	const char* hpStatus;
	if (playerHP >= 100)
		hpStatus = "  [HP満タン]";
	else
		hpStatus = "";
	wsprintf(buf, "%sHP回復 +20           (3コイン)%s",
		markers[3], hpStatus);
	TextDraw(WND_W / 10, 410, buf);

	// sel=4: 閉じる
	wsprintf(buf, "%s閉じる", markers[4]);
	TextDraw(WND_W / 10, 480, buf);
}

// SEを取得
AnimSE Shop::GetSERequest() const
{
	return seRequest;
}
// SEの要求解除
void Shop::ClearSERequest()
{
	seRequest = AnimSE::None;
}
