#include "TransitionEffect.h"
#include <base/TextureManager.h>

TransitionEffect::TransitionEffect()
    : isTransitioning_(false), isCompleted_(false), type_(TransitionType::None), timer_(0.0f), duration_(1.0f), progress_(0.0f), fadeSprite_(nullptr), fadeTextureHandle_(0) {}

TransitionEffect::~TransitionEffect() {
	// スプライトの解放
	if (fadeSprite_) {
		delete fadeSprite_;
	}
}

void TransitionEffect::Initialize() {
	// テクスチャのロード
	fadeTextureHandle_ = TextureManager::Load("white1x1.png");
	// トランジション用スプライトの作成
	CreateSprites();
}

void TransitionEffect::CreateSprites() {
	// ウィンドウサイズを取得
	const int windowWidth = WinApp::kWindowWidth;
	const int windowHeight = WinApp::kWindowHeight;

	// フェード用スプライト生成
	if (fadeSprite_ != nullptr) {
		delete fadeSprite_;
		fadeSprite_ = nullptr;
	}

	// 正しく初期化されているか確認
	if (fadeTextureHandle_ == 0) {
		// テクスチャハンドルが無効な場合、再度ロード
		fadeTextureHandle_ = TextureManager::Load("white1x1.png");
	}

	// スプライト生成
	fadeSprite_ = Sprite::Create(fadeTextureHandle_, {0, 0});
	if (fadeSprite_) {
		fadeSprite_->SetSize({static_cast<float>(windowWidth), static_cast<float>(windowHeight)});
		fadeSprite_->SetColor({0, 0, 0, 0}); // 透明に初期化
	}
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
	case TransitionType::FadeIn:
		// フェードイン（黒いスクリーンから始める）
		fadeSprite_->SetColor({0, 0, 0, 1.0f});
		break;
	case TransitionType::FadeOut:
		// フェードアウト（透明から始める）
		fadeSprite_->SetColor({0, 0, 0, 0.0f});
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
	case TransitionType::FadeIn:
		// フェードイン（不透明→透明）
		fadeSprite_->SetColor({0, 0, 0, 1.0f - progress_});
		break;
	case TransitionType::FadeOut:
		// フェードアウト（透明→不透明）
		fadeSprite_->SetColor({0, 0, 0, progress_});
		break;
	default:
		break;
	}
}

void TransitionEffect::Draw() {
	// 描画が不要な場合は早期リターン
	if (!isTransitioning_ && !isCompleted_) {
		return;
	}

	// 必ずコマンドリストを取得してから描画する
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	// Sprite::PreDrawを呼び出し側に任せず、自分で管理する
	Sprite::PreDraw(commandList);

	// フェードスプライトの描画
	if (fadeSprite_) {
		// 透明度が0より大きい場合のみ描画
		Vector4 color = fadeSprite_->GetColor();
		if (color.w > 0.0f) {
			fadeSprite_->Draw();
		}
	}

	// Sprite::PostDrawも呼び出す
	Sprite::PostDraw();
}