#include "UIManager.h"
#include <algorithm> // min/maxのために追加
#include <base/TextureManager.h>
#include <cassert>

UIManager::UIManager() {}

UIManager::~UIManager() {
	// スプライトの解放
	if (hpBarBgSprite_)
		delete hpBarBgSprite_;
	if (hpBarFillSprite_)
		delete hpBarFillSprite_;
	if (hpIconSprite_)
		delete hpIconSprite_;
	if (hpTextSprite_)
		delete hpTextSprite_;
	if (keyboardGuideSprite_)
		delete keyboardGuideSprite_;
	if (controllerGuideSprite_)
		delete controllerGuideSprite_;
}

void UIManager::Initialize() {
	// テクスチャのロード
	hpBarBgHandle_ = TextureManager::Load("ui/hp_bar_bg.png");
	hpBarFillHandle_ = TextureManager::Load("ui/hp_bar_fill.png");
	hpIconHandle_ = TextureManager::Load("ui/hp_icon.png");
	hpTextHandle_ = TextureManager::Load("ui/hp_text.png");
	keyboardGuideHandle_ = TextureManager::Load("ui/keyboard_guide.png");
	controllerGuideHandle_ = TextureManager::Load("ui/controller_guide.png");

	// スプライト生成
	// HPバー背景
	hpBarBgSprite_ = Sprite::Create(hpBarBgHandle_, {20, WinApp::kWindowHeight - 50});
	hpBarBgSprite_->SetSize({300, 30});

	// HPバー前景
	hpBarFillSprite_ = Sprite::Create(hpBarFillHandle_, {20, WinApp::kWindowHeight - 50});
	hpBarFillSprite_->SetSize({300, 30});

	// HPアイコン
	hpIconSprite_ = Sprite::Create(hpIconHandle_, {20, WinApp::kWindowHeight - 80});
	hpIconSprite_->SetSize({50, 50});

	// HPテキスト
	hpTextSprite_ = Sprite::Create(hpTextHandle_, {70, WinApp::kWindowHeight - 75});
	hpTextSprite_->SetSize({50, 30});

	// キーボード操作ガイド
	keyboardGuideSprite_ = Sprite::Create(keyboardGuideHandle_, {20, 20});
	keyboardGuideSprite_->SetSize({300, 150});

	// コントローラー操作ガイド
	controllerGuideSprite_ = Sprite::Create(controllerGuideHandle_, {20, 20});
	controllerGuideSprite_->SetSize({300, 150});
}

void UIManager::Update() {
	// 入力デバイスの検出
	DetectInputDevice();

	// 入力検出タイマーの更新
	inputDetectionTimer_ -= 1.0f / 60.0f;
	if (inputDetectionTimer_ < 0.0f) {
		inputDetectionTimer_ = 0.0f;
	}
}

void UIManager::Draw(int playerHP) {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	// スプライト描画
	Sprite::PreDraw(commandList);

	// HPバー背景描画
	if (hpBarBgSprite_) {
		hpBarBgSprite_->Draw();
	}

	// HPバー前景描画 (HPに応じてサイズ変更)
	if (hpBarFillSprite_) {
		// HPの割合に応じてバーの横幅を変更 (最大HP 200を想定)
		float hpRatio = static_cast<float>(playerHP) / 200.0f;
		// std::clampの代わりに自前で範囲制限
		if (hpRatio < 0.0f)
			hpRatio = 0.0f;
		else if (hpRatio > 1.0f)
			hpRatio = 1.0f;

		// HPの割合に応じて色を変更
		Vector4 barColor = {1.0f, 1.0f, 1.0f, 1.0f};
		if (hpRatio < 0.3f) {
			// 低HP: 赤
			barColor = {1.0f, 0.3f, 0.3f, 1.0f};
		} else if (hpRatio < 0.5f) {
			// 中HP: 黄色
			barColor = {1.0f, 1.0f, 0.3f, 1.0f};
		} else {
			// 高HP: 緑
			barColor = {0.3f, 1.0f, 0.3f, 1.0f};
		}

		hpBarFillSprite_->SetSize({300 * hpRatio, 30});
		hpBarFillSprite_->SetColor(barColor);
		hpBarFillSprite_->Draw();
	}

	// HPアイコン描画
	if (hpIconSprite_) {
		hpIconSprite_->Draw();
	}

	// HPテキスト描画
	if (hpTextSprite_) {
		hpTextSprite_->Draw();
	}

	// 操作ガイド描画
	DrawControlGuide();

	Sprite::PostDraw();
}

void UIManager::DetectInputDevice() {
	// キーボード入力の検出 - GetKeyStateの代わりにDIKキーチェック
	bool anyKeyPressed = false;
	// 一般的なゲーム操作キーをチェック - BYTEタイプを使用
	const BYTE keys[] = {DIK_W, DIK_A, DIK_S, DIK_D, DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_SPACE, DIK_RETURN, DIK_ESCAPE};

	for (BYTE key : keys) {
		if (Input::GetInstance()->PushKey(key)) {
			anyKeyPressed = true;
			break;
		}
	}

	if (anyKeyPressed) {
		keyboardInputDetected_ = true;
		controllerInputDetected_ = false;
		inputDetectionTimer_ = 2.0f; // 2秒間キーボード操作ガイドを表示
		detectedDevice_ = InputDeviceType::Keyboard;
	}

	// コントローラー入力の検出
	Input::GetInstance()->GetJoystickState(0, currentState_);
	if (currentState_.Gamepad.wButtons != 0 || abs(currentState_.Gamepad.sThumbLX) > 8000 || abs(currentState_.Gamepad.sThumbLY) > 8000 || abs(currentState_.Gamepad.sThumbRX) > 8000 ||
	    abs(currentState_.Gamepad.sThumbRY) > 8000) {

		keyboardInputDetected_ = false;
		controllerInputDetected_ = true;
		inputDetectionTimer_ = 2.0f; // 2秒間コントローラー操作ガイドを表示
		detectedDevice_ = InputDeviceType::Controller;
	}

	// 前回の状態を更新
	prevState_ = currentState_;
}

void UIManager::DrawControlGuide() {
	// 入力検出タイマーが0でない場合のみガイドを表示
	if (inputDetectionTimer_ <= 0.0f) {
		return;
	}

	// 検出されたデバイスに応じたガイドを表示
	if (detectedDevice_ == InputDeviceType::Keyboard && keyboardGuideSprite_) {
		keyboardGuideSprite_->Draw();
	} else if (detectedDevice_ == InputDeviceType::Controller && controllerGuideSprite_) {
		controllerGuideSprite_->Draw();
	}
}