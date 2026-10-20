#include "key.h"

// DirectInput ボタンインデックス (0〜9) → XInput ボタンビット 対応表
// XBox コントローラーの DI ボタン順: A=0, B=1, X=2, Y=3, LB=4, RB=5, Back=6, Start=7, LS=8, RS=9
const WORD Key::xBtnMap[10] = {
	XINPUT_GAMEPAD_A,              // 0: A ボタン
	XINPUT_GAMEPAD_B,              // 1: B ボタン
	XINPUT_GAMEPAD_X,              // 2: X ボタン
	XINPUT_GAMEPAD_Y,              // 3: Y ボタン
	XINPUT_GAMEPAD_LEFT_SHOULDER,  // 4: LB
	XINPUT_GAMEPAD_RIGHT_SHOULDER, // 5: RB
	XINPUT_GAMEPAD_BACK,           // 6: Back / View
	XINPUT_GAMEPAD_START,          // 7: Start / Menu
	XINPUT_GAMEPAD_LEFT_THUMB,     // 8: 左スティック押し込み
	XINPUT_GAMEPAD_RIGHT_THUMB,    // 9: 右スティック押し込み
};

void Key::Ctrl()
{
	// --- キーボード状態の更新 ---
	for (int vk = 0; vk < 256; vk++) {
		prev[vk] = now[vk];
		now[vk] = (GetAsyncKeyState(vk) & 0x8000) != 0;
	}

	// --- DirectInput コントローラー状態の更新 ---
	prevJs = js;
	if (!UpdateInputState(&js)) {
		memset(&js, 0, sizeof(DIJOYSTATE2));
		js.rgdwPOV[0] = -1; // POVのデフォルトは-1
	}

	// --- XInput コントローラー状態の更新 ---
	prevXs = xs;
	xConnected = (XInputGetState(0, &xs) == ERROR_SUCCESS);
	if (!xConnected) ZeroMemory(&xs, sizeof(XINPUT_STATE));
}

// --- キーボード判定 ---
bool Key::Pressed(int vk) const { return now[vk] && !prev[vk]; }
bool Key::Released(int vk) const { return !now[vk] && prev[vk]; }
bool Key::Down(int vk) const { return now[vk]; }

// --- コントローラーボタン判定（DirectInput OR XInput）---

// ボタン長押し
bool Key::JoyDown(int btn) const {
	if (btn < 0 || btn >= 128) return false;
	if (js.rgbButtons[btn] & 0x80) return true;
	if (xConnected && btn < 10) return (xs.Gamepad.wButtons & xBtnMap[btn]) != 0;
	return false;
}

// ボタンを押した瞬間
bool Key::JoyPressed(int btn) const {
	if (btn < 0 || btn >= 128) return false;
	bool diNow = (js.rgbButtons[btn] & 0x80) != 0;
	bool diPrev = (prevJs.rgbButtons[btn] & 0x80) != 0;
	if (diNow && !diPrev) return true;
	if (xConnected && btn < 10) {
		bool xNow = (xs.Gamepad.wButtons & xBtnMap[btn]) != 0;
		bool xPrev = (prevXs.Gamepad.wButtons & xBtnMap[btn]) != 0;
		if (xNow && !xPrev) return true;
	}
	return false;
}

// ボタンを離した瞬間
bool Key::JoyReleased(int btn) const {
	if (btn < 0 || btn >= 128) return false;
	bool diNow = (js.rgbButtons[btn] & 0x80) != 0;
	bool diPrev = (prevJs.rgbButtons[btn] & 0x80) != 0;
	if (!diNow && diPrev) return true;
	if (xConnected && btn < 10) {
		bool xNow = (xs.Gamepad.wButtons & xBtnMap[btn]) != 0;
		bool xPrev = (prevXs.Gamepad.wButtons & xBtnMap[btn]) != 0;
		if (!xNow && xPrev) return true;
	}
	return false;
}

