#include "game.h"

// チュートリアル用（動かない敵1体）
static const EnemySpawn tutorialEnemies[] =
{
	{ EnemyType::Humanoid, CHIPSIZE * 9, CHIPSIZE * 5, Direction::Left },
};

// ステージ1用（2体）
static const EnemySpawn stage1Enemies[] =
{
	{ EnemyType::Caterpillar, CHIPSIZE * 8,  CHIPSIZE * 5, Direction::Left },
	{ EnemyType::Humanoid,    CHIPSIZE * 20, CHIPSIZE * 5, Direction::Right },
};

// ステージ2用（3体）
static const EnemySpawn stage2Enemies[] =
{
	{ EnemyType::Float,       CHIPSIZE * 10, CHIPSIZE * 4 + 80, Direction::Left },
	{ EnemyType::Caterpillar, CHIPSIZE * 18, CHIPSIZE * 5, Direction::Left },
	{ EnemyType::Humanoid,    CHIPSIZE * 28, CHIPSIZE * 5, Direction::Right },
};

// ステージ3用（4体）
static const EnemySpawn stage3Enemies[] =
{
	{ EnemyType::Humanoid,    CHIPSIZE * 8,  CHIPSIZE * 5, Direction::Left },
	{ EnemyType::Caterpillar, CHIPSIZE * 16, CHIPSIZE * 5, Direction::Left },
	{ EnemyType::Float,       CHIPSIZE * 24, CHIPSIZE * 4 + 80, Direction::Right },
	{ EnemyType::Humanoid,    CHIPSIZE * 32, CHIPSIZE * 5, Direction::Right },
};

// シークレットステージ用（6体）secretG=575, Float は -20 低め
static const EnemySpawn secretEnemies[] =
{
	{ EnemyType::Humanoid,    400,  575, Direction::Right },
	{ EnemyType::Caterpillar, 1100, 575, Direction::Left  },
	{ EnemyType::Humanoid,    1800, 575, Direction::Left  },
	{ EnemyType::Float,       2600, 555, Direction::Right },
	{ EnemyType::Caterpillar, 3500, 575, Direction::Right },
	{ EnemyType::Humanoid,    4200, 575, Direction::Left  },
};

class Stage* stage;		// ステージクラス
class Player* player;	// プレイヤークラス
class Shot* playerShot;	// プレイヤー弾クラス
class Shot* enemyShot;	// エネミー弾クラス
class Shot* bossShot;	// Boss弾クラス
class Enemy* enemy;		// 敵クラス
class Boss* boss;		// Bossクラス

// コンストラクタ
Game::Game()
{
	stage = new Stage;						// ステージクラスの生成
	player = new Player;					// プレイヤークラスの作成
	playerShot = new Shot;					// プレイヤー弾クラス
	enemy = new Enemy[Enemy::MAX];			// 敵クラスの作成
	enemyShot = new Shot[ENEMY_SHOT_MAX];	// エネミー弾クラス
	boss = new Boss;						// Bossクラスの作成
	bossShot = new Shot;					// Boss弾クラス
}

// デストラクタ
Game::~Game()
{
	srand((unsigned int)time(NULL));		// ランダム初期化

	// ---クラス削除---
	delete stage;		// ステージクラスの削除
	delete player;		// プレイヤークラスの削除
	delete playerShot;	// プレイヤー弾クラスの削除
	delete[] enemy;		// 敵クラスの削除
	delete[] enemyShot;	// エネミー弾クラスの削除
	delete boss;		// Bossクラスの削除
	delete bossShot;	// Boss弾クラスの削除

	// ---文字削除用---
	DeleteObject(hFontDefalut);		// デフォルト
	DeleteObject(hFontNormal);		// 通常
	DeleteObject(hFontTitle);		// タイトル画面用
	DeleteObject(hFontSetting);		// 設定画面用
	DeleteObject(hFontSetting_2);	// 設定画面用
	DeleteObject(hFontShop);		// ショップ用
	DeleteObject(hFontBoss);		// ボストーク画面用
	DeleteObject(hFontO);			// ゲームオーバー用
	DeleteObject(hFontC);			// ゲームクリア用
	DeleteObject(hFontScrollEnd);	// エンドロール用
	DeleteObject(hFontEnd);			// エンド用

	if (hFadeDC) { DeleteDC(hFadeDC);        hFadeDC = nullptr; }
	if (hFadeBmp) { DeleteObject(hFadeBmp);    hFadeBmp = nullptr; }
	if (hFontStageName) { DeleteObject(hFontStageName); hFontStageName = nullptr; }

	delete shopSprite;
	delete deathEffect;
	delete titleDemoPlayer;
	delete titleDemoEnemy;

	delete hdc;
	delete hWnd;
}

