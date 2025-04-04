#pragma once
#include "KamataEngine.h"
#include "TransitionEffect.h"

using namespace KamataEngine;

class TitleScene {
public:
	TitleScene();
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	// シーン遷移状態の取得
	bool IsTransitionToGameScene() const { return isTransitionToGameScene_; }
	void ResetTransitionFlag() { isTransitionToGameScene_ = false; }

private:
	// スプライト関連
	Sprite* titleLogoSprite_ = nullptr;
	Sprite* startPromptSprite_ = nullptr;
	Sprite* backgroundSprite_ = nullptr;

	// テクスチャハンドル
	uint32_t titleLogoHandle_ = 0;
	uint32_t startPromptHandle_ = 0;
	uint32_t backgroundHandle_ = 0;

	// アニメーション用
	float startPromptAlpha_ = 1.0f;
	float animTimer_ = 0.0f;
	bool isAlphaIncreasing_ = false;

	// 入力状態
	XINPUT_STATE state_ = {}, preState_ = {};

	// 遷移フラグ
	bool isTransitionToGameScene_ = false;

	// トランジション効果
	TransitionEffect* transitionEffect_ = nullptr;
	bool isTransitioning_ = false;

	// BGM
	Audio* audio_ = nullptr;
	int titleBGMHandle_ = 0;
	int titleBGMID_ = -1;

	// 選択音
	int selectSoundHandle_ = 0;
	int selectSoundID_ = -1;
};