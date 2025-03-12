#include "Ground.h"
#include <vector>

// デストラクタを追加
Ground::~Ground() {
	// リソースの解放
	if (rigidBody_) {
		delete rigidBody_->getMotionState();
		delete rigidBody_;
	}
	delete meshShape_;
	delete triangleMesh_;
	delete groundModel_;
}

void Ground::Init(Camera* camera) {
	camera_ = camera;
	worldTransform.Initialize();
	// "ground" という名前のOBJファイルを読み込みます。
	groundModel_ = Model::CreateFromOBJ("ground", true);

	// 地面の位置設定
	position = Vector3(0, 0, 0);
	worldTransform.translation_ = position;

	// モデルから三角メッシュコリジョンを作成
	CreateTriangleMeshFromModel();

	// 剛体の初期トランスフォーム
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(position.x, position.y, position.z));

	// モーションステートの作成
	motionState_ = new btDefaultMotionState(transform);

	// 剛体の質量と慣性（地形は静的なので質量は0）
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

	// KamataEngineのモデルからメッシュデータを取得する
	// 注: ここではOBJファイルから直接読み込む代替案を示します

	FILE* file = fopen("Resources/ground.obj", "r");
	if (!file) {
		// ファイルが開けない場合はデフォルトの平面メッシュを作成
		for (int i = -10; i < 10; i++) {
			for (int j = -10; j < 10; j++) {
				btVector3 v0(i, 0, j);
				btVector3 v1(i + 1, 0, j);
				btVector3 v2(i + 1, 0, j + 1);
				btVector3 v3(i, 0, j + 1);
				triangleMesh_->addTriangle(v0, v1, v2);
				triangleMesh_->addTriangle(v0, v2, v3);
			}
		}
	} else {
		// OBJファイルからメッシュデータを読み込む
		std::vector<btVector3> vertices;
		std::vector<int> indices;

		char line[256];
		while (fgets(line, sizeof(line), file)) {
			if (line[0] == 'v' && line[1] == ' ') {
				// 頂点データ
				float x, y, z;
				sscanf(line + 2, "%f %f %f", &x, &y, &z);
				vertices.push_back(btVector3(x, y, z));
			} else if (line[0] == 'f' && line[1] == ' ') {
				// 面データ（三角形または四角形）
				int v0, v1, v2, v3;
				int matches = sscanf(line + 2, "%d %d %d %d", &v0, &v1, &v2, &v3);

				if (matches == 3) {
					// 三角形の場合
					indices.push_back(v0 - 1); // OBJは1から始まるので、0ベースに調整
					indices.push_back(v1 - 1);
					indices.push_back(v2 - 1);
				} else if (matches == 4) {
					// 四角形の場合、2つの三角形に分割
					indices.push_back(v0 - 1);
					indices.push_back(v1 - 1);
					indices.push_back(v2 - 1);

					indices.push_back(v0 - 1);
					indices.push_back(v2 - 1);
					indices.push_back(v3 - 1);
				}
			}
		}

		fclose(file);

		// 読み込んだデータから三角メッシュを構築
		for (size_t i = 0; i < indices.size(); i += 3) {
			if (i + 2 < indices.size()) {
				triangleMesh_->addTriangle(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
			}
		}
	}

	// 三角メッシュからBVH形状を作成（最適化された衝突判定用）
	meshShape_ = new btBvhTriangleMeshShape(triangleMesh_, true, true);
}

void Ground::Update() { worldTransform.TransferMatrix(); }

void Ground::Draw() { groundModel_->Draw(worldTransform, *camera_); }