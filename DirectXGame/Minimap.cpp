#include "Minimap.h"
#include <base/TextureManager.h>
#include <cmath>
#include <numbers>

Minimap::Minimap() {}

Minimap::~Minimap() {
	// スプライトの解放
	if (backgroundSprite_)
		delete backgroundSprite_;
	if (playerSprite_)
		delete playerSprite_;
	if (borderSprite_)
		delete borderSprite_;
	if (labelSprite_)
		delete labelSprite_;

	// マップチップの解放
	for (auto& chip : mapChips_) {
		if (chip)
			delete chip;
	}
	mapChips_.clear();
}

void Minimap::Initialize(Player* player, MapLoader* mapLoader, EnemyLoader* enemyLoader, const std::vector<std::vector<AABB>>& obstacles) {
	player_ = player;
	mapLoader_ = mapLoader;
	enemyLoader_ = enemyLoader;
	obstacles_ = obstacles;

	// ウィンドウサイズの取得
	const int windowWidth = WinApp::kWindowWidth;
	const int windowHeight = WinApp::kWindowHeight;

	// ミニマップのサイズを円形に適した正方形に設定
	size_ = {180, 180};
	radius_ = size_.x / 2.0f;

	// ミニマップの位置（右下）
	position_ = {static_cast<float>(windowWidth) - size_.x - 20, static_cast<float>(windowHeight) - size_.y - 20};
	center_ = {position_.x + radius_, position_.y + radius_};

	// 拡大スケールの調整
	scale_ = 0.3f;

	// テクスチャのロード
	backgroundHandle_ = TextureManager::Load("white1x1.png"); // 背景用
	playerHandle_ = TextureManager::Load("white1x1.png");     // プレイヤー用
	borderHandle_ = TextureManager::Load("white1x1.png");     // 枠用
	mapChipHandle_ = TextureManager::Load("white1x1.png");    // マップチップ用
	
	// スプライトの生成
	// 背景（暗めのグレー）
	backgroundSprite_ = Sprite::Create(backgroundHandle_, position_);
	backgroundSprite_->SetSize(size_);
	backgroundSprite_->SetColor({0.2f, 0.2f, 0.2f, 0.8f});

	// 枠（白色）
	borderSprite_ = Sprite::Create(borderHandle_, position_);
	borderSprite_->SetSize(size_);
	borderSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.7f});

	// プレイヤー（三角形マーカー用のスプライト - 実際の描画はDrawで行う）
	playerSprite_ = Sprite::Create(playerHandle_, {0, 0});
	playerSprite_->SetSize({10, 10});
	playerSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	// MAPラベル
	labelSprite_ = Sprite::Create(labelHandle_, {position_.x + size_.x - 40, position_.y + size_.y - 20});
	labelSprite_->SetSize({40, 20});
	labelSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	// 障害物データからマップチップを生成
	CreateMapChipsFromObstacles();
}

