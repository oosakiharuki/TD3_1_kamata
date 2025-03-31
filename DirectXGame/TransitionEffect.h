#pragma once
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

// トランジション効果の種類
enum class TransitionType {
	None,      // トランジションなし
	Fade,      // フェード
	DoorOpen,  // ドアが開くような効果
	CircleWipe // 円形ワイプ
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

	// フェードトランジションの描画
	void DrawFadeTransition();

	// ドア開閉トランジションの描画
	void DrawDoorTransition();

	// 円形ワイプトランジションの描画
	void DrawCircleWipeTransition();

private:
	bool isTransitioning_; // トランジション中フラグ
	bool isCompleted_;     // 完了フラグ
	TransitionType type_;  // トランジションの種類
	float timer_;          // 経過時間
	float duration_;       // トランジション時間
	float progress_;       // 進行度（0.0f〜1.0f）

	// フェード用
	Sprite* fadeSprite_;         // フェード用スプライト
	uint32_t fadeTextureHandle_; // 白テクスチャハンドル

	// ドア用
	Sprite* leftDoorSprite_;     // 左ドア用スプライト
	Sprite* rightDoorSprite_;    // 右ドア用スプライト
	uint32_t doorTextureHandle_; // ドアテクスチャハンドル

	// 円形ワイプ用
	Sprite* circleSprite_;         // 円形ワイプ用スプライト
	uint32_t circleTextureHandle_; // 円形テクスチャハンドル
};