// 初期化関数
void Game::Init()
{
	// コントローラー初期化（未接続でも続行）
	key.InitDI(hWnd);

	SetHDC(hdc);
	// デフォルト
	hFontDefalut = HMakeFont(40, 80, "メイリオ");
	// 通常
	hFontNormal = HMakeFont(10, 20, "メイリオ");
	// タイトル画面用
	hFontTitle = HMakeFont(20, 40, "ＭＳ ゴシック");
	// 設定画面用
	hFontSetting = HMakeFont(25, 50, "ＭＳ ゴシック");
	// 設定画面用2
	hFontSetting_2 = HMakeFont(40, 80, "ＭＳ ゴシック");
	// ショップ用
	hFontShop = HMakeFont(25, 50, "ＭＳ ゴシック");
	// ボストーク画面
	hFontBoss = HMakeFont(30, 60, "ＭＳ ゴシック");
	// ゲームオーバー用
	hFontO = HMakeFont(20, 40, "ＭＳ ゴシック");
	// ゲームクリア用
	hFontC = HMakeFont(20, 40, "ＭＳ ゴシック");
	// エンドロール用
	hFontScrollEnd = HMakeFont(25, 50, "ＭＳ ゴシック");
	// エンドロール後用
	hFontEnd = HMakeFont(60, 100, "ＭＳ ゴシック");

	// 暗転用メモリDCの作成（黒で塗りつぶす）
	hFadeDC = CreateCompatibleDC(hdc);
	hFadeBmp = CreateCompatibleBitmap(hdc, WND_W, WND_H);
	SelectObject(hFadeDC, hFadeBmp);
	{
		RECT r = { 0, 0, WND_W, WND_H };
		HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(hFadeDC, &r, hBlack);
		DeleteObject(hBlack);
	}
	// ステージ名フォント（大きめ）
	hFontStageName = HMakeFont(50, 80, "ＭＳ ゴシック");

	//wininitからgameからstageへhdcを受け渡した
	stage->hdc = hdc;
	player->hdc = hdc;
	playerShot->hdc = hdc;
	enemyShot->hdc = hdc;

	// 作業用DCの作成
	player->hWrkDC = CreateCompatibleDC(hdc);
	HBITMAP hBmp = CreateCompatibleBitmap(hdc, 200, 200);
	SelectObject(player->hWrkDC, hBmp);

	// 画像反転用DCの作成
	player->hReverseDC = CreateCompatibleDC(hdc);
	HBITMAP hBmp2 = CreateCompatibleBitmap(hdc, 200, 200);
	SelectObject(player->hReverseDC, hBmp2);

	// エネミー用
	for (int i = 0; i < Enemy::MAX; i++) {
		enemy[i].hdc = hdc;

		// 作業用DCの作成
		enemy[i].hWrkDC = CreateCompatibleDC(hdc);
		HBITMAP hBmpW = CreateCompatibleBitmap(hdc, 200, 200);
		SelectObject(enemy[i].hWrkDC, hBmpW);

		// 画像反転用DCの作成
		enemy[i].hReverseDC = CreateCompatibleDC(hdc);
		HBITMAP hBmpR = CreateCompatibleBitmap(hdc, 200, 200);
		SelectObject(enemy[i].hReverseDC, hBmpR);
	}

	// 作業用DCの作成(弾)
	playerShot->hWrkDC = CreateCompatibleDC(hdc);
	HBITMAP hBmpShot = CreateCompatibleBitmap(hdc, 64, 64);
	SelectObject(playerShot->hWrkDC, hBmpShot);

	// 作業用DCの作成(敵の弾)
	for (int i = 0; i < ENEMY_SHOT_MAX; i++) {
		enemyShot[i].hdc = hdc;
		enemyShot[i].hWrkDC = CreateCompatibleDC(hdc);
		HBITMAP hBmpEnemyShot = CreateCompatibleBitmap(hdc, 64, 64);
		SelectObject(enemyShot[i].hWrkDC, hBmpEnemyShot);
	}

	// Boss用DC作成
	boss->hdc = hdc;
	boss->hWrkDC = CreateCompatibleDC(hdc);
	HBITMAP hBmpBoss = CreateCompatibleBitmap(hdc, 200, 200);
	SelectObject(boss->hWrkDC, hBmpBoss);

	boss->hReverseDC = CreateCompatibleDC(hdc);
	HBITMAP hBmpBossR = CreateCompatibleBitmap(hdc, 200, 200);
	SelectObject(boss->hReverseDC, hBmpBossR);

	// Boss弾用DC作成
	bossShot->hdc = hdc;
	bossShot->hWrkDC = CreateCompatibleDC(hdc);
	HBITMAP hBmpBossShot = CreateCompatibleBitmap(hdc, 64, 64);
	SelectObject(bossShot->hWrkDC, hBmpBossShot);

	// コイン画像の初期化
	coinImg.Init(hdc);

	// Shop スプライト（ワールド内表示用）
	shopSprite = new Image;
	shopSprite->hdc = hdc;
	shopSprite->hWrkDC = CreateCompatibleDC(hdc);
	HBITMAP hBmpShopS = CreateCompatibleBitmap(hdc, 100, 100);
	SelectObject(shopSprite->hWrkDC, hBmpShopS);
	shopSprite->ReadBmp("Shop.bmp");
	shopSprite->w = shopSprite->h = 100;

	// 敵撃破エフェクト画像（btleffect.bmp: 640×64, 64×64 × 10フレーム）
	deathEffect = new Image;
	deathEffect->hdc = hdc;
	deathEffect->hWrkDC = CreateCompatibleDC(hdc);
	HBITMAP hBmpDeath = CreateCompatibleBitmap(hdc, 64, 64);
	SelectObject(deathEffect->hWrkDC, hBmpDeath);
	deathEffect->ReadBmp("btleffect.bmp");
	deathEffect->w = deathEffect->h = 64;

	// タイトルデモアニメーション用スプライト
	titleDemoPlayer = new Image;
	titleDemoPlayer->hdc = hdc;
	titleDemoPlayer->hWrkDC = CreateCompatibleDC(hdc);
	titleDemoPlayer->ReadBmp("PlayerAnime.bmp");
	titleDemoPlayer->w = titleDemoPlayer->h = 100;

	titleDemoEnemy = new Image;
	titleDemoEnemy->hdc = hdc;
	titleDemoEnemy->hWrkDC = CreateCompatibleDC(hdc);
	titleDemoEnemy->ReadBmp("EnemyCaterpillar.bmp");
	titleDemoEnemy->w = titleDemoEnemy->h = 48;

	// タイトルからstart
	stage->InitView(Mode::Title);
	bgm.BGM_Open("Title.mp3");
	bgm.BGM_SetVolume(audio.GetBGMVolume());
	bgm.BGM_Play();
	player->Init();

	coin = 0;
	cameraX = 0;
	BossDead = false;
}

