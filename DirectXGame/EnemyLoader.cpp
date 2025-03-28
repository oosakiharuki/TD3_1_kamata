#include "EnemyLoader.h"
#include <algorithm>

EnemyLoader::EnemyLoader() {}

EnemyLoader::~EnemyLoader() { ClearResources(); }

bool EnemyLoader::LoadEnemyData(const std::string& csvPath) {
	// 以前のデータをクリア
	enemyData_.clear();

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

		EnemyData data;
		if (ParseCSVLine(line, data)) {
			enemyData_.push_back(data);
		}
	}

	file.close();
	return true;
}

bool EnemyLoader::ParseCSVLine(const std::string& line, EnemyData& data) {
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

	// 敵タイプを読み込む
	if (std::getline(iss, token, ',')) {
		if (token == "redGhost") {
			data.type = EnemyType::RedGhost;
		} else if (token == "cannon") {
			data.type = EnemyType::Cannon;
		} else if (token == "spring") {
			data.type = EnemyType::Spring;
		} else {
			return false; // 未知の敵タイプ
		}
	} else {
		return false;
	}

	return true;
}

void EnemyLoader::CreateEnemies(Camera* camera, Player* player, const std::vector<std::vector<AABB>>& obstacles) {
	// 既存の敵をクリア
	ClearResources();

	// 読み込んだデータに基づいて敵を生成
	for (const auto& data : enemyData_) {
		switch (data.type) {
		case EnemyType::RedGhost: {
			RedGhost* redGhost = new RedGhost();
			redGhost->Init(camera);
			redGhost->SetPosition(data.position);
			redGhost->SetTarget(player);

			// 障害物リストを設定
			for (const auto& obstacleList : obstacles) {
				redGhost->SetObstacleList(obstacleList);
			}

			redGhosts_.push_back(redGhost);
			break;
		}
		case EnemyType::Cannon: {
			CannonEnemy* cannon = new CannonEnemy();
			cannon->Init(camera);
			cannon->SetPosition(data.position);
			cannon->SetPlayer(player);

			// 障害物リストを設定
			for (const auto& obstacleList : obstacles) {
				cannon->SetObstacleList(obstacleList);
			}

			cannonEnemies_.push_back(cannon);
			break;
		}
		case EnemyType::Spring: {
			SpringEnemy* spring = new SpringEnemy();
			spring->Init(camera);
			spring->SetPosition(data.position);
			spring->SetPlayer(player);

			// 障害物リストを設定
			for (const auto& obstacleList : obstacles) {
				spring->SetObstacleList(obstacleList);
			}

			springEnemies_.push_back(spring);
			break;
		}
		}
	}
}

void EnemyLoader::Update() {
	// 通常の敵の更新
	for (auto it = redGhosts_.begin(); it != redGhosts_.end();) {
		(*it)->Update();
		// IsDestroyedメソッドがない場合は、下記の条件を適宜修正してください
		if (false) { // 仮の条件
			delete *it;
			it = redGhosts_.erase(it);
		} else {
			++it;
		}
	}

	// 大砲敵の更新
	for (auto* cannon : cannonEnemies_) {
		cannon->Update();
	}

	// バネ敵の更新
	for (auto* spring : springEnemies_) {
		spring->Update();
	}
}

void EnemyLoader::Draw() {
	// 通常の敵の描画
	for (auto* redGhost : redGhosts_) {
		redGhost->Draw();
	}

	// 大砲敵の描画
	for (auto* cannon : cannonEnemies_) {
		cannon->Draw();
	}

	// バネ敵の描画
	for (auto* spring : springEnemies_) {
		spring->Draw();
	}
}

void EnemyLoader::ClearResources() {
	// 通常の敵のリソースを解放
	for (auto* redGhost : redGhosts_) {
		delete redGhost;
	}
	redGhosts_.clear();

	// 大砲敵のリソースを解放
	for (auto* cannon : cannonEnemies_) {
		delete cannon;
	}
	cannonEnemies_.clear();

	// バネ敵のリソースを解放
	for (auto* spring : springEnemies_) {
		delete spring;
	}
	springEnemies_.clear();
}