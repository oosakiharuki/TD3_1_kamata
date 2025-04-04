#include "GameScene.h"
#include "AABB.h"
#include "Minimap.h"
#include <base/TextureManager.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

#pragma region コンストラクタとデストラクタ
GameScene::GameScene() {}

GameScene::~GameScene() { Finalize(); }
#pragma endregion コンストラクタとデストラクタ

#pragma region 終了処理
void GameScene::Finalize() {
	delete player_;
	delete mapLoader_;

	if (enemyLoader_) {
		delete enemyLoader_;
		enemyLoader_ = nullptr;
	}

	if (stage) {
		delete stage;
		stage = nullptr;
	}

	delete block_;
	delete modelBlock_;

	delete ghostBlock_;
	delete modelGhostBlock_;

	delete skydome_;
	delete modelSkydome_;

	// トランジション効果の解放
	if (transitionEffect_) {
		delete transitionEffect_;
		transitionEffect_ = nullptr;
	}

	// ミニマップの解放
	if (minimap_) {
		delete minimap_;
		minimap_ = nullptr;
	}

	// ゴールガイドの解放
	if (goalGuideSprite_) {
		delete goalGuideSprite_;
		goalGuideSprite_ = nullptr;
	}

	// BGMの停止
	if (stageBGMID_ != -1) {
		audio_->StopWave(stageBGMID_);
		stageBGMID_ = -1;
	}
}
#pragma endregion 終了処理

#pragma region 初期化処理
void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	camera_.Initialize();

	block_ = new Block();
	modelBlock_ = Model::Create();
	// 引数の順序を修正して、Block::Initの定義に合わせる
	block_->Init(&camera_);

	ghostBlock_ = new GhostBlock();
	modelGhostBlock_ = Model::Create();
	ghostBlock_->Init(&camera_);

	// Playerの生成と初期化
	player_ = new Player();
	player_->Init(&camera_);

	stage = Model::CreateFromOBJ("stage" + std::to_string(currentStage_), true);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("sky", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// 障害物情報の読み込み - 必ず先に行う
	OutputDebugStringA("初期化: ステージAABBの読み込み開始\n");
	LoadStage("Resources/stage1/stage1.obj");

	// デバッグ出力
	int totalObstacles = 0;
	for (const auto& list : allObstacles_) {
		totalObstacles += static_cast<int>(list.size());
	}
	char buffer[256];
	sprintf_s(buffer, "初期化: 読み込んだ障害物数: %d\n", totalObstacles);
	OutputDebugStringA(buffer);

	// MapLoaderの初期化
	mapLoader_ = new MapLoader();
	std::string objectsFile = "Resources/objects" + std::to_string(currentStage_) + ".csv";
	if (mapLoader_->LoadMapData(objectsFile)) {
		mapLoader_->CreateObjects(&camera_, player_);
	}

	// EnemyLoaderの生成と初期化
	enemyLoader_ = new EnemyLoader();
	std::string enemiesFile = "Resources/enemies" + std::to_string(currentStage_) + ".csv";
	// CSVから敵の情報を読み込み
	if (enemyLoader_->LoadEnemyData(enemiesFile)) {
		// 敵を生成
		enemyLoader_->CreateEnemies(&camera_, player_, allObstacles_);
	}

	// 各種敵リストをプレイヤーに設定
	player_->SetEnemyList(enemyLoader_->GetEnemyList());

	// キャノン敵への参照をプレイヤーに設定
	if (enemyLoader_ && !enemyLoader_->GetCannonEnemyList().empty()) {
		player_->SetCannon(enemyLoader_->GetCannonEnemyList()[0]); // 一番最初のキャノン敵を設定
	}

	// バネ敵への参照をプレイヤーに設定
	if (enemyLoader_) {
		player_->SetSpringEnemies(enemyLoader_->GetSpringEnemyList());
	}

	// プレイヤーにブロックリストを設定（更新: 単一ブロックではなくリスト全体を渡す）
	const std::vector<Block*>& blocks = mapLoader_->GetBlockList();
	player_->SetBlocks(blocks);

	// 障害物リストを Player にセット - もう一度確実に設定
	for (const auto& obstacles : allObstacles_) {
		if (!obstacles.empty()) {
			player_->SetObstacleList(obstacles);
		}
	}

	// プレイヤーにGoalへの参照を設定
	if (mapLoader_ && mapLoader_->GetGoal()) {
		player_->SetGoal(mapLoader_->GetGoal());
	}

	// トランジション効果の初期化
	transitionEffect_ = new TransitionEffect();
	transitionEffect_->Initialize();

	// フェードインで開始
	transitionEffect_->Start(TransitionType::FadeIn, 1.0f);
	transitionState_ = TransitionState::FadeIn;

	// ミニマップの初期化
	minimap_ = new Minimap();
	minimap_->Initialize(player_, mapLoader_, enemyLoader_, allObstacles_);

	// ゴールガイドテクスチャのロード
	goalGuideHandle_ = TextureManager::Load("ui/goal_guide.png");

	// ゴールガイドスプライトの生成
	goalGuideSprite_ = Sprite::Create(goalGuideHandle_, {(WinApp::kWindowWidth - 300) / 2.0f, 150.0f});
	goalGuideSprite_->SetSize({300, 100});

	// ステージ1のBGM読み込みと再生
	stageBGMHandle_ = audio_->LoadWave("./sound/stage1.wav");
	audio_->playAudio(stageBGMID_, stageBGMHandle_, true, 0.25f);
}
#pragma endregion 初期化処理