// 操作関数
void Game::Ctrl()
{
	// キーバインド有効
	key.Ctrl();

	// SE取得
	AnimSE seReqP = player->GetSERequest();
	if (seReqP != AnimSE::None)
	{
		PlayAnimSE(seReqP);
		player->ClearSERequest();
	}
	for (int i = 0; i < Enemy::MAX; i++) {
		AnimSE seReqE = enemy[i].GetSERequest();
		if (seReqE != AnimSE::None)
		{
			PlayAnimSE(seReqE);
			enemy[i].ClearSERequest();
		}
	}
	AnimSE seReqBoss = boss->GetSERequest();
	if (seReqBoss != AnimSE::None)
	{
		PlayAnimSE(seReqBoss);
		boss->ClearSERequest();
	}
	AnimSE seReqT = title.GetSERequest();
	if (seReqT != AnimSE::None)
	{
		PlayAnimSE(seReqT);
		title.ClearSERequest();
	}
	AnimSE seReqS = setting.GetSERequest();
	if (seReqS != AnimSE::None)
	{
		PlayAnimSE(seReqS);
		setting.ClearSERequest();
	}
	AnimSE seReqE = end.GetSERequest();
	if (seReqE != AnimSE::None)
	{
		PlayAnimSE(seReqE);
		end.ClearSERequest();
	}
	AnimSE seReqShop = shopUI.GetSERequest();
	if (seReqShop != AnimSE::None)
	{
		PlayAnimSE(seReqShop);
		shopUI.ClearSERequest();
	}

	// ステージ情報
	Mode oldMode = stage->GetMode();
	// 現在のモードを確認(音楽再生時の確認用)
	Mode now = stage->GetMode();

	// ステージ操作
	stage->Ctrl(key);

	// プレイヤーの処理をステージに読み込ます
	if (oldMode != stage->GetMode())
	{
		Mode newMode = stage->GetMode();

		if (stage->IsReturningFromPause()) {
			// Setting/Shop から元のステージへ復帰: player/enemy 状態を保持
			stage->ClearReturningFlag();
		}
		else if (newMode == Mode::Setting || newMode == Mode::shop) {
			// Setting/Shop へ移行: player/enemy 状態を保持（初期化しない）
		}
		else {
			// 通常のステージ遷移
			player->ModeChange(newMode);

			// ステージ移動時に弾を初期化
			playerShot->SetIsShot(false);
			for (int i = 0; i < ENEMY_SHOT_MAX; i++)
			{
				enemyShot[i].SetIsShot(false);
			}

			// 各ステージで敵を初期化
			if (newMode == Mode::Tutorial || newMode == Mode::Stage1 ||
				newMode == Mode::Stage2 || newMode == Mode::Stage3 ||
				newMode == Mode::SecretStage)
			{
				InitStageEnemies(newMode);

				// SecretStage: パトロールモード設定 + コイン配置
				if (newMode == Mode::SecretStage) {
					static const int bounds[Enemy::MAX][2] = {
						{100,  1230},  // Humanoid    X=400  Section1
						{100,  1230},  // Caterpillar X=1100 Section1
						{1585, 2910},  // Humanoid    X=1800 Section2
						{1585, 2910},  // Float       X=2600 Section2
						{3180, 5050},  // Caterpillar X=3500 Section3
						{3180, 5050},  // Humanoid    X=4200 Section3
					};
					for (int i = 0; i < Enemy::MAX; i++) {
						enemy[i].SetPatrolMode(bounds[i][0], bounds[i][1]);
						enemy[i].SetMarioMode();  // 攻撃なし・Idle固定・接触ダメージはGame側
					}
					// コインをリセットして終盤(X=4200〜4700)に50枚配置
					// secretG = CHIPSIZE*5.75 = 575, コインは地面より少し上(535)に浮かせる
					for (int i = 0; i < COIN_MAX; i++) coins[i].active = false;
					for (int ci = 0; ci < 5; ci++) {
						SpawnCoins(4200 + ci * 100, 535, 10);
					}
				}
			}
			// Bossステージの初期化
			else if (newMode == Mode::Boss)
			{
				boss->Init(900, 550);
			}
			// BossTalk 初期化（Secretルートかどうかを設定）
			else if (newMode == Mode::BossTalk)
			{
				bossTalk.SetFromSecret(oldMode == Mode::SecretStage);
			}
			// ステージ遷移演出（Tutorial以外；Tutorialは下のTitle→Tutorialブロックで処理）
			if (newMode == Mode::Stage1 || newMode == Mode::Stage2 ||
				newMode == Mode::Stage3 || newMode == Mode::Boss ||
				newMode == Mode::SecretStage)
			{
				StartStageTransition(newMode);
			}
		}
	}

	// タイトル画面
	if (stage->GetMode() == Mode::Title)
	{
		//titleFrame++;
		title.Ctrl(key, *stage, hWnd);

		bool anyInput = key.Pressed(VK_UP) || key.Pressed(VK_DOWN) ||
			key.Pressed(VK_RETURN) || key.Pressed('W') || key.Pressed('S');
		if (anyInput)
		{
			titleIdleTimer = 0;
			titleDemoTimer = 0;
		}
		else if (titleIdleTimer >= 900)   // 15秒経過 → デモ再生中
		{
			titleDemoTimer++;
			if (titleDemoTimer >= 360)      // 1回再生完了 → また30秒待機へ
			{
				titleDemoTimer = 0;
				titleIdleTimer = 0;
			}
		}
		else
		{
			titleIdleTimer++;               // まだ待機中
		}
	}

	// 設定画面
	else if (stage->GetMode() == Mode::Setting) setting.Ctrl(key, audio, *stage, hWnd);

	// ショップ画面
	else if (stage->GetMode() == Mode::shop)
	{
		shopUI.Ctrl(key, *stage, coin, unlockAttack3, unlockSpin, maxStamina, (int)player->GetHP());
		// HP回復購入時の処理
		if (shopUI.GetHealRequest()) {
			shopUI.ClearHealRequest();
			player->HealHP(20.0f);
		}
	}

	// ボストーク画面
	else if (stage->GetMode() == Mode::BossTalk) bossTalk.Ctrl(key, *stage);

	// ゲームオーバークリア画面
	else if (stage->GetMode() == Mode::Over
		|| stage->GetMode() == Mode::Clear
		|| stage->GetMode() == Mode::End)
		end.Ctrl(key, *stage, hWnd);

	// それ以外（ゲームプレイ中）
	else
	{
		// Shop 近接判定: Tutorial/Stage1/2/3/SecretStage で E キーを押すと Shop が開く
		static const int shopWorldXArr[] = {
			CHIPSIZE * 13,	// Tutorial    (idx 0)
			CHIPSIZE * 3,	// Stage1      (idx 1)
			CHIPSIZE * 3,	// Stage2      (idx 2)
			CHIPSIZE * 3,	// Stage3      (idx 3)
			-1,				// BossTalk    (idx 4) - Shop なし
			-1,				// Boss        (idx 5) - Shop なし
			CHIPSIZE * 47,	// SecretStage (idx 6)
		};
		int stageIdx = (int)stage->GetMode() - (int)Mode::Tutorial;
		if (stageIdx >= 0 && stageIdx <= 6 && shopWorldXArr[stageIdx] >= 0)
		{
			int shopScreenX = shopWorldXArr[stageIdx] - stage->GetCameraX();
			int playerScreenX = player->GetPosition().x;

			if (abs(playerScreenX - shopScreenX) < 100 && (key.Pressed('E') || key.JoyPressed(2)))
			{
				stage->RequestMode(Mode::shop);
			}
		}

		// シークレットステージ入口（チュートリアルの穴の前で F キー）
		if (stage->GetMode() == Mode::Tutorial)
		{
			int pOffX = player->GetOffsetX();
			if (pOffX >= 1420 && pOffX <= 1580)
			{
				if (key.Pressed('F') || key.POVPressed(0)) {
					stage->RequestMode(Mode::SecretStage);
				}
			}
		}

		// Shop 解放フラグをプレイヤーに毎フレーム同期
		player->SetUnlockAttack3(unlockAttack3);
		player->SetUnlockSpin(unlockSpin);
		player->SetMaxStamina((float)maxStamina);

		// プレイヤー操作（歩き込み中は操作をブロック）
		if (transWalkIn) {
			stage->SetCameraX(0);						// カメラを先頭に固定
			if (player->UpdateWalkIn()) transWalkIn = false;
		}
		else {
			player->Ctrl(key, *stage);
		}

		// 攻撃フレーム変化の検出（同一フレームの重複ヒット防止）
		int currAttackFrame = player->GetAnim();
		bool attackFrameChanged = (currAttackFrame != playerPrevAttackFrame);
		if (player->GetIsAttack())
			playerPrevAttackFrame = currAttackFrame;
		else
			playerPrevAttackFrame = -1;

		// Bossステージの場合
		if (stage->GetMode() == Mode::Boss) {
			// Bossにプレイヤーの位置を渡す
			Pos ppos = player->GetPosition();
			boss->SetPlayerPos(ppos.x, ppos.y);
			// 歩き込み演出中は Boss を動かさない
			if (!transWalkIn) {
				boss->Ctrl();
			}
			// Boss弾発射
			if (boss->GetGunFiring()) {
				if (!bossShot->GetIsShot()) {
					Pos bpos = boss->GetPosition();
					int bulletX;
					if (boss->GetDirection() == Direction::Right)
						bulletX = bpos.x + 100;
					else
						bulletX = bpos.x - 20;
					bossShot->Init(hdc, bulletX, bpos.y + (CHIPSIZE / 4), boss->GetDirection(), false);
				}
			}
			bossShot->SetCameraX(stage->GetCameraX());
			bossShot->Ctrl();
		}
		// 通常ステージの場合
		else {
			// 敵
			for (int i = 0; i < Enemy::MAX; i++) {
				enemy[i].SetPlayerWorldX(player->GetOffsetX()); // ワールド座標
				enemy[i].SetCameraX(stage->GetCameraX());          // カメラ
				enemy[i].Ctrl();
			}
		}

		// プレイヤーが銃を放つ時の弾の描画
		if (player->GetGunFiring()) {
			if (!playerShot->GetIsShot()) {
				Pos p = player->GetPosition();
				int camX = stage->GetCameraX();
				int bulletX;
				// 向きに応じて弾の発射位置を変更（ワールド座標で初期化）
				if (player->GetDirection() == Direction::Right) {
					bulletX = p.x + camX + CHIPSIZE;  // 右向き：右から発射
				}
				else {
					bulletX = p.x + camX - 20;  // 左向き：左から発射
				}
				// Bossステージはy+25、通常ステージはy+52（全敵h=48のため visualTop=y+52 に合わせた）
				int bulletY;
				if (stage->GetMode() == Mode::Boss)
					bulletY = p.y + 25;
				else
					bulletY = p.y + 52;
				playerShot->Init(hdc, bulletX, bulletY, player->GetDirection(), false);
			}
		}
		playerShot->SetCameraX(stage->GetCameraX());
		playerShot->Ctrl();

		// エネミー発射
		for (int i = 0; i < Enemy::MAX; i++)
		{
			if (enemy[i].IsShotRequested())
			{
				for (int s = 0; s < ENEMY_SHOT_MAX; s++)
				{
					if (!enemyShot[s].GetIsShot())
					{
						Pos epos = enemy[i].GetPosition();
						int camX = stage->GetCameraX();

						int enemyDrawYOffset = 100 - 48;

						enemyShot[s].Init(
							hdc,
							epos.x + camX,  // ワールド座標で初期化
							epos.y + enemyDrawYOffset + 15,
							enemy[i].GetDirection(),
							enemy[i].IsFloatShot()
						);
						break;
					}
				}
				enemy[i].ClearShotRequest();
			}
		}

		for (int i = 0; i < ENEMY_SHOT_MAX; i++)
		{
			enemyShot[i].SetCameraX(stage->GetCameraX());
			enemyShot[i].Ctrl();

			// エネミーの弾とプレイヤーの当たり判定
			if (enemyShot[i].GetIsShot() && player->GetMuteki() <= 0)
			{
				if (HitCheakRect(enemyShot[i].GetParam(), player->GetParam()))
				{
					player->SetMuteki();
					player->DecHP(5, AnimState::Idle); // 弾のダメージ
					enemyShot[i].SetIsShot(false); // 弾を消す
				}
			}
		}

		// プレイヤーの弾とエネミーの当たり判定（弾の判定を上下40pxずつ拡張してFloat等に当たるようにする）
		if (playerShot->GetIsShot())
		{
			for (int i = 0; i < Enemy::MAX; i++)
			{
				if (enemy[i].GetIsDead() || enemy[i].IsDying()) continue;	// 死亡中もスキップ

				if (HitCheakRect(playerShot->GetParam(), enemy[i].GetParam()))
				{
					Pos ePos = enemy[i].GetPosition();
					bool wasDeadOrDying = enemy[i].GetIsDead() || enemy[i].IsDying();
					enemy[i].DecHP(10, player->GetState()); // 弾のダメージ
					if (!wasDeadOrDying && (enemy[i].GetIsDead() || enemy[i].IsDying()))
					{
						// チュートリアルは1枚、それ以外は3〜5枚
						int dropCount = 0;
						if (stage->GetMode() == Mode::Tutorial)
						{
							dropCount = 1;
						}
						else if (stage->GetMode() != Mode::Boss)
						{
							dropCount = rand() % 3 + 3;
						}
						SpawnCoins(ePos.x + stage->GetCameraX(), ePos.y + 48, dropCount);
					}
					playerShot->SetIsShot(false); // 弾を消す
					break; // 1発で1体だけにヒット
				}
			}
		}

		for (int i = 0; i < Enemy::MAX; i++) {
			if (enemy[i].GetIsDead() || enemy[i].IsDying()) continue;	// 死亡中もスキップ

			// ─── マリオモード: 踏みつけ & 接触ダメージ ───
			if (enemy[i].IsMarioMode()) {
				Param pParam = player->GetParam();
				Param eParam = enemy[i].GetParam();

				if (HitCheakRect(pParam, eParam)) {
					// 空中フラグ+落下速度(accel > 0)+位置関係を優先
					// プレイヤーの足元の座標 (pParam.y + pParam.h) を使用
					bool isStepping = (player->GetIsFall() || player->GetIsJump()) &&
						(pParam.y + pParam.h < eParam.y + eParam.h / 2);

					if (isStepping) {
						// 踏みつけ → 即死（コインも落とす）
						Pos ePos = enemy[i].GetPosition();
						bool wasDeadMario = enemy[i].GetIsDead() || enemy[i].IsDying();
						enemy[i].DecHP(9999, player->GetState());
						if (!wasDeadMario && (enemy[i].GetIsDead() || enemy[i].IsDying())) {
							int marioDropCount = 0;
							if (stage->GetMode() == Mode::Tutorial) marioDropCount = 1;
							else if (stage->GetMode() != Mode::Boss) marioDropCount = rand() % 3 + 3;
							if (marioDropCount > 0) SpawnCoins(ePos.x + stage->GetCameraX(), ePos.y + 48, marioDropCount);
						}
					}
					else if (player->GetMuteki() <= 0) {
						// 側面・下から触れた → プレイヤーに10ダメージ
						player->SetMuteki();
						// 接触位置（スクリーン座標）でノックバック方向を決定
						Direction knockDir;
						if (enemy[i].GetPosition().x < player->GetPosition().x)
							knockDir = Direction::Right;
						else
							knockDir = Direction::Left;
						player->SetKnockback(knockDir);
					}
				}
				// マリオモード敵は通常の攻撃判定（以下のブロック）に進む
			}

			// 接触ダメージは削除（攻撃判定のみ）

			// 1. エネミーの近接攻撃判定（Humanoidの場合）
			if (enemy[i].IsAttacking() && player->GetMuteki() <= 0) {
				// 攻撃アニメーションの特定フレーム（例：3枚目）で判定
				if (enemy[i].GetAnim() == 4) {
					Pos enemyPos = enemy[i].GetPosition();
					Param atkArea;

					// エネミーの向きに応じて攻撃範囲を設定
					if (enemy[i].GetDirection() == Direction::Right) {
						atkArea.x = enemyPos.x + 40;
						atkArea.y = enemyPos.y;
						atkArea.w = 60;
						atkArea.h = 48;
					}
					else {
						atkArea.x = enemyPos.x - 60;
						atkArea.y = enemyPos.y;
						atkArea.w = 60;
						atkArea.h = 48;
					}

					if (HitCheakRect(atkArea, player->GetParam())) {
						player->SetMuteki();
						player->DecHP(enemy[i].GetDamage(), boss->GetState());
					}
				}
			}
			// 2. プレイヤーの攻撃判定（近接）※フレーム変化した瞬間のみ発動
			if (player->GetIsAttack() && attackFrameChanged) {
				// 3枚目（anim == 2）の時にあたり判定
				if (player->GetAnim() == 2 || (player->GetState() == AnimState::ChargeRelease && player->GetAnim() == 0)) {
					Pos playerPos = player->GetPosition();
					Param atkArea;

					if (player->GetDirection() == Direction::Right) {
						// 右向き：プレイヤーの位置 + 75pxから100px分
						atkArea.x = playerPos.x + 75;
						atkArea.y = playerPos.y;
						atkArea.w = 100;
						atkArea.h = 100;
					}
					else {
						// 左向き：プレイヤーの位置 - 100pxから100px分
						atkArea.x = playerPos.x - 100;
						atkArea.y = playerPos.y;
						atkArea.w = 100;
						atkArea.h = 100;
					}

					if (HitCheakRect(atkArea, enemy[i].GetParam())) {
						// ダメージシステムを使用
						int damage = player->GetCurrentDamage();
						Pos ePos = enemy[i].GetPosition();
						bool wasDeadOrDyingMelee = enemy[i].GetIsDead() || enemy[i].IsDying();
						enemy[i].DecHP(damage, player->GetState());
						if (!wasDeadOrDyingMelee && (enemy[i].GetIsDead() || enemy[i].IsDying())) {
							int dropCount = 0;
							if (stage->GetMode() == Mode::Tutorial)
								dropCount = 1;
							else if (stage->GetMode() != Mode::Boss)
								dropCount = rand() % 3 + 3;
							SpawnCoins(ePos.x + stage->GetCameraX(), ePos.y + 48, dropCount);
						}

						// ノックバック（Attack2は小さく、重い攻撃は大きく）
						if (player->GetState() == AnimState::Attack2) {
							enemy[i].ApplyKnockback(player->GetDirection(), 20);
						}
						else if (player->GetState() == AnimState::Attack3 ||
							player->GetState() == AnimState::ChargeRelease ||
							player->GetState() == AnimState::Spin) {
							enemy[i].ApplyKnockback(player->GetDirection());
						}
					}
				}
			}
		}

		// === Boss戦の当たり判定 ===
		if (stage->GetMode() == Mode::Boss && !boss->GetIsDead()) {
			// 1. プレイヤーの攻撃 vs Boss ※フレーム変化した瞬間のみ発動
			if (player->GetIsAttack() && attackFrameChanged && player->GetAnim() == 2) {
				Pos playerPos = player->GetPosition();
				Param atkArea;

				if (player->GetDirection() == Direction::Right) {
					atkArea.x = playerPos.x + 75;
					atkArea.y = playerPos.y;
					atkArea.w = 100;
					atkArea.h = 100;
				}
				else {
					atkArea.x = playerPos.x - 100;
					atkArea.y = playerPos.y;
					atkArea.w = 100;
					atkArea.h = 100;
				}

				if (HitCheakRect(atkArea, boss->GetParam())) {
					int damage = player->GetCurrentDamage();
					boss->DecHP(damage,player->GetState());
					boss->ApplyKnockback(player->GetDirection());
				}
			}

			// 2. プレイヤー弾 Boss
			if (playerShot->GetIsShot()) {
				if (HitCheakRect(playerShot->GetParam(), boss->GetParam())) {
					boss->DecHP(10,player->GetState());
					playerShot->SetIsShot(false);
				}
			}

			// 3. Boss攻撃 プレイヤー
			if (boss->IsAttacking() && player->GetMuteki() <= 0) {
				if (boss->GetAnim() == 2) {
					Pos bossPos = boss->GetPosition();
					Param atkArea;

					if (boss->GetDirection() == Direction::Right) {
						atkArea.x = bossPos.x + 75;
						atkArea.y = bossPos.y;
						atkArea.w = 100;
						atkArea.h = 100;
					}
					else {
						atkArea.x = bossPos.x - 100;
						atkArea.y = bossPos.y;
						atkArea.w = 100;
						atkArea.h = 100;
					}

					if (HitCheakRect(atkArea, player->GetParam())) {
						player->SetMuteki();
						player->DecHP(10, boss->GetState());
						player->SetKnockback(boss->GetDirection());
					}
				}
			}

			// 4. Boss弾 プレイヤー
			if (bossShot->GetIsShot() && player->GetMuteki() <= 0) {
				if (HitCheakRect(bossShot->GetParam(), player->GetParam())) {
					player->SetMuteki();
					player->DecHP(5, boss->GetState());
					bossShot->SetIsShot(false);
				}
			}
		}

		// ゲームクリア処理
		if (BossDead || (stage->GetMode() == Mode::Boss && boss->GetIsDead())) {
			stage->RequestMode(Mode::Clear);
			BossDead = false;
		}

		if (player->GetHP() <= 0)
		{
			stage->RequestMode(Mode::Over);
		}

		// シークレットステージ: 落とし穴から落下したらゲームオーバー
		if (stage->GetMode() == Mode::SecretStage && player->GetPosition().y > CHIPSIZE * 5 + 75 + 20)
		{
			stage->RequestMode(Mode::Over);
		}

		// コイン取得判定
		UpdateCoins();
	}

	// ModeごとのBGM再生
	// Setting/Shop に入る時は BGM を変更しない（一時停止扱い）
	if (now != BGMMode && now != Mode::Setting && now != Mode::shop) {
		bgm.BGM_Close();

		switch (now)
		{
		case Mode::Title:
			bgm.BGM_Open("Title.mp3");
			break;

		case Mode::Tutorial:
			bgm.BGM_Open("Tutorial.mp3");
			break;

		case Mode::Stage1:
		case Mode::Stage2:
		case Mode::Stage3:
			bgm.BGM_Open("NormalBattle.mp3");
			break;

		case Mode::BossTalk:
			bgm.BGM_Open("BossTalk.mp3");
			break;

		case Mode::Boss:
			bgm.BGM_Open("BossBattle.mp3");
			break;

		case Mode::SecretStage:
			bgm.BGM_Open("BIT_STAR.mp3");
			break;

		case Mode::Over:
			bgm.BGM_Open("Over.mp3");
			break;

		case Mode::Clear:
			bgm.BGM_Open("GameClear.mp3");
			break;

		case Mode::End:
			bgm.BGM_Open("EndCredits.mp3");
			break;
		}
		bgm.BGM_Play();
		BGMMode = now;
	}
	bgm.BGM_Loop();

	// 遷移タイマー更新
	UpdateTransition();

	if (oldMode == Mode::Title && stage->GetMode() == Mode::Tutorial)
	{
		// タイトルからゲームに変わった時、ゲーム初期化
		player->Init();
		coin = 0;
		unlockAttack3 = false;
		unlockSpin = false;
		maxStamina = 100;
		for (int i = 0; i < COIN_MAX; i++) coins[i].active = false;
		StartStageTransition(Mode::Tutorial);	// チュートリアル遷移演出
	}
}

