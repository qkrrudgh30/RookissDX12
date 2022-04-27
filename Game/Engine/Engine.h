// Engine.h

#pragma once

class Engine
{
public:

    void Init(const WindowInfo& info);
    void Render();

public:
    void RenderBegin(); // CommandQueue에 일감들을 넣어주는 단계
    void RenderEnd();   // 쌓인 일감들이 GPU에게 맡겨져서 실질적으로 그려지는 단계.

    void ResizeWindow(int32 width, int32 height);

private:
    // 그려질 화면 크기 관련된 삼총사.
    WindowInfo		_window;
    D3D12_VIEWPORT	_viewport = {};    // GPU 쪽으로 알려줄 정보
    D3D12_RECT		_scissorRect = {}; // GPU 쪽으로 알려줄 정보

    shared_ptr<class Device>         _device;
    shared_ptr<class CommandQueue>   _cmdQueue;
    shared_ptr<class SwapChain>      _swapChain;
    shared_ptr<class DescriptorHeap> _descHeap;
};
