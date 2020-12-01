#include "Camera.h"

Camera::Camera(XMVECTOR position, XMVECTOR at, XMVECTOR up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
	SetPos(position);
	SetAt(at);
	SetUp(up);

	Reshape(windowWidth, windowHeight, nearDepth, farDepth);

	Update();

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (float)windowHeight, 0.01f, 100.0f));
}

void Camera::SetPos(XMVECTOR newEye)
{
	_eye = newEye;
}
void Camera::SetUp(XMVECTOR newUp)
{
	_up = newUp;
}
void Camera::SetAt(XMVECTOR newAt)
{
	_at = newAt;
}

XMVECTOR Camera::GetPos()
{
	return _eye;
}
XMVECTOR Camera::SetUp()
{
	return _up;
}
XMVECTOR Camera::SetAt()
{
	return _at;
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
}

void Camera::Update()
{
	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(_eye, _at, _up));
}