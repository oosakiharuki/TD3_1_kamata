#pragma once
#include "AABB.h"
#include "EnemyLoader.h"
#include "KamataEngine.h"
#include "MapLoader.h"
#include "Player.h"
#include <vector>

using namespace KamataEngine;

class Minimap {
public:
	Minimap();
	~Minimap();

	// 初期化
	void Initialize(Player* player, MapLoader* mapLoader, EnemyLoader* enemyLoader, const std::vector<std::vector<AABB>>& obstacles);

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// 障害物データからマップチップを生成
	void CreateMapChipsFromObstacles();

	// 鍵とドアのアイコンを生成
	void CreateKeyAndDoorIcons();

	// 鍵とドアのアイコンを更新
	void UpdateKeyAndDoorIcons();

	// 円形のミニマップ背景を描画
	void DrawCircle();

	// ミニマップの枠を描画
	void DrawBorder();

	// プレイヤーマーカー（三角形）を描画
	void DrawPlayerMarker();

	// 線分を描画
	void DrawLine(const Vector2& start, const Vector2& end, const Vector4& color);

	// 座標変換（3D世界座標→2Dミニマップ座標）
	Vector2 WorldToMinimap(const Vector3& worldPos);

	// 点が円の内側にあるかチェック
	bool IsInsideCircle(const Vector2& point);

	// 各種スプライト
	Sprite* backgroundSprite_ = nullptr; // 背景用
	Sprite* playerSprite_ = nullptr;     // プレイヤーマーカー用
	Sprite* borderSprite_ = nullptr;     // 枠用
	Sprite* labelSprite_ = nullptr;      // MAPラベル用
	std::vector<Sprite*> mapChips_;      // マップチップ用
	std::vector<Sprite*> keySprites_;    // 鍵アイコン用
	std::vector<Sprite*> doorSprites_;   // ドアアイコン用

	// テクスチャハンドル
	uint32_t backgroundHandle_ = 0;
	uint32_t playerHandle_ = 0; // player.png用
	uint32_t borderHandle_ = 0;
	uint32_t mapChipHandle_ = 0;
	uint32_t labelHandle_ = 0;
	uint32_t keyHandle_ = 0;  // key.png用
	uint32_t doorHandle_ = 0; // door.png用

	// 参照
	Player* player_ = nullptr;
	MapLoader* mapLoader_ = nullptr;
	EnemyLoader* enemyLoader_ = nullptr;
	std::vector<std::vector<AABB>> obstacles_;

	// ミニマップの設定
	Vector2 position_ = {0, 0}; // 左上の位置
	Vector2 size_ = {300, 300}; // サイズ（より大きくする）
	float radius_ = 150.0f;     // 円の半径
	Vector2 center_ = {0, 0};   // 円の中心位置
	float scale_ = 0.7f;        // 世界座標からミニマップ座標への変換スケール（大幅に拡大）

	// プレイヤー情報
	Vector2 playerMinimapPos_ = {0, 0}; // プレイヤーのミニマップ上の位置
	float playerRotation_ = 0.0f;       // プレイヤーの向き
};