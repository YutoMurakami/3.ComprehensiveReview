#pragma once
#include <cmath>
#include "key.h"
#include "image.h"
#include "AudioManager.h"

//ステージクラス(Imageクラスを継承)
class Stage : public Image
{
public:

	Stage();	//コンストラクタ
	~Stage();	//デストラクタ

	void InitView(Mode mode);	// UI用
	void InitStage(Mode mode);	// ステージ用
	void Ctrl(const Key& key);	// 操作関数(キーの追加)
	void Draw();				// 表示関数

	// プレイヤー位置の設定(XYの時はPosで良いがめんどくさいとのこと…泣)
	inline void SetPlayerX(int x) { this->x = x; }

	// プレイヤーの位置からカメラを設定
	inline void SetCameraX(int cameraX) { x = cameraX; }
	inline int GetCameraX() { return x; }

	// ゲームモードの取得
	inline Mode GetMode() { return mode; }

	// リクエストがあればモード切り替わり
	inline void RequestMode(Mode reqmode) { next = reqmode; }

	// Setting/Shop から元のステージへ戻るリクエスト
	inline void RequestPrev() { next = prevMode; }

	// 一時停止復帰フラグのアクセサー
	inline bool IsReturningFromPause() const { return returningFromPause; }
	inline void ClearReturningFlag() { returningFromPause = false; }

private:
	Mode mode = Mode::Title;			// ゲームモード
	Mode prevMode = Mode::Title;		// 直前のモード記録用 設定画面と元の画面に戻る際に使用
	Mode next = Mode::Title;			// 次のモードへ移行するため
	int x = 0;							// 横スクロール用
	int savedX = 0;						// Setting/Shop 中のカメラ位置保存用
	bool returningFromPause = false;	// 一時停止復帰フラグ
};