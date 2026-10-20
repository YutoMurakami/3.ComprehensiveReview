#pragma once
#include "key.h"
#include "dxInput.h"
#include "stage.h"
#include "text.h"

class Shop : public Text
{
public:
	Shop() = default;
	~Shop() = default;

	void Init();
	// coin/アンロックフラグを参照で受け取り、購入時に変更する
	void Ctrl(const Key& key, Stage& stage, int& coin,
		bool& unlockAttack3, bool& unlockSpin, int& maxStamina, int playerHP);
	// 文字入力時に有効無効の確認
	void Draw(HDC hdc, int coin, bool unlockAttack3, bool unlockSpin, int maxStamina, int playerHP);

	// SE
	AnimSE GetSERequest() const;
	void ClearSERequest();

	// 回復購入リクエスト（Game::Ctrl()で処理）
	bool GetHealRequest() const { return healRequest; }
	void ClearHealRequest() { healRequest = false; }

private:
	Mode before = Mode::Nothing;	// モード遷移前確認用
	AnimSE seRequest = AnimSE::None;// SE用
	int	sel = 0;					// 選択位置
	bool healRequest = false;		// HP回復購入フラグ
};
