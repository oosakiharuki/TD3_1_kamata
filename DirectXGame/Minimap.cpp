#include "Minimap.h"
#include <algorithm> // std::maxの使用のために必要
#include <base/TextureManager.h>
#include <cmath>
#include <numbers>

Minimap::Minimap() {}

Minimap::~Minimap() {
	// スプライトの解放
	if (backgroundSprite_) {
		delete backgroundSprite_;
		backgroundSprite_ = nullptr;
	}

	if (playerSprite_) {
		delete playerSprite_;
		playerSprite_ = nullptr;
	}

	if (borderSprite_) {
		delete borderSprite_;
		borderSprite_ = nullptr;
	}

	if (labelSprite_) {
		delete labelSprite_;
		labelSprite_ = nullptr;
	}

	// マップチップの解放
	for (auto& chip : mapChips_) {
		if (chip) {
			delete chip;
			chip = nullptr;
		}
	}
	mapChips_.clear();

	// 鍵とドアのスプライト解放
	ClearKeyAndDoorSprites();
}

void Minimap::Initialize(Player* player, MapLoader* mapLoader, EnemyLoader* enemyLoader, const std::vector<std::vector<AABB>>& obstacles) {
	player_ = player;
	mapLoader_ = mapLoader;
	enemyLoader_ = enemyLoader;
	obstacles_ = obstacles;

	// ウィンドウサイズの取得
	const int windowWidth = WinApp::kWindowWidth;
	const int windowHeight = WinApp::kWindowHeight;

	// ミニマップのサイズを大きくする (300x300)
	size_ = {300, 300};
	radius_ = size_.x / 2.0f;

	// ミニマップの位置（右下）
	position_ = {static_cast<float>(windowWidth) - size_.x - 20, static_cast<float>(windowHeight) - size_.y - 20};
	center_ = {position_.x + radius_, position_.y + radius_};

	// 拡大スケールの調整 - より大きなスケールで遠くまで見えるように
	scale_ = 0.7f; // 大幅に拡大

	// テクスチャのロード
	backgroundHandle_ = TextureManager::Load("white1x1.png"); // 背景用
	playerHandle_ = TextureManager::Load("minimap/player.png");       // プレイヤー用
	borderHandle_ = TextureManager::Load("white1x1.png");     // 枠用
	mapChipHandle_ = TextureManager::Load("white1x1.png");    // マップチップ用
	keyHandle_ = TextureManager::Load("minimap/key.png");             // 鍵用
	doorHandle_ = TextureManager::Load("minimap/key.png");           // ドア用

	// スプライトの生成
	// 背景（暗めのグレー）
	backgroundSprite_ = Sprite::Create(backgroundHandle_, position_);
	backgroundSprite_->SetSize(size_);
	backgroundSprite_->SetColor({0.2f, 0.2f, 0.2f, 0.8f});

	// 枠（白色）
	borderSprite_ = Sprite::Create(borderHandle_, position_);
	borderSprite_->SetSize(size_);
	borderSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.7f});

	// プレイヤーアイコン用スプライト
	playerSprite_ = Sprite::Create(playerHandle_, {0, 0});
	playerSprite_->SetSize({20, 20});
	playerSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	// MAPラベル
	labelSprite_ = Sprite::Create(backgroundHandle_, {position_.x + size_.x - 60, position_.y + 10});
	labelSprite_->SetSize({50, 25});
	labelSprite_->SetColor({0.3f, 0.3f, 0.3f, 0.9f});

	// 障害物データからマップチップを生成
	CreateMapChipsFromObstacles();

	// 鍵とドアのアイコンを作成
	CreateKeyAndDoorIcons();
}

