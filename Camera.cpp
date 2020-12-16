#include "Camera.h"

Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
	SetPos(position);
	SetAt(at);
	SetUp(up);
	_right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Reshape(windowWidth, windowHeight, nearDepth, farDepth);

	moveSpeed = 10.0f;

	Update();
}

void Camera::SetPos(XMFLOAT3 newEye)
{
	_eye = newEye;
}
void Camera::SetUp(XMFLOAT3 newUp)
{
	_up = newUp;
}
void Camera::SetAt(XMFLOAT3 newAt)
{
	_at = newAt;
}

void Camera::AddAt(XMFLOAT3 addAt)
{
	//add passed in float4 to current camera target
	XMFLOAT3 added = XMFLOAT3(_at.x + addAt.x, _at.y + addAt.y, _at.z + addAt.z);
	//get difference in xyz
	float dx = added.x - _at.x;
	float dy = added.y - _at.y;
	float dz = added.z - _at.z;
	//apply rotation to the passed in float4 and current camera target
	XMFLOAT3 rot = XMFLOAT3(_at.x, _at.y, _at.z);
	rot = Rotate(dx, dy, dz, rot);
	_at = XMFLOAT3(rot.x, rot.y, rot.z);

	rot = XMFLOAT3(_right.x, _right.y, _right.z);
	rot = Rotate(dx, dy, dz, rot);
	_right = XMFLOAT3(rot.x, rot.y, rot.z);
}
void Camera::Move(float deltaTime)
{
	XMVECTOR amount = XMVectorReplicate(moveSpeed * deltaTime);
	XMVECTOR target = XMLoadFloat3(&_at);
	XMVECTOR pos = XMLoadFloat3(&_eye);
	//multiply forward vector by speed, and add to position
	XMFLOAT4 out;
	XMStoreFloat4(&out, XMVectorMultiplyAdd(amount, target, pos));
	_eye = XMFLOAT3(out.x, _eye.y, out.z);
}
void Camera::Strafe(float deltaTime)
{
	XMVECTOR amount = XMVectorReplicate(moveSpeed * deltaTime);
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR pos = XMLoadFloat3(&_eye);
	//multiply right facing vector by speed, and add to position
	XMFLOAT4 out;
	XMStoreFloat4(&out, XMVectorMultiplyAdd(amount, right, pos));
	_eye = XMFLOAT3(out.x, _eye.y, out.z);
}

XMFLOAT3 Camera::Rotate(float dx, float dy, float dz, XMFLOAT3 original)
{
	dx = XMConvertToRadians(dx);
	dy = XMConvertToRadians(dy);
	dz = XMConvertToRadians(dz);

	float angle = GetAngle(XMFLOAT3(_at.x, _at.y, _at.z), XMFLOAT3(0.0f, 0.0f, 0.0f));
	if (angle > 90.0f && angle < 270)
	{
		dy = -dy;
	}

	//create rotation matrix
	XMMATRIX r = XMMatrixRotationRollPitchYaw(dy, dz, dx);
	//create transform float3 with rotation matrix
	XMFLOAT3 out = original;
	XMStoreFloat3(&out, XMVector3TransformNormal(XMLoadFloat3(&out), r));
	return out;
}

void Camera::LookAt(XMFLOAT3 pos)
{
	_at = XMFLOAT3(pos.x - _eye.x, pos.y - _eye.y, pos.z - _eye.z);
}

XMFLOAT3 Camera::GetPos()
{
	return _eye;
}
XMFLOAT3 Camera::GetAt()
{
	return _at;
}
XMFLOAT3 Camera::GetUp()
{
	return _up;
}
XMFLOAT3 Camera::GetRight()
{
	return _right;
}

XMFLOAT4X4 Camera::GetView()
{
	return _view;
}
XMFLOAT4X4 Camera::GetProjection()
{
	return _projection;
}

float Camera::GetAngle(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	float n = 270 - atan2(pos2.z - pos1.z, pos2.x - pos1.x) * 180 / XM_PI;
	float angle = fmod(n, 360);

	return angle;
}

void Camera::Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (float)windowHeight, 0.01f, 100.0f));
}

void Camera::Update()
{
	XMVECTOR eyeVec = XMLoadFloat3(&_eye);
	XMVECTOR atVec = XMLoadFloat3(&_at);
	XMVECTOR upVec = XMLoadFloat3(&_up);
	XMStoreFloat4x4(&_view, XMMatrixLookToLH(eyeVec, atVec, upVec));
}

void Camera::SetMoveSpeed(float newSpeed)
{
	moveSpeed = newSpeed;
}

float Camera::GetMoveSpeed()
{
	return moveSpeed;
}