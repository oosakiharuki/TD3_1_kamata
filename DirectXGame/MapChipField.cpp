#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>

namespace {

// 既存のグリッド用テーブル
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank },
    {"1", MapChipType::kBlock },
    {"2", MapChipType::kBlock2},
    {"3", MapChipType::kDoor  },
};

} // namespace

// 既存のグリッド方式用関数
void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// マップチップデータをリセット
	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	for (uint32_t y = 0; y < kNumBlockVirtical; ++y) {
		std::string line;
		getline(mapChipCsv, line);
		std::istringstream lien_stream(line);
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			std::string word;
			getline(lien_stream, word, ',');
			if (mapChipTable.contains(word)) {
				mapChipData_.data[y][x] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex >= kNumBlockHorizontal || yIndex >= kNumBlockVirtical) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPostionByIndex(uint32_t xIndex, uint32_t yIndex) {
	const float groundLeft = -26.10597f;
	const float groundTop = 57.103004f;
	const float groundWidth = 52.21194f;   // 26.10597 * 2
	const float groundDepth = 113.061721f; // 57.103004 - (-55.958717)

	float cellWidth = groundWidth / 40.0f;
	float cellDepth = groundDepth / 40.0f;

	float x = groundLeft + cellWidth * (xIndex + 0.5f);
	float z = groundTop - cellDepth * (yIndex + 0.5f);
	return Vector3(x, 0.0f, z);
}

IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2) / kBlockHeight);
	return indexSet;
}

Rect MapChipField::GetRectByIndex(uint32_t xindex, uint32_t yIndex) {
	Vector3 center = GetMapChipPostionByIndex(xindex, yIndex);
	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	return rect;
}

void MapChipField::SetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex, MapChipType type) {
	if (xIndex < kNumBlockHorizontal && yIndex < kNumBlockVirtical) {
		mapChipData_.data[yIndex][xIndex] = type;
	}
}

void MapChipField::InvertMap() {
	std::vector<std::vector<MapChipType>> invertedData(kNumBlockVirtical, std::vector<MapChipType>(kNumBlockHorizontal));
	for (uint32_t y = 0; y < kNumBlockVirtical; ++y) {
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			invertedData[kNumBlockVirtical - 1 - y][kNumBlockHorizontal - 1 - x] = mapChipData_.data[y][x];
		}
	}
	mapChipData_.data = invertedData;
}

// --- 新たに座標方式のCSVを読み込む処理を追加 ---
// CSV の各行は「x,y,z,blockNum」形式とする
void MapChipField::LoadCoordinateCsv(const std::string& filePath) {
	blocks_.clear();
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			continue;
		std::istringstream iss(line);
		std::string token;
		BlockDataCoord data;

		// x,y,z,blockNum の順で読み込む
		if (std::getline(iss, token, ',')) {
			data.position.x = std::stof(token);
		}
		if (std::getline(iss, token, ',')) {
			data.position.y = std::stof(token);
		}
		if (std::getline(iss, token, ',')) {
			data.position.z = std::stof(token);
		}
		if (std::getline(iss, token, ',')) {
			int typeInt = std::stoi(token);
			data.type = static_cast<MapChipType>(typeInt);
		}
		blocks_.push_back(data);
	}
	file.close();
}