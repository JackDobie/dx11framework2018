#include "GameObject.h"

GameObject::GameObject(MeshData mesh, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, float collisionRadius)
{
	_mesh = mesh;
	_position = position;
	_rotation = rotation;
	_scale = scale;
	boundingSphere.Radius = collisionRadius;
	_falling = false;
	Update();
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
void GameObject::SetFalling(bool fall)
{
	_falling = fall;
}

//objects move relative to the camera, so they only need to use the camera's forward and right vector
void GameObject::MoveWithCam(float speed, XMVECTOR camTarget)
{
	XMVECTOR amount = XMVectorReplicate(speed);
	XMVECTOR pos = XMLoadFloat3(&_position);
	//multiply forward vector by speed, and add to position
	XMFLOAT4 out;
	XMStoreFloat4(&out, XMVectorMultiplyAdd(amount, camTarget, pos));
	//restrict movement beyond 9.5 x and z
	if (out.x + boundingSphere.Radius > 10 || out.x - boundingSphere.Radius < -10)
		out.x = _position.x;
	if (out.z + boundingSphere.Radius > 10 || out.z - boundingSphere.Radius < -10)
		out.z = _position.z;
	//update position with new x and z
	_position = XMFLOAT3(out.x, _position.y, out.z);
}
void GameObject::StrafeWithCam(float speed, XMVECTOR camRight)
{
	XMVECTOR amount = XMVectorReplicate(speed);
	XMVECTOR pos = XMLoadFloat3(&_position);
	//multiply right facing vector by speed, and add to position
	XMFLOAT4 out;
	XMStoreFloat4(&out, XMVectorMultiplyAdd(amount, camRight, pos));
	//restrict movement beyond 9.5 x and z
	if (out.x + boundingSphere.Radius > 10 || out.x - boundingSphere.Radius < -10)
		out.x = _position.x;
	if (out.z + boundingSphere.Radius > 10 || out.z - boundingSphere.Radius < -10)
		out.z = _position.z;
	//update position with new x and z
	_position = XMFLOAT3(out.x, _position.y, out.z);
}

void GameObject::SetCollisionRadius(float radius)
{
	boundingSphere.Radius = radius;
}

bool GameObject::CheckCollision(XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	float distance = 0.0f;

	if (boundingSphere.Intersects(rayOrigin, rayDir, distance))
	{
		return true;
	}
	return false;
}

void GameObject::Update()
{
	//update the object transform with scale rotation and position
	XMStoreFloat4x4(&_transform, XMMatrixScaling(_scale.x, _scale.y, _scale.z) * XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z) * XMMatrixTranslation(_position.x, _position.y, _position.z));
	//update the boundingsphere centre per object. add half the radius to get the centre of the object.
	boundingSphere.Center = XMFLOAT3(_position.x + (boundingSphere.Radius / 2), _position.y - (boundingSphere.Radius / 2), _position.z - (boundingSphere.Radius / 2));
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
bool GameObject::GetFalling()
{
	return _falling;
}