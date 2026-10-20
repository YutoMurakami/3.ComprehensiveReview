#include "image.h"

// コンストラクタ
Image::Image()
{
	//メモリDCの作成
	hMemDC = CreateCompatibleDC(NULL);
}

// デストラクタ
Image::~Image()
{	//画像ﾊﾝﾄﾞﾙの削除
	DeleteObject(hBmp);
	//メモリDCの削除
	DeleteDC(hMemDC);
}

//画像読み込み関数
bool Image::ReadBmp(LPCTSTR fileName)
{
	char str[128];
	strcpy_s(str, "image/");
	strcat_s(str, fileName);

	hBmp = (HBITMAP)LoadImage(
		NULL,				//ｲﾝｽﾀﾝｽ番号(省略)
		str,			//画像ﾌｧｲﾙ名
		IMAGE_BITMAP,		//ﾋﾞｯﾄﾏｯﾌﾟ画像を設定
		0, 0,		//画像内の読込み開始座標(通常は0,0)
		LR_LOADFROMFILE);	//ﾌｧｲﾙから読込む指定
	if (hBmp == NULL)
	{
		ERR_MSG("画像ファイルの読込み失敗", fileName);
		return false;	//読込み失敗
	}

	if (!hReverseDC) hReverseDC = CreateCompatibleDC(NULL);
	if (!hWrkDC) hWrkDC = CreateCompatibleDC(NULL);

	if (hRevBmp) DeleteObject(hRevBmp);
	if (hWrkBmp) DeleteObject(hWrkBmp);

	// メンバ変数としてビットマップを作成
	HDC hdcRef = GetDC(NULL);
	hRevBmp = CreateCompatibleBitmap(hdcRef, 200, 200);
	hWrkBmp = CreateCompatibleBitmap(hdcRef, 200, 200);
	ReleaseDC(NULL, hdcRef);

	SelectObject(hReverseDC, hRevBmp);
	SelectObject(hWrkDC, hWrkBmp);

	return true;	//読込み成功
}

//画像表示関数
void Image::BBlt(int x, int y, int gx, int gy)
{
	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	//画像の表示
	BitBlt(hdc,			//画面
		x, y,			//表示先の座標x,y
		w, h,			//画像の幅、高さ
		hMemDC,			//ﾒﾓﾘDC
		gx, gy,			//画像上の座標x,y
		SRCCOPY);		//「ﾒﾓﾘDCから画面にｺﾋﾟｰ」という指定
}
// 任意サイズ切り出し 画像表示関数
void Image::BBltWH(int x, int y, int gx, int gy, int vw, int vh)
{
	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	//画像の表示
	BitBlt(hdc,			//画面
		x, y,			//表示先の座標x,y
		vw, vh,			//画像の幅、高さ
		hMemDC,			//ﾒﾓﾘDC
		gx, gy,			//画像上の座標x,y
		SRCCOPY);		//「ﾒﾓﾘDCから画面にｺﾋﾟｰ」という指定
}

//透過表示関数
void Image::TransBlt(int x, int y, int gx, int gy)
{
	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	TransparentBlt(hdc,	//画面
		x, y,			//表示先座標
		w, h,			//表示する画像の幅、高さ
		hMemDC,			//ﾒﾓﾘDC
		gx, gy,			//画像上の座標
		w, h,			//画像の幅、高さ
		TRANS_COLOR);	//透過色
}
// 任意サイズ切り出し透過表示関数
void Image::TransBltWH(int x, int y, int gx, int gy, int vw, int vh)
{
	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	TransparentBlt(hdc,	//画面
		x, y,			//表示先座標
		vw, vh,			//表示する画像の幅、高さ
		hMemDC,			//ﾒﾓﾘDC
		gx, gy,			//画像上の座標
		vw, vh,			//画像の幅、高さ
		TRANS_COLOR);	//透過色
}

// 透過表示関数[オーバーロード]
void Image::TransBlt(int x, int y, int gx, int gy, float scale)
{
	// 拡大縮小値(scale):1.0が基準
	int sw = (float)w * scale;
	int sh = (float)h * scale;

	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	TransparentBlt(hdc,	//画面
		x, y,			//表示先座標
		sw, sh,			//表示する画像の幅、高さ
		hMemDC,			//ﾒﾓﾘDC
		gx, gy,			//画像上の座標
		w, h,			//画像の幅、高さ
		TRANS_COLOR);	//透過色
}

// 任意サイズ切り出し 透過表示関数[オーバーロード]
void Image::TransBltWH(int x, int y, int gx, int gy, int vw, int vh, float scale)
{
	// 拡大縮小値(scale):1.0が基準
	int sw = (float)vw * scale;
	int sh = (float)vh * scale;

	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	TransparentBlt(hdc,	//画面
		x, y,			//表示先座標
		sw, sh,			//表示する画像の幅、高さ
		hMemDC,			//ﾒﾓﾘDC
		gx, gy,			//画像上の座標
		vw, vh,			//画像の幅、高さ
		TRANS_COLOR);	//透過色
}

