#pragma once
#include "global.h"

//マクロ定義
#define APPLI_NAME "再起 - Rebirth from Despair"	//ｱﾌﾟﾘｹｰｼｮﾝ名
#define TITLE_NAME "再起 - Rebirth from Despair"	//ｹﾞｰﾑﾀｲﾄﾙ

//ゲームウィンドウ構造体
typedef struct _GameWindow
{
	HWND	hWnd;		//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ
	SIZE	size;		//ｳｨﾝﾄﾞｳｻｲｽﾞ
	DWORD	dwFPS;		//FPS(Frame Per Second)値
	HDC		hScreenDC;	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ
}GameWindow;

// ウィンドウ作成クラス
class WinInit
{
public:	//アクセス指定子:public:他のクラス等から操作可能

	//コンストラクタ:初期処理用(最初だけ動作) 値返せない
	WinInit();
	//デストラクタ:終了処理用(クラスやゲームを削除終わる時に動作) 値返せない
	~WinInit();	// " ~ " チルダ(にょろにょろでも可)

	//ウィンドウ作成関数
	HRESULT MakeWnd(HWND* hWnd, HINSTANCE hInstance);
};

//フルスクリーンの切り替え
bool FullScreenOn(HWND hWnd);	//ON
void FullScreenOff(HWND hWnd);	//OFF