void Minimap::CreateKeyAndDoorIcons() {
	// 既存の鍵とドアスプライトを安全に解放
	ClearKeyAndDoorSprites();

	// MapLoaderから鍵の情報を取得
	if (mapLoader_) {
		// 鍵のアイコンを作成
		const std::vector<Key*>& keys = mapLoader_->GetKeyList();
		for (const auto& key : keys) {
			if (key) {
				Vector3 keyPos = key->GetWorldPosition();
				Vector2 minimapPos = WorldToMinimap(keyPos);

				if (IsInsideCircle(minimapPos)) {
					// 修正: 鍵アイコンのサイズを調整し、位置を中心に合わせる
					Sprite* keyIcon = Sprite::Create(keyHandle_, {0, 0});
					if (keyIcon) { // nullチェックを追加
						// サイズを小さく設定（16x16）
						keyIcon->SetSize({16, 16});
						// 位置を中心に調整
						keyIcon->SetPosition({minimapPos.x - 8.0f, minimapPos.y - 8.0f});
						keyIcon->SetColor({1.0f, 1.0f, 1.0f, 0.9f});
						keySprites_.push_back(keyIcon);
					}
				}
			}
		}

		// ドアのアイコンを作成
		const std::vector<Door*>& doors = mapLoader_->GetDoorList();
		for (const auto& door : doors) {
			if (door) {
				Vector3 doorPos = door->GetWorldPosition();
				Vector2 minimapPos = WorldToMinimap(doorPos);

				if (IsInsideCircle(minimapPos)) {
					Sprite* doorIcon = Sprite::Create(doorHandle_, {0, 0});
					if (doorIcon) { // nullチェックを追加
						// サイズと位置を調整
						doorIcon->SetSize({20, 20});
						doorIcon->SetPosition({minimapPos.x - 10.0f, minimapPos.y - 10.0f});
						doorIcon->SetColor({1.0f, 1.0f, 1.0f, 0.9f});
						doorSprites_.push_back(doorIcon);
					}
				}
			}
		}
	}
}
void Minimap::CreateMapChipsFromObstacles() {
	// 既存のマップチップを安全に解放
	for (auto& chip : mapChips_) {
		if (chip) {
			delete chip;
			chip = nullptr;
		}
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

					// サイズをスケールに合わせて調整 - 視認性向上のためサイズを大きめに
					float chipWidth = width * scale_ * 1.5f;
					float chipDepth = depth * scale_ * 1.5f;
					// 最小サイズを設定して小さすぎる要素も見えるようにする
					chipWidth = (chipWidth < 10.0f) ? 10.0f : chipWidth;
					chipDepth = (chipDepth < 10.0f) ? 10.0f : chipDepth;
					chip->SetSize({chipWidth, chipDepth});
					chip->SetColor({0.5f, 0.5f, 0.5f, 0.9f}); // 灰色

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
					chip->SetSize({15, 15});                  // ブロックのサイズも大きく
					chip->SetColor({0.5f, 0.5f, 0.5f, 0.9f}); // 灰色
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

	// 鍵とドアのアイコンを更新
	UpdateKeyAndDoorIcons();
}

void Minimap::UpdateKeyAndDoorIcons() {
	// 鍵の状態が変わった場合にアイコンを更新
	bool needUpdate = false;

	// MapLoaderから鍵の情報を取得
	if (mapLoader_) {
		const std::vector<Key*>& keys = mapLoader_->GetKeyList();

		// 鍵の数が変わった場合、または取得状態が変わった場合に更新
		if (keys.size() != keySprites_.size()) {
			needUpdate = true;
		} else {
			for (size_t i = 0; i < keys.size(); i++) {
				// 鍵が取得された場合、アイコンの色を半透明に
				if (keys[i] && keys[i]->IsKeyObtained() && i < keySprites_.size()) {
					keySprites_[i]->SetColor({1.0f, 1.0f, 1.0f, 0.4f}); // 半透明
				}
			}
		}

		// ドアが開いた場合、アイコンの色を変更
		const std::vector<Door*>& doors = mapLoader_->GetDoorList();
		for (size_t i = 0; i < doors.size() && i < doorSprites_.size(); i++) {
			if (doors[i] && doors[i]->IsDoorOpened()) {
				doorSprites_[i]->SetColor({0.7f, 0.7f, 0.7f, 0.9f}); // ドアが開いたら少し暗く
			}
		}
	}

	// 更新が必要な場合は鍵とドアのアイコンを再作成
	if (needUpdate) {
		CreateKeyAndDoorIcons();
	}
}

void Minimap::Draw() {
	// 背景の円形部分を描画
	DrawCircle();

	// マップチップを描画 - スケールを考慮したサイズで表示
	for (auto& chip : mapChips_) {
		if (chip && IsInsideCircle(chip->GetPosition())) {
			// マップチップのサイズを若干大きくして見やすくする
			Vector2 currentSize = chip->GetSize();
			Vector2 adjustedSize = {currentSize.x * 1.2f, currentSize.y * 1.2f};
			chip->SetSize(adjustedSize);
			chip->Draw();
		}
	}

	// 鍵のアイコンを描画
	for (auto& keySprite : keySprites_) {
		if (keySprite && IsInsideCircle(keySprite->GetPosition())) {
			keySprite->Draw();
		}
	}

	// ドアのアイコンを描画
	for (auto& doorSprite : doorSprites_) {
		if (doorSprite && IsInsideCircle(doorSprite->GetPosition())) {
			doorSprite->Draw();
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
	// 背景の円全体を描画（半透明の明るいグレー）
	backgroundSprite_->SetPosition(position_);
	backgroundSprite_->SetSize(size_);
	backgroundSprite_->SetRotation(0);
	backgroundSprite_->SetColor({0.3f, 0.3f, 0.3f, 0.8f});
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

	// プレイヤーアイコンのサイズ設定
	const float iconSize = 20.0f;

	// 以前はプレイヤーの向きに合わせてスプライトを回転していた
	// float angle = playerRotation_;

	// 回転を適用しない（固定向き）
	float angle = 0.0f;

	// プレイヤーアイコンを描画
	playerSprite_->SetPosition(Vector2(playerMinimapPos_.x - iconSize / 2, playerMinimapPos_.y - iconSize / 2));
	playerSprite_->SetSize(Vector2(iconSize, iconSize));
	playerSprite_->SetRotation(angle);
	playerSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f}); // 白色（透過画像前提）
	playerSprite_->Draw();
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

void Minimap::ClearKeyAndDoorSprites() {
	// 鍵スプライトの解放
	for (auto& keySprite : keySprites_) {
		if (keySprite) {
			delete keySprite;
			keySprite = nullptr;
		}
	}
	keySprites_.clear();

	// ドアスプライトの解放
	for (auto& doorSprite : doorSprites_) {
		if (doorSprite) {
			delete doorSprite;
			doorSprite = nullptr;
		}
	}
	doorSprites_.clear();
}