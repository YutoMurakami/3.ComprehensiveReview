#pragma once
#include "global.h"
#pragma comment(lib, "msimg32.lib")	//画像表示関係ﾗｲﾌﾞﾗﾘ

//マクロ関数
#define TRANS_COLOR RGB(0,255,0)	//透過色(RGB:グリーンバック)

// 描画クラス
class Image
{
public:		//他のクラス等から操作できる(記載忘れ プラベになる)

	HDC hdc;			// デバイスコンテキスト
	HDC hWrkDC;			// 作業用DC
	HDC hReverseDC;		// 画像反転用DC
	int w, h;			// 画像の幅、高さ
	int Bw, Bh;			// 攻撃時の画像の幅、高さ

	Image();	// コンストラクタ
	~Image();	// デストラクタ

	// 画像読み込み関数
	bool ReadBmp(LPCTSTR fileName);

	// 画像表示関数
	void BBlt(int x, int y, int gx, int gy);
	// 任意サイズ切り出し 画像表示関数
	void BBltWH(int x, int y, int gx, int gy, int vw, int vh);

	// 透過表示関数
	void TransBlt(int x, int y, int gx, int gy);
	// 任意サイズ切り出し 透過表示関数
	void TransBltWH(int x, int y, int gx, int gy, int vw, int vh);

	// 透過表示関数[オーバーロード]
	void TransBlt(int x, int y, int gx, int gy, float scale);
	// 任意サイズ切り出し透過表示関数[オーバーロード]
	void TransBltWH(int x, int y, int gx, int gy, int vw, int vh, float scal);

	// 半透明表示関数
	void AlphaBlt(int x, int y, int gx, int gy, int alpha);
	//任意サイズ切り出し 半透明表示関数
	void AlphaBltWH(int x, int y, int gx, int gy, int vw, int vh, int alpha);

	// 反転表示関数
	void ReverseBlt(int x, int y, int gx, int gy);
	// 任意サイズ切り出し 反転表示関数
	void ReverseBltWH(int x, int y, int gx, int gy, int vw, int vh);

	// 反転半透明表示関数
	void ReverseAlphaBlt(int x, int y, int gx, int gy, int alpha);
	// 任意サイズ切り出し 反転半透明表示関数
	void ReverseAlphaBltWH(int x, int y, int gx, int gy, int vw, int vh, int alpha);

private:	//クラス内だけで操作可能
	HDC		hMemDC;				// メモリDC
	HBITMAP	hBmp;				// 画像バンドル
	HBITMAP hRevBmp = NULL;
	HBITMAP hWrkBmp = NULL;
};