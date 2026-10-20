#pragma once
#include "global.h"
#include "dxInput.h"
#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")

class Key : public DxInput {
public:
	Key() = default;    // ｺﾝｽﾄﾗｸﾀ
	~Key() = default;   // ﾃﾞｽﾄﾗｸﾀ

	void Ctrl();	    // 操作関数

	// --- キーボード用 ---
	bool Pressed(int vk) const;     //押下時
	bool Released(int vk) const;    //離した時
	bool Down(int vk) const;        //長押し時

	// --- コントローラー用 ---
	bool JoyPressed(int btn) const;  // ボタンを押した瞬間（DI + XInput）
	bool JoyReleased(int btn) const; // ボタンを離した瞬間（DI + XInput）
	bool JoyDown(int btn) const;     // ボタン長押し（DI + XInput）
	bool POVPressed(DWORD dir) const; // 十字キーを押した瞬間（DI + XInput）
	bool POVDown(DWORD dir) const;    // 十字キー長押し（DI + XInput）

	// スティック閾値越え瞬間判定（メニュー操作用）
	bool StickUpPressed() const;    // 左スティック上へ入った瞬間
	bool StickDownPressed() const;  // 左スティック下へ入った瞬間
	bool StickRightPressed() const; // 左スティック右へ入った瞬間
	bool StickLeftPressed() const;  // 左スティック左へ入った瞬間

	// スティック・トリガーの状態取得
	long GetLX() const; // スティック左右（XInput 優先）
	long GetLZ() const { return js.lZ; } // RTトリガー(XBOXではLZ軸)
	DWORD GetPOV() const { return js.rgdwPOV[0]; } // 十字キーの状態（生値）
	// RTトリガーを押した瞬間（DirectInput OR XInput）
	bool RTPressed(long threshold = -500) const {
		if (js.lZ < threshold && prevJs.lZ >= threshold) return true;
		if (xConnected) {
			bool xNow  = xs.Gamepad.bRightTrigger > 128;
			bool xPrev = prevXs.Gamepad.bRightTrigger > 128;
			if (xNow && !xPrev) return true;
		}
		return false;
	}

private:
	// キーボード用
	bool now[256]{};        // 現在フレーム
	bool prev[256]{};       // 前フレーム

	// DirectInput コントローラー
	DIJOYSTATE2 js{};      // 今回のフレーム
	DIJOYSTATE2 prevJs{};  // 前のフレーム

	// XInput コントローラー（DirectInput の POV 同時押し問題を補う）
	XINPUT_STATE xs{};
	XINPUT_STATE prevXs{};
	bool xConnected = false;

	// DirectInput ボタンインデックス → XInput ボタンビット 対応表
	static const WORD xBtnMap[10];
};