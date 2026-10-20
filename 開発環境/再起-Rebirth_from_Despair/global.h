#pragma once
#include <Windows.h>
#include <cstdio>
#include <cassert>

#define WND_W	1280	//ｳｨﾝﾄﾞｳの幅
#define WND_H	720		//ｳｨﾝﾄﾞｳの高さ

#define ERR_MSG(s,f) MessageBox(NULL, s, f, MB_OK)	//ｴﾗｰﾒｯｾｰｼﾞ
#define ToRadian(x)(3.14f*(x)/180.0f)	//角度→ﾗｼﾞｱﾝ変換

static const int CHIPSIZE = 100;		// ステージチップ画像サイズ
static const int PlayerAttack = 125;	// プレイヤー攻撃画像サイズ

//ゲームモード
enum class Mode
{
	Title,			//0:ﾀｲﾄﾙ
	Tutorial,		//1:チュートリアル用
	Stage1,			//2:ステージ1
	Stage2,			//3:ステージ2
	Stage3,			//4:ステージ3
	BossTalk,		//5:Boss会話
	Boss,			//6:Bossステージ
	SecretStage,	//7:隠しステージ
	Over,			//8:ｹﾞｰﾑｵｰﾊﾞｰ
	Clear,			//9:ｹﾞｰﾑｸﾘｱ
	End,			//10:ENDロール
	shop,			//11:ショップ
	Setting = 50,	//50:設定画面
	Nothing = 90,	//90:空
};

//方向
enum class Direction
{
	Down,	//0:下
	Left,	//1:左
	Right,	//2:右
	Up,		//3:上
};

// アニメーション状態
enum class AnimState {
	Idle,			// 立ち
	Walk,			// 歩き
	Run,			// 走り
	BehindDodge,	// 緊急(後方)回避
	AheadDodge,		// 緊急(前方)回避
	Gun,			// 銃
	Attack1,		// 通常攻撃1
	Attack2,		// 通常攻撃2
	Attack3,		// 通常攻撃3
	Spin,			// 回転斬り
	ChargeHold,		// 溜め攻撃(溜めの時)
	ChargeRelease,	// 溜め攻撃(放つ時)
};

// エネミー状態
enum class EnemyState {
	Idle,
	Move,
	Attack,
	Cooldown,
	Dead
};

// エネミー種別
enum class EnemyType {
	Caterpillar, // キャタピラ（射撃）
	Float,       // 浮遊（射撃）
	Humanoid     // 二足（近接）
};

// アニメーション画像管理用
struct Animimage {
	int row;        // 何段目か
	int frameCount; // 枚数
	int frameW;		// 幅
	int frameH;		// 高さ
	bool loop;		// ループの有無
};

// SE
enum class AnimSE {
	None,
	Attack1,
	Attack2,
	Attack3,
	Spin,
	Dodge,
	Hit,
	HitAttack1,
	HitAttack2,
	HitAttack3,
	ChargeHold,
	ChargeRelease,
	Gun,
	EnemyAttack1,
	EnemyAttack2,
	EnemyDead,
	sentaku,
	ketei
};

// エネミー配置
struct EnemySpawn
{
	EnemyType type;
	int x;
	int y;
	Direction dir;
};

// ダメージ時のエフェクト
struct Spark
{
	int x;
	int y;
	int vx;
	int vy;
	int timer;
	float life;
};

// 位置
struct Pos
{
	int x, y;
};

// あたり判定用情報
struct Param
{
	int x, y;
	int w, h;
};

// Key用
struct KeyState {
	bool now;	// 現在フレーム確認用
	bool prev;	// 前フレーム確認用
};