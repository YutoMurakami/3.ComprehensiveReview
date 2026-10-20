#include "wininit.h"
#include "game.h"
#include "resource.h"

//コンストラクタ:初期処理用
WinInit::WinInit()
{
}

//デストラクタ:終了処理用
WinInit::~WinInit()
{
}

HDC hGlobalDC;	//hScreenDCの退避用

//スレッド関数
DWORD WINAPI ThreadFunc(LPVOID vdParam)
{
	//変数宣言
	GameWindow* gameWindow;	//ｹﾞｰﾑｳｨﾝﾄﾞｳ構造体
	DWORD frame = 0;		//ﾌﾚｰﾑ数
	DWORD beforeTime;		//前の時間

	//ｳｨﾝﾄﾞｳ関数から渡されたｹﾞｰﾑｳｨﾝﾄﾞｳ構造体を取得
	gameWindow = (GameWindow*)vdParam;

	// クラス宣言
	class Game game;	// ゲームクラス
	game.hdc = gameWindow->hScreenDC;	// 初期化前にhdcを受け渡す
	game.hWnd = gameWindow->hWnd;		// Shot用?
	game.Init();		// 初期化処理

	//時間の取得
	beforeTime = timeGetTime();

	//ゲームループ
	while (true)
	{
		DWORD nowTime;		//現在時間
		DWORD progress;		//経過時間
		DWORD idealTime;	//理想時間

		game.Ctrl();	// 操作処理
		game.Draw();	// 表示処理	(ゲームのデストロイは自動で処理される)

		nowTime = timeGetTime();		//現在の時間を取得
		progress = nowTime - beforeTime;//経過時間を算出
		//理想時間の計算(現在のﾌﾚｰﾑ×(1000÷FPS))
		idealTime = (DWORD)(frame * (1000.0f / gameWindow->dwFPS));

		//画面をコピー
		BitBlt(hGlobalDC, 0, 0, WND_W, WND_H,
			gameWindow->hScreenDC, 0, 0, SRCCOPY);

		//表示の時間待ち
		if (idealTime > progress)Sleep(idealTime - progress);

		//ウィンドウの再表示
		InvalidateRect(gameWindow->hWnd, NULL, FALSE);

		if (progress >= 1000)
		{
			//1秒を超えた時
			beforeTime = nowTime;	//現在の時間で更新
			frame = 0;	//ﾌﾚｰﾑ数初期化
		}
		frame++;	//ﾌﾚｰﾑ数ｶｳﾝﾄｱｯﾌﾟ
	}
	return TRUE;
}

