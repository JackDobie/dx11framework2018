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

    MeshData StarMesh = LoadMesh("Models/star.obj");

    objects.push_back(new GameObject(StarMesh, XMFLOAT3(-2.5f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), StarMesh.FurthestPoint));
    objects.push_back(new GameObject(StarMesh, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), StarMesh.FurthestPoint));
    objects.push_back(new GameObject(StarMesh, XMFLOAT3(2.5f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), StarMesh.FurthestPoint));
    objects.push_back(new GameObject(LoadMesh("Models/RoomNoRoof.obj"), XMFLOAT3(0.0f, 4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 0.0f));

    // Initialize the view matrix
    XMFLOAT3 Eye = XMFLOAT3(0.0f, 1.0f, -4.0f);
    XMFLOAT3 At = XMFLOAT3(0.0f, 0.0f, 1.0f);
    XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
    cam = new Camera(Eye, At, Up, _WindowWidth, _WindowHeight, 0.0f, 100.0f);

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
    rasterState = 1;

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
    // Update time
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

    Inputs();

    for each (GameObject* obj in objects)
    {
        obj->Update();
    }
    cam->Update();
}

void Application::Inputs()
{
    // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    //0x8000 is when key is held
    //`& 0x8000 != 0` gets when the key is just pressed down, not held
    if (GetAsyncKeyState(VK_SPACE) & 0x8000 != 0)//if space is pressed down
    {
        rasterState = rasterState == 0 ? 1 : 0;
    }

    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 != 0)//if left mouse is pressed down
    {
        MousePick();
    }

    if (GetAsyncKeyState(0x57) & 0x8000)//if W is pressed down
    {
        //move cam forwards
        cam->Move(0.0005f);
    }
    else if (GetAsyncKeyState(0x53) & 0x8000)//if S is pressed down
    {
        //move cam backwards
        cam->Move(-0.0005f);
    }
    if (GetAsyncKeyState(0x41) & 0x8000)//if A is pressed down
    {
        //move cam left
        cam->Strafe(-0.001f);
    }
    else if (GetAsyncKeyState(0x44) & 0x8000)//if D is pressed down
    {
        //move cam right
        cam->Strafe(0.001f);
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000)//if up arrow is pressed
    {
        //point cam up
        cam->AddAt(XMFLOAT3(0.0f, -0.025f, 0.0f));
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)//if down arrow is pressed
    {
        //point cam down
        cam->AddAt(XMFLOAT3(0.0f, 0.025f, 0.0f));
    }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000)//if left arrow is pressed
    {
        //point cam up
        cam->AddAt(XMFLOAT3(0.0f, 0.0f, -0.025f));
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)//if right arrow is pressed
    {
        //point cam down
        cam->AddAt(XMFLOAT3(0.0f, 0.0f, 0.025f));
    }

    if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000 != 0)//if numpad1 is pressed down
    {
        XMFLOAT3 obj1Pos = objects[0]->GetPosition();
        objects[0]->SetPosition(XMFLOAT3(obj1Pos.x, obj1Pos.y, obj1Pos.z + 5.0f));
    }
    else if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000 != 0)//if numpad4 is pressed down
    {
        XMFLOAT3 obj1Pos = objects[0]->GetPosition();
        objects[0]->SetPosition(XMFLOAT3(obj1Pos.x, obj1Pos.y, obj1Pos.z - 5.0f));
    }
    if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000 != 0)//if numpad2 is pressed down
    {
        XMFLOAT3 obj2Pos = objects[1]->GetPosition();
        objects[1]->SetPosition(XMFLOAT3(obj2Pos.x, obj2Pos.y, obj2Pos.z + 5.0f));
    }
    else if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000 != 0)//if numpad5 is pressed down
    {
        XMFLOAT3 obj2Pos = objects[1]->GetPosition();
        objects[1]->SetPosition(XMFLOAT3(obj2Pos.x, obj2Pos.y, obj2Pos.z - 5.0f));
    }

    if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000 != 0)//if numpad3 is pressed down
    {
        XMFLOAT3 obj3Pos = objects[2]->GetPosition();
        objects[2]->SetPosition(XMFLOAT3(obj3Pos.x, obj3Pos.y, obj3Pos.z + 5.0f));
    }
    else if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000 != 0)//if numpad6 is pressed down
    {
        XMFLOAT3 obj3Pos = objects[2]->GetPosition();
        objects[2]->SetPosition(XMFLOAT3(obj3Pos.x, obj3Pos.y, obj3Pos.z - 5.0f));
    }

    if (GetAsyncKeyState(0x52) & 0x8000 != 0)//if r is pressed down
    {
        //look at object 2
        cam->LookAt(objects[1]->GetPosition());
    }
}

void Application::Draw()
{
    _pImmediateContext->RSSetState(rasterState == 0 ? _wireFrame : _solid);

    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX view = XMLoadFloat4x4(&cam->GetView());
	XMMATRIX projection = XMLoadFloat4x4(&cam->GetProjection());

    // Update variables
    lightDirection = XMFLOAT3(0.2f, 0.5f, -1.0f);
    diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
    diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
    ambientLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    specularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    specularPower = 5.0f;
    //EyePosW = cam->GetPos();
    EyePosW = XMFLOAT4(0.0f, 4.0f, -1.0f, 0.0f);

    ConstantBuffer cb;
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


    _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
    _pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);


    for each (GameObject* obj in objects)
    {
        XMMATRIX world = XMLoadFloat4x4(&obj->GetTransform());
        cb.mWorld = XMMatrixTranspose(world);
        _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

        _pImmediateContext->IASetVertexBuffers(0, 1, &obj->GetMesh()->VertexBuffer, &obj->GetMesh()->VBStride, &obj->GetMesh()->VBOffset);
        _pImmediateContext->IASetIndexBuffer(obj->GetMesh()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        _pImmediateContext->DrawIndexed(obj->GetMesh()->IndexCount, 0, 0);
    }

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

void Application::MousePick()
{
    POINT mousePos;
    GetCursorPos(&mousePos);

    XMMATRIX invView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&cam->GetView()));
    XMMATRIX invProj = XMMatrixInverse(nullptr, XMLoadFloat4x4(&cam->GetProjection()));

    //convert pixel selected to a normalised screen coordinate

    float fNormalisedScreenCoordinates[2];
    fNormalisedScreenCoordinates[0] = (2.0f * mousePos.x) / _WindowWidth - 1.0f;
    fNormalisedScreenCoordinates[1] = 1.0f - (2.0f * mousePos.y) / _WindowHeight;

    //extract the camera position from the view matrix

    XMVECTOR eyePos;
    XMVECTOR dummy;
    XMMatrixDecompose(&dummy, &dummy, &eyePos, invView);

    //convert normalised screen coordinated from projection to view space

    XMVECTOR rayOrigin = XMVectorSet(fNormalisedScreenCoordinates[0], fNormalisedScreenCoordinates[1], 0, 0);
    rayOrigin = XMVector3Transform(rayOrigin, invProj);
    //at this point the origin will be pointing down the +Z axis (left handed) or -Z axis (right handed)

    //convert result from view to world space

    rayOrigin = XMVector3Transform(rayOrigin, invView);

    //subtract the eye position from this ray to producte a ray direction

    XMVECTOR rayDir = rayOrigin - eyePos;

    //normalise the ray, or ray intersections won't work properly

    rayDir = XMVector3Normalize(rayDir);

    //check if ray intersects with bounding sphere on objects

    for each (GameObject* obj in objects)
    {
        if (obj->CheckCollision(rayOrigin, rayDir))
        {
            //hit
        }
    }
}