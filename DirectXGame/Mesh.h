#pragma once

#include <vector>
#include "Vector3.h"

namespace KamataEngine {

class Mesh {
public:
    struct Vertex {
        Vector3 pos;
        // 他の頂点属性（法線、UV座標など）を追加できます
    };

    const std::vector<Vertex>& GetVertices() const { return vertices_; }
    const std::vector<unsigned int>& GetIndices() const { return indices_; }

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
};

} // namespace KamataEngine
#include "Ground.h"
#include <vector>

void Ground::Init(Camera* camera) {
	camera_ = camera;
	worldTransform.Initialize();
	// "ground" という名前のOBJファイルを読み込みます。
	groundModel_ = Model::CreateFromOBJ("ground", true);

	// 地面の位置設定
	position = Vector3(0, 0, 0);
	worldTransform.translation_ = position;

	// 三角メッシュの作成とコリジョン形状の設定
	CreateTriangleMeshFromModel();

	// 剛体の初期トランスフォーム
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(position.x, position.y, position.z));

	// モーションステートの作成
	motionState_ = new btDefaultMotionState(transform);

	// 剛体の質量と慣性（地面は静的なので質量は0）
	btScalar mass = 0.0f;
	btVector3 localInertia(0, 0, 0);

	// 剛体の作成
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState_, meshShape_, localInertia);
	rbInfo.m_friction = 0.5f;    // 摩擦係数
	rbInfo.m_restitution = 0.1f; // 反発係数（低めに設定）
	rigidBody_ = new btRigidBody(rbInfo);

	// 静的オブジェクトとして設定
	rigidBody_->setCollisionFlags(rigidBody_->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	// 衝突応答フラグを設定
	rigidBody_->setContactProcessingThreshold(0.0f);

	worldTransform.TransferMatrix();
}

void Ground::CreateTriangleMeshFromModel() {
	// 新しい三角メッシュを作成
	triangleMesh_ = new btTriangleMesh();

	// モデルからメッシュデータを取得
	// 注: 以下は実際のKamataEngineのAPIに合わせて調整する必要があります
	// ここでは、モデルから頂点と面情報を取得するための擬似コードを示しています

	// モデルが持つすべてのメッシュを処理
	const auto& meshes = groundModel_->GetMeshes();

	for (const auto& mesh : meshes) {
		const auto& vertices = mesh->GetVertices();
		const auto& indices = mesh->GetIndices();

		// 三角形ごとに三角メッシュに追加
		for (size_t i = 0; i < indices.size(); i += 3) {
			if (i + 2 < indices.size()) {
				const auto& v0 = vertices[indices[i]].pos;
				const auto& v1 = vertices[indices[i + 1]].pos;
				const auto& v2 = vertices[indices[i + 2]].pos;

				// ワールド変換を適用（必要に応じて）
				btVector3 btv0(v0.x, v0.y, v0.z);
				btVector3 btv1(v1.x, v1.y, v1.z);
				btVector3 btv2(v2.x, v2.y, v2.z);

				// 三角メッシュに追加
				triangleMesh_->addTriangle(btv0, btv1, btv2);
			}
		}
	}

	// 三角メッシュからBVH形状を作成（最適化された衝突判定用）
	meshShape_ = new btBvhTriangleMeshShape(triangleMesh_, true, true);
}

void Ground::Update() { worldTransform.TransferMatrix(); }

void Ground::Draw() { groundModel_->Draw(worldTransform, *camera_); }

Ground::~Ground() {
	// リソースの解放
	delete rigidBody_;
	delete motionState_;
	delete meshShape_;
	delete triangleMesh_;
	delete groundModel_;
}
