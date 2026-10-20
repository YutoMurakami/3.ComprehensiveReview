#include "stage.h"

// コントラクタ
Stage::Stage()
{
	InitView(mode);
	InitStage(mode);
}

// デストラクタ
Stage::~Stage()
{
}

// UI用
void Stage::InitView(Mode mode)
{
	this->mode = mode;
	switch (mode)
	{
	case Mode::Title:
		ReadBmp("Title.bmp");
		w = WND_W; h = WND_H;
		break;
	case Mode::Setting:
		ReadBmp("ShopUI.bmp");
		w = WND_W; h = WND_H;
		break;
	case Mode::shop:
		ReadBmp("ShopUI.bmp");
		w = WND_W; h = WND_H;
		break;
	case Mode::BossTalk:
		ReadBmp("BossStage.bmp");
		w = WND_W; h = WND_H;
		break;
	case Mode::Over:
		ReadBmp("gameover.bmp");
		w = WND_W; h = WND_H;
		break;
	case Mode::Clear:
		ReadBmp("gameclear.bmp");
		w = WND_W; h = WND_H;
		break;
	case Mode::End:
		ReadBmp("BlackScreen.bmp");
		w = WND_W; h = WND_H;
		break;
	}
}

// 初期化関数(戦闘等のステージ用)
void Stage::InitStage(Mode mode)
{
	this->mode = mode;
	switch (mode)
	{
	case Mode::Tutorial:
		ReadBmp("TutorialStage.bmp");
		w = 2560; h = WND_H;
		x = 0;
		break;
	case Mode::Stage1:
		ReadBmp("RoadStage1.bmp");
		w = 2560; h = WND_H;
		x = 0;
		break;
	case Mode::Stage2:
		ReadBmp("CyberStage2.bmp");
		w = 2560; h = WND_H;
		x = 0;
		break;
	case Mode::Stage3:
		ReadBmp("UndergroundStage3.bmp");
		w = 3840; h = WND_H;
		x = 0;
		break;
	case Mode::SecretStage:
		ReadBmp("SecretStage.bmp");
		w = 5120; h = WND_H;
		x = 0;
		break;
	case Mode::Boss:
		ReadBmp("BossStage.bmp");
		w = WND_W; h = WND_H;
		x = 0;
		break;
	}
}

// 操作関数
void Stage::Ctrl(const Key& key)
{
	// ゲームプレイ中かどうか
	bool isGamePlay = (mode == Mode::Tutorial || mode == Mode::Stage1 ||
		mode == Mode::Stage2 || mode == Mode::Stage3 ||
		mode == Mode::SecretStage || mode == Mode::Boss);

	// ESCキー / Startボタン: ゲーム中→Setting / Setting・Shop→元のステージへ
	if (key.Pressed(VK_ESCAPE) || key.JoyPressed(7)) {
		if (isGamePlay) {
			// カメラ位置を保存
			savedX = x;
			prevMode = mode;
			next = Mode::Setting;
		}
		else if (mode == Mode::Setting || mode == Mode::shop) next = prevMode;
	}

	// Bボタン: Setting・Shop から元の画面へ戻る（ゲーム中は無効）
	if ((mode == Mode::Setting || mode == Mode::shop) && key.JoyPressed(1)) {
		next = prevMode;
	}

	// 遷移
	if (next != mode)
	{
		Mode from = mode;
		mode = next;
		returningFromPause = false;

		if (mode == Mode::Setting || mode == Mode::shop) {
			// 一時停止画面へ移行（RequestMode 経由の場合もカバー）
			savedX = x;			// カメラ位置を保存
			prevMode = from;
			InitView(mode);
		}
		else if (from == Mode::Setting || from == Mode::shop) {
			// 一時停止から復帰
			if (mode == Mode::Title || mode == Mode::BossTalk ||
				mode == Mode::Over || mode == Mode::Clear || mode == Mode::End)
				// UIモードへの復帰はInitView（InitStageにTitleケースなし）
				InitView(mode);
			else {
				// ゲームステージへの復帰: 画像再読み込み後にカメラ位置を復元
				InitStage(mode);
				x = savedX;						// カメラ位置を上書き
				returningFromPause = true;
			}
		}
		else if (mode == Mode::Title ||
			mode == Mode::BossTalk ||
			mode == Mode::Over ||
			mode == Mode::Clear ||
			mode == Mode::End) InitView(mode);
		else InitStage(mode);
	}
}

// 表示関数
void Stage::Draw()
{
	if ((mode == Mode::Tutorial) ||
		(mode == Mode::Stage1) ||
		(mode == Mode::Stage2) ||
		(mode == Mode::Stage3) ||
		(mode == Mode::SecretStage) ||
		(mode == Mode::Boss))
		BBlt(-x, 0, 0, 0);
	else
	{
		if (mode == Mode::Title)
		{
			static int driftFrame = 0;
			driftFrame++;

			double rad = driftFrame * 0.015;     // ゆっくり
			int driftY = (int)(sin(rad) * 3);   // ±3px

			BBlt(0, driftY, 0, 0);
		}
		else
		{
			BBlt(0, 0, 0, 0);
		}
	}
}