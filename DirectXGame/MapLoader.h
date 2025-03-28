// DirectXGame/MapLoader.h に追加/修正
#pragma once
#include "Block.h" // Blockをインクルード
#include "Door.h"
#include "KamataEngine.h"
#include "Key.h"
#include "Player.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace KamataEngine;

// マップオブジェクトの種類を表す列挙型
enum class MapObjectType {
	Key,
	Door,
	Block, // Blockタイプを追加
	       // 将来的に他のオブジェクトタイプを追加可能
};

// CSVから読み込んだオブジェクトデータの構造体
struct MapObjectData {
	Vector3 position;
	MapObjectType type;
	int id = 0; // 追加: オブジェクトのID（キーの識別などに使用）
};

class MapLoader {
public:
	MapLoader();
	~MapLoader();

	// CSVファイルからマップデータを読み込む
	bool LoadMapData(const std::string& csvPath);

	// 読み込んだデータに基づいてオブジェクトを生成・初期化
	void CreateObjects(Camera* camera, Player* player);

	// オブジェクトの更新
	void Update();

	// オブジェクトの描画
	void Draw();

	// 鍵とドアの相互参照を設定
	void SetupObjectReferences();

	// 鍵が取得されたかどうかを確認
	bool IsKeyObtained() const;

	// ドアが開いたかどうかを確認
	bool IsDoorOpened() const;

	// ステージ切り替え
	void ChangeStage(int stageNumber, Camera* camera, Player* player);

	void GLoadStage(std::string objFile);

	// ブロックリストへのアクセス
	const std::vector<Block*>& GetBlockList() const { return blocks_; }

private:
	// 読み込んだマップオブジェクトデータのリスト
	std::vector<MapObjectData> mapObjectsData_;

	// 生成された鍵のリスト
	std::vector<Key*> keys_;

	// 生成されたドアのリスト
	std::vector<Door*> doors_;

	// 生成されたブロックのリスト
	std::vector<Block*> blocks_;

	// CSVから座標とオブジェクトタイプを解析
	bool ParseCSVLine(const std::string& line, MapObjectData& data);

	// リソースのクリーンアップ
	void ClearResources();
};