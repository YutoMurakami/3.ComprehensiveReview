#pragma once
#include "key.h"
#include "stage.h"
#include "text.h"

class Title : public Text
{
public:

	Title();		// ｺﾝｽﾄﾗｸﾀ
	~Title() = default;		// ﾃﾞｽﾄﾗｸﾀ

	void Ctrl(const Key& key, Stage& stage, HWND& hWnd);	// 操作関数(キーの追加 Mode確認用 ゲーム終了用ハンドル)
	void Draw(HDC hdc);					// 表示関数

	AnimSE GetSERequest() const;		// SE要求
	void ClearSERequest();				// SE要求解除

private:
	Mode before = Mode::Nothing;		// モード遷移前確認用
	AnimSE seRequest = AnimSE::None;	// SEクラス
	int sel = 0;						// 決定用のsel
};