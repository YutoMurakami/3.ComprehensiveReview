#include "shot.h"
#include <math.h>
#include <cmath>

Shot::Shot()
{
	w = h = SIZE;	// Imageクラスのメンバ
}

Shot::~Shot()
{
}

// 初期化関数（弾を発射するたびに呼ぶ）
void Shot::Init(HDC hdc, int startX, int startY, Direction dir, bool isFloat)
{
	this->hdc = hdc;
	x = startX;
	y = startY;
	isShot = true;

	// 対応する敵によって、表示する画像と弾サイズを変更する
	if (isFloat) {
		w = h = FLOAT_BULLET_SIZE;
		ReadBmp("FloatBullet.bmp");
	}
	else {
		w = h = NORMAL_BULLET_SIZE;
		ReadBmp("Bullet.bmp");
	}

	// 移動スピード
	if (dir == Direction::Right)
		currentSpeed = 8.0f;
	else
		currentSpeed = -8.0f;
}

// 操作関数
void Shot::Ctrl()
{
	if (isShot)
	{
		// 座標に加速させる
		x += (int)currentSpeed;

		// 画面外（スクリーン座標で判定）に出たら消去
		int screenX = x - cameraX;
		if (screenX < -BULLET_MARGIN || screenX > WND_W + BULLET_MARGIN) isShot = false;
	}
}

// 表示関数
void Shot::Draw()
{
	if (isShot) TransBltWH(x - cameraX, y, 0, 0, w, h);
}