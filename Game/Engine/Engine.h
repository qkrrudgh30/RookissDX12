// Engine.h

#pragma once

class Engine
{
public:

    void Init(const WindowInfo& info);
    void Render();

public:
    void RenderBegin(); // CommandQueue�� �ϰ����� �־��ִ� �ܰ�
    void RenderEnd();   // ���� �ϰ����� GPU���� �ð����� ���������� �׷����� �ܰ�.

    void ResizeWindow(int32 width, int32 height);

private:
    // �׷��� ȭ�� ũ�� ���õ� ���ѻ�.
    WindowInfo		_window;
    D3D12_VIEWPORT	_viewport = {};    // GPU ������ �˷��� ����
    D3D12_RECT		_scissorRect = {}; // GPU ������ �˷��� ����

    shared_ptr<class Device>         _device;
    shared_ptr<class CommandQueue>   _cmdQueue;
    shared_ptr<class SwapChain>      _swapChain;
    shared_ptr<class DescriptorHeap> _descHeap;
};
