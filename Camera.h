#pragma once

#include <directxmath.h>

using namespace DirectX;

class Camera
{
private:
	// private attributes to store camera position and view volume

	XMVECTOR _eye;
	XMVECTOR _at;
	XMVECTOR _up;

	float _windowWidth;
	float _windowHeight;
	float _nearDepth;
	float _farDepth;

	// attributes to hold the view and projection matrices to be passed to the shader
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

public:
	Camera(XMVECTOR position, XMVECTOR at, XMVECTOR up, float windowWidth, float windowHeight, float nearDepth, float farDepth);
	~Camera();

	void Update();

	void SetPos(XMVECTOR newEye);
	void SetAt(XMVECTOR newAt);
	void SetUp(XMVECTOR newUp);

	XMVECTOR GetPos();
	XMVECTOR SetAt();
	XMVECTOR SetUp();

	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProjection();

	// function to reshape the camera volume if the window is resized
	void Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth);
};