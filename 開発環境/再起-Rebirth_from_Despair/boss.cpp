#include "boss.h"

// プレイヤーアニメーション定義（同じものを使用）
static const Animimage bossAnim[] = {
	{ 0, 1, 100, 100, true  }, // Idle
	{ 1, 6, 100, 100, true  }, // Walk
	{ 2, 6, 100, 100, true  }, // Run
	{ 3, 5, 100, 100, false }, // BehindDodge(後方)
	{ 4, 5, 100, 100, false }, // AheadDodge(前方)
	{ 5, 7, 100, 100, false }, // Gun
	{ 6, 5, 125, 125, false }, // Attack1
	{ 7, 3, 125, 125, false }, // Attack2
	{ 6, 5, 125, 125, false }, // Attack3
	{ 8, 5, 125, 125, false }, // Spin
	{ 9, 1, 125, 125, false }, // ChargeHold
	{ 9, 2, 125, 125, false }, // ChargeRelease
};

Boss::Boss()
{
	ReadBmp("BossAnime.bmp");	// Boss用画像
	w = 100;	// Imageクラスのメンバ
	h = 100;	// Imageクラスのメンバ
}

Boss::~Boss()
{
}

void Boss::Init(int startX, int startY)
{
	x = startX;
	y = startY;

	HP = 300;
	isDead = false;
	mutekiTime = 0;

	state = AnimState::Idle;
	dir = Direction::Left;

	animFrame = 0;
	animTimer = 0;

	srcX = 0;
	srcY = 0;

	aiTimer = 0;
	attackCooldown = 0;
	gunCooldown = 0;
	postDodgeTimer = 0;
	hitFlashTimer = 0;

	seRequest = AnimSE::None;

	// ダメージ時の火花演出用
	imgSpark.ReadBmp("spark.bmp");
	imgSpark.w = 16;
	imgSpark.h = 16;

	for (int i = 0; i < MAX_SPARK; i++)
	{
		sparks[i].timer = 0;
	}
	imgSpark.hdc = hdc;
}

void Boss::Ctrl()
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

	// クールタイム減少
	if (attackCooldown > 0) attackCooldown--;
	if (gunCooldown > 0) gunCooldown--;
	if (mutekiTime > 0) mutekiTime--;

	UpdateAI();
	UpdateAnimation();
}

// SE要求
AnimSE Boss::GetSERequest() const
{
	return seRequest;
}

// SE要求解除
void Boss::ClearSERequest()
{
	seRequest = AnimSE::None;
}


