#pragma once
#include "key.h"
#include "image.h"
#include "stage.h"

//プレイヤークラス(Image,DxInputクラスを継承)
class Player : public Image
{
public:

	Player();	//コンストラクタ
	~Player();	//デストラクタ

	void Init();								// 初期化関数
	void Ctrl(const Key& Key, Stage& stage);	// 操作関数(キーの追加)
	void Draw();								// 表示関数

	void StartWalkIn();        // 歩き込み開始（画面外左にセット）
	bool UpdateWalkIn();       // 毎フレーム呼び出し、完了したら true を返す

	AnimSE GetSERequest() const;		// SE要求
	void ClearSERequest();				// SE要求解除
	bool IsLockState();					// 状態を維持(アニメーションを再生しきるため
	bool IsAnimFinished() const;		// アニメーション終了確認
	void ModeChange(Mode modeChange);	// モードチェンジ

	// スタミナ消費状況
	bool UseStamina(float cost) { return stamina >= cost; }

	// 攻撃ダメージ量
	int GetCurrentDamage() {
		if (state == AnimState::Attack1) return 10;
		if (state == AnimState::Attack2) return 20;
		if (state == AnimState::Attack3) return 30;
		if (state == AnimState::Spin)    return 25;
		if (state == AnimState::Gun)     return 15;

		// Vキーの溜め攻撃
		if (state == AnimState::ChargeRelease) {
			if (chargeTimer >= 3.0f) return 70;
			if (chargeTimer >= 1.5f) return 30;
			return 10;
		}
		return 0;
	}

	// === アクセサー ===
	// ゲッター(値取得)
	// キャラクターの位置
	inline Pos GetPosition() {
		Pos p = { x,y };
		return p;
	}

	// キャラクターの位置(スクロール用)
	inline int GetOffsetX() { return offsetX; }

	// 余白
	inline Param GetParam()
	{
		Param param = {
			x + adjustW,
			y + adjustH,
			w - adjustW * 2,
			h - adjustH
		};
		return param;
	}

	// 無敵時間（Dodge中は常に無敵扱い）
	inline int GetMuteki() {
		if (isDodging) return 1;	// Dodge中はダメージ無効
		return (int)mutekiTime;
	}
	inline void SetMuteki() {
		mutekiTime = 120;	// 120フレームだから2秒間無敵状態
		HP--;				// HPを減らす
		// 溜め中ならダメージでキャンセル
		if (isCharging) {
			isCharging = false;
			shakeX = 0;
			chargeTimer = 0;
			state = AnimState::Idle;
			animFrame = animTimer = 0;
		}
	}

	// ノックバック設定
	inline void SetKnockback(Direction attackDir, int power = 25) {
		if (attackDir == Direction::Right)
			knockbackVX = power;
		else
			knockbackVX = -power;
		knockbackTimer = 8;
	}

	// HP取得
	inline int GetHP() { return HP; }
	// HP 減少処理
	inline void DecHP(int dec, AnimState attackState)
	{ 
		HP -= dec; 

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
	}
	// HP 回復処理（最大100まで）
	inline void HealHP(float amount)
	{
		HP += amount;
		if (HP > 100.0f) HP = 100.0f;
	}
	// スタミナ取得
	inline int GetStamina() { return stamina; }
	// MAXスタミナ取得
	inline int GetMaxStamina() { return maxStamina; }

	// Gunの取得 アニメーションで4フレーム目の時だけ弾を出す
	inline bool GetGunFiring() const { return (state == AnimState::Gun && animFrame == 3); }

	// 攻撃中の取得
	inline bool GetIsAttack() { return isAttack; }

	// アニメーションフレームの取得
	inline int GetAnim() { return animFrame; }

	// ダメージ量
	inline int GetDamage() { return damage; }

	// 向きを取得
	inline Direction GetDirection() { return dir; }

	// ジャンプ・落下状態の取得
	inline bool GetIsJump() const { return isJump; }
	inline bool GetIsFall() const { return isFall; }

	// 状態を取得
	inline AnimState GetState() const { return state; }

	// ショップ解放フラグのセッター
	inline void SetUnlockAttack3(bool v) { unlockAttack3 = v; }
	inline void SetUnlockSpin(bool v) { unlockSpin = v; }

