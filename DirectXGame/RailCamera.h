#pragma once
#include "Mymath.h"
#include <3d/Camera.h>
#include <3d/WorldTransform.h>

class Player;

class RailCamera {

public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	void Initialize(const KamataEngine::Vector3& pos, const KamataEngine::Vector3& rad);
	void Update();

	Player* target_ = nullptr;

	void setTarget(Player* target) { target_ = target; }
	const Camera& GetViewProjection() { return camera_; }
	const WorldTransform& GetWorldTransform() { return worldtransfrom_; }

private:
	KamataEngine::WorldTransform worldtransfrom_;

	KamataEngine::Camera camera_;

	KamataEngine::Vector3 velocity_ = {1.0f, 1.0f, 1.0f};
	KamataEngine::Vector3 rotationVelocity_ = {0.0f, 0.0f, 0.0f};
};