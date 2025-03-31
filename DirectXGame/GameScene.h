#pragma once
#include "Block.h"
#include "CannonEnemy.h"
#include "Door.h"
#include "Enemy.h"
#include "EnemyLoader.h"
#include "GhostBlock.h"
#include "Goal.h"
#include "Ground.h"
#include "KamataEngine.h"
#include "Key.h"
#include "MapLoader.h"
#include "Player.h"
#include "Skydome.h"
#include "SpringEnemy.h"
#include "TransitionEffect.h" // 追加

using namespace KamataEngine;

class GameScene {
public:
	// コンストラクタ
	GameScene();

	// デストラクタ
	~GameScene();
	void Finalize();

	void Initialize();
	void Update();
	void Draw();

	// ステージを変更する
	void ChangeStage(int nextStage);

private:
	void AddObstacle(std::vector<std::vector<AABB>>& allObstacles, const Vector3& min, const Vector3& max);
	void LoadStage(std::string objFile);
	void UpdateStageAABB();

	// トランジションの状態   // 追加
	enum class TransitionState {
		None,       // トランジションなし
		FadeOut,    // フェードアウト中（現ステージ）
		FadeIn,     // フェードイン中（次ステージ）
		WaitForNext // 次のステージ待ち
	};

	// トランジション処理     // 追加
	void UpdateTransition();
	void StartTransitionToStage(int stageNumber);

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	WorldTransform worldTransform_;
	Camera camera_;
	Player* player_ = nullptr;

	// 障害物リスト
	std::vector<std::vector<AABB>> allObstacles_;

	uint32_t textureHandle = 0;

	Model* stage = nullptr;
	std::stringstream Command;

	// MapLoaderのインスタンス
	MapLoader* mapLoader_ = nullptr;

	// EnemyLoaderのインスタンス
	EnemyLoader* enemyLoader_ = nullptr;

	Block* block_ = nullptr;
	Model* modelBlock_ = nullptr;

	GhostBlock* ghostBlock_ = nullptr;
	Model* modelGhostBlock_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	// 現在のステージ番号
	int currentStage_ = 1;

	float longPress = 1.0f;
	const float RestartTimer = 1.0f;
	XINPUT_STATE state = {}, preState = {}; // 初期化を追加

	// トランジション関連    // 追加
	TransitionEffect* transitionEffect_ = nullptr;
	TransitionState transitionState_ = TransitionState::None;
	int nextStage_ = 0; // 次のステージ番号
};