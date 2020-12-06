#include "Camera.h"

Camera::Camera(XMFLOAT4 position, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
	SetPos(position);
	SetAt(at);
	SetUp(up);
	_right = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	Reshape(windowWidth, windowHeight, nearDepth, farDepth);

	Update();
}

void Camera::SetPos(XMFLOAT4 newEye)
{
	_eye = newEye;
}
void Camera::SetUp(XMFLOAT4 newUp)
{
	_up = newUp;
}
void Camera::SetAt(XMFLOAT4 newAt)
{
	_at = newAt;
}

void Camera::AddAt(XMFLOAT4 addAt)
{
	//add passed in float4 to current camera target
	XMFLOAT4 added = XMFLOAT4(_at.x + addAt.x, _at.y + addAt.y, _at.z + addAt.z, _at.w + addAt.w);
	//get difference in xyz
	float dx = added.x - _at.x;
	float dy = added.y - _at.y;
	float dz = added.z - _at.z;
	//apply rotation to the passed in float4 and current camera target
	XMFLOAT3 rot = XMFLOAT3(_at.x, _at.y, _at.z);
	rot = Rotate(dx, dy, dz, rot);
	_at = XMFLOAT4(rot.x, rot.y, rot.z, _at.w);

	rot = XMFLOAT3(_right.x, _right.y, _right.z);
	rot = Rotate(dx, dy, dz, rot);
	_right = XMFLOAT4(rot.x, rot.y, rot.z, _right.w);
}
void Camera::Move(float speed)
{
	XMVECTOR amount = XMVectorReplicate(speed);
	XMVECTOR target = XMLoadFloat4(&_at);
	XMVECTOR pos = XMLoadFloat4(&_eye);
	//multiply forward vector by speed, and add to position
	XMFLOAT4 out;
	XMStoreFloat4(&out, XMVectorMultiplyAdd(amount, target, pos));
	_eye = XMFLOAT4(out.x, _eye.y, out.z, out.w);
}
void Camera::Strafe(float speed)
{
	XMVECTOR amount = XMVectorReplicate(speed);
	XMVECTOR right = XMLoadFloat4(&_right);
	XMVECTOR pos = XMLoadFloat4(&_eye);
	//multiply right facing vector by speed, and add to position
	XMFLOAT4 out;
	XMStoreFloat4(&out, XMVectorMultiplyAdd(amount, right, pos));
	_eye = XMFLOAT4(out.x, _eye.y, out.z, out.w);
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
	//FXMVECTOR angle = XMVector3AngleBetweenVectors(XMLoadFloat3(&original), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));


	//create rotation matrix
	XMMATRIX r = XMMatrixRotationRollPitchYaw(dy, dz, dx);
	//create transform float3 with rotation matrix
	XMFLOAT3 out = original;
	XMStoreFloat3(&out, XMVector3TransformNormal(XMLoadFloat3(&out), r));
	return out;
}

void Camera::LookAt(XMFLOAT4 pos)
{
	_at = XMFLOAT4(pos.x - _eye.x, pos.y - _eye.y, pos.z - _eye.z, pos.w - _eye.w);
}

XMFLOAT4 Camera::GetPos()
{
	return _eye;
}
XMFLOAT4 Camera::GetAt()
{
	return _at;
}
XMFLOAT4 Camera::GetUp()
{
	return _up;
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
	float n = 270 - atan2(pos2.z - pos1.z, pos2.x - pos1.x) * 180 / M_PI;
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
	XMVECTOR eyeVec = XMVectorSet(_eye.x, _eye.y, _eye.z, _eye.w);
	XMVECTOR atVec = XMVectorSet(_at.x, _at.y, _at.z, _at.w);
	XMVECTOR upVec = XMVectorSet(_up.x, _up.y, _up.z, _up.w);
	XMStoreFloat4x4(&_view, XMMatrixLookToLH(eyeVec, atVec, upVec));
}