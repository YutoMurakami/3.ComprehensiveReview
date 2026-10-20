#pragma once
#include <ctime>
#include <math.h>
#include <cmath>
#include "global.h"
#include "player.h"
#include "enemy.h"
#include "shot.h"
#include "boss.h"
#include "stage.h"
#include "AudioManager.h"
#include "Setting.h"
#include "shop.h"
#include "coin.h"
#include "Title.h"
#include "BossTalk.h"
#include "End.h"
#include "text.h"
#include "key.h"

//ゲームクラス
class Game : public Text
{
public://他のクラス等から操作できる(記載忘れ プラベになる)

	HDC hdc;	// デバイスコンテキスト
	HWND hWnd;	// ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ

	Game();		// ｺﾝｽﾄﾗｸﾀ
	~Game();	// ﾃﾞｽﾄﾗｸﾀ

	void Init();	// 初期化関数 Unity(start)
	void Ctrl();	// 操作関数 Unity(Update)
	void Draw();	// 表示関数

	// エネミースポーン関数
	void InitStageEnemies(Mode mode);
	// あたり判定(円)
	bool HitCheak(Pos Atk, int aw, int ah, Pos Def, int dw, int dh);
	// あたり判定(四角)
	bool HitCheakRect(Pos atkPos, int aw, int ah,
		Pos defPos, int dw, int dh);
	// あたり判定(四角):[オーバーロード関数]
	bool HitCheakRect(Param atkParam, Param defParam);

private:
	int cameraX = 0;							// 画面左端のワールド座標（スクロール用）
	static const int ENEMY_SHOT_MAX = 16;		// 敵発射
	bool BossDead = false;						// ボス死亡判定
	int playerPrevAttackFrame = -1;				// 攻撃フレーム変化追跡用（重複ヒット防止）
	//int titleFrame = 0;							// 描画フレームカウント用

	// Shop 解放フラグ / スタミナ上限
	bool unlockAttack3 = false;		// B連撃(3段目)解放フラグ
	bool unlockSpin = false;		// R回転斬り解放フラグ
	int  maxStamina = 100;			// スタミナ上限（Shop で増強可能）

	// ステージ遷移アニメーション
	int  fadeAlpha = 0;						// 暗転アルファ (255→0)
	int  stageNameAlpha = 0;				// ステージ名アルファ
	int  stageNameTimer = 0;				// ステージ名残りフレーム
	bool transWalkIn = false;				// 歩き込み中フラグ（操作ブロック用）
	Mode transitionMode = Mode::Nothing;	// 表示するステージ名

	// タイトルデモアニメーション用スプライト
	Image* titleDemoPlayer = nullptr;	// PlayerAnime.bmp（タイトル演出用）
	Image* titleDemoEnemy = nullptr;	// EnemyCaterpillar.bmp（タイトル演出用）
	int    titleDemoTimer = 0;			// フレームカウンタ（0-359 ループ）
	int    titleIdleTimer = 0;			// 無操作タイマー（3分=10800フレームでデモ起動）

	// Shop ワールド内スプライト
	Image* shopSprite = nullptr;	// Shop.bmp

	// 敵撃破エフェクト画像（btleffect.bmp: 640×64, 64×64 × 10フレーム）
	Image* deathEffect = nullptr;

	// チュートリアルメッセージ
	int tutorialMsgIndex = 0;		// 現在表示中メッセージインデックス
	int tutorialTimer = 0;			// 表示タイマー

	// HPバー / スタミナバー レイアウト定数
	static const int HP_BAR_X = 70;
	static const int HP_BAR_Y = 20;
	static const int HP_BAR_W = 200;
	static const int HP_BAR_H = 15;
	static const int ST_BAR_X = 70;
	static const int ST_BAR_Y = 40;
	static const int ST_BAR_W = 200;
	static const int ST_BAR_H = 15;

	// コインシステム
	int coin = 0;								// コイン所持数
	struct CoinData { int worldX, y; bool active; };
	static const int COIN_MAX = 60;
	CoinData coins[COIN_MAX] = {};	// 全要素ゼロ初期化（active=false）
	Coin coinImg;					// コイン画像
	// コイン管理
	void SpawnCoins(int worldX, int y, int count);
	void UpdateCoins();

	Mode BGMMode = Mode::Nothing;	// BGM管理用モード
	Title title;					// タイトル画面(メンバ変数)
	BossTalk bossTalk;				// ボストーク画面(メンバ変数)
	End end;						// ゲームオーバークリアエンドロール画面(メンバ変数)
	Setting setting;				// 設定画面用(メンバ変数)
	Shop shopUI;					// ショップ画面用(メンバ変数)
	Key key;						// キーバインド用(メンバ変数)
	BGM bgm;						// BGM設定(メンバ変数)
	SE  se;							// SE設定(メンバ変数)
	AudioManager audio;				// 音量設定(メンバ変数)

	HDC     hFadeDC = nullptr;				// 暗転用メモリDC（黒で塗りつぶし済み）
	HBITMAP hFadeBmp = nullptr;				// 暗転用ビットマップ

	HFONT   hFontStageName = nullptr;	// ステージ名フォント（大きめ）
	HFONT hFontDefalut = nullptr;		// デフォルト
	HFONT hFontNormal = nullptr;		// 通常
	HFONT hFontTitle = nullptr;			// タイトル画面用
	HFONT hFontSetting = nullptr;		// 設定画面用
	HFONT hFontSetting_2 = nullptr;		// 設定画面用
	HFONT hFontShop = nullptr;			// ショップ用
	HFONT hFontBoss = nullptr;			// ボストーク画面
	HFONT hFontO = nullptr;				// ゲームオーバー用
	HFONT hFontC = nullptr;				// ゲームクリア用
	HFONT hFontScrollEnd = nullptr;		// エンドロール用
	HFONT hFontEnd = nullptr;			// エンドロール後用

	void StartStageTransition(Mode newMode);	// 遷移アニメ開始
	void UpdateTransition();					// 毎フレーム遷移タイマー更新
	void DrawFadeOverlay();						// 暗転オーバーレイ描画
	void DrawStageName();						// ステージ名テキスト描画
	void PlayAnimSE(AnimSE seType);				// SE
};