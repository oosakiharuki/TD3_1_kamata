#include "TitleScene.h"
#include <base/TextureManager.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	// スプライトの解放
	if (titleLogoSprite_) {
		delete titleLogoSprite_;
		titleLogoSprite_ = nullptr;
	}

	if (startPromptSprite_) {
		delete startPromptSprite_;
		startPromptSprite_ = nullptr;
	}

	if (backgroundSprite_) {
		delete backgroundSprite_;
		backgroundSprite_ = nullptr;
	}

	// トランジション効果の解放
	if (transitionEffect_) {
		delete transitionEffect_;
		transitionEffect_ = nullptr;
	}

	// BGMの停止
	if (titleBGMID_ != -1) {
		audio_->StopWave(titleBGMID_);
		titleBGMID_ = -1;
	}
}

void TitleScene::Initialize() {
	// テクスチャのロード
	titleLogoHandle_ = TextureManager::Load("title_logo.png");
	startPromptHandle_ = TextureManager::Load("press_start.png");
	backgroundHandle_ = TextureManager::Load("title_background.png");

	// 背景スプライト生成
	backgroundSprite_ = Sprite::Create(backgroundHandle_, {0, 0});

	// タイトルロゴスプライト生成
	titleLogoSprite_ = Sprite::Create(titleLogoHandle_, {0, 0});
	// 中央配置
	const int windowWidth = WinApp::kWindowWidth;
	const int windowHeight = WinApp::kWindowHeight;
	titleLogoSprite_->SetPosition({(windowWidth - titleLogoSprite_->GetSize().x) / 2, 100});

	// スタートプロンプトスプライト生成
	startPromptSprite_ = Sprite::Create(startPromptHandle_, {0, 0});
	// 中央下部に配置
	startPromptSprite_->SetPosition({(windowWidth - startPromptSprite_->GetSize().x) / 2, windowHeight - 150});

	// トランジション効果の初期化
	transitionEffect_ = new TransitionEffect();
	transitionEffect_->Initialize();

	// BGM初期化
	audio_ = Audio::GetInstance();
	titleBGMHandle_ = audio_->LoadWave("./sound/title_bgm.wav");
	selectSoundHandle_ = audio_->LoadWave("./sound/select.wav");

	// BGM再生
	audio_->playAudio(titleBGMID_, titleBGMHandle_, true, 0.3f);
}

void TitleScene::Update() {
	// 入力状態の取得
	Input::GetInstance()->GetJoystickState(0, state_);

	// アニメーション更新
	const float deltaTime = 1.0f / 60.0f;
	animTimer_ += deltaTime;

	// 点滅アニメーション
	if (isAlphaIncreasing_) {
		startPromptAlpha_ += deltaTime * 2.0f;
		if (startPromptAlpha_ >= 1.0f) {
			startPromptAlpha_ = 1.0f;
			isAlphaIncreasing_ = false;
		}
	} else {
		startPromptAlpha_ -= deltaTime * 2.0f;
		if (startPromptAlpha_ <= 0.3f) {
			startPromptAlpha_ = 0.3f;
			isAlphaIncreasing_ = true;
		}
	}

	// アルファ値の更新
	startPromptSprite_->SetColor({1.0f, 1.0f, 1.0f, startPromptAlpha_});

	// トランジション効果の更新
	if (transitionEffect_) {
		transitionEffect_->Update();

		// トランジション完了チェック
		if (isTransitioning_ && transitionEffect_->IsCompleted()) {
			isTransitionToGameScene_ = true;
			isTransitioning_ = false;
		}
	}

	// 入力チェック（SPACE or Aボタン）でゲーム開始
	if (!isTransitioning_ && (Input::GetInstance()->TriggerKey(DIK_SPACE) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)))) {

		// 選択音再生
		audio_->playAudio(selectSoundID_, selectSoundHandle_, false, 0.5f);

		// フェードアウト開始
		transitionEffect_->Start(TransitionType::FadeOut, 1.0f);
		isTransitioning_ = true;
	}

	// 前回の入力状態を更新
	Input::GetInstance()->GetJoystickStatePrevious(0, preState_);
}

void TitleScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	// 背景描画
	Sprite::PreDraw(commandList);

	// 背景描画
	if (backgroundSprite_) {
		backgroundSprite_->Draw();
	}

	// タイトルロゴ描画
	if (titleLogoSprite_) {
		titleLogoSprite_->Draw();
	}

	// スタートプロンプト描画
	if (startPromptSprite_) {
		startPromptSprite_->Draw();
	}

	Sprite::PostDraw();

	// 3Dモデル描画処理があれば実行
	Model::PreDraw(commandList);
	Model::PostDraw();

	// トランジション効果は独自のPreDraw/PostDrawを持つため、
	// 別途描画する（TransitionEffect.cppを修正済み）
	if (transitionEffect_) {
		transitionEffect_->Draw();
	}
}