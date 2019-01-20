#include "D3D.h"
#include "Common.h"
namespace TaoD3D
{
bool D3D::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool fullScreen)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    InitRefreshRate();
    InitSwapChain(hwnd, fullScreen);
    InitViewPort();
    InitDepthStencilBuffer();
    InitRaster();
    InitRenderTarget();
    InitMatrix();

    return true;
}
void D3D::GetProjectMatrix(D3DXMATRIX &mat)
{
    mat = m_projectMat;
}
void D3D::GetWorldMatrix(D3DXMATRIX &mat)
{
    mat = m_worldMat;
}
void D3D::GetOrthoMatrix(D3DXMATRIX &mat)
{
    mat = m_orthoMat;
}
void D3D::Shutdown()
{
    m_swapChain->SetFullscreenState(false, nullptr);
    m_swapChain->Release();
    m_targetView->Release();
    m_device->Release();
    m_context->Release();
    m_depthStencilBuffer->Release();
    m_depthStencilState->Release();
    m_depthStencilView->Release();
    m_rasterState->Release();
}
void D3D::InitRefreshRate()
{
    IDXGIFactory *factory;
    IDXGIAdapter *adapter;
    IDXGIOutput *adapterOutput;
    unsigned int numModes;
    DXGI_MODE_DESC *modeList;
    DXGI_ADAPTER_DESC adapterDesc;
    ThrowIfFailed(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory), "CreateDXGIFactory");

    ThrowIfFailed(factory->EnumAdapters(0, &adapter), "EnumAdapters");
    ThrowIfFailed(adapter->EnumOutputs(0, &adapterOutput), "EnumOutputs");
    ThrowIfFailed(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr), "");
    modeList = new DXGI_MODE_DESC[numModes];
    ThrowIfFailed(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, modeList), "");
    for (int i = 0; i < numModes; ++i)
    {
        if (modeList[i].Width == m_screenWidth && modeList[i].Height == m_screenHeight)
        {
            m_numerator = modeList[i].RefreshRate.Numerator;
            m_denominator = modeList[i].RefreshRate.Denominator;
            break;
        }
    }
    ThrowIfFailed(adapter->GetDesc(&adapterDesc), "GetDesc");
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
    size_t strLen = 0;
    wcstombs_s(&strLen, m_videoCardDescription, 128, adapterDesc.Description, 128);
    delete[] modeList;
    modeList = nullptr;
    adapterOutput->Release();
    factory->Release();
}
void D3D::InitDepthStencilBuffer()
{
    // depth buffer
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

    ZeroMemory(&depthBufferDesc, sizeof depthBufferDesc);
    depthBufferDesc.Width = m_screenWidth;
    depthBufferDesc.Height = m_screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    ThrowIfFailed(m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer), "CreateTexture2D");

    ZeroMemory(&depthStencilDesc, sizeof depthStencilDesc);
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    ThrowIfFailed(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState), "CreateDepthStencilState");
    m_context->OMSetDepthStencilState(m_depthStencilState, 1);

    ZeroMemory(&depthStencilViewDesc, sizeof depthStencilViewDesc);
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    ThrowIfFailed(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView), "CreateDepthStencilView");
}
void D3D::InitSwapChain(HWND hwnd, bool fullScreen)
{
    // Direct3D init
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof swapChainDesc);
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = m_screenWidth;
    swapChainDesc.BufferDesc.Height = m_screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (m_vsyncEnabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = m_numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = m_denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 8;
    swapChainDesc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
    swapChainDesc.Windowed = !fullScreen;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    ThrowIfFailed(
        D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_context),
        "D3D11CreateDeviceAndSwapChain ");
}
void D3D::InitRenderTarget()
{
    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer;
    ThrowIfFailed(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer), "GetBuffer");
    // create render target
    ThrowIfFailed(m_device->CreateRenderTargetView(pBackBuffer, nullptr, &m_targetView), "CreateRenderTargetView");

    pBackBuffer->Release();
    // set render target
    m_context->OMSetRenderTargets(1, &m_targetView, m_depthStencilView);
}
void D3D::InitRaster()
{
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof rasterDesc);

    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    ThrowIfFailed(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState), "CreateRasterizerState");
    m_context->RSSetState(m_rasterState);
}
void D3D::InitViewPort()
{
    // set view port
    D3D11_VIEWPORT viewPort;
    ZeroMemory(&viewPort, sizeof viewPort);
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = m_screenWidth;
    viewPort.Height = m_screenHeight;
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &viewPort);
}
void D3D::InitMatrix()
{
    float fieldOfView = (float)D3DX_PI / 4.0f;
    float screenAspect = (float)m_screenWidth / m_screenHeight;
    float screenNear = 0.1f;
    float screenFar = 1000.0f;
    D3DXMatrixPerspectiveFovLH(&m_projectMat, fieldOfView, screenAspect, screenNear, screenNear);
    D3DXMatrixIdentity(&m_worldMat);
    D3DXMatrixOrthoLH(&m_orthoMat, (float)m_screenWidth, (float)m_screenHeight, screenNear, screenFar);
}

void D3D::BeginScene(float r, float g, float b, float a)
{
    float color[4];
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;

    m_context->ClearRenderTargetView(m_targetView, color);
    m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void D3D::EndScene()
{
    if (m_vsyncEnabled)
    {
        m_swapChain->Present(1, 0);
    }
    else
    {
        m_swapChain->Present(0, 0);
    }
}
} // namespace TaoD3D