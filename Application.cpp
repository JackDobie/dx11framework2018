#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

    objMeshData = OBJLoader::Load("Models/star.obj", _pd3dDevice);

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -1.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    cam1 = new Camera(Eye, At, Up, _WindowWidth, _WindowHeight, 0.0f, 1.0f);

    XMVECTOR Eye2 = XMVectorSet(2.0f, 4.0f, -1.0f, 0.0f);
    XMVECTOR At2 = XMVectorSet(0.0f, 0.5f, 0.0f, 0.0f);
    XMVECTOR Up2 = XMVectorSet(0.0f, 0.5f, 0.0f, 0.0f);
    cam2 = new Camera(Eye2, At2, Up2, _WindowWidth, _WindowHeight, 0.0f, 1.0f);

    XMVECTOR Eye3 = XMVectorSet(0.0f, 4.0f, 2.0f, 0.0f);
    XMVECTOR At3 = XMVectorSet(2.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up3 = XMVectorSet(0.0f, 1.0f, 1.0f, 0.0f);
    cam3 = new Camera(Eye3, At3, Up3, _WindowWidth, _WindowHeight, 0.0f, 1.0f);

    currentCam = cam1;

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    ID3D11ShaderResourceView* _pTextureRV = nullptr;
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"Images/Crate_COLOR.dds", nullptr, &_pTextureRV);

    if (FAILED(hr))
        return hr;

    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV); //create texture from file

    ID3D11SamplerState* _pSamplerLinear = nullptr;

    // create the sampler state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

    _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);



    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

    // Set index buffer
    _pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    if (FAILED(hr))
        return hr;

    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width              = _WindowWidth;
    depthStencilDesc.Height             = _WindowHeight;
    depthStencilDesc.MipLevels          = 1;
    depthStencilDesc.ArraySize          = 1;
    depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags     = 0;
    depthStencilDesc.MiscFlags          = 0;

    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    if (FAILED(hr))
        return hr;

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_SOLID;
    wfdesc.CullMode = D3D11_CULL_BACK;
    hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_solid);
    rasterState = 0;

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
    if (_depthStencilView) _depthStencilView->Release();
    if (_depthStencilBuffer) _depthStencilBuffer->Release();
    if (_wireFrame) _wireFrame->Release();
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

    if (GetAsyncKeyState(VK_SPACE) & 0x8000 != 0)//if space is pressed DOWN
    {
        rasterState = rasterState == 0 ? 1 : 0;
    }

    if (GetAsyncKeyState(0x31) & 0x8000 != 0)//if 1 key is pressed DOWN
    {
        currentCam = cam1;
    }
    else if (GetAsyncKeyState(0x32) & 0x8000 != 0)//if 2 key is pressed DOWN
    {
        currentCam = cam2;
    }
    else if (GetAsyncKeyState(0x33) & 0x8000 != 0)//if 3 key is pressed DOWN
    {
        currentCam = cam3;
    }

    //
    // Animate the cube
    //
    //result = scale * selfRotation * translation * orbitRotation
    XMStoreFloat4x4(&_world, XMMatrixRotationZ(t));//centre cube

    XMStoreFloat4x4(&_world2, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationZ(t) * XMMatrixTranslation(3.0f, 0.0f, 0.0f) * XMMatrixRotationZ(t * 1.8f));//spinning around centre

    XMStoreFloat4x4(&_world3, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationZ(-t * 1.4f) * XMMatrixTranslation(6.0f, .0f, 0.0f) * XMMatrixRotationZ(t * 0.6f));//spinning around centre

    XMStoreFloat4x4(&_world4, XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixRotationZ(t) * XMMatrixTranslation(3.0f, 0.0f, 0.0f) * XMMatrixRotationZ(t * 1.8f) * XMMatrixRotationZ(t) * XMMatrixTranslation(3.0f, 0.0f, 0.0f) * XMMatrixRotationZ(t * 1.8f));//spinning around world2

    XMStoreFloat4x4(&_world5, XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixRotationZ(-t * 1.4f) * XMMatrixTranslation(6.0f, .0f, 0.0f) * XMMatrixRotationZ(t) * XMMatrixRotationZ(-t * 1.4f) * XMMatrixTranslation(6.0f, .0f, 0.0f) * XMMatrixRotationZ(t));//spinning around world3

    //XMStoreFloat4x4(&_world6, XMMatrixRotationZ(t));
}

void Application::Draw()
{
    _pImmediateContext->RSSetState(rasterState == 0 ? _wireFrame : _solid);

    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(&currentCam->GetView());
	XMMATRIX projection = XMLoadFloat4x4(&currentCam->GetProjection());

    // Update variables
    lightDirection = XMFLOAT3(0.2f, 0.5f, -1.0f);
    diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
    diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
    ambientLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    specularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    specularPower = 10.0f;
    EyePosW = XMFLOAT4(0.0f, 4.0f, -1.0f, 0.0f);

    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
    cb.DiffuseMtrl = diffuseMaterial;
    cb.DiffuseLight = diffuseLight;
    cb.LightVecW = lightDirection;
    cb.AmbientLight = ambientLight;
    cb.AmbientMtrl = ambientMaterial;
    cb.SpeculatMtrl = specularMaterial;
    cb.SpecularLight = specularLight;
    cb.SpecularPower = specularPower;
    cb.EyePosW = EyePosW;


	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    // Draws first cube
    objMeshData = LoadMesh("Models/star.obj");
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;

    _pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &objMeshData.VBStride, &objMeshData.VBOffset);
    _pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    _pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

    // Draws second cube
    objMeshData = LoadMesh("Models/cube.obj");
    world = XMLoadFloat4x4(&_world2);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    _pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &objMeshData.VBStride, &objMeshData.VBOffset);
    _pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    _pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

    //cube 3
    objMeshData = LoadMesh("Models/cube.obj");
    world = XMLoadFloat4x4(&_world3);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    _pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &objMeshData.VBStride, &objMeshData.VBOffset);
    _pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    _pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

    //cube 4
    objMeshData = LoadMesh("Models/star.obj");
    world = XMLoadFloat4x4(&_world4);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    _pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &objMeshData.VBStride, &objMeshData.VBOffset);
    _pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    _pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

    //cube 5
    objMeshData = LoadMesh("Models/star.obj");
    world = XMLoadFloat4x4(&_world5);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    _pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &objMeshData.VBStride, &objMeshData.VBOffset);
    _pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    _pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

    // Present our back buffer to our front buffer
    _pSwapChain->Present(0, 0);
}

MeshData Application::LoadMesh(string path)
{
    return OBJLoader::Load((char*)path.c_str(), _pd3dDevice);
}

void Application::LoadTexture(string path)
{
    ID3D11ShaderResourceView* _pTextureRV = nullptr;
    CreateDDSTextureFromFile(_pd3dDevice, (wchar_t*)path.c_str(), nullptr, &_pTextureRV);

    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
}