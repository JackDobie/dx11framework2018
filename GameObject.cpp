#include "GameObject.h"

GameObject::GameObject(MeshData mesh, XMFLOAT4 position, XMFLOAT4 rotation, XMFLOAT4 scale)
{
	_mesh = mesh;
	_position = position;
	_rotation = rotation;
	_scale = scale;
}

void GameObject::SetOBJ(MeshData mesh)
{
	_mesh = mesh;
}

void GameObject::SetPosition(XMFLOAT4 position)
{
	_position = position;
}
void GameObject::SetRotation(XMFLOAT4 rotation)
{
	_rotation = rotation;
}
void GameObject::SetScale(XMFLOAT4 scale)
{
	_scale = scale;
}
void GameObject::SetTransform(XMFLOAT4X4 transform)
{
	_transform = transform;
}

void GameObject::Update()
{
	XMStoreFloat4x4(&_transform, XMMatrixScaling(_scale.x, _scale.y, _scale.z) * XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z) * XMMatrixTranslation(_position.x, _position.y, _position.z));
}

XMFLOAT4 GameObject::GetPosition()
{
	return _position;
}
XMFLOAT4 GameObject::GetRotation()
{
	return _rotation;
}
XMFLOAT4 GameObject::GetScale()
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