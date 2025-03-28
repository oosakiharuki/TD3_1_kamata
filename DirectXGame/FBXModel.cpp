#include "FBXModel.h"
#include <iostream> // デバッグ用

FBXModel::FBXModel() {
	// 初期化処理
}

FBXModel::~FBXModel() {
	// リソース解放処理
	for (auto model : meshModels) {
		delete model;
	}
}

bool FBXModel::LoadModel(const std::string& filePath) {
	// Assimpの読み込み設定
	unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;

	// FBXファイルの読み込み
	scene = importer.ReadFile(filePath, flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	// 必要に応じてメッシュやアニメーションのデータをパースする処理を追加する
	// （現時点では未実装）

	return true;
}

void FBXModel::Update(float deltaTime) {
	// アニメーション更新処理
	// ここにアニメーションの更新ロジックを追加する
}

void FBXModel::Draw(const WorldTransform& transform, Camera& camera) {
	// 描画処理
	for (size_t i = 0; i < meshModels.size(); i++) {
		if (meshModels[i]) {
			meshModels[i]->Draw(transform, camera);
		}
	}
}

void FBXModel::PlayAnimation(const std::string& animName) {
	// アニメーション切替処理の実装（仮実装）
	std::cout << "Playing animation: " << animName << std::endl;
	// 本来はanimNameに応じたアニメーションデータの切替を行う
}
