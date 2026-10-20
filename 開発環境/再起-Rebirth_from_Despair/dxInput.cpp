#include "dxInput.h"

LPDIRECTINPUT8			pDI = NULL;		// DxInputｵﾌﾞｼﾞｪｸﾄ
LPDIRECTINPUTDEVICE8	pPad = NULL;	// ﾃﾞﾊﾞｲｽ(ｺﾝﾄﾛｰﾗ)ｵﾌﾞｼﾞｪｸﾄ

// ｺﾝｽﾄﾗｸﾀ
DxInput::DxInput()
{
}

// ﾃﾞｽﾄﾗｸﾀ
DxInput::~DxInput()
{
	// ｵﾌﾞｼﾞｪｸﾄの解放
	if (pPad) { pPad->Release(); pPad = NULL; }
	if (pDI) { pDI->Release();  pDI = NULL; }
}

// ジョイスティック列挙関数
BOOL CALLBACK EnumJoysticksCallBack(
	const DIDEVICEINSTANCE* pdidInstance,	// ｺﾝﾄﾛｰﾗのｲﾝｽﾀﾝｽ
	VOID* pContext)
{
	HRESULT hRlt;		// 関数復帰値

	// ﾃﾞﾊﾞｲｽ(ｺﾝﾄﾛｰﾗ)の作成
	hRlt = pDI->CreateDevice(
		pdidInstance->guidInstance,		// ﾃﾞﾊﾞｲｽの番号
		&pPad,							// (out)作成されるﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄ(ｺﾝﾄﾛｰﾗ)
		NULL);

	if (hRlt != DI_OK)
	{
		return DIENUM_CONTINUE;		// 次のﾃﾞﾊﾞｲｽを要求
	}
	return DIENUM_STOP;				// 列挙停止
}

// ﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄ(ｺﾝﾄﾛｰﾗ)の列挙関数
BOOL CALLBACK EnumObjectsCallBack(
	const DIDEVICEOBJECTINSTANCE* pdidoi,	// ｺﾝﾄﾛｰﾗのｲﾝｽﾀﾝｽ
	VOID* pContext)
{
	// アナログスティックがあるか?
	if (pdidoi->dwType & DIDFT_AXIS)
	{
		// 入力情報の範囲を設定する
		DIPROPRANGE diprg;								// 範囲設定用構造体
		diprg.diph.dwSize = sizeof(DIPROPRANGE);		// 構造体のサイズ
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);	// ﾍｯﾀﾞｰ情報のｻｲｽﾞ
		diprg.diph.dwObj = pdidoi->dwType;				// 軸(ｵﾌﾞｼﾞｪｸﾄ)
		diprg.diph.dwHow = DIPH_BYID;					// dwObjに設定されるのが、ｵﾌﾞｼﾞｪｸﾄの種類かｲﾝｽﾀﾝｽ番号か?
		diprg.lMax = 1000;								// スティックの最大値(変更可)
		diprg.lMin = -1000;								// スティックの最小値(変更可)

		// 範囲設定	
		if (FAILED(pPad->SetProperty(
			DIPROP_RANGE,				// 範囲	
			&diprg.diph)))				// 範囲構造体
		{
			return DIENUM_STOP;			// 列挙停止
		}
	}
	return DIENUM_CONTINUE;				// 次のｵﾌﾞｼﾞｪｸﾄを要求
}

// 初期化
HRESULT DxInput::InitDI(HWND hWnd)
{
	HRESULT			hRlt;			// 関数復帰値
	LPDIRECTINPUT8	pDIo = NULL;	// DxInputｵﾌﾞｼﾞｪｸﾄ

	// DxInputｵﾌﾞｼﾞｪｸﾄの作成
	hRlt = DirectInput8Create(
		GetModuleHandle(NULL),	// DxInputを作成するｱﾌﾟﾘのﾊﾝﾄﾞﾙ
		DIRECTINPUT_VERSION,	// DxInputのﾊﾞｰｼﾞｮﾝ
		IID_IDirectInput8,		// UniCodeとAnsiの区別用
		(VOID**)&pDI,			// (out)DxInputｵﾌﾞｼﾞｪｸﾄ
		NULL);					// NULL固定
	if (hRlt != DI_OK) {
		ERR_MSG("DxInputの作成に失敗", "Error");
		return E_FAIL;
	}

	// 使えるｺﾝﾄﾛｰﾗを見つける(列挙する)
	hRlt = pDI->EnumDevices(
		DI8DEVCLASS_GAMECTRL,		// 全てのｺﾝﾄﾛｰﾗ
		EnumJoysticksCallBack,		// ｺﾝﾄﾛｰﾗの列挙
		NULL,						// ｺｰﾙﾊﾞｯｸ関数からの値(受け取らない)
		DIEDFL_ATTACHEDONLY);		// 繋がっている物のみ
	if (hRlt != DI_OK) {
		ERR_MSG("コントローラの確認に失敗", "Error");
	}

	// コントローラの接続確認
	if (pPad != NULL) {
		// コントローラ構造体のデータフォーマットを作成
		hRlt = pPad->SetDataFormat(&c_dfDIJoystick2);
		if (hRlt != DI_OK) {
			ERR_MSG("データフォーマットの作成失敗", "Error");
		}

		// (他のデバイスとの)協調レベルの設定
		hRlt = pPad->SetCooperativeLevel(
			hWnd,
			DISCL_EXCLUSIVE |		// 排他アクセス	
			DISCL_FOREGROUND);		// ﾌｫｱｸﾞﾗﾝﾄﾞｱｸｾｽ権(上記もしくはこちらが使用される)
		if (hRlt != DI_OK) {
			ERR_MSG("協調レベルの設定失敗", "Error");
		}

		// 使用可能なオブジェクト(ﾎﾞﾀﾝ、ｽﾃｨｯｸ等)の列挙
		hRlt = pPad->EnumObjects(
			EnumObjectsCallBack,	// ｵﾌﾞｼﾞｪｸﾄ列挙関数
			(VOID*)hWnd,			// ｺｰﾙﾊﾞｯｸ関数に送る情報
			DIDFT_ALL);				// 全てのｵﾌﾞｼﾞｪｸﾄ
		if (hRlt != DI_OK) {
			ERR_MSG("オブジェクトの列挙に失敗", "Error");
		}
	}
	return S_OK;
}

// 入力確認関数
bool DxInput::UpdateInputState(DIJOYSTATE2* js)
{
	HRESULT hRlt;		// 関数復帰値

	js->rgdwPOV[0] = -1;

	// コントローラの接続確認
	if (pPad == NULL) return false;

	// コントローラが変更されていないかを確認
	hRlt = pPad->Poll();
	if (hRlt != DI_OK && hRlt != DI_NOEFFECT)	//DI_NOEFFECT:確認不要
	{
		// コントローラのアクセス権を取得する
		hRlt = pPad->Acquire();
		while (hRlt == DIERR_INPUTLOST) {	// (見つかるまで繰り返し取得)
			hRlt = pPad->Acquire();
		}
		return true;
	}

	// コントローラの状態を取得
	hRlt = pPad->GetDeviceState(
		sizeof(DIJOYSTATE2),	// ﾃﾞﾊﾞｲｽ情報のｻｲｽﾞ
		js);					// (out)ﾃﾞﾊﾞｲｽ情報
	if (hRlt != DI_OK) return false;

	return true;
}