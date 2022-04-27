// CommandQueue.h

#pragma once

// CommandQueue는 마치 "외주 목록"과 같음.
// DX9 시절에는 device를 통해서 일감을 그때그때 gpu에게 던져주면 되었음.
// DX12부터는 일감을 던져주는 그 방식이 좀 복잡해짐.

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
    // CommandQueue : DX12에 등장
    // 외주를 요청할 때, 하나씩 요청하면 비효율적
    // [외주 목록]에 일감을 차곡차곡 기록했다가 한 방에 요청하는 것
    ComPtr<ID3D12CommandQueue>        _cmdQueue;
    ComPtr<ID3D12CommandAllocator>    _cmdAlloc;
    ComPtr<ID3D12GraphicsCommandList> _cmdList;

    // Fence : 울타리(?)
    // 위에서 기록된 외주 목록 중에, 결과물이 와야지 진행할 수 있는 외주도 있을것임.
    // 그럴 때, 울타리를 치고 외주 결과물이 올 때까지 해당 외주를 맡기지 않게끔 하는 기능.
    // CPU / GPU 동기화를 위한 간단한 도구
    ComPtr<ID3D12Fence> _fence;
    uint32              _fenceValue = 0; // 결과물이 올때까지 기다릴 외주 번호. 해당 번호의 외주가 끝나서 결과물이 와야 울타리가 거두어짐.
    HANDLE              _fenceEvent = INVALID_HANDLE_VALUE;

    // CommandQueue는 일감들을 모아서 gpu에 전달하는 부분이라서,
    // 이후에 swapChain / descHeap에 구현된 기능들이 필요하기 때문에 넣어둠.
    shared_ptr<SwapChain>       _swapChain; // 현재 화면을 그리기 위해서 필요함.
    shared_ptr<DescriptorHeap>  _descHeap;  // gpu에게 back buffer, 즉 rtv 알려줄 때 필요함.
};
