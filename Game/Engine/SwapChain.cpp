// SwapChain.cpp

#include "pch.h"
#include "SwapChain.h"

void SwapChain::Init(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
    // 이전에 만든 정보 날린다. init() 두 번 했을 까봐.
    _swapChain.Reset();

    // 현재 게임 세상에 있는 상황을 묘사
    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = static_cast<uint32>(info.width); // 버퍼의 해상도 너비
    sd.BufferDesc.Height = static_cast<uint32>(info.height); // 버퍼의 해상도 높이
    sd.BufferDesc.RefreshRate.Numerator = 60; // 화면 갱신 비율
    sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 갱신 비율
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링할 것 
    sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 전면+후면 버퍼
    sd.OutputWindow = info.hwnd;
    sd.Windowed = info.windowed;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // device 컴개체가 아님. dxgi를 통해서 만듦. 출력과 관련된 기능들은 dxgi가 갖고 있음.
    dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);

    for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
        _swapChain->GetBuffer(i, IID_PPV_ARGS(&_renderTargets[i]));
    // GetBuffer()는 두 번째 인자(지금은 _renderTargets)에다가 만들어진 버퍼를 채워줌.
}

void SwapChain::Present()
{
    // Present the frame.
    _swapChain->Present(0, 0);
    // 현재 화면에 어떤 특수 종이가 그려질 지 지정이 되었다면,
    // 해당 종이를 그려달라는 명령어.
}

void SwapChain::SwapIndex()
{
    _backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}