#pragma region 更新処理
void GameScene::Update() {
	// 入力状態の取得
	Input::GetInstance()->GetJoystickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);

	// リスタート処理
	if (Input::GetInstance()->PushKey(DIK_R) || ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) && (preState.Gamepad.wButtons & XINPUT_GAMEPAD_Y))) {
		longPress -= 1.0f / 60.0f;
	} else {
		longPress = RestartTimer;
	}
	// 0になった時リスタート / 押しなおさないと更新されなくする
	if (longPress < 0 && longPress > -0.017f) {
		Finalize();
		Initialize();
		return; // 初期化後は他の処理をスキップ
	}

	// トランジション処理の更新
	UpdateTransition();

	// ステージ切り替え中のみスキップ（TransitionState::ChangeStageの状態を新たに追加）
	// フェードイン・フェードアウト中は更新を続ける
	if (transitionState_ == TransitionState::ChangeStage || transitionState_ == TransitionState::ToTitle) {
		return;
	}

	// ゴールからタイトルへの遷移チェック
	CheckReturnToTitle();

	// MapLoaderが管理するGoalの状態をチェック
	if (mapLoader_ && mapLoader_->GetGoal() && mapLoader_->GetGoal()->IsClear()) {
		// ゴールクリア時にガイドを表示
		showGoalGuide_ = true;
	} else {
		showGoalGuide_ = false;
	}

	// プレイヤーの更新
	player_->Update();

	// EnemyLoaderの更新
	if (enemyLoader_) {
		enemyLoader_->Update();
	}

	// MapLoaderの更新
	if (mapLoader_) {
		mapLoader_->Update();
	}

	// スプリング敵の更新
	if (enemyLoader_) {
		const auto& springEnemies = enemyLoader_->GetSpringEnemyList();
		for (auto& springEnemy : springEnemies) {
			if (springEnemy) {
				springEnemy->Update();
			}
		}
	}

	// ブロックの更新
	block_->Update();
	ghostBlock_->Update();

	// UIの更新処理をここから削除（Draw関数内に移動）
	// player_->DrawUI(); ← この行を削除

	// 天球の更新
	skydome_->Update();

	// ミニマップの更新
	if (minimap_) {
		minimap_->Update();
	}

	// ドアが開いたら次のステージへトランジション開始
	// フェードアウト中でなければトランジション開始
	if (mapLoader_ && mapLoader_->IsDoorOpened() && transitionState_ == TransitionState::None) {
		// 次のステージ番号を計算
		int nextStage = currentStage_ + 1;

		// トランジション開始
		StartTransitionToStage(nextStage);
	}
}
#pragma endregion 更新処理