//ウィンドウ関数(ゲームのプログラムを書く所)
LRESULT CALLBACK WindowProc(
	HWND	hWnd,	//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ
	UINT	uMsg,	//ﾒｯｾｰｼﾞ(ｷｰが押された等)
	WPARAM	wParam,	//ﾒｯｾｰｼﾞに対応した情報
	LPARAM	lParam)	//ﾒｯｾｰｼﾞに対応した情報
{
	//=== スレッド用変数 ===
	static GameWindow gameWindow;	//ｹﾞｰﾑｳｨﾝﾄﾞｳ構造体
	DWORD	dwID;	//ｽﾚｯﾄﾞID
	HRESULT hRlt;	//関数復帰値
	HBITMAP hBMP;	//ﾋﾞｯﾄﾏｯﾌﾟﾊﾝﾄﾞﾙ

	//変数宣言
	HDC				hdc;	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ(以下DC)。画面の事。
	PAINTSTRUCT		ps;		//描画構造体
	static bool fullScreen = false;	//ﾌﾙｽｸﾘｰﾝ

	//メッセージ(ｳｨﾝﾄﾞｳに何をしたか？)別の処理
	switch (uMsg)
	{
	case WM_CREATE:		//ｳｨﾝﾄﾞｳが作成される時
	{
		// タイトルバー・タスクバーのアイコン設定
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		HICON hIcon = LoadIcon(cs->hInstance, MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		//スレッドに渡す値を設定
		gameWindow.hWnd = hWnd;		//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ
		gameWindow.size.cx = WND_W;	//ｳｨﾝﾄﾞｳ幅
		gameWindow.size.cy = WND_H;	//ｳｨﾝﾄﾞｳ高さ
		gameWindow.dwFPS = 60;		//FPS値

		hdc = GetDC(hWnd);	//HDCを取得
		gameWindow.hScreenDC = CreateCompatibleDC(hdc);//HDCをｺﾋﾟｰ
		hBMP = CreateCompatibleBitmap(hdc, WND_W, WND_H);
		SelectObject(gameWindow.hScreenDC, hBMP);//BMPﾊﾝﾄﾞﾙを割り当て

		hGlobalDC = CreateCompatibleDC(hdc);
		HBITMAP hBMP2;
		hBMP2 = CreateCompatibleBitmap(hdc, WND_W, WND_H);
		SelectObject(hGlobalDC, hBMP2);

		ReleaseDC(hWnd, hdc);	//HDCを解放

		//スレッドの作成と実行
		CreateThread(
			NULL,
			0,
			ThreadFunc,	//ｽﾚｯﾄﾞ関数名
			(LPVOID)&gameWindow,	//ｽﾚｯﾄﾞに渡す構造体
			0,			//0:作成と同時に実行
			&dwID);		//(out)ｽﾚｯﾄﾞID

		return 0;	//WinMainに戻る
	}
	case WM_KEYDOWN:	//何かキーが押された時
		//キー別の処理
		switch (wParam)
		{
		case VK_F6:		//F6ｷｰ
			if (fullScreen == false) {
				fullScreen = FullScreenOn(hWnd);
			}
			else
			{
				FullScreenOff(hWnd);
				fullScreen = false;
			}
			break;
		}

		//画面を再描画(WM_PAINTを動かす)
		InvalidateRect(hWnd, NULL, FALSE);//FALSE:上書き
		return 0;	//WinMainに戻る

	case WM_ERASEBKGND:
		return 0;
		break;

	case WM_PAINT:		//表示する時(最初とｳｨﾝﾄﾞｳが更新された時)

		hdc = BeginPaint(hWnd, &ps);	//表示開始

		BitBlt(hdc, 0, 0, WND_W, WND_H,
			hGlobalDC,
			0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);			//表示終了
		return 0;

	case WM_DESTROY:	//ｳｨﾝﾄﾞｳが終了された時
		//ｱﾌﾟﾘｹｰｼｮﾝの終了をWindowsに伝える
		PostQuitMessage(0);
		return 0;	//WinMainに戻る
	}

	//メインに返す情報
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//ウィンドウ作成関数
HRESULT WinInit::MakeWnd(HWND* hWnd, HINSTANCE hInstance)
{
	WNDCLASS	wc;			//ｳｨﾝﾄﾞｳｸﾗｽ構造体

	DWORD	ProcessAffinityMask;//CPUを指定
	HANDLE	hProcess;			//ﾌﾟﾛｾｽﾊﾝﾄﾞﾙ
	DWORD	errnum;				//ｴﾗｰNo
	BOOL	rtn;				//関数復帰値

	//自身のプロセスのハンドルを取得
	hProcess = GetCurrentProcess();

	//自身を動作させるCPUを指定する
	ProcessAffinityMask = 1;	//CPU 0を使う(2進数で指定)
	rtn = SetProcessAffinityMask(hProcess, ProcessAffinityMask);
	if (rtn == FALSE) {
		errnum = GetLastError();	//ｴﾗｰ情報の取得
		return 0;
	}

	//ウィンドウクラス(作るウィンドウの情報)の登録
	wc.style = CS_HREDRAW | CS_VREDRAW;	//縦横のｻｲｽﾞが変更された時再描画
	wc.lpfnWndProc = WindowProc;		//ｳｨﾝﾄﾞｳ関数の登録
	wc.cbClsExtra = 0;			//使わない
	wc.cbWndExtra = 0;			//使わない
	wc.hInstance = hInstance;	//ｲﾝｽﾀﾝｽ番号
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));	//ｱｲｺﾝの登録
	wc.hCursor = NULL;			//ｶｰｿﾙの登録(ﾃﾞﾌｫﾙﾄを設定)
	wc.hbrBackground =
		(HBRUSH)COLOR_BACKGROUND + 1;//ｳｨﾝﾄﾞｳの背景色
	wc.lpszMenuName = NULL;			//使わない
	wc.lpszClassName = APPLI_NAME;	//ｱﾌﾟﾘｹｰｼｮﾝ名

	//ウィンドウクラスをWindowsに登録する
	if (RegisterClass(&wc) == false)
	{
		//登録できなかった時のエラーメッセージの表示
		ERR_MSG("ウィンドウクラスの登録に失敗", "エラーメッセージ");
		return E_FAIL;
	}

	//ウィンドウ領域の調整
	RECT	rect;		//矩形構造体
	DWORD	dwStyle;	//ｳｨﾝﾄﾞｳｽﾀｲﾙ
	rect.top = 0;	//上
	rect.left = 0;	//左
	rect.right = WND_W;	//右
	rect.bottom = WND_H;	//下
	dwStyle = WS_OVERLAPPEDWINDOW;	//普通のｳｨﾝﾄﾞｳ
	//(最大最小閉じるﾎﾞﾀﾝとﾀｲﾄﾙﾊﾞｰ付き)
	if (!AdjustWindowRect(
		&rect,	//(in)画面ｻｲｽﾞが入った矩形構造体
		//(out)計算結果
		dwStyle,//ｳｨﾝﾄﾞｳｽﾀｲﾙ
		NULL))	//ﾒﾆｭｰを持つかどうかの指定
	{
		ERR_MSG("ウィンドウ領域の調整に失敗", "エラーメッセージ");
		return E_FAIL;
	}

	//ウィンドウの作成
	*hWnd = CreateWindow(
		APPLI_NAME,				//ｱﾌﾟﾘｹｰｼｮﾝ名
		TITLE_NAME,				//ｳｨﾝﾄﾞｳのﾀｲﾄﾙ
		dwStyle,				//普通のｳｨﾝﾄﾞｳ
		100, 50,				//表示するXY座標
		rect.right - rect.left,	//ｳｨﾝﾄﾞｳの幅
		rect.bottom - rect.top,	//ｳｨﾝﾄﾞｳの高さ
		NULL,					//親ｳｨﾝﾄﾞｳのﾊﾝﾄﾞﾙ(これが親なので無し)
		NULL,					//ﾒﾆｭｰの設定(ﾒﾆｭｰは使用しない)
		hInstance,				//ｲﾝｽﾀﾝｽ番号
		NULL);					//使いません(NULLを入れておく)
	if (*hWnd == NULL)
	{
		ERR_MSG("ウィンドウの作成に失敗", "エラーメッセージ");
		return E_FAIL;
	}

	//ウィンドウを表示する
	ShowWindow(*hWnd, SW_SHOW);

	return S_OK;
}

