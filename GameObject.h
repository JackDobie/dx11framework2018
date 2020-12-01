#pragma once

#include "OBJLoader.h"

class GameObject
{
private:
	XMFLOAT4 _position;
	XMFLOAT4 _rotation;
	XMFLOAT4 _scale;
	XMFLOAT4X4 _transform;

	MeshData _mesh;
public:
	GameObject(MeshData mesh, XMFLOAT4 position, XMFLOAT4 rotation, XMFLOAT4 scale);

	void SetOBJ(MeshData mesh);

	void SetPosition(XMFLOAT4 position);
	void SetRotation(XMFLOAT4 rotation);
	void SetScale(XMFLOAT4 scale);
	void SetTransform(XMFLOAT4X4 transform);

	void Update();

	XMFLOAT4 GetPosition();
	XMFLOAT4 GetRotation();
	XMFLOAT4 GetScale();
	XMFLOAT4X4 GetTransform();
	MeshData* GetMesh();
};

