#pragma once
#include "global.h"

// テキスト表示クラス
class Text
{
public:
	Text();		// ｺﾝｽﾄﾗｸﾀ
	~Text() {};		// ﾃﾞｽﾄﾗｸﾀ

	//フォントの組み込み関数
	bool AddFont(LPCTSTR fontPath);
	//フォント作成関数
	bool MakeFont(int w, int h, LPCTSTR fontName);
	// 直接フォントを作成
	HFONT HMakeFont(int w, int h, LPCTSTR fontName);
	// 直線描画関数
	void DrawLine(HDC hdc, int x1, int y1, int x2, int y2);
	// フォントの割り当て
	void SetFont();
	//テキスト表示関数
	void TextDraw(int x, int y, const char* text);
	// 表示用
	void SetHDC(HDC hdc) { this->hdc = hdc; }

private:
	HDC hdc;				// ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ
	HFONT hFont;			// ﾌｫﾝﾄﾊﾝﾄﾞﾙ
};