//フルスクリーンON
bool FullScreenOn(HWND hWnd)
{
	//ウィンドウ種別を枠無しに変更
	SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);

	//デバイスモードを変更
	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);	//構造体のｻｲｽﾞ
	devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;//幅、高さの設定
	devMode.dmPelsWidth = WND_W;	//ｳｨﾝﾄﾞｳの幅
	devMode.dmPelsHeight = WND_H;	//ｳｨﾝﾄﾞｳの高さ

	//解像度の変更
	LONG lResult =
		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	if (lResult != DISP_CHANGE_SUCCESSFUL) {
		ERR_MSG("ディスプレイの設定を変更できません", "ERROR");
		return false;
	}

	//画面更新
	ShowWindow(hWnd, SW_SHOW);

	//ウィンドウの大きさを変更
	SetWindowPos(hWnd,
		HWND_TOPMOST,	//最前面のｳｨﾝﾄﾞｳにする
		0, 0,			//始点
		WND_W, WND_H,	//終点
		SWP_NOSIZE);	//ｵﾌﾟｼｮﾝ:ｳｨﾝﾄﾞｳの現在のｻｲｽﾞを維持

	return true;
}

//フルスクリーンOFF
void FullScreenOff(HWND hWnd)
{
	//ウィンドウ種別を戻す
	SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

	//解像度を戻す
	LONG lResult = ChangeDisplaySettings(NULL, 0);

	//画面更新
	ShowWindow(hWnd, SW_SHOW);

	//ウィンドウの大きさを変更
	SetWindowPos(hWnd,
		HWND_TOPMOST,	//最前面のｳｨﾝﾄﾞｳにする
		100, 50,		//始点
		WND_W, WND_H,		//終点
		SWP_SHOWWINDOW);	//ｵﾌﾟｼｮﾝ:ｳｨﾝﾄﾞｳを表示する
}