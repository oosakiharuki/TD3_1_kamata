#pragma once
#include "KamataEngine.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include <vector>

using namespace KamataEngine;

class FBXModel {
public:
	FBXModel();
	~FBXModel();

	bool LoadModel(const std::string& filePath);
	void Update(float deltaTime);
	void Draw(const WorldTransform& transform, Camera& camera);
	// アニメーション再生用メソッド追加
	void PlayAnimation(const std::string& animName);

private:
	// Assimpのオブジェクト
	Assimp::Importer importer;
	const aiScene* scene = nullptr;

	// モデルデータ
	std::vector<Model*> meshModels;
	std::vector<uint32_t> textureHandles;
};
