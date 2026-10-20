#include "Wininit.h"

//メイン(Windowsプログラムのメイン)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int)		//hInstance:ｲﾝｽﾀﾝｽ番号(ｳｨﾝﾄﾞｳの番号) その他不使用
{
	HWND		hWnd;		//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ
	MSG			msg;		//ｳｨﾝﾄﾞｳﾒｯｾｰｼﾞ

	class WinInit winInit; //ウィンドウ作成関数(class なくてもいいが、構造体と区別ができないため記載)

	//ウィンドウの作成
	if (FAILED(winInit.MakeWnd(&hWnd, hInstance))) return 0; // FAILED:失敗したとき SUCCEEDED:成功したとき

	//メッセージループ(ｳｨﾝﾄﾞｳが閉じられるまで無限ﾙｰﾌﾟ)
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		//ウィンドウ関数にメッセージを送る
		DispatchMessage(&msg);
	}
	return 0;	//ﾌﾟﾛｸﾞﾗﾑの終了をWindowsに伝える
}