	// スタミナ上限を外部から設定（Shop購入時）
	inline void SetMaxStamina(float v) {
		maxStamina = v;
		if (stamina > maxStamina) stamina = maxStamina;
	}

private:
	// キャラ設定
	const int JUMP_POWER = 30;			// ジャンプ力
	const int GRAVITY = 2;				// 重力
	const int WALKSPEED = 2;			// 歩く速度
	const int RUNSPEED = 5;				// 走る速度
	int currentSpeed = WALKSPEED;		// 移動スピード
	float HP = 100;						// 体力
	float stamina = 100;				// スタミナ
	float maxStamina = 100;				// 上限スタミナ
	int accel = 0;						// 加速度
	int damage = 0;						// 攻撃力

	// 回避用
	bool isDodging = false;				// 回避中フラグ
	int step = 40;						// 1フレームで進む量（調整用）
	int dodgeRemain = 200;				// 残り回避距離
	int Dodge = 0;						// 比較用

	// 攻撃用
	int speed = 10;						// アニメ再生用
	int combo = 0;						// コンボカウント
	int comboTimer = 0;					// コンボ受付タイマー
	int attackCooldown = 0;				// 攻撃無効時間用
	int gunCooldown = 0;				// 銃クールタイム
	int spinCooldown = 0;				// 回転斬りクールタイム
	bool canCombo = false;				// コンボ確認用
	bool isCharging = false;			// 溜め攻撃フラグ
	const int gunSeq[7] =				// Gunアニメ遷移用配列
	{ 0,1,2,3,4,1,0 };
	float chargeTimer = 0;				// 溜め時間用

	// 各種フラグ カウント等
	float mutekiTime = 0;				// 無敵時間
	int knockbackVX = 0;				// ノックバック速度
	int knockbackTimer = 0;				// ノックバック残りフレーム
	bool usingStamina = false;			// スタミナ使用するものをまとめたもの用
	bool staminaLocked = false;			// スタミナ枯渇ロック
	int staminaRecoverWait = 0;			// スタミナ回復待機タイマー（0になったら回復開始）
	bool isJump = false;				// ジャンプ中
	bool isFall = false;				// 落下中
	bool isAttack = false;				// 攻撃フラグ

	// アニメーション管理用
	AnimState state = AnimState::Idle;	// キャラの状態
	int animFrame = 0;					// アニメ再生用フレーム
	int animTimer = 0;					// アニメ再生間の時間
	int srcX = 0;						// 画像X軸上
	int srcY = 0;						// 画像Y軸上
	int drawY = 0;						// 画像Y軸(100or125)
	int x = CHIPSIZE, y = CHIPSIZE * 5;	// 表示位置(※x プレイヤーの画面内位置)
	int gx = 0, gy = 0;					// 画像位置
	int offsetX = 0;					// スクロール座標(※カメラ位置画面端)
	int shakeX = 0;						// プルプル演出用
	int finalDrawX = 0;					// 上記演出更新用
	int stageLimit = 0;					// 限界まで
	int stageMaxX = 0;					// ステージ右までプレイヤーが移動できる距離
	int GROUND = CHIPSIZE * 5;			// 地面の高さ
	float normalG = CHIPSIZE * 5;		// 高さ 通常
	float specialG = CHIPSIZE * 5.5f;	// 高さ 特殊
	float secretG = CHIPSIZE * 5.75f;	// 高さ シークレット用

	// 入力チェック
	bool moveInput = false;				// 移動キーが押下有無

	// カメラ・スクロール用
	static constexpr int CAMERA_CENTER_X = WND_W / 2;
	int cameraX = 0;
	int fullStageW = 0;
	int targetCameraX = 0;
	bool useScroll = false;

	// 画面等
	int adjustW = 15, adjustH = 15;		// サイズ調整用

	// Shop解放フラグ
	bool unlockAttack3 = false;		// B連撃(3段目)解放フラグ
	bool unlockSpin = false;		// R回転斬り解放フラグ

	// 歩き込み演出
	bool walkInActive = false;			// 歩き込み中フラグ
	int  walkInTargetOffsetX = 0;		// 歩き込み目標のワールドX座標

	// プレイヤーのアニメーション更新用
	void UpdateAnimation();

	// メンバ変数関連
	Mode mode = Mode::Nothing;			// ゲームモード
	Direction dir = Direction::Right;	// 向き
	AnimSE seRequest = AnimSE::None;	// SEクラス
};