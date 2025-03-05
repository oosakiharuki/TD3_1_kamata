#include "Ground.h"
#include <cfloat>
#include <cmath>
#include <fstream>
#include <sstream>
using namespace KamataEngine::MathUtility;

Ground::Ground() {}

Ground::~Ground() { delete groundModel_; }

void Ground::Init(Camera* camera) {
	camera_ = camera;
	worldTransform.Initialize();
	// "ground" という名前でOBJファイルからモデルを読み込み
	groundModel_ = Model::CreateFromOBJ("ground", true);
	// OBJファイルから三角形メッシュを読み込む（ファイル名は"ground.obj"と仮定）
	LoadTrianglesFromOBJ("ground.obj");
}

void Ground::Update() { worldTransform.TransferMatrix(); }

void Ground::Draw() { groundModel_->Draw(worldTransform, *camera_); }

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

// レイと三角形の交差判定（Möller-Trumboreアルゴリズム）
static bool IntersectRayTriangle(const Vector3& rayOrigin, const Vector3& rayDir, const Triangle& tri, float& t) {
	const float epsilon = 0.000001f;
	Vector3 edge1 = tri.v1 - tri.v0;
	Vector3 edge2 = tri.v2 - tri.v0;
	Vector3 h = rayDir.cross(edge2);
	float a = edge1.dot(h);
	if (fabs(a) < epsilon)
		return false;
	float f = 1.0f / a;
	Vector3 s = rayOrigin - tri.v0;
	float u = f * s.dot(h);
	if (u < 0.0f || u > 1.0f)
		return false;
	Vector3 q = s.cross(edge1);
	float v = f * rayDir.dot(q);
	if (v < 0.0f || u + v > 1.0f)
		return false;
	t = f * edge2.dot(q);
	return (t > epsilon);
}

float Ground::GetHeightAt(const Vector3& pos) {
	// 足元から下方向へレイキャスト（方向は(0, -1, 0)）
	Vector3 rayOrigin = pos;
	Vector3 rayDir = {0, -1, 0};
	float closestT = FLT_MAX;
	bool hit = false;

	for (const auto& tri : triangles) {
		float t;
		if (IntersectRayTriangle(rayOrigin, rayDir, tri, t)) {
			if (t < closestT) {
				closestT = t;
				hit = true;
			}
		}
	}

	if (hit) {
		// 交差点の座標から Y 値を返す
		Vector3 hitPoint = {rayOrigin.x + rayDir.x * closestT, rayOrigin.y + rayDir.y * closestT, rayOrigin.z + rayDir.z * closestT};
		return hitPoint.y;
	}
	return 0.0f;
}