// 表示関数
void Game::Draw()
{
	stage->Draw();	// ステージ表示

	// BGMボリュームの取得
	bgm.BGM_SetVolume(audio.GetBGMVolume());

	switch (stage->GetMode())
	{
		// タイトル画面
	case Mode::Title:
	{
		// アニメーション描画（画面上部）
		if (titleDemoPlayer && titleDemoEnemy)
		{
			int t = titleDemoTimer;
			const int DEMO_GROUND = 200;	// 地面基準Y
			const int PLAYER_Y = DEMO_GROUND - 100;	// プレイヤー描画Y（100x100）
			const int ENEMY_Y = DEMO_GROUND - 48;	// 敵描画Y（48x48）

			// ── プレイヤー位置（-100→300 を80フレームで）──
			int playerX;
			if (t < 80) playerX = -100 + t * 400 / 80;	// 歩き込み
			else         playerX = 300;					// 停止

			// ── 敵位置（1380→850 を80フレームで）──
			const int ENEMY_STOP = 850;
			int enemyX;
			if (t < 80)       enemyX = 1380;
			else if (t < 160) enemyX = 1380 - (t - 80) * 530 / 80;	// 歩き込み
			else              enemyX = ENEMY_STOP;

			// ── フェーズ判定 ──
			bool playerWalking = (t >= 0 && t < 80);
			bool enemyWalking = (t >= 80 && t < 160);
			bool gunPhase = (t >= 200 && t < 230);
			bool bulletPhase = (t >= 230 && t < 250);
			bool deathPhase = (t >= 250 && t < 300);

			// ── プレイヤー描画（デス演出中も表示し続ける・t<300）──
			if (t < 300)
			{
				int pSrcX;
				int pSrcY;
				if (playerWalking)
				{
					// Walk（row1, 6コマ, 100x100）
					int wFrame = (t / 8) % 6;
					pSrcX = wFrame * 100;
					pSrcY = 100;
				}
				else if (gunPhase)
				{
					// Gun（row5, 7コマ）
					int gFrame = (t - 200) * 7 / 30;
					if (gFrame > 6) gFrame = 6;
					pSrcX = gFrame * 100;
					pSrcY = 500;
				}
				else
				{
					// Idle（row0, 1コマ）
					pSrcX = 0;
					pSrcY = 0;
				}
				titleDemoPlayer->TransBltWH(playerX, PLAYER_Y, pSrcX, pSrcY, 100, 100);
			}

			// ── 敵描画（死亡エフェクト中は非表示）──
			if (t >= 80 && !deathPhase && t < 300)
			{
				// 敵は左向き（プレイヤーの方を向く）→ 反転描画
				titleDemoEnemy->ReverseBltWH(enemyX, ENEMY_Y, 0, 0, 48, 48);
			}

			// ── 弾描画 ──
			if (bulletPhase)
			{
				// プレイヤー右端(400)から ENEMY_STOP(850) まで20フレームで到達
				int bulletX = 400 + (t - 230) * 24;
				HBRUSH hBullet = CreateSolidBrush(RGB(255, 220, 50));
				RECT   br = { bulletX, DEMO_GROUND - 30, bulletX + 14, DEMO_GROUND - 20 };
				FillRect(hdc, &br, hBullet);
				DeleteObject(hBullet);
			}

			// ── 死亡エフェクト ──
			if (deathPhase && deathEffect)
			{
				int dFrame = (t - 250) / 5;
				if (dFrame > 9) dFrame = 9;
				deathEffect->TransBltWH(ENEMY_STOP - 8, ENEMY_Y - 8, dFrame * 64, 0, 64, 64);
			}
		}
		SelectObject(hdc, hFontTitle);
		title.Draw(hdc);
		break;
	}
	// 設定画面
	case Mode::Setting:
		// 設定画面の描画
		SelectObject(hdc, hFontSetting);
		setting.Draw(hdc);
		SelectObject(hdc, hFontSetting_2);
		setting.DrawChoice(hdc);
		//テキスト設定
		SetTextColor(hdc, RGB(255, 255, 255));
		//音量
		char BGMvolumeStr[32];
		wsprintf(BGMvolumeStr, "現在のBGM音量:%d", audio.bgmVolume);
		TextDraw(550, 80, BGMvolumeStr);
		char SEvolumeStr[32];
		wsprintf(SEvolumeStr, "現在のSE音量: %d", audio.seVolume);
		TextDraw(550, 180, SEvolumeStr);
		break;
		// ショップ画面
	case Mode::shop:
		SelectObject(hdc, hFontShop);
		shopUI.Draw(hdc, coin, unlockAttack3, unlockSpin, maxStamina, (int)player->GetHP());
		break;
		// ボストーク画面
	case Mode::BossTalk:
		//テキスト設定
		SetTextColor(hdc, RGB(255, 255, 255));
		SelectObject(hdc, hFontBoss);
		bossTalk.Draw(hdc);
		break;
		// ｹﾞｰﾑｵｰﾊﾞｰ画面
	case Mode::Over:
		SelectObject(hdc, hFontO);
		stage->Draw();
		end.DrawOver(hdc);
		break;
		// ゲームクリア画面
	case Mode::Clear:
		SelectObject(hdc, hFontC);
		end.DrawClear(hdc);
		break;
		// ゲームエンド画面
	case Mode::End:
		// スクロール
		if (!end.IsScrollEnd())
		{
			SelectObject(hdc, hFontScrollEnd);
			end.DrawScrollEnd(hdc);
		}
		else
		{
			// スクロールが終わったら モード選択表示
			SelectObject(hdc, hFontEnd);
			end.DrawEnd(hdc);
		}
		break;

	default:
		SelectObject(hdc, hFontDefalut);
		player->Draw();			// プレイヤー表示
		playerShot->Draw();		// 弾表示

		// Bossステージ
		if (stage->GetMode() == Mode::Boss) {
			boss->Draw();
			bossShot->Draw();
		}
		// 通常ステージ
		else {
			// 敵
			for (int i = 0; i < Enemy::MAX; i++) {
				enemy[i].Draw();
			}
			for (int i = 0; i < ENEMY_SHOT_MAX; i++)
			{
				enemyShot[i].Draw();
			}

			// 死亡エフェクト（btleffect.bmp 10フレーム）
			if (deathEffect) {
				int camX = stage->GetCameraX();
				for (int i = 0; i < Enemy::MAX; i++) {
					if (!enemy[i].IsDying()) continue;
					int srcX = enemy[i].GetDyingAnim() * 64;
					deathEffect->TransBltWH(
						enemy[i].GetDeathX() - camX,
						enemy[i].GetDeathY(),
						srcX, 0, 64, 64);
				}
			}
		}

		// --- プレイヤーの HP スタミナ 表示
			//テキスト設定
		SetTextColor(hdc, RGB(255, 255, 255));
		SelectObject(hdc, hFontNormal);
		// HP 文字表示
		TextDraw(30, 20, "HP");
		// スタミナ
		TextDraw(10, 40, "STAMINA");

		// HPバー（定数は game.h の HP_BAR_* / ST_BAR_* を参照）
		RECT hpRectBg;
		hpRectBg.left = HP_BAR_X - 1;
		hpRectBg.top = HP_BAR_Y - 1;
		hpRectBg.right = HP_BAR_X + HP_BAR_W + 1;
		hpRectBg.bottom = HP_BAR_Y + HP_BAR_H + 1;
		FillRect(hdc, &hpRectBg, (HBRUSH)GetStockObject(GRAY_BRUSH));

		int hpWidthNow = player->GetHP() * HP_BAR_W / 100;
		RECT hpRect;
		hpRect.left = HP_BAR_X;
		hpRect.top = HP_BAR_Y;
		hpRect.right = HP_BAR_X + hpWidthNow;
		hpRect.bottom = HP_BAR_Y + HP_BAR_H;
		HBRUSH hBrushHp = CreateSolidBrush(RGB(0, 0, 255));
		FillRect(hdc, &hpRect, hBrushHp);
		DeleteObject(hBrushHp);

		// スタミナバー
		RECT stRectBg;
		stRectBg.left = ST_BAR_X - 1;
		stRectBg.top = ST_BAR_Y - 1;
		stRectBg.right = ST_BAR_X + ST_BAR_W + 1;
		stRectBg.bottom = ST_BAR_Y + ST_BAR_H + 1;
		FillRect(hdc, &stRectBg, (HBRUSH)GetStockObject(GRAY_BRUSH));

		int stWidthNow = player->GetStamina() * ST_BAR_W / player->GetMaxStamina();
		RECT stRect;
		stRect.left = ST_BAR_X;
		stRect.top = ST_BAR_Y;
		stRect.right = ST_BAR_X + stWidthNow;
		stRect.bottom = ST_BAR_Y + ST_BAR_H;
		HBRUSH hBrushSta = CreateSolidBrush(RGB(0, 255, 0));
		FillRect(hdc, &stRect, hBrushSta);
		DeleteObject(hBrushSta);

		// コイン数表示
		char coinStr[32];
		wsprintf(coinStr, "COIN: %d", coin);
		TextDraw(300, 20, coinStr);

		// Shop スプライト描画（Tutorial/Stage1/2/3/SecretStage）
		{
			static const int shopWorldXArr[] = {
				CHIPSIZE * 13,	// Tutorial    (idx 0)
				CHIPSIZE * 3,	// Stage1      (idx 1)
				CHIPSIZE * 3,	// Stage2      (idx 2)
				CHIPSIZE * 3,	// Stage3      (idx 3)
				-1,				// BossTalk    (idx 4) - Shop なし
				-1,				// Boss        (idx 5) - Shop なし
				CHIPSIZE * 47,	// SecretStage (idx 6)
			};
			int sIdx = (int)stage->GetMode() - (int)Mode::Tutorial;
			if (sIdx >= 0 && sIdx <= 6 && shopWorldXArr[sIdx] >= 0 && shopSprite && shopSprite->hWrkDC) {
				int screenX = shopWorldXArr[sIdx] - stage->GetCameraX();
				if (screenX > -100 && screenX < WND_W + 100) {
					// SecretStage は地面が高い(secretG=575)のでY座標を合わせる
					int shopDrawY = CHIPSIZE * 6 - 100;
					if (stage->GetMode() == Mode::SecretStage) shopDrawY = 575;
					shopSprite->TransBlt(screenX, shopDrawY, 0, 0);
				}
			}
		}

		// コイン描画
		{
			int camX = stage->GetCameraX();
			for (int ci = 0; ci < COIN_MAX; ci++) {
				if (!coins[ci].active) continue;
				int screenX = coins[ci].worldX - camX;
				if (screenX > -32 && screenX < WND_W + 32) {
					coinImg.Draw(screenX, coins[ci].y);
				}
			}
		}

		// チュートリアルメッセージ
		if (stage->GetMode() == Mode::Tutorial) {
			struct TutMsg { const char* text; int triggerX; };
			static const TutMsg tutMsgs[] = {
				{ "右矢印キー or D   十字キー右 or スティック右: 右移動  ",0},
				{ "左矢印キー or A   十字キー左 or スティック左: 左移動",100},
				{ "W / Space   Aボタン: ジャンプ",200},
				{ "Shift   LBボタン: ダッシュ    Shift+(W / Space)   LBボタン+Aボタン: 走り跳び", 300  },
				{ "Q   RBボタン: 回避 (スタミナ消費)  ※空中でも使用可", 450  },
				{ "G   RTトリガー: 銃攻撃",600  },
				{ "B   Xボタン: 攻撃   B 連打   Xボタン 連打: 連撃",                750  },
				{ "V   Yボタン: 溜め攻撃",900  },
				{ " ",1150 },
				{ "E   Xボタン:Shopロボ画像の前で押下 Shopを開く",   1220 },
				{ " ",1370 },
				{ "F   十字キー上: ??? [SECRET]",1420 },
				{ " ",1581 },
			};
			int numMsgs = (int)(sizeof(tutMsgs) / sizeof(tutMsgs[0]));
			int offsetX = player->GetOffsetX();
			const char* dispMsg = nullptr;
			for (int ti = numMsgs - 1; ti >= 0; ti--) {
				if (offsetX >= tutMsgs[ti].triggerX) {
					dispMsg = tutMsgs[ti].text;
					break;
				}
			}
			if (dispMsg) {
				SetTextColor(hdc, RGB(255, 255, 100));
				SelectObject(hdc, hFontNormal);
				TextDraw(400, 640, dispMsg);
			}
		}
		break;
	}

	// ステージ遷移演出（Setting/Shop 中は非表示）
	{
		Mode curMode = stage->GetMode();
		if (curMode != Mode::Setting && curMode != Mode::shop) {
			DrawFadeOverlay();
			DrawStageName();
		}
	}
}

