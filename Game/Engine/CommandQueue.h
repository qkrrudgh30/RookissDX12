// CommandQueue.h

#pragma once

// CommandQueue�� ��ġ "���� ���"�� ����.
// DX9 �������� device�� ���ؼ� �ϰ��� �׶��׶� gpu���� �����ָ� �Ǿ���.
// DX12���ʹ� �ϰ��� �����ִ� �� ����� �� ��������.

class SwapChain;
class DescriptorHeap;

class CommandQueue
{
public:
    ~CommandQueue();

    void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain, shared_ptr<DescriptorHeap> descHeap);
    void WaitSync();

    void RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect);
    void RenderEnd();

    ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }

private:
    // CommandQueue : DX12�� ����
    // ���ָ� ��û�� ��, �ϳ��� ��û�ϸ� ��ȿ����
    // [���� ���]�� �ϰ��� �������� ����ߴٰ� �� �濡 ��û�ϴ� ��
    ComPtr<ID3D12CommandQueue>        _cmdQueue;
    ComPtr<ID3D12CommandAllocator>    _cmdAlloc;
    ComPtr<ID3D12GraphicsCommandList> _cmdList;

    // Fence : ��Ÿ��(?)
    // ������ ��ϵ� ���� ��� �߿�, ������� �;��� ������ �� �ִ� ���ֵ� ��������.
    // �׷� ��, ��Ÿ���� ġ�� ���� ������� �� ������ �ش� ���ָ� �ñ��� �ʰԲ� �ϴ� ���.
    // CPU / GPU ����ȭ�� ���� ������ ����
    ComPtr<ID3D12Fence> _fence;
    uint32              _fenceValue = 0; // ������� �ö����� ��ٸ� ���� ��ȣ. �ش� ��ȣ�� ���ְ� ������ ������� �;� ��Ÿ���� �ŵξ���.
    HANDLE              _fenceEvent = INVALID_HANDLE_VALUE;

    // CommandQueue�� �ϰ����� ��Ƽ� gpu�� �����ϴ� �κ��̶�,
    // ���Ŀ� swapChain / descHeap�� ������ ��ɵ��� �ʿ��ϱ� ������ �־��.
    shared_ptr<SwapChain>       _swapChain; // ���� ȭ���� �׸��� ���ؼ� �ʿ���.
    shared_ptr<DescriptorHeap>  _descHeap;  // gpu���� back buffer, �� rtv �˷��� �� �ʿ���.
};