void Boss::UpdateAI()
{
	aiTimer++;

	// 回避直後はクールタイム（Idle のまま待機してすぐに走り出さない）
	if (postDodgeTimer > 0) {
		postDodgeTimer--;
		return;
	}
	int dist = abs(x - playerX);	// プレイヤーとの距離
	bool phase2 = (HP <= 100);  // フェーズ2：HP3/1以下で攻撃的に変化
	bool nearWall = (x < 200 || x > 1050);  // 壁際判定（BehindDodge禁止ゾーン）
	// 壁際にいてかつプレイヤーに追い込まれている（壁と逆方向にプレイヤー）
	bool cornered = nearWall && dist <= 120 &&
		((x < 200 && playerX > x) || (x > 1050 && playerX < x));

	// 攻撃・回避アニメーション中は向きを固定（スプライトが途中で反転しないように）
	bool lockDir = (state == AnimState::Attack1 || state == AnimState::Attack2 ||
		state == AnimState::Attack3 || state == AnimState::Spin ||
		state == AnimState::Gun || state == AnimState::AheadDodge ||
		state == AnimState::BehindDodge);

	// dist > 50 の時のみ方向更新（近すぎる場合はチラつき防止のため固定）
	if (!lockDir && dist > 50) {
		if (x > playerX)
			dir = Direction::Left;
		else
			dir = Direction::Right;
	}

	// --- AI行動判断（Idle時に即時実行） ---
	if (state == AnimState::Idle) {
		// 追い込まれたら前方回避で脱出（フェーズ共通・低確率）
		if (cornered && rand() % 5 == 0) {
			if (x < playerX)
				dir = Direction::Right;
			else
				dir = Direction::Left;
			state = AnimState::AheadDodge;
			animFrame = animTimer = 0;
		}
		else if (phase2) {
			// ===== フェーズ2：より攻撃的 =====
			if (dist > 300) {
				// 遠距離：急接近ダッシュ
				state = AnimState::Run;
				animFrame = animTimer = 0;
			}
			else if (dist > 130) {
				// 中距離：攻撃 or ダッシュ
				if (attackCooldown == 0) {
					int action = rand() % 5;
					if (action == 0) { state = AnimState::Attack1; attackCooldown = 60; }
					else if (action == 1) { state = AnimState::Spin;    attackCooldown = 80; }
					else if (action == 2) { state = AnimState::Attack2; attackCooldown = 50; }
					else if (action == 3 && gunCooldown == 0) { state = AnimState::Gun; gunCooldown = 90; }
					else { state = AnimState::Run; }  // 接近して攻める
					animFrame = animTimer = 0;
				}
				else
					state = AnimState::Walk;  // クールタイム中は接近継続
			}
			else {
				// 至近距離：連続近接攻撃（壁際でも基本は戦う）
				if (attackCooldown == 0) {
					int action = rand() % 4;
					if (action == 0) { state = AnimState::Attack3; attackCooldown = 80; }
					else if (action == 1) { state = AnimState::Spin;    attackCooldown = 80; }
					else if (action == 2) { state = AnimState::Attack1; attackCooldown = 50; }
					else if (!nearWall) { state = AnimState::BehindDodge; }  // 回避して仕切り直し
					else { state = AnimState::Attack3; attackCooldown = 80; }  // 壁際は攻撃に変える
					animFrame = animTimer = 0;
				}
				else if (!nearWall) {
					// クールタイム中は後退して距離を取る
					state = AnimState::BehindDodge;
					animFrame = animTimer = 0;
				}
				// nearWall && attackCooldown > 0: Idle のまま壁際で待機
			}
		}
		else {
			// ===== フェーズ1：通常時 =====
			if (dist > 450) {
				// 遠距離：接近
				state = AnimState::Walk;
			}
			else if (dist > 200) {
				// 中遠距離：銃撃 or 接近
				if (gunCooldown == 0 && (rand() % 3 != 0)) {
					state = AnimState::Gun;
					animFrame = animTimer = 0;
					gunCooldown = 150;
				}
				else
					state = AnimState::Walk;
			}
			else {
				// 近距離：各種攻撃
				if (attackCooldown == 0) {
					int action = rand() % 5;
					if (action == 0) { state = AnimState::Attack1; attackCooldown = 80; }
					else if (action == 1) { state = AnimState::Attack2; attackCooldown = 60; }
					else if (action == 2) { state = AnimState::Spin;    attackCooldown = 120; }
					else if (action == 3 && !nearWall) { state = AnimState::BehindDodge; }
					else if (action == 3) { state = AnimState::Attack1; attackCooldown = 80; }  // 壁際は攻撃
					else {
						if (gunCooldown == 0) { state = AnimState::Gun; gunCooldown = 120; }
						else { state = AnimState::Walk; }
					}
					animFrame = animTimer = 0;
				}
				else {
					if (dist > 130) state = AnimState::Walk;  // 中距離はクールタイム中に接近
					// 近距離（dist <= 130）は待機（Idle横移動バグ防止）
				}
			}
		}
	}

	// --- 移動処理（毎フレーム実行） ---

	// 通常接近（Walk）
	if (state == AnimState::Walk) {
		if (dir == Direction::Right)
			x += SPEED;
		else
			x -= SPEED;
		if (dist <= 130) state = AnimState::Idle;  // 近づいたら停止して次の判断へ
	}

	// 急接近ダッシュ（Run：フェーズ2専用）
	if (state == AnimState::Run) {
		if (dir == Direction::Right)
			x += SPEED * 2;
		else
			x -= SPEED * 2;
		if (dist <= 130) state = AnimState::Idle;  // 到達 → 次フレームで攻撃判断
	}

	// 前方回避移動（追い込まれた時の脱出：dirの方向へ移動）
	if (state == AnimState::AheadDodge && animFrame < 4) {
		if (dir == Direction::Right)
			x += 10;
		else
			x -= 10;
		if (x < 50)   x = 50;
		if (x > 1200) x = 1200;
	}

	// 後方回避移動（プレイヤーから遠ざかる方向へ・壁に当たったら止まる）
	if (state == AnimState::BehindDodge && animFrame < 3) {
		int evadeDir;
		if (x < playerX)
			evadeDir = -1;  // プレイヤーの反対方向
		else
			evadeDir = 1;
		int newX = x + evadeDir * 15;
		// 壁に当たったら止まる（AheadDodgeには切り替えない）
		if (newX < 50)   newX = 50;
		if (newX > 1200) newX = 1200;
		x = newX;
	}

	// 画面端制限
	if (x < 50) x = 50;
	if (x > 1200) x = 1200;
}