// エネミースポーン関数
void Game::InitStageEnemies(Mode mode)
{
	const EnemySpawn* enemies = nullptr;
	int count = 0;

	// ステージに応じた敵配置を選択
	switch (mode)
	{
	case Mode::Tutorial:
		enemies = tutorialEnemies;
		count = _countof(tutorialEnemies);
		break;
	case Mode::Stage1:
		enemies = stage1Enemies;
		count = _countof(stage1Enemies);
		break;
	case Mode::Stage2:
		enemies = stage2Enemies;
		count = _countof(stage2Enemies);
		break;
	case Mode::Stage3:
		enemies = stage3Enemies;
		count = _countof(stage3Enemies);
		break;
	case Mode::SecretStage:
		enemies = secretEnemies;
		count = _countof(secretEnemies);
		break;
	default:
		count = 0;
		break;
	}

	// 敵を初期化
	for (int i = 0; i < count && i < Enemy::MAX; i++)
	{
		enemy[i].SetType(enemies[i].type);  // 先に画像とサイズを設定
		enemy[i].Init(enemies[i].x, enemies[i].y, enemies[i].dir);

		// チュートリアルの場合は動かない & HP多め
		if (mode == Mode::Tutorial) {
			enemy[i].SetTutorialMode(true);
			enemy[i].SetHP(200);  // サンドバッグ用に多め
		}
	}

	// 余った敵は殺しておく
	for (int i = count; i < Enemy::MAX; i++)
	{
		enemy[i].SetIsDead();
	}

	int px = player->GetOffsetX();
	for (int i = 0; i < Enemy::MAX; i++)
	{
		enemy[i].SetPlayerWorldX(px);
		enemy[i].SetCameraX(stage->GetCameraX());
	}
}

