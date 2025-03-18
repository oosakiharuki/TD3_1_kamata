#include "MapChip.h"

MapChip::MapChip() { worldTransform_.Initialize(); }

MapChip::~MapChip() {
	delete blockModel_;
	delete block2Model_;
	delete doorModel_;
}

bool MapChip::Init(Camera* camera, const std::string& csvFile) {
	camera_ = camera;
	if (!LoadCSV(csvFile)) {
		return false;
	}
	// 各ブロック用のモデルを読み込み
	blockModel_ = Model::CreateFromOBJ("block", true);
	block2Model_ = Model::CreateFromOBJ("block", true);
	doorModel_ = Model::CreateFromOBJ("block", true);
	return true;
}

bool MapChip::LoadCSV(const std::string& csvFile) {
	std::ifstream file(csvFile);
	if (!file.is_open()) {
		return false;
	}
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			continue;
		std::istringstream iss(line);
		std::string token;
		BlockData data;
		// CSV の形式：x,y,z,blockNum
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
	return true;
}

void MapChip::Draw() {
	if (!camera_)
		return;
	for (const auto& block : blocks_) {
		worldTransform_.Initialize();
		worldTransform_.translation_ = block.position;
		worldTransform_.TransferMatrix();

		// ブロックタイプに応じたモデルを描画
		switch (block.type) {
		case MapChipType::kBlock:
			if (blockModel_) {
				blockModel_->Draw(worldTransform_, *camera_);
			}
			break;
		case MapChipType::kBlock2:
			if (block2Model_) {
				block2Model_->Draw(worldTransform_, *camera_);
			}
			break;
		case MapChipType::kDoor:
			if (doorModel_) {
				doorModel_->Draw(worldTransform_, *camera_);
			}
			break;
		default:
			break;
		}
	}
}
