#include "TileMap.h"
#include <fstream>
#include <sstream>
#include <vector>

TileMap::TileMap() {
	// "block.obj" を "block" という名前で読み込み、モデルを生成します。
	blockModel_ = Model::CreateFromOBJ("block", true);
}

TileMap::~TileMap() { delete blockModel_; }

void TileMap::LoadMap(const std::string& csvFile) {
	std::ifstream file(csvFile);
	if (!file.is_open()) {
		// ファイルが開けなかった場合は何もせずリターン
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			continue; // 空行はスキップ
		std::istringstream iss(line);
		std::string token;
		std::vector<std::string> tokens;
		// カンマ区切りで各トークンを抽出
		while (std::getline(iss, token, ',')) {
			tokens.push_back(token);
		}
		if (tokens.size() < 4)
			continue; // 不十分な行はスキップ

		float x = std::stof(tokens[0]);
		float y = std::stof(tokens[1]);
		float z = std::stof(tokens[2]);
		int blockNum = std::stoi(tokens[3]);

		// BlockNumが1の場合にのみブロックとして配置
		if (blockNum == 1) {
			Tile tile;
			tile.position = {x, y, z};

			// ブロックのサイズを1.0（中心から±0.5）と仮定してAABBを設定
			float halfSize = 0.5f;
			tile.aabb.min = {x - halfSize, y - halfSize, z - halfSize};
			tile.aabb.max = {x + halfSize, y + halfSize, z + halfSize};

			tiles_.push_back(tile);
		}
	}
	file.close();
}

void TileMap::Update() {
	// 必要に応じて更新処理を追加してください
}

void TileMap::Draw(Camera* camera) {
	WorldTransform world;
	// 各Tile（ブロック）について描画を行います
	for (const auto& tile : tiles_) {
		world.Initialize();
		// WorldTransform では位置設定に translation_ を利用します
		world.translation_ = tile.position;
		// 必要に応じて回転やスケールを設定してください
		world.TransferMatrix();

		blockModel_->Draw(world, *camera);
	}
}

bool TileMap::CheckCollisionWithPlayer(const AABB& playerAABB) const {
	for (const auto& tile : tiles_) {
		if (IsCollisionAABB(playerAABB, tile.aabb)) {
			return true;
		}
	}
	return false;
}