// コインスポーン
void Game::SpawnCoins(int worldX, int y, int count)
{
	int spawned = 0;
	for (int i = 0; i < COIN_MAX && spawned < count; i++) {
		if (!coins[i].active) {
			coins[i].worldX = worldX + (rand() % 60 - 30);	// 少し散らばらせる
			coins[i].y = y;
			coins[i].active = true;
			spawned++;
		}
	}
}

// コイン取得判定
void Game::UpdateCoins()
{
	int camX = stage->GetCameraX();
	Pos pPos = player->GetPosition();
	int playerWorldX = pPos.x + camX;

	for (int i = 0; i < COIN_MAX; i++) {
		if (!coins[i].active) continue;

		// プレイヤーとの距離チェック
		int dx = abs(playerWorldX - coins[i].worldX);
		int dy = abs(pPos.y - coins[i].y);
		if (dx < CHIPSIZE / 2 && dy < CHIPSIZE / 2) {
			coins[i].active = false;
			coin++;
		}
	}
}

//当たり判定(円)
bool Game::HitCheak(Pos Atk, int aw, int ah, Pos Def, int dw, int dh)
{
	//中心座標を求める
	int center_ax = Atk.x + aw / 2;	//攻撃側x座標
	int center_ay = Atk.y + ah / 2;	//攻撃側y座標
	int center_dx = Def.x + dw / 2;	//防御側x座標
	int center_dy = Def.y + dh / 2;	//防御側y座標

	//各辺の長さを求める
	double a, b, c;
	a = center_dy - center_ay;
	b = center_dx - center_ax;
	c = sqrt(a * a + b * b);	//三平方(ﾋﾟﾀｺﾞﾗｽ)の定理

	if (c <= aw / 2 + dw / 2) {
		return true;	//当たっている
	}
	return false;	//外れている
}

