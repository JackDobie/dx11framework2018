#include "GameObject.h"

GameObject::GameObject(MeshData mesh, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, float collisionRadius)
{
	_mesh = mesh;
	_position = position;
	_rotation = rotation;
	_scale = scale;
	boundingSphere.Center = _position;
	boundingSphere.Radius = collisionRadius;
}

void GameObject::SetOBJ(MeshData mesh)
{
	_mesh = mesh;
}

void GameObject::SetPosition(XMFLOAT3 position)
{
	_position = position;
}
void GameObject::SetRotation(XMFLOAT3 rotation)
{
	_rotation = rotation;
}
void GameObject::SetScale(XMFLOAT3 scale)
{
	_scale = scale;
}
void GameObject::SetTransform(XMFLOAT4X4 transform)
{
	_transform = transform;
}

void GameObject::SetCollisionRadius(float radius)
{
	boundingSphere.Radius = radius;
}

bool GameObject::CheckCollision(XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	float distance = 1.0f;

	//rayDir = XMVector3Normalize(rayDir);
	if (boundingSphere.Intersects(rayOrigin, rayDir, distance))
	{
		SetPosition(XMFLOAT3(_position.x, _position.y, _position.z + 1));
		return true;
	}
	return false;
}

void GameObject::Update()
{
	XMStoreFloat4x4(&_transform, XMMatrixScaling(_scale.x, _scale.y, _scale.z) * XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z) * XMMatrixTranslation(_position.x, _position.y, _position.z));
	boundingSphere.Center = _position;
}

XMFLOAT3 GameObject::GetPosition()
{
	return _position;
}
XMFLOAT3 GameObject::GetRotation()
{
	return _rotation;
}
XMFLOAT3 GameObject::GetScale()
{
	return _scale;
}
XMFLOAT4X4 GameObject::GetTransform()
{
	return _transform;
}
MeshData* GameObject::GetMesh()
{
	return &_mesh;
}