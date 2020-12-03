#pragma once

#include <directxmath.h>
#include <tgmath.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace DirectX;

class Camera
{
private:
	// private attributes to store camera position and view volume

	XMFLOAT4 _eye;
	XMFLOAT4 _at;
	XMFLOAT4 _up;
	XMFLOAT4 _right;

	float _windowWidth;
	float _windowHeight;
	float _nearDepth;
	float _farDepth;

	// attributes to hold the view and projection matrices to be passed to the shader
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

public:
	Camera(XMFLOAT4 position, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);

	void Update();

	// functions to set values
	void SetPos(XMFLOAT4 newEye);
	void SetAt(XMFLOAT4 newAt);
	void SetUp(XMFLOAT4 newUp);

	void AddPos(XMFLOAT4 addEye);
	void AddAt(XMFLOAT4 addAt);

	void LookAt(XMFLOAT4 pos);
	XMFLOAT3 Rotate(float dx, float dy, float dz, XMFLOAT3 original);

	void Move(float speed);
	void Strafe(float speed);

	// functions to return values
	XMFLOAT4 GetPos();
	XMFLOAT4 GetAt();
	XMFLOAT4 GetUp();
	float GetAngle(XMFLOAT4 pos1, XMFLOAT4 pos2);

	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProjection();

	// function to reshape the camera volume if the window is resized
	void Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth);
};