//当たり判定(四角)
bool Game::HitCheakRect(Pos atkPos, int aw, int ah, Pos defPos, int dw, int dh)
{
	if (((atkPos.x >= defPos.x && atkPos.x <= defPos.x + dw) || (defPos.x >= atkPos.x && defPos.x <= atkPos.x + aw))
		&& ((atkPos.y >= defPos.y && atkPos.y <= defPos.y + dh) || (defPos.y >= atkPos.y && defPos.y <= atkPos.y + ah)))
	{
		return true;	//当たっている
	}
	return false;	//外れている
}

//当たり判定(四角)[オーバーロード]
bool Game::HitCheakRect(Param atkParam, Param defParam)
{
	Pos atkPos = { atkParam.x,atkParam.y };
	Pos defPos = { defParam.x,defParam.y };
	int aw = atkParam.w;
	int ah = atkParam.h;
	int dw = defParam.w;
	int dh = defParam.h;

	if (((atkPos.x >= defPos.x && atkPos.x <= defPos.x + dw)
		|| (defPos.x >= atkPos.x && defPos.x <= atkPos.x + aw))
		&& ((atkPos.y >= defPos.y && atkPos.y <= defPos.y + dh)
			|| (defPos.y >= atkPos.y && defPos.y <= atkPos.y + ah)))
	{
		return true;	//当たっている
	}
	return false;	//外れている
}

