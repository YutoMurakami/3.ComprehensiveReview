#pragma once
#include "key.h"
#include "stage.h"
#include "text.h"

class BossTalk : public Text
{
public:

	BossTalk() = default;
	~BossTalk() = default;

	void Ctrl(const Key& key, Stage& stage);
	void Draw(HDC hdc);

	// Secret ルートかどうかを設定（BossTalkに入る直前に呼ぶ）
	inline void SetFromSecret(bool secret)
	{
		fromSecret = secret;
		step = 0;
	}

private:
	// テキスト表示位置定数
	static const int TEXT_X = WND_W / 2 - 100;    // メインテキストX
	static const int TEXT_Y = WND_H / 2 - 80;     // メインテキストY（少し上寄せ）
	static const int HINT_X = WND_W / 2 - 120;    // ENTER or SPACE ヒントX
	static const int HINT_Y = WND_H / 2 + 80;     // ENTER or SPACE ヒントY
	int  step = 0;				// 現在のステップ (ノベルと同様の仕様)
	bool fromSecret = false;	// SecretStage ルートフラグ
};
