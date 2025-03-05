#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

class Ground {
public:
	Ground();
	~Ground();
	// 初期化処理。引数としてカメラのポインタを受け取ります。
	void Init(Camera* camera);

	// 更新処理。ワールドトランスフォームの更新を行います。
	void Update();

	// 描画処理。読み込んだモデルを描画します。
	void Draw();

private:
	WorldTransform worldTransform;
	Camera* camera_ = nullptr;
	Model* groundModel_ = nullptr;
	Vector3 position;
};
