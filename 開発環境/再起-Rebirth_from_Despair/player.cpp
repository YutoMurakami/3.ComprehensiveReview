#include "player.h"

// プレイヤーアニメーション定義
static const Animimage playerAnim[] = {
	// row, frameCount, frameW, frameH, loop
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

// コントラクタ
Player::Player()
{
	ReadBmp("PlayerAnime.bmp");
	w = 100;	// 画像幅（Imageクラスのメンバ）
	h = 100;	// 画像高さ（Imageクラスのメンバ）
}

// デストラクタ
Player::~Player()
{
}

// 初期化関数
void Player::Init()
{
	switch (mode)
	{
	case Mode::Title:				// ゲームタイトル
		offsetX = shakeX = 0;
		x = CHIPSIZE;
		y = GROUND = normalG;
		gx = 0;						// 画像位置x座標(使用しない)
		gy = 0;						// 画像位置y座標
		isJump = isFall = isAttack = staminaLocked = isCharging = moveInput = useScroll = false;	// 各状態フラグ
		accel = 0;
		mutekiTime = 0;				// 無敵時間
		attackCooldown = 0;			// 攻撃無効時間
		gunCooldown = 0;			// 銃クールタイム
		dir = Direction::Right;		// 初期向き
		state = AnimState::Idle;	// 初期立ち絵
		HP = 100;					// 体力性
		maxStamina = 100;			// 現状100 shopで上限解放(200)
		stamina = maxStamina;		// スタミナ
		break;
	case Mode::Tutorial:
		offsetX = shakeX = 0;
		x = CHIPSIZE;
		y = GROUND;
		gx = 0;						// 画像位置x座標(使用しない)
		gy = 0;						// 画像位置y座標
		isJump = isFall = isAttack = staminaLocked = isCharging = moveInput = false;	// 各状態フラグ
		accel = 0;
		mutekiTime = 0;						// 無敵時間
		attackCooldown = 0;					// 攻撃無効時間
		dir = Direction::Right;				// 初期向き
		state = AnimState::Idle;			// 初期立ち絵
		HP = 100;							// 体力性
		maxStamina = 100;					// 現状100 shopで上限解放(200)
		stamina = maxStamina;				// スタミナ
		fullStageW = 2560;					// 2560の画像
		stageLimit = fullStageW - WND_W;
		stageMaxX = fullStageW - w;
		useScroll = true;
		break;
	case Mode::Stage1:
		offsetX = shakeX = 0;
		x = CHIPSIZE;
		y = GROUND;
		gx = 0;						// 画像位置x座標(使用しない)
		gy = 0;						// 画像位置y座標
		isJump = isFall = isAttack = staminaLocked = isCharging = moveInput = false;	// 各状態フラグ
		accel = 0;
		mutekiTime = 0;						// 無敵時間
		attackCooldown = 0;					// 攻撃無効時間
		dir = Direction::Right;				// 初期向き
		state = AnimState::Idle;			// 初期立ち絵
		HP = 100;							// 体力性
		maxStamina = 100;					// 現状100 shopで上限解放(200)
		stamina = maxStamina;				// スタミナ
		fullStageW = 2560;		// 2560の画像
		stageLimit = fullStageW - WND_W;
		stageMaxX = fullStageW - w;
		useScroll = true;
		break;
	case Mode::Stage2:
		offsetX = shakeX = 0;
		x = CHIPSIZE;
		y = GROUND;
		gx = 0;						// 画像位置x座標(使用しない)
		gy = 0;						// 画像位置y座標
		dir = Direction::Right;			// 初期向き
		state = AnimState::Idle;		// 初期立ち絵
		fullStageW = 2560;				// 2560の画像
		stageLimit = fullStageW - WND_W;
		stageMaxX = fullStageW - w;
		useScroll = true;
		break;
	case Mode::Stage3:
		offsetX = shakeX = 0;
		x = CHIPSIZE;
		y = GROUND;
		gx = 0;						// 画像位置x座標(使用しない)
		gy = 0;						// 画像位置y座標
		isJump = isFall = isAttack = staminaLocked = isCharging = moveInput = false;	// 各状態フラグ
		dir = Direction::Right;				// 初期向き
		state = AnimState::Idle;			// 初期立ち絵
		fullStageW = 3840;					// 3840の画像
		stageLimit = fullStageW - WND_W;
		stageMaxX = fullStageW - w;
		useScroll = true;
		break;
	case Mode::SecretStage:
		offsetX = shakeX = 0;
		x = CHIPSIZE;
		y = GROUND = secretG;
		gx = 0;						// 画像位置x座標(使用しない)
		gy = 0;						// 画像位置y座標
		isJump = isFall = isAttack = staminaLocked = isCharging = moveInput = false;	// 各状態フラグ
		dir = Direction::Right;				// 初期向き
		state = AnimState::Idle;			// 初期立ち絵
		fullStageW = 5120;					// 5120の画像
		stageLimit = fullStageW - WND_W;
		stageMaxX = fullStageW - w;
		useScroll = true;
		break;
	case Mode::Boss:
		offsetX = shakeX = 0;
		x = CHIPSIZE;
		y = GROUND = specialG;
		gx = 0;						// 画像位置x座標(使用しない)
		gy = 0;						// 画像位置y座標
		isJump = isFall = isAttack = staminaLocked = isCharging = moveInput = useScroll = isDodging = false;	// 各状態フラグ
		accel = 0;					// 重力加速度リセット
		mutekiTime = 0;				// 無敵時間
		attackCooldown = 0;			// 攻撃無効時間
		gunCooldown = 0;			// 銃クールタイム
		spinCooldown = 0;			// 回転斬りクールタイム
		knockbackVX = 0;			// ノックバック速度リセット（SecretStage の余韻を消す）
		knockbackTimer = 0;			// ノックバックタイマーリセット
		combo = 0;					// コンボリセット
		comboTimer = 0;				// コンボタイマーリセット
		chargeTimer = 0;			// 溜め時間リセット
		dir = Direction::Right;		// 初期向き
		state = AnimState::Idle;	// 初期立ち絵
		stageMaxX = WND_W - w;
		stageLimit = 0;
		break;
	}
}

void Player::Ctrl(const Key& key, Stage& stage)
{
	// --- 初期化等 ---
	// 移動前の座標を退避
	int oldx = offsetX, oldy = y;

	// 落とし穴ゾーン判定（SecretStage のみ: このゾーンでは地面クランプをしない）
	bool inHoleZone = (mode == Mode::SecretStage &&
		((offsetX >= 1240 && offsetX <= 1455) || (offsetX >= 2940 && offsetX <= 3050)));

	moveInput = false;	// 移動有無の初期化

	// 無敵時間の減少
	if (mutekiTime > 0) mutekiTime--;
	// 攻撃無効時間
	if (attackCooldown > 0) attackCooldown--;
	// 銃クールタイム
	if (gunCooldown > 0) gunCooldown--;
	// 回転斬りクールタイム
	if (spinCooldown > 0) spinCooldown--;

	// --- 操作内容 ---

	// 横移動（アナログスティック or 十字キー）
	//（ChargeHold 中は完全禁止。それ以外は攻撃中でもジャンプ・落下中は可）
	bool canMove = state != AnimState::ChargeHold && (!IsLockState() || isJump || isFall);
	if (canMove) {
		// 右移動: Dキー or 十字キー右 or スティック右
		if (key.Down(VK_RIGHT)
			|| key.Down('D')
			|| key.POVDown(9000)
			|| key.GetLX() > 500) {
			moveInput = true;
			dir = Direction::Right;
			// ジャンプ中や他アニメーション中でない時
			if (!isJump && !isFall && !IsLockState()) {
				state = AnimState::Walk;	// 地上でのみ歩きアニメ
			}
			// 画面スクロール
			if (useScroll) {
				offsetX += currentSpeed;
				if (offsetX > stageMaxX) offsetX = stageMaxX;
			}
			else {
				x += currentSpeed;
				if (x > WND_W - w) x = WND_W - w;
			}
		}

		// 左移動: Aキー or 十字キー左 or スティック左
		if (key.Down(VK_LEFT)
			|| key.Down('A')
			|| key.POVDown(27000)
			|| key.GetLX() < -500) {
			moveInput = true;
			dir = Direction::Left;
			if (!isJump && !isFall && !IsLockState()) {
				state = AnimState::Walk;	// 地上でのみ歩きアニメ
			}
			// スクロール有無で移動範囲
			if (useScroll) {
				offsetX -= currentSpeed;
				if (offsetX < 0) offsetX = 0;
			}
			else {
				x -= currentSpeed;
				if (x < 0) x = 0;
			}
		}
	}

	// シフトキー/LBボタン で走る（方向キーが押されている時のみダッシュ）
	if ((key.Down(VK_SHIFT) || key.JoyDown(4)) && moveInput && !staminaLocked && UseStamina(0.25f)) {
		state = AnimState::Run;
		currentSpeed = RUNSPEED;
		stamina -= 0.25f;
	}
	else currentSpeed = WALKSPEED;

	// ジャンプキー（回避中は禁止）// Aボタン or POV上 が「押された瞬間」のみ
	if (!isJump && !isFall && !isDodging) {
		if (key.Pressed(VK_SPACE)
			|| key.Pressed(VK_UP)
			|| key.Pressed('W')
			|| key.JoyPressed(0)
			|| key.POVPressed(0)) {
			isJump = true;
			// 走りジャンプ: SHIFT＋方向キーで跳躍中なら初速を上げる（滞空時間↑→横距離↑）
			if (currentSpeed == RUNSPEED && moveInput)
				accel = -6;
			else
				accel = 0;
		}
	}

	// 緊急回避用（RBボタン: 5番）
	// （溜め中でも優先して発動・溜めをキャンセル）
	if (!isDodging && (key.Pressed('Q')
		|| key.JoyPressed(5)) && !staminaLocked && UseStamina(30)) {
		// 溜め中なら強制キャンセル（放つアニメには移行しない）
		if (isCharging) {
			isCharging = false;
			shakeX = 0;
			chargeTimer = 0;
		}
		isDodging = true;
		state = AnimState::AheadDodge;
		animFrame = animTimer = 0;
		dodgeRemain = 200;
		step = 40;
		stamina -= 30;
		seRequest = AnimSE::Dodge;
	}

	// 攻撃（Xボタン: 2番）
	if (((key.Pressed('B')) || key.JoyPressed(2)) && attackCooldown == 0) {

		canCombo = false;

		// Idle/Walk/Runなどの時はいつでも開始OK
		if (!IsLockState()) canCombo = true;
		// Attack1か2の最中なら、アニメが終わっている時だけ次へ
		else if ((state == AnimState::Attack1 || state == AnimState::Attack2) && IsAnimFinished()) canCombo = true;
		// unlockAttack3 の有無でコンボの最大段数を決定
		int maxCombo = 2;
		if (unlockAttack3)
		{
			maxCombo = 3;
		}

		if (canCombo && combo < maxCombo)
		{
			// コンボ遷移
			if (combo == 0)
			{
				state = AnimState::Attack1;
				combo = 1;
			}
			else if (combo == 1)
			{
				state = AnimState::Attack2;
				combo = 2;
			}
			else if (combo == 2)
			{
				// ここに来るのは maxCombo==3 (unlockAttack3==true) の時だけ
				state = AnimState::Attack3;
				combo = 3;
			}

			animFrame = 0;
			animTimer = 0;
			comboTimer = 0;
		}
	}

	// 溜め攻撃斬りキー（Xボタン: 2番）
	if ((key.Down('V') || key.JoyDown(3)) && attackCooldown == 0) {
		if (!isCharging && !IsLockState()) {
			isCharging = true;
			state = AnimState::ChargeHold;
			animFrame = animTimer = 0;
		}
		if (state == AnimState::ChargeHold) {
			// 描画用のオフセットに値を入れる
			shakeX = (rand() % 3 - 1);
			// 溜め時間を毎フレーム加算（60fps 想定）
			chargeTimer += 1.0f / 60.0f;
		}
	}
	else {
		// 溜めていない時は 0 に戻す
		if (state != AnimState::ChargeHold) shakeX = 0;
	}

	// 離した瞬間（ボタンを離した瞬間）
	if (isCharging && (key.Released('V') || key.JoyReleased(3))) {
		isCharging = false;
		state = AnimState::ChargeRelease;
		animFrame = animTimer = 0;
		attackCooldown = 45;
	}

	if (!(IsLockState() && !IsAnimFinished())) {
		// 必殺技/回転斬り（Bボタン: 1番）
		if (!IsLockState() || IsAnimFinished()) {
			if ((key.Pressed('R') || key.JoyPressed(1)) && unlockSpin && !staminaLocked && spinCooldown == 0 && UseStamina(20)) {
				state = AnimState::Spin;
				animFrame = animTimer = 0;
				stamina -= 20;
			}
			// 銃発射（RTトリガー: 通常は lZ がマイナス）
			if ((key.Pressed('G') || key.RTPressed()) && gunCooldown == 0) {
				state = AnimState::Gun;
				animFrame = animTimer = 0;
				gunCooldown = 120;
			}
		}
	}

	// 攻撃フラグの設定
	isAttack = (state == AnimState::Attack1 || state == AnimState::Attack2 ||
		state == AnimState::Attack3 || state == AnimState::Spin ||
		state == AnimState::ChargeRelease);

	// 立ち絵に戻る
	if (IsLockState() && IsAnimFinished()) {
		// 溜め(Hold)以外はアニメが終わったらIdleへ
		if (state != AnimState::ChargeHold) {
			if (state == AnimState::Attack3) attackCooldown = 15;
			if (state == AnimState::ChargeRelease) chargeTimer = 0;
			state = AnimState::Idle;
			animFrame = 0;
			animTimer = 0;
		}
	}

	// --- 実際の動作内容 ---

		// 緊急回避
	if (isDodging) {
		Dodge = min(step, dodgeRemain);

		if (dir == Direction::Right) {
			if (useScroll) {
				offsetX += Dodge;
				if (offsetX > stageMaxX) offsetX = stageMaxX;
			}
			else {
				x += Dodge;
				if (x > WND_W - w) x = WND_W - w;
			}
		}
		else {
			if (useScroll) {
				offsetX -= Dodge;
				if (offsetX < 0) offsetX = 0;
			}
			else {
				x -= Dodge;
				if (x < 0) x = 0;
			}
		}

		dodgeRemain -= Dodge;

		step = max(step - 6, 6); // 急減速

		if (dodgeRemain <= 0) {
			isDodging = false;
			state = AnimState::Idle;
		}
	}

	// ノックバック
	if (knockbackTimer > 0) {
		if (useScroll) {
			offsetX += knockbackVX;
			if (offsetX < 0) offsetX = 0;
			if (offsetX > stageMaxX) offsetX = stageMaxX;
		}
		else {
			x += knockbackVX;
			if (x < 0) x = 0;
			if (x > WND_W - w) x = WND_W - w;
		}
		knockbackTimer--;
	}

	// ジャンプ処理
	if (isJump) {
		// 鉛直投げ上げ:y = v0t - 1/2gt^2
		y -= JUMP_POWER - accel;
		accel += GRAVITY;			// 重力を加算

		if (y >= GROUND && !inHoleZone) {	// 地面（穴ゾーンでは落下させる）
			y = GROUND;
			isJump = false;
			isFall = false;
			accel = 0;
		}
	}

	// 降りる処理(地面の上じゃなくて、ジャンプ中でない時)
	if (y < GROUND && !isJump) {
		isFall = true;	// 落下中にする
		accel = 0;		// 加速値初期化
		accel += GRAVITY;

		if (y >= GROUND && !inHoleZone) {	// 地面より下か？（穴ゾーンでは落下させる）
			y = GROUND;		// 地面に合わせる
			accel = 0;		// 加速値初期化
			isFall = false;	// 落下解除
		}
	}

	// 穴ゾーンで地面に立っている時、強制落下させる（回避中は落ちない）
	if (inHoleZone && !isJump && !isDodging && y >= GROUND) {
		isFall = true;
		y += accel;
		accel += GRAVITY;
	}

	// --- データの更新 ---

		// 回転斬りの後、アニメーションが終わったらIdleに戻す
	if (state == AnimState::Spin && IsAnimFinished()) {
		state = AnimState::Idle;
		animFrame = 0;
		animTimer = 0;
		spinCooldown = 45;	// 約0.75秒のクールタイム
	}

	// 操作がなければIdleに戻す
	if (!IsLockState() && !moveInput && !isJump && !isFall) {
		state = AnimState::Idle;
	}

	// --- スタミナ回復 ---
	usingStamina = (((key.Down(VK_SHIFT) || key.JoyDown(4)) && moveInput) || isDodging || state == AnimState::Spin);
	// スタミナが実質枯渇（ダッシュすら使えない0.25未満）でロックを設定
	if (stamina < 0.25f && !staminaLocked) {
		stamina = 0.0f;
		staminaLocked = true;
		staminaRecoverWait = 60;	// 1秒待機
	}
	if (staminaLocked) {
		if (staminaRecoverWait > 0) {
			staminaRecoverWait--;
		}
		else {
			// 固定レートで回復（Max100なら1秒、Max200なら2秒）
			stamina += 100.0f / 60.0f;
			if (stamina >= maxStamina) {
				stamina = maxStamina;
				staminaLocked = false;
			}
		}
	}
	else {
		if (!usingStamina) stamina += 0.2f;
	}
	if (stamina < 0.0f) stamina = 0.0f;
	if (stamina > maxStamina) stamina = maxStamina;

	// コンボ受付タイマーの更新（攻撃中でない時にカウント）攻撃カウント
	if (combo > 0) {
		// 攻撃アニメが完全に終わってIdleに戻るまでの猶予をカウント
		comboTimer++;

		// 攻撃が終わってIdleに戻った後、一定時間（60フレーム）経ったらコンボリセット
		if (state == AnimState::Idle && comboTimer > 60) combo = 0;
		// 念のため：攻撃アニメ中に止まってしまった場合の保険（1.5秒）
		if (comboTimer > 90) {
			combo = 0;
			if (IsLockState()) state = AnimState::Idle;
		}
	}
	// アニメーション更新
	UpdateAnimation();

	// カメラの位置
	if (useScroll) {
		targetCameraX = offsetX - CAMERA_CENTER_X;

		// ステージが画面サイズ以下の時は強制的にカメラを0にする
		if (targetCameraX < 0) cameraX = 0;
		else if (targetCameraX > stageLimit) cameraX = stageLimit;
		else cameraX = targetCameraX;

		x = offsetX - cameraX;
		stage.SetCameraX(cameraX);
	}
	else
	{
		cameraX = 0;
		stage.SetCameraX(0);
	}
	if (offsetX > stageMaxX) offsetX = stageMaxX;

	if (useScroll && offsetX >= stageMaxX)
	{
		// 現在のモードに応じて次のモードを要求
		Mode nextMode = mode;
		if (mode == Mode::Tutorial)          nextMode = Mode::Stage1;
		else if (mode == Mode::Stage1)       nextMode = Mode::Stage2;
		else if (mode == Mode::Stage2)       nextMode = Mode::Stage3;
		else if (mode == Mode::Stage3)       nextMode = Mode::BossTalk;
		else if (mode == Mode::SecretStage)  nextMode = Mode::BossTalk;
		stage.RequestMode(nextMode);
	}
}

// 表示関数
void Player::Draw()
{
	//// アニメーションごとに変更
	const Animimage& a = playerAnim[(int)state];
	int baseH = playerAnim[(int)state].frameH;
	drawY = y + h - baseH;

	// 表示位置の基本
	int drawX = x + shakeX;

	if (dir == Direction::Left && a.frameW > 100) {
		drawX -= (a.frameW - 100);
	}

	if (mutekiTime > 0)
	{
		if (dir == Direction::Right)
			AlphaBltWH(drawX, drawY, srcX, srcY, a.frameW, a.frameH, 96);
		else
			ReverseAlphaBltWH(drawX, drawY, srcX, srcY, a.frameW, a.frameH, 96);
	}
	else
	{
		if (dir == Direction::Right)
			TransBltWH(drawX, drawY, srcX, srcY, a.frameW, a.frameH);
		else
			ReverseBltWH(drawX, drawY, srcX, srcY, a.frameW, a.frameH);
	}
}

// SE要求
AnimSE Player::GetSERequest() const
{
	return seRequest;
}

// SE要求解除
void Player::ClearSERequest()
{
	seRequest = AnimSE::None;
}

// 状態を維持(アニメーションを再生しきるため)
bool Player::IsLockState()
{
	return state == AnimState::AheadDodge
		|| state == AnimState::Attack1
		|| state == AnimState::Attack2
		|| state == AnimState::Attack3
		|| state == AnimState::Spin
		|| state == AnimState::ChargeHold
		|| state == AnimState::ChargeRelease
		|| state == AnimState::Gun;
}

// アニメーション終了確認
bool Player::IsAnimFinished() const
{
	const Animimage& a = playerAnim[(int)state];
	return !a.loop && animFrame >= a.frameCount - 1;
}


void Player::ModeChange(Mode modeChange)
{
	mode = modeChange;
	Init();
}

// プレイヤーのアニメーション更新用
void Player::UpdateAnimation() {

	const Animimage& a = playerAnim[(int)state];

	// --- 音声 ---
	if (animFrame == 0 && animTimer == 0)
	{
		switch (state)
		{
			// 縦斬り1回目
		case AnimState::Attack1:
			seRequest = AnimSE::Attack1;
			break;
			// 横斬り2回目
		case AnimState::Attack2:
			seRequest = AnimSE::Attack2;
			break;
			// 縦斬り3回目
		case AnimState::Attack3:
			seRequest = AnimSE::Attack3;
			break;
			// 回転斬り
		case AnimState::Spin:
			seRequest = AnimSE::Spin;
			break;
			// 溜め
		case AnimState::ChargeHold:
			seRequest = AnimSE::ChargeHold;
			break;
			// 溜め攻撃
		case AnimState::ChargeRelease:
			seRequest = AnimSE::ChargeRelease;
			break;
			// 銃発射
		case AnimState::Gun:
			seRequest = AnimSE::Gun;
			break;
		default:
			break;
		}
	}

	if (a.loop) speed = 15;
	else if (state == AnimState::Gun) speed = 5;
	else speed = 10;
	animTimer++;

	// フレーム更新
	if (animTimer >= speed) {
		animTimer = 0;
		animFrame++;

		// 最終フレームに達した時の処理
		if (animFrame >= a.frameCount) {
			if (a.loop) animFrame = 0;
			// 非ループ(攻撃等)は最後のコマで止めておく
			else animFrame = a.frameCount - 1;
		}
	}

	// 表示する画像の切り出し位置(srcX)の決定 : 銃は決まった順番(gunSeq)でコマを表示する
	if (state == AnimState::Gun) srcX = gunSeq[animFrame] * 100;
	// 溜め解放は1コマ目(Hold)を飛ばして2コマ目から表示
	else if (state == AnimState::ChargeRelease) srcX = (animFrame + 1) * a.frameW;
	// 通常のアニメーション
	else srcX = animFrame * a.frameW;

	// Y座標の計算（100pxと125pxの切り替え）
	if (a.row <= 5) srcY = a.row * 100;
	else srcY = 600 + (a.row - 6) * 125;
}

// 歩き込み開始（画面外左から初期位置まで歩いてくる演出）
void Player::StartWalkIn()
{
	walkInTargetOffsetX = 0;	// 通常の初期offsetX
	walkInActive = true;
	offsetX = -300;				// 画面外左からスタート（300px外）
	x = offsetX;				// cameraX=0前提
	cameraX = 0;				// カメラをリセット
	state = AnimState::Walk;
	dir = Direction::Right;
	animFrame = 0;
	animTimer = 0;
}

// 歩き込み更新（毎フレーム呼び出し、完了したら true を返す）
bool Player::UpdateWalkIn()
{
	// 歩行アニメを再生
	state = AnimState::Walk;
	dir = Direction::Right;
	UpdateAnimation();

	// 移動（3px/frame → 100フレームで完了 ≒1.7秒）
	offsetX += 3;
	x = offsetX;	// cameraX=0のため

	if (offsetX >= walkInTargetOffsetX)
	{
		offsetX = walkInTargetOffsetX;
		x = offsetX;
		walkInActive = false;
		state = AnimState::Idle;
		return true;
	}
	return false;
}