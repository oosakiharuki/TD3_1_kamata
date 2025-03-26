#pragma once
#include "RedGhost.h"
#include "BlueGhost.h"
#include "YellowGhost.h"
#include "CannonEnemy.h"
#include "SpringEnemy.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <KamataEngine.h>
#include "Player.h"

using namespace KamataEngine;

// 敵オブジェクトの種類を表す列挙型
enum class EnemyType {
	RedGhost,  // 赤いゴースト
	BlueGhost, // 青いゴースト
	YellowGhost, // 黄色いゴースト
	Cannon,    // 大砲敵
	Spring     // バネ敵
};

// CSVから読み込んだ敵データの構造体
struct EnemyData {
	Vector3 position;
	EnemyType type;
};

class EnemyLoader {
public:
	EnemyLoader();
	~EnemyLoader();

	// CSVファイルから敵データを読み込む
	bool LoadEnemyData(const std::string& csvPath);

	// 読み込んだデータに基づいて敵オブジェクトを生成・初期化
	void CreateEnemies(Camera* camera, Player* player, const std::vector<std::vector<AABB>>& obstacles);

	// 敵の更新
	void Update();

	// 敵の描画
	void Draw();

	// 各種敵リストへのアクセッサ
	const std::vector<RedGhost*>& GetRedGhostList() const { return redGhosts_; }
	const std::vector<BlueGhost*>& GetBlueGhostList() const { return blueGhosts_; }
	const std::vector<YellowGhost*>& GetYellowGhostList() const { return yellowGhosts_; }
	const std::vector<CannonEnemy*>& GetCannonEnemyList() const { return cannonEnemies_; }
	const std::vector<SpringEnemy*>& GetSpringEnemyList() const { return springEnemies_; }

private:
	// 読み込んだ敵データのリスト
	std::vector<EnemyData> enemyData_;

	// 生成された各種敵のリスト
	std::vector<RedGhost*> redGhosts_;
	std::vector<BlueGhost*> blueGhosts_;
	std::vector<YellowGhost*> yellowGhosts_;
	std::vector<CannonEnemy*> cannonEnemies_;
	std::vector<SpringEnemy*> springEnemies_;

	// CSVから座標と敵タイプを解析
	bool ParseCSVLine(const std::string& line, EnemyData& data);

	// リソースのクリーンアップ
	void ClearResources();
};