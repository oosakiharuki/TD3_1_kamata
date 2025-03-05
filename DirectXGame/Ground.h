#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

class Ground {
public:
	Ground();
	~Ground();
	// 初期化処理
	void Init(Camera* camera);
	// 更新処理
	void Update();
	// 描画処理
	void Draw();
	// 指定座標における地面の高さを返すメソッド
	float GetHeightAt(const Vector3& pos);

private:
	WorldTransform worldTransform;
	Camera* camera_ = nullptr;
	Model* groundModel_ = nullptr;
	Vector3 position;
};