#pragma region 描画処理
void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();

	// モデル描画
	Model::PreDraw(commandList);

	stage->Draw(worldTransform_, camera_);

	player_->Draw();

	// EnemyLoaderで読み込んだ敵の描画
	if (enemyLoader_) {
		enemyLoader_->Draw();
	}

	// MapLoaderで読み込んだオブジェクト（鍵、ドア、ブロック、ゴール）の描画
	if (mapLoader_) {
		mapLoader_->Draw();
	}

	ghostBlock_->Draw();

	skydome_->Draw();

	Model::PostDraw();

	// UIスプライト描画 - ここでゴールクリアテキストも描画
	Sprite::PreDraw(commandList);

	// UI描画を追加（元々Update内で呼び出していた）
	if (player_) {
		player_->DrawUI();
	}

	// MapLoaderのスプライト描画処理を呼び出す
	if (mapLoader_) {
		mapLoader_->DrawSprites(commandList);
	}

	// ゴールガイドを表示（ゴールクリア時のみ）
	if (showGoalGuide_ && goalGuideSprite_) {
		goalGuideSprite_->Draw();
	}

	// ミニマップの描画
	if (minimap_) {
		minimap_->Draw();
	}

	Sprite::PostDraw();

	// トランジション効果は独自のPreDraw/PostDrawを持つため、
	// 別途描画する（TransitionEffect.cppを修正済み）
	if (transitionEffect_) {
		transitionEffect_->Draw();
	}
}
#pragma endregion 描画処理

#pragma region タイトル遷移処理
// 追加: ゴールクリア時のタイトル遷移チェック
void GameScene::CheckReturnToTitle() {
	// ゴールクリア状態かつSPACEキーまたはXボタンでタイトルへ
	if (mapLoader_ && mapLoader_->GetGoal() && mapLoader_->GetGoal()->IsClear()) {
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_X))) {

			// タイトルへ遷移開始
			StartTransitionToTitle();
		}
	}
}

// タイトルへ遷移開始
void GameScene::StartTransitionToTitle() {
	// 既にトランジション中なら何もしない
	if (transitionState_ != TransitionState::None && transitionState_ != TransitionState::FadeIn) {
		return;
	}

	// トランジション状態を設定
	transitionState_ = TransitionState::ToTitle;

	// フェードアウトを開始
	transitionEffect_->Start(TransitionType::FadeOut, 1.0f);
}
#pragma endregion タイトル遷移処理

