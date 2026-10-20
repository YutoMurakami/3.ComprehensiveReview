#pragma once
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#define ERR_MSG(s,f) MessageBox(NULL, s, f, MB_OK)	//ｴﾗｰﾒｯｾｰｼﾞ

// DirectInputクラス
class DxInput
{
public:
	DxInput();
	~DxInput();

	// 初期化関数
	HRESULT InitDI(HWND hWnd);

	// 入力確認関数
	bool UpdateInputState(DIJOYSTATE2* js);
};