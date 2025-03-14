#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include <string>
#include <vector>

using namespace KamataEngine;

// CSVから読み込んだ各ブロックの位置とAABBを保持する構造体
struct Tile {
	Vector3 position; // ブロックの中心位置
	AABB aabb;        // 衝突判定用のAABB
};

class TileMap {
public:
	TileMap();
	~TileMap();

	// CSVファイル（例："map.csv"）を読み込み、ブロック情報をロードします。
	// CSVフォーマット: x,y,z,BlockNum
	void LoadMap(const std::string& csvFile);

	// 更新処理（将来的な拡張用）
	void Update();

	// 指定されたカメラを用いて、各ブロックを描画します。
	void Draw(Camera* camera);

	// 引数のプレイヤーAABBと各ブロックのAABBとの衝突判定を行います。
	// ひとつでも衝突があればtrueを返します。
	bool CheckCollisionWithPlayer(const AABB& playerAABB) const;

private:
	Model* blockModel_;       // block.objから生成したモデル
	std::vector<Tile> tiles_; // CSVファイルから読み込んだブロック情報のリスト
};