#pragma region ステージ変更処理
void GameScene::ChangeStage(int nextStage) {
	// 現在のBGMを停止
	if (stageBGMID_ != -1) {
		audio_->StopWave(stageBGMID_);
		stageBGMID_ = -1; // リセット
	}

	OutputDebugStringA("ChangeStage: ステージ変更処理開始\n");

	// 当たり判定情報をクリア
	allObstacles_.clear();
	Command.str("");
	Command.clear();

	currentStage_ = nextStage;
	stage = Model::CreateFromOBJ("stage" + std::to_string(currentStage_), true);

	// プレイヤーと敵の障害物リストをクリア
	if (player_) {
		player_->ClearObstacleList();
	}

	if (enemyLoader_) {
		for (auto& enemy : enemyLoader_->GetEnemyList()) {
			if (enemy) {
				enemy->ClearObstacleList();
			}
		}
	}

	// 新しい障害物データをロード - CSVロード前に行う
	std::string stageFile = "Resources/stage" + std::to_string(currentStage_) + "/stage" + std::to_string(currentStage_) + ".obj";
	OutputDebugStringA(("ChangeStage: " + stageFile + " を読み込み中\n").c_str());
	LoadStage(stageFile);

	// 障害物データの状態を確認
	int totalObstacles = 0;
	for (const auto& list : allObstacles_) {
		totalObstacles += static_cast<int>(list.size());
	}
	char buffer[256];
	sprintf_s(buffer, "ChangeStage: ステージ%dの障害物数: %d\n", currentStage_, totalObstacles);
	OutputDebugStringA(buffer);

	// 障害物がゼロの場合はダミーの地面を追加
	if (totalObstacles == 0) {
		OutputDebugStringA("警告: 障害物が読み込めませんでした。ダミーの地面を追加します。\n");
		// ダミーの地面を追加
		Vector3 groundMin = {-1000.0f, -10.0f, -1000.0f};
		Vector3 groundMax = {1000.0f, -5.0f, 1000.0f};

		if (allObstacles_.empty()) {
			allObstacles_.emplace_back();
		}

		AABB ground;
		ground.min = groundMin;
		ground.max = groundMax;
		allObstacles_[0].push_back(ground);
	}

	// マップオブジェクトをロード
	if (mapLoader_) {
		std::string objectsFile = "Resources/objects" + std::to_string(currentStage_) + ".csv";
		mapLoader_->ChangeStage(currentStage_, &camera_, player_);
		if (mapLoader_->LoadMapData(objectsFile)) {
			mapLoader_->CreateObjects(&camera_, player_);
		}
	}

	// プレイヤーに新しい障害物リストを設定 - 必ず障害物データロード後に行う
	if (player_) {
		for (const auto& obstacles : allObstacles_) {
			if (!obstacles.empty()) {
				player_->SetObstacleList(obstacles);
			}
		}
	}

	// 敵の当たり判定も再設定
	if (enemyLoader_) {
		delete enemyLoader_;
		enemyLoader_ = nullptr;
	}

	enemyLoader_ = new EnemyLoader();
	std::string enemiesFile = "Resources/enemies" + std::to_string(currentStage_) + ".csv";

	if (enemyLoader_->LoadEnemyData(enemiesFile)) {
		// 敵生成時に当たり判定情報を渡す
		enemyLoader_->CreateEnemies(&camera_, player_, allObstacles_);
	}

	// 敵を正しくセットアップ
	if (player_) {
		player_->SetEnemyList(enemyLoader_->GetEnemyList());

		if (!enemyLoader_->GetCannonEnemyList().empty()) {
			player_->SetCannon(enemyLoader_->GetCannonEnemyList()[0]);
		}

		player_->SetSpringEnemies(enemyLoader_->GetSpringEnemyList());
	}

	// プレイヤーにブロックリストを設定
	const std::vector<Block*>& blocks = mapLoader_->GetBlockList();
	if (player_) {
		player_->SetBlocks(blocks);
	}

	// プレイヤーにGoalへの参照を再設定
	if (mapLoader_ && mapLoader_->GetGoal()) {
		player_->SetGoal(mapLoader_->GetGoal());
	}

	// ミニマップを再初期化 - 障害物データの更新後に行う
	if (minimap_) {
		delete minimap_;
		minimap_ = nullptr;
	}
	minimap_ = new Minimap();
	minimap_->Initialize(player_, mapLoader_, enemyLoader_, allObstacles_);

	// 次のステージに応じてBGMを再生
	std::string stageBGMPath = "./sound/stage" + std::to_string(currentStage_) + ".wav";
	stageBGMHandle_ = audio_->LoadWave(stageBGMPath.c_str());
	audio_->playAudio(stageBGMID_, stageBGMHandle_, true, 0.25f);

	OutputDebugStringA("ChangeStage: ステージ変更処理完了\n");
}

#pragma endregion ステージ変更処理

