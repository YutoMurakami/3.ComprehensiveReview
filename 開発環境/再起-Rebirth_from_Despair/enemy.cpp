#include "enemy.h"
#include <math.h>	// C
#include <cmath>	// C++

// コントラクタ
Enemy::Enemy()
{
	w = h = SIZE;	// Imageクラスのメンバ
}

// デストラクタ
Enemy::~Enemy()
{
}

// 初期化関数
void Enemy::Init(int startX, int startY, Direction d)
{
	x = startX;
	y = startY;
	dir = d;

	if (dir == Direction::Left)
		addX = -SPEED;
	else
		addX = SPEED;

	anim = animWait = 0;
	isDead = false;
	isTutorialMode = false;  // リセット
	isPatrolMode = false;    // リセット
	patrolBoundLeft = 0;
	patrolBoundRight = 99999;
	isMarioMode = false;     // リセット
	state = EnemyState::Idle;

	// ダメージ時の火花演出用
	imgSpark.ReadBmp("spark.bmp");
	imgSpark.w = 16;
	imgSpark.h = 16;

	for (int i = 0; i < MAX_SPARK; i++)
	{
		sparks[i].timer = 0;
	}
}

// 操作関数
void Enemy::Ctrl()
{
	if (isDead) return;

	// ヒット時の揺れタイマー
	if (hitFlashTimer > 0)
	{
		hitFlashTimer--;
	}
	// ヒット時の火花演出
	for (int i = 0; i < MAX_SPARK; i++)
	{
		if (sparks[i].timer > 0)
		{
			sparks[i].x += sparks[i].vx;
			sparks[i].y += sparks[i].vy;
			sparks[i].timer--;
		}
	}
	// 死亡アニメーション中（state=Dead, まだ完全消去前）
	if (state == EnemyState::Dead) {
		dyingTimer++;
		if (dyingTimer >= 5) {		// 5フレームごとにコマ送り（10コマ×5 = 50フレーム ≒0.8秒）
			dyingTimer = 0;
			dyingAnim++;
			if (dyingAnim >= 10) {
				SetIsDead();		// アニメ完了 → 完全消去
			}
		}
		return;		// AI・移動・通常アニメはすべてスキップ
	}

	// チュートリアルモードの場合は動かない
	if (!isTutorialMode) {
		UpdateAI();       // 状態を決める
		UpdateMove();     // 実際に動かす
	}
	UpdateAnim();     // 見た目更新
}

// 表示関数
void Enemy::Draw()
{
	if (isDead) return;
	if (state == EnemyState::Dead) return;	// 死亡エフェクトはGame::Draw()側で描画

	int srcX = w * anim;
	// プレイヤーと地面の高さを合わせる
	// プレイヤーのh=100に合わせて調整
	int drawY = y + (100 - h);

	// ヒットフラッシュ
	int shake = 0;
	if (hitFlashTimer > 0)
	{
		shake = rand() % 5 - 1;
	}

	if (dir == Direction::Right)
		TransBltWH(x - cameraX + shake, drawY, srcX, 0, w, h);
	else
		ReverseBltWH(x - cameraX + shake, drawY, srcX, 0, w, h);

	imgSpark.hdc = hdc;

	// 火花演出描画
	for (int i = 0; i < MAX_SPARK; i++)
	{
		if (sparks[i].timer > 0)
		{
			int sx = sparks[i].x - cameraX;
			int sy = sparks[i].y;
			imgSpark.TransBltWH(sx, sy, 0, 0, 16, 16);
		}
	}
}

// SE要求
AnimSE Enemy::GetSERequest() const
{
	return seRequest;
}

// SE要求解除
void Enemy::ClearSERequest()
{
	seRequest = AnimSE::None;
}

// 攻撃関数
void Enemy::Attack()
{
	switch (Etype) {
	case EnemyType::Caterpillar:
		// SE: gun.mp3
		seRequest = AnimSE::Gun;
		// Bullet生成
		shotRequest = true;
		break;

	case EnemyType::Float:
		// SE: EnemyAttack2.mp3
		seRequest = AnimSE::EnemyAttack2;
		// FloatBullet生成
		shotRequest = true;
		break;

	case EnemyType::Humanoid:
		// 近接攻撃判定
		// SE: EnemyAttack1.mp3
		seRequest = AnimSE::EnemyAttack1;
		break;
	}
	attackTimer = 180;  // 3秒のクールタイム（60フレーム/秒）
	state = EnemyState::Cooldown;
}

