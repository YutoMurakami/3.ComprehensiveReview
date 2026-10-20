#pragma once
#include "image.h"

//弾クラス(Imageクラスを継承)
class Shot : public Image
{
public:
	HWND hWnd;

	Shot();	//コンストラクタ
	~Shot();	//デストラクタ

	void Init(HDC hdc, int startX, int startY, Direction dir, bool isFloat);	// 初期化(エネミーorプレイヤー)
	void Ctrl();				// 操作関数
	void Draw();				// 表示関数

	// === アクセサー ===
	//セッター
	inline void SetPosition(Pos pos) { playerPos = pos; }
	// ゲッター(値取得)
	inline Pos GetPosition() {
		Pos p = { x,y };
		return p;
	}

	// 発射状況確認用
	inline bool GetIsShot() { return isShot; }
	// 弾を消す
	inline void SetIsShot(bool shot) { isShot = shot; }

	// あたり判定用のパラメータ取得（スクリーン座標で返す）
	inline Param GetParam() {
		Param param = { x - cameraX, y, w, h };
		return param;
	}

	// カメラX座標の設定
	inline void SetCameraX(int cx) { cameraX = cx; }

private:
	const int SIZE = 64;				// 画像サイズ（読み込み用）

	static const int FLOAT_BULLET_SIZE = 48;	// 浮遊弾サイズ
	static const int NORMAL_BULLET_SIZE = 16;	// 通常弾サイズ
	static const int BULLET_MARGIN = 100;		// 画面外判定マージン

	int x = 0, y = 0;			// ワールド座標
	int cameraX = 0;			// スクロール補正用
	float currentSpeed = 0.0f;	// 移動速度
	bool isShot = false;		// 発射中フラグ
	Pos playerPos = {};			// プレイヤーの位置
};
