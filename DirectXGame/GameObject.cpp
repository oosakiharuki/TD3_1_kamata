#include "GameObject.h"

GameObject::GameObject(Model* model) : model_(model) {
    transform_.Initialize();
}

GameObject::~GameObject() {}

void GameObject::Update() {
    transform_.UpdateMatrix();
}

void GameObject::Draw(Camera& camera) {
    if (isActive) {
        model_->Draw(transform_, camera);
    }
}

AABB GameObject::GetAABB() const {
    Vector3 min = transform_.translation_ - Vector3(0.5f, 0.5f, 0.5f);
    Vector3 max = transform_.translation_ + Vector3(0.5f, 0.5f, 0.5f);
    return AABB(min, max);
}