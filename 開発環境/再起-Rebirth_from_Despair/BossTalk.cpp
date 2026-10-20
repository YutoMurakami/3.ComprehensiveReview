#include "BossTalk.h"

void BossTalk::Ctrl(const Key& key, Stage& stage)
{
	if (key.Pressed(VK_RETURN) || key.Pressed(VK_SPACE) || key.JoyPressed(0))
	{
		if (fromSecret)
		{
			// Secret ルート: 3ステップ
			// step0: 「特殊シーケンス」
			// step1: 「仮想からの解放」
			// step2: 「強敵を打ち滅ぼし」+「目を覚ましてください」
			// step3: Boss
			if (step < 2) step++;
			else stage.RequestMode(Mode::Boss);
		}
		else
		{
			// 通常ルート: 2ステップ
			// step0: 「最終シーケンス」
			// step1: 「戦闘力測定」+「自分との闘い」+「※※※目の挑戦」
			// step2: Boss
			if (step < 1)
				step++;
			else
				stage.RequestMode(Mode::Boss);
		}
	}
}

void BossTalk::Draw(HDC hdc)
{
	const int cy = WND_H / 2;   // 360: 画面中央Y
	const int ch = 60;          // フォント高さ (HMakeFont の h = 60)
	const int gap = 30;          // 行間余白（かぶらないよう広めに）

	SetBkMode(hdc, TRANSPARENT);

	// 全幅 RECT に DT_CENTER で描画 → 横は自動中央寄せ
	auto drawCenter = [&](int y, LPCTSTR text) {
		RECT r = { 0, y, WND_W, y + ch };
		DrawText(hdc, text, -1, &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		};

	if (fromSecret)
	{
		if (step == 0)
		{
			// 1行: "特殊シーケンス"
			drawCenter(cy - ch / 2, TEXT("特殊シーケンス"));
		}
		else if (step == 1)
		{
			// 1行: "仮想からの解放"
			drawCenter(cy - ch / 2, TEXT("仮想からの解放"));
		}
		else
		{
			// 2行: totalH = ch*2 + gap = 150
			int y1 = cy - (ch * 2 + gap) / 2;
			drawCenter(y1, TEXT("強敵を打ち滅ぼし"));
			drawCenter(y1 + ch + gap, TEXT("目を覚ましてください"));
		}
	}
	else
	{
		if (step == 0)
		{
			// 1行: "最終シーケンス"
			drawCenter(cy - ch / 2, TEXT("最終シーケンス"));
		}
		else
		{
			// 3行: totalH = ch*3 + gap*2 = 240
			int y1 = cy - (ch * 3 + gap * 2) / 2;
			drawCenter(y1, TEXT("戦闘力測定"));
			drawCenter(y1 + ch + gap, TEXT("自分との闘い"));
			drawCenter(y1 + (ch + gap) * 2, TEXT("※※※目の挑戦"));
		}
	}
	// ヒント（常に表示）: 最大コンテンツ下端 (3行時 = cy+120+60=540) より下
	drawCenter(WND_H - 100, TEXT("Enter or Space or Aボタン"));
}