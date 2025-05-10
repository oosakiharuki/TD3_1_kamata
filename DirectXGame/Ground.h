#pragma once
#include "KamataEngine.h"
#include "math/Vector3.h"
#include <string>
#include <vector>

using namespace KamataEngine;

// 三角形構造体（OBJから読み込む地形メッシュ用）
struct Triangle {
	Vector3 v0, v1, v2;
};

class Ground {
public:
	Ground();
	~Ground();

	void Init(Camera* camera);
	void Update();
	void Draw();

	// 指定座標における地面の高さを、OBJメッシュに基づくレイキャストで取得
	float GetHeightAt(const Vector3& pos);

private:
	WorldTransform worldTransform;
	Camera* camera_ = nullptr;
	Model* groundModel_ = nullptr;
	Vector3 position;

	// OBJファイルから抽出した三角形メッシュのリスト
	std::vector<Triangle> triangles;

	// 簡易的なOBJパーサー（"v" と "f" のみ対応、1-indexed と仮定）
	void LoadTrianglesFromOBJ(const std::string& filename);
};
