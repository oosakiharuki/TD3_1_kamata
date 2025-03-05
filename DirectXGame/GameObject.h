#pragma once
#include <KamataEngine.h>
using namespace KamataEngine;
#include "AABB.h"

class GameObject {
public:
    GameObject(Model* model);
    ~GameObject();

    void Update();
    void Draw(Camera& camera);

    AABB GetAABB() const; // AABB取得

    bool isActive = true; // 消滅判定

    WorldTransform transform_;

private:
    Model* model_;
};