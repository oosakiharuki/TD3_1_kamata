#pragma once
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

// トランジション効果の種類をシンプルに
enum class TransitionType {
	None,   // トランジションなし
	FadeIn, // フェードイン（透明 → 不透明）
	FadeOut // フェードアウト（不透明 → 透明）
};

class TransitionEffect {
public:
	TransitionEffect();
	~TransitionEffect();

	// 初期化
	void Initialize();

	// トランジション開始
	void Start(TransitionType type, float duration = 1.0f);

	// 更新
	void Update();

	// 描画
	void Draw();

	// トランジション中かどうか
	bool IsTransitioning() const { return isTransitioning_; }

	// トランジションが完了したかどうか
	bool IsCompleted() const { return isCompleted_; }

	// トランジション完了をリセット
	void ResetCompleted() { isCompleted_ = false; }

private:
	// トランジション用スプライト生成
	void CreateSprites();

	bool isTransitioning_; // トランジション中フラグ
	bool isCompleted_;     // 完了フラグ
	TransitionType type_;  // トランジションの種類
	float timer_;          // 経過時間
	float duration_;       // トランジション時間
	float progress_;       // 進行度（0.0f〜1.0f）

	// フェード用
	Sprite* fadeSprite_;         // フェード用スプライト
	uint32_t fadeTextureHandle_; // 白テクスチャハンドル
};