#pragma region 障害物関連処理
void GameScene::AddObstacle(std::vector<std::vector<AABB>>& allObstacles, const Vector3& min, const Vector3& max) {
	// 極端に小さいAABBや無効なAABBをフィルタリング
	const float minSize = 0.001f;
	if (min.x >= max.x - minSize || min.y >= max.y - minSize || min.z >= max.z - minSize) {
		return; // 無効なAABBは追加しない
	}

	AABB obstacle;
	obstacle.min = min;
	obstacle.max = max;

	if (allObstacles.empty() || allObstacles.back().size() >= 500) {
		allObstacles.emplace_back();
	}

	allObstacles.back().push_back(obstacle);
}

void GameScene::LoadStage(std::string objFile) {
	// ステージデータの読み込み
	std::ifstream file;
	file.open(objFile);

	if (!file.is_open()) {
		char buffer[256];
		sprintf_s(buffer, "エラー: ファイルを開けません: %s\n", objFile.c_str());
		OutputDebugStringA(buffer);

		// ファイルが開けない場合はダミーの地面を作成
		allObstacles_.clear();
		allObstacles_.emplace_back();

		Vector3 groundMin = {-1000.0f, -10.0f, -1000.0f};
		Vector3 groundMax = {1000.0f, -5.0f, 1000.0f};

		AABB ground;
		ground.min = groundMin;
		ground.max = groundMax;
		allObstacles_[0].push_back(ground);

		OutputDebugStringA("ダミーの地面を作成しました\n");
		return;
	}

	// Commandをリセット
	Command.str("");
	Command.clear();

	Command << file.rdbuf();
	file.close();

	// 障害物データをクリア
	allObstacles_.clear();

	// ステージAABBを更新
	UpdateStageAABB();

	// 障害物データのチェック
	int totalObstacles = 0;
	for (const auto& list : allObstacles_) {
		totalObstacles += static_cast<int>(list.size());
	}

	char buffer[256];
	sprintf_s(buffer, "LoadStage: %d個の障害物を読み込みました\n", totalObstacles);
	OutputDebugStringA(buffer);

	// 障害物がゼロならダミーの地面を追加
	if (totalObstacles == 0) {
		OutputDebugStringA("警告: 障害物を読み込めませんでした。ダミーの地面を追加します。\n");

		if (allObstacles_.empty()) {
			allObstacles_.emplace_back();
		}

		Vector3 groundMin = {-1000.0f, -10.0f, -1000.0f};
		Vector3 groundMax = {1000.0f, -5.0f, 1000.0f};

		AABB ground;
		ground.min = groundMin;
		ground.max = groundMax;
		allObstacles_[0].push_back(ground);
	}

	// 新しい当たり判定情報をプレイヤーに設定
	if (player_) {
		player_->ClearObstacleList(); // 古い障害物リストを削除
		// 新しい障害物リストを設定
		for (const auto& obstacles : allObstacles_) {
			if (!obstacles.empty()) {
				player_->SetObstacleList(obstacles);
			}
		}
	}
}

