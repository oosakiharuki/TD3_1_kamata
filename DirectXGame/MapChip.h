#pragma once
#include "KamataEngine.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace KamataEngine;

// MapChipField と MapChipRenderer の enum はほぼ同一なので、
// ここでは MapChipType として統一しています。
enum class MapChipType {
	kBlank = 0,
	kBlock = 1,
	kBlock2 = 2,
	kDoor = 3,
};

struct BlockData {
	Vector3 position; // CSV の x,y,z 座標
	MapChipType type; // CSV の blockNum に対応
};

class MapChip {
public:
	MapChip();
	~MapChip();

	// カメラ設定と CSV ファイルからマップデータ読み込み
	bool Init(Camera* camera, const std::string& csvFile);

	// 読み込んだ各ブロックを描画する
	void Draw();

	// 衝突判定などに利用するため、読み込んだブロック情報を取得
	const std::vector<BlockData>& GetBlocks() const { return blocks_; }

private:
	// CSV ファイル（"x,y,z,blockNum"形式）を読み込む
	bool LoadCSV(const std::string& csvFile);

	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;

	// 各ブロックタイプに対応するモデル
	Model* blockModel_ = nullptr;
	Model* block2Model_ = nullptr;
	Model* doorModel_ = nullptr;

	// CSV から読み込んだブロック配置情報
	std::vector<BlockData> blocks_;
};
