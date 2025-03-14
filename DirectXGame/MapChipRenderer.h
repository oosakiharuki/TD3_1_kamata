#pragma once
#include"KamataEngine.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace KamataEngine;

enum class BlockType {
	kBlank = 0,
	kBlock = 1,
	kBlock2 = 2,
	kDoor = 3,
};

struct BlockData {
	Vector3 position; // ブロックの位置
	BlockType type;   // CSV の blockNum に応じたタイプ
};

class MapChipRenderer {
public:
	MapChipRenderer();
	~MapChipRenderer();

	// カメラ設定と、CSV ファイルから直接ブロックの座標とタイプを読み込む
	bool Init(Camera* camera, const std::string& csvFile);

	// 読み込んだ座標情報に基づきブロックを描画する
	void Draw();

private:
	// CSV をパースして blocks_ に情報を格納する
	bool LoadBlockCoordinatesFromCSV(const std::string& csvFile);

	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;

	// 各ブロックタイプに対応するモデル
	Model* blockModel_ = nullptr;
	Model* block2Model_ = nullptr;
	Model* doorModel_ = nullptr;

	// CSV から読み込んだブロック配置情報
	std::vector<BlockData> blocks_;
};