void Minimap::CreateMapChipsFromObstacles() {
	// 既存のマップチップをクリア
	for (auto& chip : mapChips_) {
		if (chip)
			delete chip;
	}
	mapChips_.clear();

	// 障害物リストからマップチップを生成
	for (const auto& obstacleList : obstacles_) {
		for (const auto& obstacle : obstacleList) {
			// 障害物のサイズと位置から必要なチップ数を計算
			float width = obstacle.max.x - obstacle.min.x;
			float depth = obstacle.max.z - obstacle.min.z;
			float height = obstacle.max.y - obstacle.min.y;

			// 床レベルの障害物のみを表示（高さで判断）
			if (height < 5.0f) {
				// 障害物の中心位置
				Vector3 center = {(obstacle.min.x + obstacle.max.x) * 0.5f, (obstacle.min.y + obstacle.max.y) * 0.5f, (obstacle.min.z + obstacle.max.z) * 0.5f};

				// マップチップの作成
				Vector2 chipPos = WorldToMinimap(center);
				// ミニマップの円の内側にあるかチェック
				if (IsInsideCircle(chipPos)) {
					Sprite* chip = Sprite::Create(mapChipHandle_, chipPos);

					// サイズをスケールに合わせて調整
					float chipWidth = width * scale_;
					float chipDepth = depth * scale_;
					chip->SetSize({chipWidth, chipDepth});
					chip->SetColor({0.1f, 0.1f, 0.1f, 0.9f}); // 暗めの色

					mapChips_.push_back(chip);
				}
			}
		}
	}

	// ブロックをマップチップとして追加
	if (mapLoader_) {
		for (const auto& block : mapLoader_->GetBlockList()) {
			if (block && block->IsActive()) {
				AABB blockAABB = block->GetAABB();
				Vector3 center = {(blockAABB.min.x + blockAABB.max.x) * 0.5f, (blockAABB.min.y + blockAABB.max.y) * 0.5f, (blockAABB.min.z + blockAABB.max.z) * 0.5f};

				Vector2 chipPos = WorldToMinimap(center);
				if (IsInsideCircle(chipPos)) {
					Sprite* chip = Sprite::Create(mapChipHandle_, chipPos);
					chip->SetSize({8, 8});
					chip->SetColor({0.3f, 0.3f, 0.3f, 0.9f}); // やや明るい色
					mapChips_.push_back(chip);
				}
			}
		}
	}
}

void Minimap::Update() {
	if (!player_)
		return;

	// ミニマップの位置をプレイヤーの周囲に更新
	Vector3 playerPos = player_->GetWorldPosition();
	playerMinimapPos_ = WorldToMinimap(playerPos);

	// プレイヤーの向きを取得（worldTransform_の回転から）
	playerRotation_ = player_->GetWorld()->rotation_.y;
}

void Minimap::Draw() {
	// 背景の円形部分を描画
	DrawCircle();

	// マップチップを描画
	for (auto& chip : mapChips_) {
		if (chip && IsInsideCircle(chip->GetPosition())) {
			chip->Draw();
		}
	}

	// プレイヤーマーカー（三角形）を描画
	DrawPlayerMarker();

	// 外枠の円を描画
	DrawBorder();

	// MAPラベルを描画
	if (labelSprite_) {
		labelSprite_->Draw();
	}
}

void Minimap::DrawCircle() {
	// 背景を円形に見せるために、複数の細長い四角形を扇状に並べる
	const int segments = 36; // 円を近似するための分割数
	const float angleStep = 2.0f * static_cast<float>(std::numbers::pi) / segments;

	for (int i = 0; i < segments; i++) {
		float angle1 = i * angleStep;
		// angle2は使用しないので削除

		// x1, y1, x2, y2は使わないので削除

		// 扇形の四角形近似を描画
		Vector2 segmentPos = {center_.x - 2, center_.y - 2};
		Vector2 segmentSize = {4, radius_};

		backgroundSprite_->SetPosition(segmentPos);
		backgroundSprite_->SetSize(segmentSize);
		backgroundSprite_->SetRotation(angle1); // 回転させて扇形に
		backgroundSprite_->Draw();
	}

	// 背景の円全体を描画（半透明の明るいグレー）
	backgroundSprite_->SetPosition(position_);
	backgroundSprite_->SetSize(size_);
	backgroundSprite_->SetRotation(0);
	backgroundSprite_->SetColor({0.4f, 0.4f, 0.4f, 0.6f});
	backgroundSprite_->Draw();
}

