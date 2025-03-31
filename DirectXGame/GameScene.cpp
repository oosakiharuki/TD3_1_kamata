#include "GameScene.h"
#include "AABB.h"
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

	// MapLoaderの初期化
	mapLoader_ = new MapLoader();
	std::string objectsFile = "Resources/objects" + std::to_string(currentStage_) + ".csv";
	if (mapLoader_->LoadMapData(objectsFile)) {
		mapLoader_->CreateObjects(&camera_, player_);
	}

	// 障害物情報の読み込み
	LoadStage("Resources/stage1/stage1.obj");

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

	// 障害物リストを Player にセット
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

	// プレイヤーにGoalへの参照を設定
	if (mapLoader_ && mapLoader_->GetGoal()) {
		player_->SetGoal(mapLoader_->GetGoal());
	}

	// トランジション効果の初期化
	transitionEffect_ = new TransitionEffect();
	transitionEffect_->Initialize();
}
#pragma endregion 初期化処理

#pragma region 更新処理
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

	// トランジション中は他の更新処理をスキップ
	if (transitionState_ != TransitionState::None) {
		return;
	}

	// MapLoaderが管理するGoalの状態をチェック
	if (mapLoader_ && mapLoader_->GetGoal() && mapLoader_->GetGoal()->IsClear()) {
		return; // ゴールクリア状態なら更新処理をスキップ
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

	// UIの更新
	player_->DrawUI();

	// 天球の更新
	skydome_->Update();

	// ドアが開いたら次のステージへトランジション開始
	if (mapLoader_ && mapLoader_->IsDoorOpened()) {
		// 次のステージ番号を計算
		int nextStage = currentStage_ + 1;

		// トランジション開始
		StartTransitionToStage(nextStage);
	}
}
#pragma endregion 更新処理
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

	// ghostBlock_->Draw();

	skydome_->Draw();

	Model::PostDraw();

	// UIスプライト描画 - ここでゴールクリアテキストも描画
	Sprite::PreDraw(commandList);

	// MapLoaderのスプライト描画処理を呼び出す
	if (mapLoader_) {
		mapLoader_->DrawSprites(commandList);
	}

	// トランジション効果の描画
	if (transitionEffect_) {
		transitionEffect_->Draw();
	}

	Sprite::PostDraw();
}
#pragma endregion 描画処理

#pragma region ステージ変更処理
void GameScene::ChangeStage(int nextStage) {
	allObstacles_.clear();
	Command.str("");
	Command.clear();

	currentStage_ = nextStage;
	stage = Model::CreateFromOBJ("stage" + std::to_string(currentStage_), true);

	// **プレイヤーと敵の障害物リストをクリア**
	player_->ClearObstacleList();
	if (enemyLoader_) {
		for (auto& enemy : enemyLoader_->GetEnemyList()) {
			if (enemy) {
				enemy->ClearObstacleList();
			}
		}
	}

	// 新しいオブジェクトデータをロード
	if (mapLoader_) {
		std::string objectsFile = "Resources/objects" + std::to_string(currentStage_) + ".csv";
		mapLoader_->ChangeStage(currentStage_, &camera_, player_);
		if (mapLoader_->LoadMapData(objectsFile)) {
			mapLoader_->CreateObjects(&camera_, player_);
		}
	}

	// **新しい障害物データをロード**
	std::string stageFile = "Resources/stage" + std::to_string(currentStage_) + "/stage" + std::to_string(currentStage_) + ".obj";
	LoadStage(stageFile);

	// バネ敵の障害物リストクリア
	if (enemyLoader_) {
		const auto& springEnemies = enemyLoader_->GetSpringEnemyList();
		for (auto& springEnemy : springEnemies) {
			if (springEnemy) {
				springEnemy->ClearObstacleList();
			}
		}
	}

	// **プレイヤーに新しい障害物リストを設定**
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

	// **敵の当たり判定も再設定**
	if (enemyLoader_) {
		delete enemyLoader_;
		enemyLoader_ = nullptr;
	}

	enemyLoader_ = new EnemyLoader();

	std::string enemiesFile = "Resources/enemies" + std::to_string(currentStage_) + ".csv";
	if (enemyLoader_->LoadEnemyData(enemiesFile)) {
		enemyLoader_->CreateEnemies(&camera_, player_, allObstacles_);
	}

	// 敵の当たり判定リストを再設定
	if (enemyLoader_) {
		for (auto& enemy : enemyLoader_->GetEnemyList()) {
			if (enemy) {
				for (const auto& obstacles : allObstacles_) {
					enemy->SetObstacleList(obstacles);
				}
			}
		}
	}

	// プレイヤーに敵リストを設定
	player_->SetEnemyList(enemyLoader_->GetEnemyList());

	if (enemyLoader_) {
		player_->SetSpringEnemies(enemyLoader_->GetSpringEnemyList());

		// キャノン敵への参照をプレイヤーに設定
		if (!enemyLoader_->GetCannonEnemyList().empty()) {
			player_->SetCannon(enemyLoader_->GetCannonEnemyList()[0]);
		}
	}

	// プレイヤーにブロックリストを設定（更新：単一ブロックではなくリスト全体を渡す）
	const std::vector<Block*>& blocks = mapLoader_->GetBlockList();
	player_->SetBlocks(blocks);

	// プレイヤーにGoalへの参照を再設定
	if (mapLoader_ && mapLoader_->GetGoal()) {
		player_->SetGoal(mapLoader_->GetGoal());
	}

	// 次のステージに応じてプレイヤーの座標を設定
	Vector3 newPosition;
	if (nextStage == 2) {
		// Stage 2への移行時の座標
		newPosition = {-55.070f, 1.649f, -68.019f};
	} else if (nextStage == 3) {
		// Stage 3への移行時の座標
		newPosition = {-37.0f, -18.512f, -51.500f};
	}

	// プレイヤーの座標を変更
	player_->SetPosition(newPosition);
}
#pragma endregion ステージ変更処理

