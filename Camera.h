#pragma once

#include <directxmath.h>

using namespace DirectX;

class Camera
{
private:
	// private attributes to store camera position and view volume

	XMFLOAT3 _eye;
	XMFLOAT3 _at;
	XMFLOAT3 _up;
	XMFLOAT3 _right;

	float _windowWidth;
	float _windowHeight;
	float _nearDepth;
	float _farDepth;

	// attributes to hold the view and projection matrices to be passed to the shader
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

	float moveSpeed;

public:
	Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);

	void Update();

	// functions to set values
	void SetPos(XMFLOAT3 newEye);
	void SetAt(XMFLOAT3 newAt);
	void SetUp(XMFLOAT3 newUp);
	void SetMoveSpeed(float newSpeed);

	void AddAt(XMFLOAT3 addAt);

	void LookAt(XMFLOAT3 pos);
	XMFLOAT3 Rotate(float dx, float dy, float dz, XMFLOAT3 original);

	void Move(float deltaTime);
	void Strafe(float deltaTime);

	// functions to return values
	XMFLOAT3 GetPos();
	XMFLOAT3 GetAt();
	XMFLOAT3 GetRight();
	XMFLOAT3 GetUp();
	float GetAngle(XMFLOAT3 pos1, XMFLOAT3 pos2);
	float GetMoveSpeed();

	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProjection();

	// function to reshape the camera volume if the window is resized
	void Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth);
	void Reshape(float windowWidth, float windowHeight);
};