void GameScene::UpdateStageAABB() {
	std::string line;
	uint32_t cornerNumber = 0;

	Vector3 max;
	Vector3 min;

	bool start = false;
	bool reverse = false;

	// 処理したオブジェクト数をカウント
	int objectCount = 0;

	while (getline(Command, line)) {
		if (line.empty())
			continue;

		std::istringstream line_stream(line);
		std::string word;

		if (!getline(line_stream, word, ' ')) {
			continue; // 空行または読み取り失敗
		}

		// 頂点データのみ処理
		if (word == "v") {
			cornerNumber++;

			float x = 0.0f, y = 0.0f, z = 0.0f;

			// x座標読み取り
			if (getline(line_stream, word, ' ')) {
				try {
					x = static_cast<float>(std::stof(word));
				} catch (const std::exception&) {
					cornerNumber--;
					continue;
				}
			} else {
				cornerNumber--;
				continue;
			}

			// y座標読み取り
			if (getline(line_stream, word, ' ')) {
				try {
					y = static_cast<float>(std::stof(word));
				} catch (const std::exception&) {
					cornerNumber--;
					continue;
				}
			} else {
				cornerNumber--;
				continue;
			}

			// z座標読み取り
			if (getline(line_stream, word, ' ')) {
				try {
					z = static_cast<float>(std::stof(word));
				} catch (const std::exception&) {
					cornerNumber--;
					continue;
				}
			} else {
				cornerNumber--;
				continue;
			}

			// 頂点を処理
			if (!start) {
				max = min = {x, y, z};
				start = true;
			} else {
				// 前よりも大きいとき
				if (max.x <= x) {
					max.x = x;
				}
				// 前よりも小さいとき
				if (min.x > x) {
					min.x = x;
				}

				if (max.y <= y) {
					max.y = y;
				}

				if (min.y > y) {
					min.y = y;
				}

				if (max.z <= z) {
					max.z = z;
				}

				if (min.z > z) {
					min.z = z;
				}
			}
		} else if (word == "vn") {
			// 法線データが始まったら頂点データの処理は終了
			break;
		} else {
			continue;
		}

		// 8頂点でオブジェクト1つ分のAABBを生成
		if (cornerNumber >= 8) {
			if (!reverse) {
				AddObstacle(allObstacles_, min, max); // 結合した基盤となるobj
				objectCount++;
			} else {
				float minX = -max.x;
				float maxX = -min.x;
				min.x = minX;
				max.x = maxX;
				AddObstacle(allObstacles_, min, max); // それ以外のすべてobj
				objectCount++;
			}

			cornerNumber = 0;
			start = false;
			reverse = true;
		}
	}

	// デバッグ出力
	char buffer[256];
	sprintf_s(buffer, "UpdateStageAABB: %d個のオブジェクトを処理しました\n", objectCount);
	OutputDebugStringA(buffer);

	// 障害物がゼロの場合は最低限の地面を追加
	if (objectCount == 0) {
		OutputDebugStringA("警告: OBJファイルから障害物が読み込めませんでした。\n");
	}
}
#pragma endregion 障害物関連処理

#pragma region トランジション関連処理
// トランジション状態の更新（フェード中もゲーム更新を継続）
void GameScene::UpdateTransition() {
	// トランジション効果が無効なら何もしない
	if (!transitionEffect_) {
		return;
	}

	// トランジション効果の更新
	transitionEffect_->Update();

	// 現在のトランジション状態に応じた処理
	switch (transitionState_) {
	case TransitionState::None:
		// トランジションなし状態では何もしない
		break;

	case TransitionState::FadeOut:
		// フェードアウト完了チェック
		if (transitionEffect_->IsCompleted()) {
			// ステージ切り替え中の状態に移行
			transitionState_ = TransitionState::ChangeStage;
			transitionEffect_->ResetCompleted();

			// ステージ切り替え処理
			ChangeStage(nextStage_);

			// すぐにフェードインへ移行
			transitionState_ = TransitionState::FadeIn;

			// フェードインを開始
			transitionEffect_->Start(TransitionType::FadeIn, 1.0f);
		}
		break;

	case TransitionState::FadeIn:
		// フェードイン完了チェック
		if (transitionEffect_->IsCompleted()) {
			// トランジション完了
			transitionState_ = TransitionState::None;
			transitionEffect_->ResetCompleted();
		}
		break;

	case TransitionState::ToTitle:
		// タイトルへのフェードアウト完了チェック
		if (transitionEffect_->IsCompleted()) {
			// タイトル遷移フラグを立てる
			isTransitionToTitle_ = true;
			transitionEffect_->ResetCompleted();
		}
		break;

	default:
		break;
	}
}

// ステージ遷移トランジションの開始
void GameScene::StartTransitionToStage(int stageNumber) {
	// 既にトランジション中なら何もしない
	if (transitionState_ != TransitionState::None) {
		return;
	}

	// 次のステージ番号を設定
	nextStage_ = stageNumber;

	// トランジション状態を設定
	transitionState_ = TransitionState::FadeOut;

	// フェードアウトを開始
	transitionEffect_->Start(TransitionType::FadeOut, 1.0f);
}
#pragma endregion トランジション関連処理