// 状態判定
void Enemy::UpdateAI()
{
	// プレイヤーとの距離
	int dist = abs(x - playerWorldX);

	// マリオモード: 横移動のみ・攻撃なし
	if (isMarioMode) {
		state = EnemyState::Move;
		return;
	}

	// パトロールモード以外は検知範囲外でIdle
	if (!isPatrolMode && dist > detectRange) {
		state = EnemyState::Idle;
		return;
	}

	// 攻撃後のクールタイム
	if (state == EnemyState::Cooldown) {
		if (--attackTimer <= 0) {
			state = EnemyState::Idle;
		}
		return;
	}

	// 攻撃時
	if (state == EnemyState::Attack) {
		if (Etype == EnemyType::Humanoid) {
			// フレーム4でSEを鳴らす（当たり判定と同タイミング）
			if (anim == 4 && animWait == 0) {
				seRequest = AnimSE::EnemyAttack1;
			}
			// 全フレーム再生完了後にCooldownへ
			if (anim >= animMax - 1) {
				attackTimer = 180;
				state = EnemyState::Cooldown;
			}
		}
		else {
			// Caterpillar/Float: 左から4枚目(index3)でショット発射
			if (anim == 3 && animWait == 0) {
				if (Etype == EnemyType::Caterpillar)
					seRequest = AnimSE::Gun;
				else
					seRequest = AnimSE::EnemyAttack2;
				shotRequest = true;
			}
			// アニメーション終了後にCooldownへ
			if (anim >= animMax - 1) {
				attackTimer = 180;
				state = EnemyState::Cooldown;
			}
		}
		return;
	}

	if (dist <= attackRange) {
		// 攻撃開始時にプレイヤーの方向を向く
		if (x < playerWorldX)
			dir = Direction::Right;
		else
			dir = Direction::Left;
		state = EnemyState::Attack;
		anim = animWait = 0;
		return;
	}
	state = EnemyState::Move;
}

// 実移動
void Enemy::UpdateMove()
{
	if (state != EnemyState::Move) return;

	// パトロールモード: 指定範囲を往復
	if (isPatrolMode) {
		if (dir == Direction::Right) {
			x += SPEED;
			if (x >= patrolBoundRight) { x = patrolBoundRight; dir = Direction::Left; }
		}
		else {
			x -= SPEED;
			if (x <= patrolBoundLeft) { x = patrolBoundLeft; dir = Direction::Right; }
		}
		return;
	}

	if (x < playerWorldX) {
		dir = Direction::Right;
		x += SPEED;
	}
	else {
		dir = Direction::Left;
		x -= SPEED;
	}
}

// アニメ
void Enemy::UpdateAnim()
{
	// Attack以外はすべて1枚目固定
	if (state != EnemyState::Attack) {
		anim = 0;
		animWait = 0;
		return;
	}
	// Attack中: 最後のフレームで止める
	animWait++;
	if (animWait > 15) {
		animWait = 0;
		if (anim < animMax - 1) {
			anim++;
		}
	}
}

// エネミー識別
void Enemy::SetType(EnemyType et)
{
	Etype = et;

	switch (Etype) {
	case EnemyType::Caterpillar:
		ReadBmp("EnemyCaterpillar.bmp");
		w = 48; h = 48;    // 画像サイズ
		animMax = 7;       // 仕様：7枚
		hp = 150;          // 仕様：150
		attackDamage = 5;  // 遠距離攻撃力
		attackRange = 300;
		detectRange = 500;
		break;
	case EnemyType::Float:
		ReadBmp("EnemyFloat.bmp");
		w = 48; h = 48;    // 画像サイズ
		animMax = 5;       // 仕様：5枚
		hp = 100;           // 仕様：100
		attackDamage = 5;  // 遠距離攻撃力
		attackRange = 400;
		detectRange = 600;
		break;
	case EnemyType::Humanoid:
		ReadBmp("EnemyHumanoid.bmp");
		w = 40; h = 48;    // 仕様：40x48
		animMax = 8;       // 仕様：8枚
		hp = 120;          // 仕様：120
		attackDamage = 15; // 近接攻撃力
		attackRange = 80;
		detectRange = 400;
		break;
	}
}