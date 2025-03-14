#pragma once
#include "KamataEngine.h"
#include <assert.h>
#include <stdint.h>
#include <string>
#include <vector>

enum class MapChipType {
	kBlank,  // 空白
	kBlock,  // ブロック
	kBlock2, // ブロック
	kDoor,   // ドア
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};

struct Rect {
	float left;
	float right;
	float bottom;
	float top;
};

// 新たにCSV座標方式用の構造体を定義
struct BlockDataCoord {
	Vector3 position; // CSVのx,y,z
	MapChipType type; // CSVのblockNum (0,1,2,3...など)
};

class MapChipField {
public:
	// 既存のグリッド方式用関数
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPostionByIndex(uint32_t xIndex, uint32_t yIndex);
	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }
	IndexSet GetMapChipIndexSetByPosition(const Vector3& posotopn);
	Rect GetRectByIndex(uint32_t xindex, uint32_t yIndex);
	void SetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex, MapChipType type);
	void InvertMap();

	// 新たにCSV座標方式で読み込む関数
	void LoadCoordinateCsv(const std::string& filePath);
	// 読み込んだブロック配置情報を取得する
	const std::vector<BlockDataCoord>& GetBlocks() const { return blocks_; }

private:
	// 1ブロックのサイズ（グリッド方式用）
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// グリッドのブロック個数
	static inline const uint32_t kNumBlockVirtical = 40;
	static inline const uint32_t kNumBlockHorizontal = 40;
	MapChipData mapChipData_;

	// CSV座標方式で読み込んだブロック情報を格納するベクター
	std::vector<BlockDataCoord> blocks_;
};