// --- 十字キー（POV / XInput D-pad）判定 ---

// XInput D-pad ビットを POV 角度から取得するヘルパー
static WORD DirToBit(DWORD dir)
{
	if (dir == 0)     return XINPUT_GAMEPAD_DPAD_UP;
	if (dir == 9000)  return XINPUT_GAMEPAD_DPAD_RIGHT;
	if (dir == 18000) return XINPUT_GAMEPAD_DPAD_DOWN;
	if (dir == 27000) return XINPUT_GAMEPAD_DPAD_LEFT;
	return 0;
}

// 十字キー長押し（DirectInput POV OR XInput D-pad）
bool Key::POVDown(DWORD dir) const {
	if (js.rgdwPOV[0] == dir) return true;
	if (xConnected) {
		WORD bit = DirToBit(dir);
		if (bit != 0 && (xs.Gamepad.wButtons & bit) != 0) return true;
	}
	return false;
}

// 十字キーを押した瞬間（DirectInput POV OR XInput D-pad）
bool Key::POVPressed(DWORD dir) const {
	bool diNow = (js.rgdwPOV[0] == dir);
	bool diPrev = (prevJs.rgdwPOV[0] == dir);
	if (diNow && !diPrev) return true;
	if (xConnected) {
		WORD bit = DirToBit(dir);
		if (bit != 0) {
			bool xNow = (xs.Gamepad.wButtons & bit) != 0;
			bool xPrev = (prevXs.Gamepad.wButtons & bit) != 0;
			if (xNow && !xPrev) return true;
		}
	}
	return false;
}

// --- スティック ---

// 左スティック X 軸（XInput 接続時は XInput 優先、デッドゾーン適用）
long Key::GetLX() const {
	if (xConnected) {
		SHORT lx = xs.Gamepad.sThumbLX;
		if (lx < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			lx > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) return 0;
		return (long)lx / 32; // -1024 〜 +1024 に正規化（DI の -1000〜+1000 と近い値）
	}
	return js.lX;
}

// --- スティック閾値越え瞬間判定（メニュー操作用）---
// DI: lY 上 = 負値、下 = 正値 / XInput: sThumbLY 上 = 正値、下 = 負値（逆）

bool Key::StickUpPressed() const {
	bool diNow = (js.lY < -500);
	bool diPrev = (prevJs.lY < -500);
	if (diNow && !diPrev) return true;
	if (xConnected) {
		bool xNow = (xs.Gamepad.sThumbLY > 16000);
		bool xPrev = (prevXs.Gamepad.sThumbLY > 16000);
		if (xNow && !xPrev) return true;
	}
	return false;
}

bool Key::StickDownPressed() const {
	bool diNow = (js.lY > 500);
	bool diPrev = (prevJs.lY > 500);
	if (diNow && !diPrev) return true;
	if (xConnected) {
		bool xNow = (xs.Gamepad.sThumbLY < -16000);
		bool xPrev = (prevXs.Gamepad.sThumbLY < -16000);
		if (xNow && !xPrev) return true;
	}
	return false;
}

bool Key::StickRightPressed() const {
	bool diNow = (js.lX > 500);
	bool diPrev = (prevJs.lX > 500);
	if (diNow && !diPrev) return true;
	if (xConnected) {
		bool xNow = (xs.Gamepad.sThumbLX > 16000);
		bool xPrev = (prevXs.Gamepad.sThumbLX > 16000);
		if (xNow && !xPrev) return true;
	}
	return false;
}

bool Key::StickLeftPressed() const {
	bool diNow = (js.lX < -500);
	bool diPrev = (prevJs.lX < -500);
	if (diNow && !diPrev) return true;
	if (xConnected) {
		bool xNow = (xs.Gamepad.sThumbLX < -16000);
		bool xPrev = (prevXs.Gamepad.sThumbLX < -16000);
		if (xNow && !xPrev) return true;
	}
	return false;
}