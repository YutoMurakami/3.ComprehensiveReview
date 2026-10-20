#include "text.h"

Text::Text()
{
	hFont = hFont;
	hdc = hdc;
}

//フォントの組み込み関数
bool Text::AddFont(LPCTSTR fontPath)
{
	if (AddFontResourceEx(
		fontPath,	//ﾌｫﾝﾄﾊﾟｽ
		FR_PRIVATE,	//終了時に削除
		NULL) == NULL)
	{
		ERR_MSG("文字フォントの組み込みに失敗", fontPath);
		return false;
	}
	return true;
}

//フォント作成関数
bool Text::MakeFont(int w, int h, LPCTSTR fontName)
{
	//フォントの作成
	HFONT hFont = CreateFont(
		h,		//文字の大きさ
		w,		//文字幅
		0,		//角度
		0,		//ﾍﾞｰｽﾗｲﾝ角度
		FW_REGULAR,	//太さ(太字:FW_BOLD)
		FALSE,	//斜体
		FALSE,	//下線
		FALSE,	//打消し線
		SHIFTJIS_CHARSET,	//文字ｾｯﾄ
		OUT_DEFAULT_PRECIS,	//出力精度
		CLIP_DEFAULT_PRECIS,//ｸﾘｯﾋﾟﾝｸﾞ精度
		DEFAULT_QUALITY,	//出力品質
		VARIABLE_PITCH | FF_ROMAN,//可変ﾋﾟｯﾁとﾌｫﾝﾄﾌｧﾐﾘ
		fontName);	//書体(NULL:現在使用中の書体)
	if (hFont == NULL)
	{
		ERR_MSG("フォントの作成に失敗", fontName);
		return false;
	}
	return true;
}

// 直接フォントを作成
HFONT Text::HMakeFont(int w, int h, LPCTSTR fontName)
{
	// フォントの作成
	HFONT hFont = CreateFont(
		h,							// 文字の大きさ
		w,							// 文字幅
		0,							// 角度
		0,							// ベースライン角度 (メモ:斜め上になるとか)
		FW_REGULAR,					// 太さ(太字:FW_BOLD)
		FALSE,						// 斜体
		FALSE,						// 下線 (メモ:アンダーライン)
		FALSE,						// 打消し線
		SHIFTJIS_CHARSET,			// 文字セット 
		OUT_DEFAULT_PRECIS,			// 出力精度
		CLIP_DEFAULT_PRECIS,		// クリッピング精度
		DEFAULT_QUALITY,			// 出力品質
		VARIABLE_PITCH | FF_ROMAN,	// 可変ピッチとフォントファミリ
		fontName);					// 書体(NULL:現在使用中の書体)

	if (hFont == NULL)
	{
		ERR_MSG("フォントの作成に失敗", fontName);
	}
	return hFont;
}

// 直線描画関数
void Text::DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL); // 開始点を設定
	LineTo(hdc, x2, y2);         // 終了点まで線を引く
}

// フォントの割り当て
void Text::SetFont() { SelectObject(hdc, hFont); }

//テキスト表示関数
void Text::TextDraw(int x, int y, const char* text)
{
	TextOut(hdc,	//画面
		x, y,		//表示先の座標xy
		text,			//表示する文字列
		lstrlen(text));	//表示する長さ
	SetBkMode(hdc, TRANSPARENT);//透明にする
}