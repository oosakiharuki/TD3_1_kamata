#include "GameScene.h"
#include "TitleScene.h"
#include <KamataEngine.h>
using namespace KamataEngine;

// シーン管理用の列挙型
enum class Scene { Title, Game };

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	// シーン管理変数
	Scene currentScene = Scene::Title;

	// 各シーンの初期化
	TitleScene* titleScene = new TitleScene();
	titleScene->Initialize();

	// ゲームシーンはこの時点では生成しない（必要になったときに生成）
	GameScene* gameScene = nullptr;

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();

		// シーン切り替え処理
		if (currentScene == Scene::Title && titleScene->IsTransitionToGameScene()) {
			// タイトルからゲームへ
			if (gameScene == nullptr) {
				// ゲームシーンの初期化を遅延して行う
				gameScene = new GameScene();
				gameScene->Initialize();
			}
			titleScene->ResetTransitionFlag();
			currentScene = Scene::Game;
		} else if (currentScene == Scene::Game && gameScene != nullptr && gameScene->IsTransitionToTitle()) {
			// ゲームからタイトルへ
			delete titleScene; // 古いタイトルシーンがあれば削除
			titleScene = new TitleScene();
			titleScene->Initialize();
			gameScene->ResetTransitionFlag();
			currentScene = Scene::Title;
		}

		// 現在のシーンを更新
		try {
			if (currentScene == Scene::Title) {
				titleScene->Update();
			} else if (gameScene != nullptr) {
				gameScene->Update();
			}
		} catch (const std::exception& e) {
			// 例外をキャッチしてエラーメッセージを表示
			OutputDebugStringA("シーン更新中に例外が発生しました: ");
			OutputDebugStringA(e.what());
			OutputDebugStringA("\n");
		}

		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// 現在のシーンを描画
		try {
			if (currentScene == Scene::Title) {
				titleScene->Draw();
			} else if (gameScene != nullptr) {
				gameScene->Draw();
			}
		} catch (const std::exception& e) {
			// 例外をキャッチしてエラーメッセージを表示
			OutputDebugStringA("シーン描画中に例外が発生しました: ");
			OutputDebugStringA(e.what());
			OutputDebugStringA("\n");
		}

		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各シーンを解放
	delete titleScene;
	if (gameScene != nullptr) {
		delete gameScene;
	}

	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}