#pragma once

#include <DirectXCollision.h>
#include "OBJLoader.h"

class GameObject
{
private:
	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;
	XMFLOAT4X4 _transform;

	MeshData _mesh;

	BoundingSphere boundingSphere;

public:
	GameObject(MeshData mesh, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, float collisionRadius);

	void SetOBJ(MeshData mesh);

	void SetPosition(XMFLOAT3 position);
	void SetRotation(XMFLOAT3 rotation);
	void SetScale(XMFLOAT3 scale);
	void SetTransform(XMFLOAT4X4 transform);

	void SetCollisionRadius(float radius);
	void SetCollisionBox();
	bool CheckCollision(XMVECTOR rayOrigin, XMVECTOR rayDirection);

	void Update();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();
	XMFLOAT4X4 GetTransform();
	MeshData* GetMesh();
};

