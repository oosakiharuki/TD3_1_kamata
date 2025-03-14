#include "MapChipRenderer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Mymath.h"

// ユーティリティ関数：前後の空白を除去
static std::string trim(const std::string& s) {
	std::string result = s;
	result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) { return !std::isspace(ch); }));
	result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), result.end());
	return result;
}

MapChipRenderer::MapChipRenderer() {
	// ワールド変換の初期化
	worldTransform_.Initialize();
}

MapChipRenderer::~MapChipRenderer() {
	delete blockModel_;
	delete block2Model_;
	delete doorModel_;
	// gridTransforms_ 内のポインタを解放
	for (auto wt : gridTransforms_) {
		delete wt;
	}
	gridTransforms_.clear();
}

bool MapChipRenderer::Init(Camera* camera, const std::string& csvFile) {
	camera_ = camera;
	if (!camera_) {
		std::cerr << "カメラが設定されていません。" << std::endl;
		return false;
	}

	// CSV ファイルからブロック情報を読み込む
	if (!LoadBlockCoordinatesFromCSV(csvFile)) {
		std::cerr << "CSV 読み込みに失敗しました: " << csvFile << std::endl;
		return false;
	}

	// モデルのロード（OBJ ファイル名は環境に合わせてください）
	blockModel_ = Model::CreateFromOBJ("block", true);
	block2Model_ = Model::CreateFromOBJ("block", true);
	doorModel_ = Model::CreateFromOBJ("block", true);

	if (!blockModel_ || !block2Model_ || !doorModel_) {
		std::cerr << "モデルのロードに失敗しました。" << std::endl;
		return false;
	}

	// グリッド方式用ワールド変換を生成
	GenerateBlocks();

	return true;
}

bool MapChipRenderer::LoadBlockCoordinatesFromCSV(const std::string& csvFile) {
	std::ifstream file(csvFile);
	if (!file.is_open()) {
		std::cerr << "CSV ファイルが開けませんでした: " << csvFile << std::endl;
		return false;
	}
	std::string line;
	int lineNumber = 0;
	while (std::getline(file, line)) {
		++lineNumber;
		if (line.empty())
			continue;
		std::istringstream iss(line);
		std::string token;
		BlockData data;
		// 先頭 4 トークン（x, y, z, blockNum）のみ取得
		if (!std::getline(iss, token, ','))
			continue;
		try {
			data.position.x = std::stof(trim(token));
		} catch (...) {
			std::cerr << "行 " << lineNumber << " の x 値変換エラー: " << token << std::endl;
			continue;
		}
		if (!std::getline(iss, token, ','))
			continue;
		try {
			data.position.y = std::stof(trim(token));
		} catch (...) {
			std::cerr << "行 " << lineNumber << " の y 値変換エラー: " << token << std::endl;
			continue;
		}
		if (!std::getline(iss, token, ','))
			continue;
		try {
			data.position.z = std::stof(trim(token));
		} catch (...) {
			std::cerr << "行 " << lineNumber << " の z 値変換エラー: " << token << std::endl;
			continue;
		}
		if (!std::getline(iss, token, ','))
			continue;
		try {
			data.type = static_cast<BlockType>(std::stoi(trim(token)));
		} catch (...) {
			std::cerr << "行 " << lineNumber << " の blockNum 変換エラー: " << token << std::endl;
			continue;
		}
		blocks_.push_back(data);
	}
	file.close();
	return true;
}

void MapChipRenderer::GenerateBlocks() {
	// gridTransforms_ の初期化：既存の変換があれば削除
	for (auto wt : gridTransforms_) {
		delete wt;
	}
	gridTransforms_.clear();

	// blocks_ のサイズに合わせて gridTransforms_ を初期化
	gridTransforms_.resize(blocks_.size(), nullptr);

	// 各ブロックごとにワールド変換を生成
	for (size_t i = 0; i < blocks_.size(); ++i) {
		BlockData& block = blocks_[i];
		// BlockType が kBlank でない場合のみワールド変換を生成
		if (block.type == BlockType::kBlock || block.type == BlockType::kBlock2 || block.type == BlockType::kDoor) {
			WorldTransform* wt = new WorldTransform();
			wt->Initialize();
			wt->translation_ = block.position;
			wt->matWorld_ = MakeAffineMatrix(wt->scale_, wt->rotation_, wt->translation_);
			wt->TransferMatrix();
			gridTransforms_[i] = wt;
		}
	}
}

void MapChipRenderer::Draw() {
	if (!camera_)
		return;

	// CSV から読み込んだ各ブロックを描画
	// ※ここでは blocks_ の情報と対応する gridTransforms_ を利用して描画します
	for (size_t i = 0; i < blocks_.size(); ++i) {
		BlockData& block = blocks_[i];
		WorldTransform* wt = gridTransforms_[i];
		if (!wt)
			continue;

		// ブロックタイプに応じたモデルを描画
		switch (block.type) {
		case BlockType::kBlock:
			if (blockModel_) {
				blockModel_->Draw(*wt, *camera_);
			}
			break;
		case BlockType::kBlock2:
			if (block2Model_) {
				block2Model_->Draw(*wt, *camera_);
			}
			break;
		case BlockType::kDoor:
			if (doorModel_) {
				doorModel_->Draw(*wt, *camera_);
			}
			break;
		default:
			// kBlank などは描画しない
			break;
		}
	}

	// 再更新が必要な場合はワールド変換の行列更新を実施
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}