void Boss::UpdateAnimation()
{
	const Animimage& a = bossAnim[(int)state];

	// アニメーション終了確認
	bool isFinished = (!a.loop && animFrame >= a.frameCount - 1);

	if (isFinished) {
		// 回避完了後はすぐに走り出さないようにクールタイムを設ける
		if (state == AnimState::AheadDodge || state == AnimState::BehindDodge) {
			postDodgeTimer = 40;
		}
		state = AnimState::Idle;
		animFrame = 0;
		animTimer = 0;
		return;
	}

	animTimer++;
	int speed;
	if (a.loop)
		speed = 15;
	else
		speed = 10;

	if (animTimer >= speed) {
		animTimer = 0;
		animFrame++;

		if (animFrame >= a.frameCount) {
			if (a.loop) animFrame = 0;
			else animFrame = a.frameCount - 1;
		}
	}

	// 表示する画像の切り出し位置
	srcX = animFrame * a.frameW;

	// Y座標の計算（100pxと125pxの切り替え）
	if (a.row <= 5) srcY = a.row * 100;
	else srcY = 600 + (a.row - 6) * 125;
}

void Boss::Draw()
{
	if (isDead) return;

	const Animimage& a = bossAnim[(int)state];
	int baseH = a.frameH;
	int drawY = y + h - baseH;
	int drawX = x;
	// ヒットフラッシュ
	int shake = 0;
	if (hitFlashTimer > 0)	shake = rand() % 5 - 1;

	if (dir == Direction::Left && a.frameW > 100) {
		drawX -= (a.frameW - 100);
	}

	// 無敵時間中は半透明
	if (mutekiTime > 0) {
		if (dir == Direction::Right)
			AlphaBltWH(drawX, drawY, srcX + shake, srcY, a.frameW, a.frameH, 96);
		else
			ReverseAlphaBltWH(drawX, drawY, srcX + shake, srcY, a.frameW, a.frameH, 96);
	}
	else {
		if (dir == Direction::Right)
			TransBltWH(drawX, drawY, srcX + shake, srcY, a.frameW, a.frameH);
		else
			ReverseBltWH(drawX, drawY, srcX + shake, srcY, a.frameW, a.frameH);
	}

	// 火花演出描画
	for (int i = 0; i < MAX_SPARK; i++)
	{
		if (sparks[i].timer > 0)
		{
			int sx = sparks[i].x;
			int sy = sparks[i].y;
			imgSpark.TransBltWH(sx, sy, 0, 0, 16, 16);
		}
	}
}