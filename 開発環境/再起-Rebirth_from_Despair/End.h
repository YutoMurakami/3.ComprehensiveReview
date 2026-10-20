#pragma once
#include "key.h"
#include "stage.h"
#include "text.h"
#include "image.h"

class End : public Text
{
public:

	End();		// ｺﾝｽﾄﾗｸﾀ
	~End();		// ﾃﾞｽﾄﾗｸﾀ

	void Ctrl(const Key& key, Stage& stage, HWND& hWnd);	// 操作関数(キーの追加 Mode確認用 ゲーム終了用ハンドル)
	void DrawOver(HDC hdc);						// 表示関数(文字の設定 ゲームオーバー)
	void DrawClear(HDC hdc);					// 表示関数(文字の設定 ゲームクリア)
	void DrawScrollEnd(HDC hdc);				// 表示関数(文字の設定 エンドロール)
	void DrawEnd(HDC hdc);						// 表示関数(文字の設定 エンドロール後)

	AnimSE GetSERequest() const;		// SE要求
	void ClearSERequest();				// SE要求解除

	// スクロール終わり
	inline bool IsScrollEnd() { return scrollEnd; }

private:
	Mode before = Mode::Nothing;		// モード遷移前確認用
	AnimSE seRequest = AnimSE::None;	// SEクラス
	HFONT hHintFont = nullptr;			// スキップヒント用フォント
	Spark sparks[10];					// スパーク配列制御
	Image spark;						// キラキラエフェクト
	Image ESimg[5];						// Endロール用 表示画像
	int clearTimer = 0;					// clear時のタイマー
	int sel = 0;						// 決定用のsel
	float creditY = WND_H;				// 文字スクロール用
	int textX = 200;					// 文字の横の位置用
	bool scrollEnd = false;				// スクロール終わり
	int skipHoldTimer = 0;				// Space長押しスキップ用カウンタ
};