#include "MapChipRenderer.h"

MapChipRenderer::MapChipRenderer() { worldTransform_.Initialize(); }

MapChipRenderer::~MapChipRenderer() {
	delete blockModel_;
	delete block2Model_;
	delete doorModel_;
}

bool MapChipRenderer::Init(Camera* camera, const std::string& csvFile) {
	camera_ = camera;

	// CSV ファイルからブロック座標情報を読み込む
	if (!LoadBlockCoordinatesFromCSV(csvFile)) {
		return false;
	}

	// 各ブロック用のモデルを読み込み
	blockModel_ = Model::CreateFromOBJ("block", true);
	block2Model_ = Model::CreateFromOBJ("block", true);
	doorModel_ = Model::CreateFromOBJ("block", true);

	return true;
}

bool MapChipRenderer::LoadBlockCoordinatesFromCSV(const std::string& csvFile) {
	std::ifstream file(csvFile);
	if (!file.is_open()) {
		return false;
	}

	// 既存のブロックデータをクリア
	blocks_.clear();

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			continue;

		std::istringstream iss(line);
		std::string token;
		BlockData data;

		// x座標の読み込み
		if (std::getline(iss, token, ',')) {
			data.position.x = std::stof(token);
		} else {
			continue; // x座標が読み込めない場合はスキップ
		}

		// y座標の読み込み
		if (std::getline(iss, token, ',')) {
			data.position.y = std::stof(token);
		} else {
			continue; // y座標が読み込めない場合はスキップ
		}

		// z座標の読み込み
		if (std::getline(iss, token, ',')) {
			data.position.z = std::stof(token);
		} else {
			continue; // z座標が読み込めない場合はスキップ
		}

		// ブロックタイプの読み込み
		if (std::getline(iss, token, ',')) {
			int typeInt = std::stoi(token);
			data.type = static_cast<BlockType>(typeInt);
		} else {
			continue; // タイプが読み込めない場合はスキップ
		}

		// デバッグ用：読み込んだ値を確認
		// std::cout << "Loaded block: (" << data.position.x << ", "
		//           << data.position.y << ", " << data.position.z
		//           << "), type: " << static_cast<int>(data.type) << std::endl;

		blocks_.push_back(data);
	}

	file.close();
	return true;
}

void MapChipRenderer::Draw() {
	if (!camera_)
		return;

	// 各ブロックの描画
	for (const auto& block : blocks_) {
		// ブロックの座標に合わせてワールド変換を設定
		worldTransform_.Initialize();

		// 位置を設定 - これが反映されていることを確認
		worldTransform_.translation_ = block.position;

		// デバッグ用：位置が正しく設定されているか確認
		// std::cout << "Block position: (" << block.position.x << ", "
		//           << block.position.y << ", " << block.position.z << ")" << std::endl;

		// 変換行列を更新 - この呼び出しが必要
		worldTransform_.TransferMatrix();

		// block.type に応じたモデルを描画
		switch (block.type) {
		case BlockType::kBlock:
			if (blockModel_) {
				blockModel_->Draw(worldTransform_, *camera_);
			}
			break;
		case BlockType::kBlock2:
			if (block2Model_) {
				block2Model_->Draw(worldTransform_, *camera_);
			}
			break;
		case BlockType::kDoor:
			if (doorModel_) {
				doorModel_->Draw(worldTransform_, *camera_);
			}
			break;
		default:
			break;
		}
	}
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}