// 半透明表示関数
void Image::AlphaBlt(int x, int y, int gx, int gy, int alpha)
{
	// 透過構造体(透明度:0(完全透明)～255(完全不透明))
	BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, alpha, 0 };

	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	//①表示座標の背景をWrkDCにコピー
	BitBlt(hWrkDC, 0, 0, w, h, hdc, x, y, SRCCOPY);

	//②WrkDCの背景の上に透過表示
	TransparentBlt(hWrkDC, 0, 0, w, h, hMemDC, gx, gy, w, h, TRANS_COLOR);

	//③WrkDCの画像を画面に半透明表示
	AlphaBlend(hdc,		// 画面
		x, y,			// 表示位置
		w, h,			// 画像幅、高さ
		hWrkDC,			// 作業用DC
		0, 0,			// 作業用DC内の画像座標
		w, h,			// 画像の幅、高さ
		blendFunc);		// 透過構造体
}

// 任意サイズ切り出し 半透明表示関数
void Image::AlphaBltWH(int x, int y, int gx, int gy, int vw, int vh, int alpha)
{
	// 透過構造体(透明度:0(完全透明)～255(完全不透明))
	BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, alpha, 0 };

	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	//①表示座標の背景をWrkDCにコピー
	BitBlt(hWrkDC, 0, 0, vw, vh, hdc, x, y, SRCCOPY);

	//②WrkDCの背景の上に透過表示
	TransparentBlt(hWrkDC, 0, 0, vw, vh, hMemDC, gx, gy, vw, vh, TRANS_COLOR);

	//③WrkDCの画像を画面に半透明表示
	AlphaBlend(hdc,		// 画面
		x, y,			// 表示位置
		vw, vh,			// 画像幅、高さ
		hWrkDC,			// 作業用DC
		0, 0,			// 作業用DC内の画像座標
		vw, vh,			// 画像の幅、高さ
		blendFunc);		// 透過構造体
}

// 反転表示関数
void Image::ReverseBlt(int x, int y, int gx, int gy)
{
	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	// 反転表示
	StretchBlt(
		hReverseDC,		// 反転用DC
		w, 0,			// 画像の左上をDC内の右上に
		-(w + 1), h,	// 画像幅をマイナスにして反転
		hMemDC,
		gx, gy,
		w, h,
		SRCCOPY);

	TransparentBlt(hdc, x, y, w, h,
		hReverseDC,		// 反転用DC
		0, 0,			// 反転用DC上の座標
		w, h, TRANS_COLOR);
}

// 任意サイズ切り出し 反転表示関数
void Image::ReverseBltWH(int x, int y, int gx, int gy, int vw, int vh)
{
	SelectObject(hMemDC, hBmp);

	// 作業領域を一旦クリア（透過色でクリア）
	HBRUSH hBrush = CreateSolidBrush(TRANS_COLOR);
	RECT clearRect = { 0, 0, 200, 200 };
	FillRect(hReverseDC, &clearRect, hBrush);
	DeleteObject(hBrush);

	StretchBlt(
		hReverseDC, vw - 1, 0, -vw, vh,  // コピー先：反転描画
		hMemDC, gx, gy, vw, vh,        // コピー元：通常通り
		SRCCOPY);

	// 画面へ透過描画
	TransparentBlt(hdc, x, y, vw, vh,
		hReverseDC, 0, 0, vw, vh, TRANS_COLOR);
}

// 反転半透明表示関数
void Image::ReverseAlphaBlt(int x, int y, int gx, int gy, int alpha)
{
	// 透過構造体(透明度:0(完全透明)～255(完全不透明))
	BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, alpha, 0 };

	//表示する画像をﾒﾓﾘDCに展開
	SelectObject(hMemDC, hBmp);

	//①反転表示
	StretchBlt(
		hReverseDC,		// 反転用DC
		w, 0,			// 画像の左上をDC内の右上に
		-(w + 1), h,	// 画像幅をマイナスにして反転
		hMemDC,
		gx, gy,
		w, h,
		SRCCOPY);

	//②表示座標の背景をWrkDCにコピー
	BitBlt(hWrkDC, 0, 0, w, h, hdc, x, y, SRCCOPY);

	//③WrkDCの背景の上に反転透過表示
	TransparentBlt(hWrkDC, 0, 0, w, h, hReverseDC, 0, 0, w, h, TRANS_COLOR);

	//④WrkDCの画像を画面に半透明表示
	AlphaBlend(hdc,		// 画面
		x, y,			// 表示位置
		w, h,			// 画像幅、高さ
		hWrkDC,			// 作業用DC
		0, 0,			// 作業用DC内の画像座標
		w, h,			// 画像の幅、高さ
		blendFunc);		// 透過構造体
}
// 任意サイズ切り出し 反転半透明表示関数
void Image::ReverseAlphaBltWH(int x, int y, int gx, int gy, int vw, int vh, int alpha)
{
	BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, (BYTE)alpha, 0 };
	SelectObject(hMemDC, hBmp);

	PatBlt(hReverseDC, 0, 0, 200, 200, BLACKNESS);

	// 同様の修正
	StretchBlt(hReverseDC, 0, 0, vw, vh, hMemDC, gx + vw - 1, gy, -vw, vh, SRCCOPY);

	// WrkDC（背景）のコピー先を (0,0) に固定して処理
	BitBlt(hWrkDC, 0, 0, vw, vh, hdc, x, y, SRCCOPY);
	TransparentBlt(hWrkDC, 0, 0, vw, vh, hReverseDC, 0, 0, vw, vh, TRANS_COLOR);
	AlphaBlend(hdc, x, y, vw, vh, hWrkDC, 0, 0, vw, vh, blendFunc);
}