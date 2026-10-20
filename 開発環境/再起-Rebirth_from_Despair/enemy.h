#pragma once
#include "image.h"

//敵クラス(Imageクラスを継承)
class Enemy : public Image
{
public:
	Enemy();	//コンストラクタ
	~Enemy();	//デストラクタ

	void Init(int startX, int startY, Direction d);	// 初期化関数
	void Ctrl();	// 操作関数
	void Draw();	// 表示関数

	// チュートリアルモード設定（動かない）
	inline void SetTutorialMode(bool isTutorial) {
		this->isTutorialMode = isTutorial;
	}

	// パトロールモード設定（指定範囲を往復）
	inline void SetPatrolMode(int boundLeft, int boundRight) {
		isPatrolMode = true;
		patrolBoundLeft = boundLeft;
		patrolBoundRight = boundRight;
		state = EnemyState::Move;
	}

	// マリオモード設定（横移動のみ・攻撃なし・接触ダメージはGame側で処理）
	inline void SetMarioMode() { isMarioMode = true; }
	inline bool IsMarioMode() const { return isMarioMode; }

	AnimSE GetSERequest() const;		// SE要求
	void ClearSERequest();				// SE要求解除
	void Attack();			// 攻撃関数
	void UpdateAI();      // 状態判定
	void UpdateMove();    // 実移動
	void UpdateAnim();    // アニメ

	// === アクセサー ===
	// 位置の取得（スクリーン座標）
	inline Pos GetPosition()
	{
		Pos pos = { x - cameraX, y };
		return pos;
	}
	inline Param GetParam()
	{
		// 描画位置 drawY = y + (100 - h) に合わせて当たり判定も揃える
		int visualTop = y + (100 - h);
		Param param = {
			x - cameraX + adjustW,
			visualTop + adjustH,
			w - adjustW * 2,
			h - adjustH * 2
		};
		return param;
	}

	// 敵の死亡設定（完全消去）
	inline void SetIsDead()
	{
		isDead = true;
		x = y = -100;		// 画面外へ
	}

	// 死亡フラグ取得
	inline bool GetIsDead() { return isDead; }

	// 死亡アニメーション中かどうか
	inline bool IsDying() const { return state == EnemyState::Dead && !isDead; }

	// 死亡エフェクト描画位置（ワールドX / スクリーンY）
	inline int GetDeathX()    const { return deathX; }
	inline int GetDeathY()    const { return deathY; }
	inline int GetDyingAnim() const { return dyingAnim; }

	// HP取得
	inline int GetHP() { return hp; }
	// HP設定
	inline void SetHP(int newHP) { hp = newHP; }
	// HP減少処理
	inline void DecHP(int damage, AnimState attackState)
	{
		if (isDead || state == EnemyState::Dead) return;	// 二重処理防止
		hp -= damage;

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

		if (hp <= 0) {
			hp = 0;
			state = EnemyState::Dead;
			seRequest = AnimSE::EnemyDead;	// 死亡SE
			deathX = x;					// ワールドX（描画時にcameraXを引く）
			deathY = y + (100 - h);		// Enemy::Draw()のdrawYと同じ計算
			dyingAnim = 0;
			dyingTimer = 0;
		}
	}

	// ノックバック
	inline void ApplyKnockback(Direction attackDir, int distance = 50)
	{
		if (attackDir == Direction::Right)
			x += distance;
		else
			x -= distance;
	}

	// 弾発射状況確認等
	bool IsShotRequested() const { return shotRequest; }
	void ClearShotRequest() { shotRequest = false; }
	bool IsFloatShot() const { return (Etype == EnemyType::Float); }
	Direction GetDirection() const { return dir; }

	// 攻撃力取得
	inline int GetDamage() { return attackDamage; }

	// プレイヤーの座標
	void SetPlayerWorldX(int x) { playerWorldX = x; }
	void SetCameraX(int x) { cameraX = x; }

	// 攻撃中かどうかを取得
	inline bool IsAttacking() const { return (state == EnemyState::Attack); }

	// アニメーションフレームの取得
	inline int GetAnim() const { return anim; }

	// エネミー識別
	void SetType(EnemyType et);

	static const int MAX = 6;		// 障害物の数

private:
	const int SIZE = 48;			// 画像サイズ
	const int SPEED = 3;			// 速度
	int x = 0, y = 0;				// 表示位置
	int playerWorldX = 0;			// AI判断用
	int cameraX = 0;				// 描画補正用
	int anim = 0, animWait = 0;		// アニメーション用
	bool isDead = true;				// 死亡フラグ
	bool isTutorialMode = false;	// チュートリアルモード（動かない）
	int hp = 0;						// HP
	int attackDamage = 0;			// 攻撃力
	bool shotRequest = false;		// ショット判定
	int detectRange = 500;			// 検知範囲
	int keepDistance = 0;			// 保ちたい距離（0なら接近）
	int attackRange = 80;			// 攻撃可能距離
	int attackTimer = 0;			// アタック描画フレーム
	int attackInterval = 0;			// アタック描画フレーム
	int animMax = 0;				// エネミー用のアニメーション更新用
	int addX = 0;					// xの移動量
	int adjustW = 0, adjustH = 5;	// 敵サイズ調整用
	// 敵ダメージ時
	int hitFlashTimer = 0;			// 白フラッシュタイマー
	static const int MAX_SPARK = 8;
	Spark sparks[MAX_SPARK];
	Image imgSpark;
	// 死亡アニメーション
	int dyingAnim = 0;				// btleffect.bmp のコマ番号（0〜9）
	int dyingTimer = 0;				// コマ送りタイマー
	int deathX = 0;					// 死亡時ワールドX
	int deathY = 0;					// 死亡時スクリーンY
	// パトロールモード
	bool isPatrolMode = false;
	int  patrolBoundLeft = 0;
	int  patrolBoundRight = 99999;
	// マリオモード（攻撃なし・Idle アニメ固定・接触判定は Game 側）
	bool isMarioMode = false;

	EnemyState state = EnemyState::Idle;		// エネミー用アニメーション
	EnemyType Etype = EnemyType::Caterpillar;	// エネミータイプ
	AnimSE seRequest = AnimSE::None;			// SEクラス
	Direction dir = Direction::Left;			// 向き
};