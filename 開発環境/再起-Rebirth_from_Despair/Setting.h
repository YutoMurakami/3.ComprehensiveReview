#pragma once
#include "key.h"
#include "AudioManager.h"
#include "stage.h"
#include "text.h"

class Setting : public Text
{
public:
	Setting();		// ｺﾝｽﾄﾗｸﾀ
	~Setting() = default;		// ﾃﾞｽﾄﾗｸﾀ

	// 操作関数(キーの追加 音量設定 Mode確認用 ゲーム終了用ハンドル)
	void Ctrl(const Key& key, AudioManager& audio, Stage& stage, HWND& hWnd);
	void Draw(HDC hdc);			// 表示関数
	void DrawChoice(HDC hdc);	// 選択表示関数

	AnimSE GetSERequest() const;		// SE要求
	void ClearSERequest();				// SE要求解除

private:
	Mode before = Mode::Nothing;		// モード遷移前確認用
	AnimSE seRequest = AnimSE::None;	// SEクラス
	int sel = 0;						// 決定用のsel
};