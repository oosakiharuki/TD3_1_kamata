#include "Ground.h"
#include <cfloat>
#include <cmath>
#include <fstream>
#include <sstream>
using namespace KamataEngine::MathUtility;

Ground::Ground() {}

Ground::~Ground() {
	delete groundModel_;
	delete groundScaffoldModel_; // GroundScaffoldのモデルを解放
}

void Ground::Init(Camera* camera) {
	camera_ = camera;
	worldTransform.Initialize();
	// "ground" という名前でOBJファイルからモデルを読み込み
	groundModel_ = Model::CreateFromOBJ("ground", true);
	// "groundScaffold" という名前でOBJファイルからモデルを読み込み
	groundScaffoldModel_ = Model::CreateFromOBJ("GroundScaffold2", true);
	// OBJファイルから三角形メッシュを読み込む（ファイル名は"ground.obj"と仮定）
	LoadTrianglesFromOBJ("ground.obj");
}

void Ground::Update() { worldTransform.TransferMatrix(); }

void Ground::Draw() {
	groundModel_->Draw(worldTransform, *camera_);
}

void Ground::LoadTrianglesFromOBJ(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open())
		return;

	std::vector<Vector3> vertices;
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		if (prefix == "v") {
			Vector3 v;
			iss >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		} else if (prefix == "f") {
			int i0, i1, i2;
			iss >> i0 >> i1 >> i2;
			Triangle tri;
			tri.v0 = vertices[i0 - 1];
			tri.v1 = vertices[i1 - 1];
			tri.v2 = vertices[i2 - 1];
			triangles.push_back(tri);
		}
	}
	file.close();
}

float Ground::GetHeightAt(const Vector3& pos) {
	// 足元から下方向へレイキャスト（方向は(0, -1, 0)）
	Vector3 rayOrigin = pos;
	Vector3 rayDir = {0, -1, 0};
	float closestT = FLT_MAX;
	bool hit = false;

	if (hit) {
		// 交差点の座標から Y 値を返す
		Vector3 hitPoint = {rayOrigin.x + rayDir.x * closestT, rayOrigin.y + rayDir.y * closestT, rayOrigin.z + rayDir.z * closestT};
		return hitPoint.y;
	}
	return 0.0f;
}