#pragma region 障害物関連処理
// AddObstacle、LoadStage、UpdateStageAABBメソッドはそのまま以前の実装を使用
void GameScene::AddObstacle(std::vector<std::vector<AABB>>& allObstacles, const Vector3& min, const Vector3& max) {
	AABB obstacle;
	obstacle.min = min;
	obstacle.max = max;
	if (allObstacles.empty() || allObstacles.back().size() >= 500) { // 100個の障害物を追加
		allObstacles.emplace_back();
	}
	allObstacles.back().push_back(obstacle);
}

void GameScene::LoadStage(std::string objFile) {
	// ステージデータの読み込み
	std::ifstream file;
	file.open(objFile);
	assert(file.is_open());

	// Commandをリセット
	Command.str("");
	Command.clear();

	Command << file.rdbuf();
	file.close();

	// 障害物データをクリア
	allObstacles_.clear();

	UpdateStageAABB();

	player_->ClearObstacleList(); // 古い障害物リストを削除
	// 新しい障害物リスト
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

	if (enemyLoader_) {
		for (auto& enemy : enemyLoader_->GetEnemyList()) {
			if (enemy) {
				enemy->ClearObstacleList();
				for (const auto& obstacles : allObstacles_) {
					enemy->SetObstacleList(obstacles);
				}
			}
		}
	}
}

void GameScene::UpdateStageAABB() {
	std::string line;
	uint32_t cornerNumber = 0;

	Vector3 max;
	Vector3 min;

	// AABB stageAABB;
	bool start = false;
	bool reverse = false;

	while (getline(Command, line)) {
		std::istringstream line_stream(line);

		std::string word;

		getline(line_stream, word, ' ');

		if (word.find("v") == 0) {
			cornerNumber++;
		} else if (word.find("vn") == 0) {
			break;
		} else {
			continue;
		}

		if (cornerNumber > 0) {
			getline(line_stream, word, ' ');
			float x = (float)std::atof(word.c_str());

			getline(line_stream, word, ' ');
			float y = (float)std::atof(word.c_str());

			getline(line_stream, word, ' ');
			float z = (float)std::atof(word.c_str());

			if (!start) {
				max = {x, y, z};
				min = {x, y, z};
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
		}

		if (cornerNumber == 8) {
			if (!reverse) {
				AddObstacle(allObstacles_, min, max); // 結合した基盤となるobj
			} else {
				float minX;
				float maxX;
				maxX = -(max.x);
				minX = -(min.x);

				min.x = maxX;
				max.x = minX;
				AddObstacle(allObstacles_, {min.x, min.y, min.z}, {max.x, max.y, max.z}); // それ以外のすべてobj
			}

			cornerNumber = 0;
			start = false;
			reverse = true;
		}
	}
}
#pragma endregion 障害物関連処理

#pragma region トランジション関連処理
// トランジション状態の更新（シンプル化）
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
			// ステージ切り替え
			ChangeStage(nextStage_);

			// フェードインへ移行
			transitionState_ = TransitionState::FadeIn;
			transitionEffect_->ResetCompleted();

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

	default:
		break;
	}
}

// ステージ遷移トランジションの開始（シンプル化）
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