// ===== ステージ遷移アニメーション =====

// 遷移アニメ開始
void Game::StartStageTransition(Mode newMode)
{
	fadeAlpha = 255;
	stageNameTimer = 210;	// 30(in) + 120(表示) + 60(out) フレーム
	stageNameAlpha = 0;
	transWalkIn = true;
	transitionMode = newMode;
	player->StartWalkIn();
}

// 毎フレーム遷移タイマー更新
void Game::UpdateTransition()
{
	// 暗転フェードアウト（約50フレームで255→0）
	if (fadeAlpha > 0)
	{
		fadeAlpha -= 5;
		if (fadeAlpha < 0) fadeAlpha = 0;
	}

	// ステージ名アルファ更新
	if (stageNameTimer > 0)
	{
		stageNameTimer--;
		if (stageNameTimer > 180) stageNameAlpha = (210 - stageNameTimer) * 255 / 30;
		else if (stageNameTimer > 60)  stageNameAlpha = 255;
		else                           stageNameAlpha = stageNameTimer * 255 / 60;
	}
	else
	{
		stageNameAlpha = 0;
	}
}

// 暗転オーバーレイ描画
void Game::DrawFadeOverlay()
{
	if (fadeAlpha <= 0 || !hFadeDC) return;
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)fadeAlpha, 0 };
	AlphaBlend(hdc, 0, 0, WND_W, WND_H, hFadeDC, 0, 0, WND_W, WND_H, bf);
}

// ステージ名テキスト描画
void Game::DrawStageName()
{
	if (stageNameAlpha <= 0 || stageNameTimer <= 0) return;

	SelectObject(hdc, hFontStageName);
	SetBkMode(hdc, TRANSPARENT);

	BYTE a = (BYTE)stageNameAlpha;
	int centerY = WND_H / 2 - 40;
	RECT shadowRect = { 3,       centerY + 3, WND_W + 3, centerY + 83 };
	RECT mainRect = { 0,       centerY,     WND_W,     centerY + 80 };

	auto drawCentered = [&](LPCTSTR text)
		{
			SetTextColor(hdc, RGB(0, 0, 0));
			DrawText(hdc, text, -1, &shadowRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			SetTextColor(hdc, RGB(a, a, a));
			DrawText(hdc, text, -1, &mainRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		};

	switch (transitionMode)
	{
	case Mode::Tutorial:    drawCentered(TEXT("チュートリアルステージ"));      break;
	case Mode::Stage1:      drawCentered(TEXT("第1ステージ「廃墟都市」"));      break;
	case Mode::Stage2:      drawCentered(TEXT("第2ステージ「空中都市」"));      break;
	case Mode::Stage3:      drawCentered(TEXT("第3ステージ「地下都市」"));      break;
	case Mode::Boss:        drawCentered(TEXT("ボスステージ"));                 break;
	case Mode::SecretStage: drawCentered(TEXT("シークレットステージ"));         break;
	default: break;
	}
}

// SE再生
void Game::PlayAnimSE(AnimSE seType)
{
	if (seType == AnimSE::None) return;

	int vol = audio.GetSEVolume();
	if (vol <= 0) return;

	switch (seType)
	{
	case AnimSE::Attack1:
		se.SE_Play_Multi("attack1.mp3", vol);
		break;
	case AnimSE::Attack2:
		se.SE_Play_Multi("attack2.mp3", vol);
		break;
	case AnimSE::Attack3:
		se.SE_Play_Multi("attack3.mp3", vol);
		break;
	case AnimSE::Spin:
		se.SE_Play_Multi("TwoConsecutiveHits.mp3", vol);
		break;
	case AnimSE::Dodge:
		se.SE_Play_Multi("avoidance.mp3", vol);
		break;
	case AnimSE::Hit:
		se.SE_Play_Once("Hit.mp3", vol);
		break;
	case AnimSE::HitAttack1:
		se.SE_Play_Once("HitAttack1.mp3", vol);
		break;
	case AnimSE::HitAttack2:
		se.SE_Play_Once("HitAttack2.mp3", vol);
		break;
	case AnimSE::HitAttack3:
		se.SE_Play_Once("HitAttack3.mp3", vol);
		break;
	case AnimSE::ChargeHold:
		se.SE_Play_Once("PullSheath.mp3", vol);
		break;
	case AnimSE::ChargeRelease:
		se.SE_Play_Multi("Iaigiri.mp3", vol);
		break;
	case AnimSE::Gun:
		se.SE_Play_Multi("gun.mp3", vol);
		break;
	case AnimSE::EnemyAttack1:
		se.SE_Play_Multi("EnemyAttack1.mp3", vol);
		break;
	case AnimSE::EnemyAttack2:
		se.SE_Play_Multi("EnemyAttack2.mp3", vol);
		break;
	case AnimSE::EnemyDead:
		se.SE_Play_Multi("explosion.mp3", vol);
		break;
	case AnimSE::sentaku:
		se.SE_Play_Multi("sentaku.mp3", vol);
		break;
	case AnimSE::ketei:
		se.SE_Play_Multi("ketei.mp3", vol);
		break;
	}
}