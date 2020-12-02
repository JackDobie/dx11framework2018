#include "Camera.h"

Camera::Camera(XMFLOAT4 position, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
	SetPos(position);
	SetAt(at);
	SetUp(up);

	Reshape(windowWidth, windowHeight, nearDepth, farDepth);

	// Initialize the projection matrix
	//XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (float)windowHeight, 0.01f, 100.0f));

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

void Camera::AddPos(XMFLOAT4 addEye)
{
	_eye = XMFLOAT4(_eye.x + addEye.x, _eye.y + addEye.y, _eye.z + addEye.z, _eye.w + addEye.w);
	//_eye = XMVectorAdd(_eye, addEye);
}
void Camera::AddAt(XMFLOAT4 addAt)
{
	//XMFLOAT4 newAt;
	//XMVECTOR newAtVec = XMVectorSet(_at.x, _at.y, _at.z, _at.w);
	//float angle = GetAngle(_eye, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	/*newAt.x = _at.x + cos(angle);
	newAt.y = _at.y;
	newAt.z = _at.z + sin(angle);*/
	//newAt = XMFLOAT4((_at.x + (2* cos(angle))), (_at.y), (_at.z + (2 * sin(angle))), (_at.w));
	//XMVectorRotateLeft(newAtVec, (angle - GetAngle(_at, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f))));
	//XMStoreFloat4(&_at, newAtVec);

	//newAt = Rotate(angle);
	//_at = newAt;

	//XMStoreFloat4x4(_at, XMMatrixRotationZ());

	//_at = XMFLOAT4(newAt.x + addAt.x, newAt.y + addAt.y, newAt.z + addAt.z, newAt.w + addAt.w);

	XMFLOAT4 added = XMFLOAT4(_at.x + addAt.x, _at.y + addAt.y, _at.z + addAt.z, _at.w + addAt.w);
	float angle = GetAngle(added, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	XMVECTOR vec = Rotate(angle);
	XMStoreFloat4(&_at, vec);
}

XMVECTOR Camera::Rotate(float angle)
{
	XMMATRIX m = XMMatrixTranslation(_eye.x, _eye.y, _eye.z) * XMMatrixRotationZ(angle);

	XMVECTOR vec = XMVECTOR();
	vec = XMVector3Transform(XMVectorSet(_at.x, _at.y, _at.z, _at.w), m);
	return vec;
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

float Camera::GetAngle(XMFLOAT4 pos1, XMFLOAT4 pos2)
{
	float dot = _eye.x * _at.x + _eye.y * _at.y + _eye.z * _at.z;
	float mag1 = sqrt(_eye.x * _eye.x + _eye.y * _eye.y + _eye.z * _eye.z);
	float mag2 = sqrt(_at.x * _at.x + _at.y * _at.y + _at.z * _at.z);
	float angle = acos(dot / (mag1 * mag2));
	return angle;
}

XMFLOAT4X4 Camera::GetView()
{
	return _view;
}
XMFLOAT4X4 Camera::GetProjection()
{
	return _projection;
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
	//XMMATRIX view = XMMatrixLookAtLH(_eye, _at, _up);
	XMStoreFloat4x4(&_view, XMMatrixLookToLH(eyeVec, atVec, upVec));
}