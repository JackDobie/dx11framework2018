#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <string>
#include "resource.h"
#include "DDSTextureLoader.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "Camera.h"
#include "GameObject.h"

using namespace DirectX;
using namespace std;

struct ConstantBuffer
{
	XMMATRIX	mWorld;
	XMMATRIX	mView;
	XMMATRIX	mProjection;

	XMFLOAT4	DiffuseMtrl;
	XMFLOAT4	DiffuseLight;
	XMFLOAT3	LightVecW;

	XMFLOAT4	AmbientMtrl;
	XMFLOAT4	AmbientLight;

	XMFLOAT4	SpeculatMtrl;
	XMFLOAT4	SpecularLight;
	float		SpecularPower;
	XMFLOAT4	EyePosW;
};

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pConstantBuffer;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	ID3D11RasterizerState*	_wireFrame;
	ID3D11RasterizerState*	_solid;
	XMFLOAT3				lightDirection;
	XMFLOAT4				diffuseMaterial;
	XMFLOAT4				diffuseLight;
	XMFLOAT4				ambientMaterial;
	XMFLOAT4				ambientLight;
	XMFLOAT4				specularMaterial;
	XMFLOAT4				specularLight;
	float					specularPower;
	XMFLOAT4				EyePosW;
	Camera*					cam;
	vector<GameObject*>		objects;

	GameObject*				selectedObject = nullptr;

	float					deltaTime;
	float					oldTime;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	UINT _WindowHeight;
	UINT _WindowWidth;

	/// <summary> 0 = wireframe, 1 = solid </summary>
	int rasterState;

	MeshData LoadMesh(string path);

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
	void Inputs();
	void MousePick();
	void ResizeWindow(int height, int width);

	XMFLOAT2 _MousePos = XMFLOAT2(0, 0);
};