void Minimap::DrawBorder() {
	// 円形の外枠を描画
	const int segments = 36;
	const float angleStep = 2.0f * static_cast<float>(std::numbers::pi) / segments;
	const float borderThickness = 2.0f;

	for (int i = 0; i < segments; i++) {
		float angle1 = i * angleStep;
		float angle2 = (i + 1) * angleStep;

		float x1 = center_.x + radius_ * static_cast<float>(std::cos(angle1));
		float y1 = center_.y + radius_ * static_cast<float>(std::sin(angle1));
		float x2 = center_.x + radius_ * static_cast<float>(std::cos(angle2));
		float y2 = center_.y + radius_ * static_cast<float>(std::sin(angle2));

		// 線分を描画
		Vector2 segmentPos = {x1 - borderThickness / 2, y1 - borderThickness / 2};
		Vector2 segmentSize = {static_cast<float>(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2))) + borderThickness, borderThickness};

		float segmentAngle = static_cast<float>(std::atan2(y2 - y1, x2 - x1));

		borderSprite_->SetPosition(segmentPos);
		borderSprite_->SetSize(segmentSize);
		borderSprite_->SetRotation(segmentAngle);
		borderSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.8f});
		borderSprite_->Draw();
	}
}

void Minimap::DrawPlayerMarker() {
	if (!player_ || !IsInsideCircle(playerMinimapPos_))
		return;

	// プレイヤーの三角形マーカーを描画
	const float markerSize = 8.0f;

	// 三角形の3つの頂点を計算（プレイヤーの向きに合わせて回転）
	float angle = playerRotation_; // プレイヤーの向き

	// 三角形の頂点座標（相対座標）
	Vector2 point1 = {markerSize * static_cast<float>(std::sin(angle)), -markerSize * static_cast<float>(std::cos(angle))};
	Vector2 point2 = {markerSize * static_cast<float>(std::sin(angle + 2.1f)), -markerSize * static_cast<float>(std::cos(angle + 2.1f))};
	Vector2 point3 = {markerSize * static_cast<float>(std::sin(angle - 2.1f)), -markerSize * static_cast<float>(std::cos(angle - 2.1f))};

	// 三角形の描画（3つの点を結ぶ3つの線分で描画）
	// 中央に三角形を配置するためにplayerMinimapPos_を足す
	Vector2 p1 = {playerMinimapPos_.x + point1.x, playerMinimapPos_.y + point1.y};
	Vector2 p2 = {playerMinimapPos_.x + point2.x, playerMinimapPos_.y + point2.y};
	Vector2 p3 = {playerMinimapPos_.x + point3.x, playerMinimapPos_.y + point3.y};

	// 線分1: p1->p2
	DrawLine(p1, p2, {1.0f, 1.0f, 1.0f, 1.0f});

	// 線分2: p2->p3
	DrawLine(p2, p3, {1.0f, 1.0f, 1.0f, 1.0f});

	// 線分3: p3->p1
	DrawLine(p3, p1, {1.0f, 1.0f, 1.0f, 1.0f});
}

void Minimap::DrawLine(const Vector2& start, const Vector2& end, const Vector4& color) {
	// 2点間の線分を描画
	float length = static_cast<float>(std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2)));
	float angle = static_cast<float>(std::atan2(end.y - start.y, end.x - start.x));

	playerSprite_->SetPosition(start);
	playerSprite_->SetSize({length, 2.0f});
	playerSprite_->SetRotation(angle);
	playerSprite_->SetColor(color);
	playerSprite_->Draw();
}

Vector2 Minimap::WorldToMinimap(const Vector3& worldPos) {
	// 世界座標からミニマップ座標への変換
	// X-Z平面を上から見た形でミニマップに投影
	float minimapX = center_.x + worldPos.x * scale_;
	float minimapY = center_.y - worldPos.z * scale_; // Z座標が奥行きなので符号を反転

	return {minimapX, minimapY};
}

bool Minimap::IsInsideCircle(const Vector2& point) {
	// 点がミニマップの円の内側にあるかチェック
	float dx = point.x - center_.x;
	float dy = point.y - center_.y;
	float distanceSquared = dx * dx + dy * dy;

	return distanceSquared <= radius_ * radius_;
}