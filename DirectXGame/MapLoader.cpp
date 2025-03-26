#include "MapLoader.h"
#include <algorithm>
#include <iostream>

MapLoader::MapLoader() {}

MapLoader::~MapLoader() { ClearResources(); }

bool MapLoader::LoadMapData(const std::string& csvPath) {
	// 以前のデータをクリア
	mapObjectsData_.clear();

	// CSVファイルを開く
	std::ifstream file(csvPath);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	// ヘッダー行をスキップする場合はここでgetline(file, line);を追加

	// CSVの各行を読み込む
	while (std::getline(file, line)) {
		if (line.empty())
			continue;

		MapObjectData data;
		if (ParseCSVLine(line, data)) {
			mapObjectsData_.push_back(data);
		}
	}

	file.close();
	return true;
}

bool MapLoader::ParseCSVLine(const std::string& line, MapObjectData& data) {
	std::istringstream iss(line);
	std::string token;

	// x座標を読み込む
	if (std::getline(iss, token, ',')) {
		data.position.x = std::stof(token);
	} else {
		return false;
	}

	// y座標を読み込む
	if (std::getline(iss, token, ',')) {
		data.position.y = std::stof(token);
	} else {
		return false;
	}

	// z座標を読み込む
	if (std::getline(iss, token, ',')) {
		data.position.z = std::stof(token);
	} else {
		return false;
	}

	// オブジェクトタイプを読み込む
	if (std::getline(iss, token, ',')) {
		if (token == "key") {
			data.type = MapObjectType::Key;
		} else if (token == "door") {
			data.type = MapObjectType::Door;
		} else {
			return false; // 未知のオブジェクトタイプ
		}
	} else {
		return false;
	}

	return true;
}

void MapLoader::CreateObjects(Camera* camera, Player* player) {
	// 既存のオブジェクトをクリア
	ClearResources();

	// 読み込んだデータに基づいてオブジェクトを生成
	for (const auto& objectData : mapObjectsData_) {
		if (objectData.type == MapObjectType::Key) {
			Key* key = new Key();
			key->Init(camera);
			key->SetPosition(objectData.position);
			key->SetPlayer(player);
			keys_.push_back(key);
		} else if (objectData.type == MapObjectType::Door) {
			Door* door = new Door();
			door->Init(camera);
			door->SetPosition(objectData.position);
			door->SetPlayer(player);
			doors_.push_back(door);
		}
	}

	// 鍵とドアの相互参照を設定
	SetupObjectReferences();
}

void MapLoader::SetupObjectReferences() {
	// すべてのドアに対して、すべての鍵への参照を設定
	for (auto* door : doors_) {
		for (auto* key : keys_) {
			door->SetKey(key);
		}
	}
}

void MapLoader::Update() {
	// すべての鍵を更新
	for (auto* key : keys_) {
		key->Update();
	}

	// すべてのドアを更新
	for (auto* door : doors_) {
		door->Update();
	}
}

void MapLoader::Draw() {
	// すべての鍵を描画
	for (auto* key : keys_) {
		key->Draw();
	}

	// すべてのドアを描画
	for (auto* door : doors_) {
		door->Draw();
	}
}

bool MapLoader::IsKeyObtained() const {
	// いずれかの鍵が取得されているかチェック
	return std::any_of(keys_.begin(), keys_.end(), [](const Key* key) { return key->IsKeyObtained(); });
}

bool MapLoader::IsDoorOpened() const {
	// いずれかのドアが開いているかチェック
	return std::any_of(doors_.begin(), doors_.end(), [](const Door* door) { return door->IsDoorOpened(); });
}

void MapLoader::ClearResources() {
	// 鍵のリソースを解放
	for (auto* key : keys_) {
		delete key;
	}
	keys_.clear();

	// ドアのリソースを解放
	for (auto* door : doors_) {
		delete door;
	}
	doors_.clear();
}

void MapLoader::ChangeStage(int stageNumber, Camera* camera, Player* player) {
	// 既存のオブジェクトを削除
	ClearResources();

	// ステージ番号に応じたCSVファイル名を決定
	std::string csvPath = "Resources/objects" + std::to_string(stageNumber) + ".csv";

	// マップデータを読み込み
	if (LoadMapData(csvPath)) {
		// 新しいオブジェクトを作成
		CreateObjects(camera, player);
	} else {
		std::cerr << "Failed to load map data: " << csvPath << std::endl;
	}
}