#pragma once
#include "image.h"

// Bossクラス（プレイヤーのコピー）
class Boss : public Image
{
public:
	Boss();		// コンストラクタ
	~Boss();	// デストラクタ

	void Init(int startX, int startY);	// 初期化関数
	void Ctrl();						// AI制御関数
	void Draw();						// 表示関数
	AnimSE GetSERequest() const;		// SE要求
	void ClearSERequest();				// SE要求解除

	// === アクセサー ===
	inline Pos GetPosition() {
		Pos p = { x, y };
		return p;
	}

	inline Param GetParam() {
		Param param = {
			x + adjustW,
			y + adjustH,
			w - adjustW * 2,
			h - adjustH * 2
		};
		return param;
	}

	// HP取得
	inline int GetHP() { return HP; }
	// HP減少
	inline void DecHP(int damage, AnimState attackState)
	{
		if (mutekiTime > 0) return;

		HP -= damage;
		mutekiTime = 20;

		if (HP <= 0) isDead = true;

		switch (attackState)
		{
		case AnimState::Attack1:
			seRequest = AnimSE::HitAttack1;
			break;

		case AnimState::Attack2:
			seRequest = AnimSE::HitAttack2;
			break;

		case AnimState::Attack3:
			seRequest = AnimSE::HitAttack3;
			break;

		case AnimState::Spin:
		case AnimState::ChargeHold:
		case AnimState::ChargeRelease:
			seRequest = AnimSE::None;
			break;

		default:
			seRequest = AnimSE::Hit;
			break;
		}

		// ヒットフラッシュ開始
		hitFlashTimer = 6;

		for (int i = 0; i < MAX_SPARK; i++)
		{
			if (sparks[i].timer <= 0)
			{
				sparks[i].x = x + rand() % 20 - 10;
				sparks[i].y = y + (100 - h) + rand() % 20;
				sparks[i].vx = rand() % 7 - 3;
				sparks[i].vy = -(rand() % 4);
				sparks[i].timer = 12;
				break;
			}
		}

	}
	// 死亡フラグ取得
	inline bool GetIsDead() { return isDead; }
	// 向き取得
	inline Direction GetDirection() { return dir; }
	// 状態を取得
	inline AnimState GetState() const { return state; }
	// アタックの確認
	inline bool IsAttacking() const {
		return (state == AnimState::Attack1 || state == AnimState::Attack2 || state == AnimState::Attack3);
	}
	// アニメフレーム取得
	inline int GetAnim() const { return animFrame; }

	inline bool GetGunFiring() const { return (state == AnimState::Gun && animFrame == 3); }

	// プレイヤーの座標設定
	void SetPlayerPos(int px, int py) {
		playerX = px;
		playerY = py;
	}

	// ノックバック
	inline void ApplyKnockback(Direction attackDir) {
		const int distance = 50;
		if (attackDir == Direction::Right)
			x += distance;
		else
			x -= distance;
		if (x < 50)   x = 50;
		if (x > 1200) x = 1200;
	}

private:
	const int SPEED = 3;
	const int JUMP_POWER = 30;
	const int GRAVITY = 2;

	int x = 900, y = 550;			// 初期位置
	int playerX = 0, playerY = 0;	// プレイヤーの位置
	int HP = 300;					// 体力
	bool isDead = false;			// 死亡フラグ
	int mutekiTime = 0;				// 無敵時間

	Direction dir = Direction::Left;	// 向き
	AnimState state = AnimState::Idle;	// 状態
	int animFrame = 0, animTimer = 0;	// アニメーション用
	int srcX = 0, srcY = 0;			// 画像切り出し位置
	int adjustW = 15, adjustH = 15;		// サイズ調整用

	int aiTimer = 0;			// AI判断用タイマー
	int attackCooldown = 0;		// 攻撃クールタイム
	int gunCooldown = 0;		// 銃クールタイム
	int postDodgeTimer = 0;		// 回避後クールタイム（すぐに走り出さないように）

	// 敵ダメージ時
	int hitFlashTimer = 0;			// 白フラッシュタイマー
	static const int MAX_SPARK = 8;
	Spark sparks[MAX_SPARK];
	Image imgSpark;
	AnimSE seRequest = AnimSE::None;			// SEクラス

	void UpdateAI();		// AI更新
	void UpdateAnimation();	// アニメーション更新
};