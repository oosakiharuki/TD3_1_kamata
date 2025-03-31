#include "TransitionEffect.h"
#include <base/TextureManager.h>

TransitionEffect::TransitionEffect()
    : isTransitioning_(false), isCompleted_(false), type_(TransitionType::None), timer_(0.0f), duration_(1.0f), progress_(0.0f), fadeSprite_(nullptr), leftDoorSprite_(nullptr),
      rightDoorSprite_(nullptr), circleSprite_(nullptr), fadeTextureHandle_(0), doorTextureHandle_(0), circleTextureHandle_(0) {}

TransitionEffect::~TransitionEffect() {
	// スプライトの解放
	if (fadeSprite_) {
		delete fadeSprite_;
	}
	if (leftDoorSprite_) {
		delete leftDoorSprite_;
	}
	if (rightDoorSprite_) {
		delete rightDoorSprite_;
	}
	if (circleSprite_) {
		delete circleSprite_;
	}
}

void TransitionEffect::Initialize() {
	// テクスチャのロード
	fadeTextureHandle_ = TextureManager::Load("white1x1.png");

	// ドア用テクスチャ - 既存の白テクスチャを代用
	doorTextureHandle_ = TextureManager::Load("white1x1.png");

	// 円形ワイプ用テクスチャ - 既存の白テクスチャを代用
	circleTextureHandle_ = TextureManager::Load("white1x1.png");

	// トランジション用スプライトの作成
	CreateSprites();
}

void TransitionEffect::CreateSprites() {
	// ウィンドウサイズを取得
	const int windowWidth = WinApp::kWindowWidth;
	const int windowHeight = WinApp::kWindowHeight;

	// フェード用スプライト生成
	fadeSprite_ = Sprite::Create(fadeTextureHandle_, {0, 0});
	fadeSprite_->SetSize({static_cast<float>(windowWidth), static_cast<float>(windowHeight)});
	fadeSprite_->SetColor({0, 0, 0, 0}); // 透明に初期化

	// ドア用スプライト生成
	leftDoorSprite_ = Sprite::Create(doorTextureHandle_, {0, 0});
	leftDoorSprite_->SetSize({static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight)});
	leftDoorSprite_->SetColor({0, 0, 0, 1.0f});

	rightDoorSprite_ = Sprite::Create(doorTextureHandle_, {static_cast<float>(windowWidth) / 2, 0});
	rightDoorSprite_->SetSize({static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight)});
	rightDoorSprite_->SetColor({0, 0, 0, 1.0f});

	// 円形ワイプ用スプライト生成
	circleSprite_ = Sprite::Create(circleTextureHandle_, {static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight) / 2});
	circleSprite_->SetAnchorPoint({0.5f, 0.5f}); // 中心を原点に
	circleSprite_->SetSize({0, 0});              // 最初はサイズ0
	circleSprite_->SetColor({0, 0, 0, 1.0f});
}

void TransitionEffect::Start(TransitionType type, float duration) {
	// トランジション開始
	isTransitioning_ = true;
	isCompleted_ = false;
	type_ = type;
	timer_ = 0.0f;
	duration_ = duration;
	progress_ = 0.0f;

	// トランジションタイプに応じた初期化
	switch (type_) {
	case TransitionType::Fade:
		fadeSprite_->SetColor({0, 0, 0, 0}); // 透明から始める
		break;
	case TransitionType::DoorOpen:
		// ドアが閉じた状態から始める（中央に寄せる）
		{
			const int windowWidth = WinApp::kWindowWidth;
			leftDoorSprite_->SetPosition({0, 0});
			rightDoorSprite_->SetPosition({static_cast<float>(windowWidth) / 2, 0});
		}
		break;
	case TransitionType::CircleWipe:
		// 円形ワイプは最初は小さい円から
		circleSprite_->SetSize({0, 0});
		break;
	default:
		break;
	}
}

void TransitionEffect::Update() {
	if (!isTransitioning_) {
		return;
	}

	// 時間経過
	const float deltaTime = 1.0f / 60.0f; // 60FPS想定
	timer_ += deltaTime;

	// 進行度を計算（0.0f〜1.0f）
	progress_ = timer_ / duration_;
	if (progress_ >= 1.0f) {
		progress_ = 1.0f;
		isTransitioning_ = false;
		isCompleted_ = true;
	}

	// トランジションタイプに応じた更新
	switch (type_) {
	case TransitionType::Fade:
		// フェードイン（透明→不透明）
		fadeSprite_->SetColor({0, 0, 0, progress_});
		break;
	case TransitionType::DoorOpen:
		// ドアが開いていく演出（左右に移動）
		{
			const int windowWidth = WinApp::kWindowWidth;
			const float moveAmount = static_cast<float>(windowWidth) / 2 * progress_;
			leftDoorSprite_->SetPosition({-moveAmount, 0});
			rightDoorSprite_->SetPosition({static_cast<float>(windowWidth) / 2 + moveAmount, 0});
		}
		break;
	case TransitionType::CircleWipe:
		// 円が広がる演出
		{
			// 画面対角線の長さを計算して使用
			const float windowDiagonal = static_cast<float>(sqrt(pow(WinApp::kWindowWidth, 2) + pow(WinApp::kWindowHeight, 2)));
			const float size = windowDiagonal * progress_;
			circleSprite_->SetSize({size, size});
		}
		break;
	default:
		break;
	}
}

void TransitionEffect::Draw() {
	if (!isTransitioning_ && !isCompleted_) {
		return;
	}

	// トランジションタイプに応じた描画
	switch (type_) {
	case TransitionType::Fade:
		DrawFadeTransition();
		break;
	case TransitionType::DoorOpen:
		DrawDoorTransition();
		break;
	case TransitionType::CircleWipe:
		DrawCircleWipeTransition();
		break;
	default:
		break;
	}
}

void TransitionEffect::DrawFadeTransition() {
	if (fadeSprite_) {
		fadeSprite_->Draw();
	}
}

void TransitionEffect::DrawDoorTransition() {
	if (leftDoorSprite_ && rightDoorSprite_) {
		leftDoorSprite_->Draw();
		rightDoorSprite_->Draw();
	}
}

void TransitionEffect::DrawCircleWipeTransition() {
	if (circleSprite_) {
		circleSprite_->Draw();
	}
}