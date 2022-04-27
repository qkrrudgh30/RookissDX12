// DescriptorHeap.h

#pragma once

// DescriptorHeap은 기안서와 같음.
// 외주를 맡길 때 이런 저런 정보들을 같이 넘겨줘야 하는데,
// 아무 형태로나 요청하면 못 알아먹는다.
// ~~를 ~식으로 그려주세요. 라고 요청하는 표준 양식. 
//   ex. 특수 종이 _renderTargets[0]을 기안서 _rtvHandle[0]식으로 그려주세요.
// 각종 리소스를 어떤 용도로 사용하는지 꼼꼼하게 적어서 넘겨주는 용도
// Dx11까지는 View라는 이름으로 불림.
// 어찌되었건, GPU에게 뭔가를 요청할 때 리소스만 덜컥 보내는게 아니고
// 해당 리소스를 한 번 더 Wrapping해가지고 해당 리소스를 어떤 용도로 사용할건지 설명해서 보냄.
class DescriptorHeap
{
public:
    void Init(ComPtr<ID3D12Device> device, shared_ptr<class SwapChain> swapChain);

    D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(int32 idx) { return _rtvHandle[idx]; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView();

private:
    // rtv == Render Target View
    // DescHeap [ [View] [View] ]
    //               ↓
    //            [resource]
    // DH 안에는 여러 개의 View가 있고, 각 View는 어떤 리소스를 묘사하는데 사용되고 있음.
    ComPtr<ID3D12DescriptorHeap>    _rtvHeap;
    uint32                          _rtvHeapSize = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE     _rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

    shared_ptr<class SwapChain>     _swapChain; // 나중에 swapChain을 활용하겠다는 암시.

    // 지금은 만든게 rtv 밖에 없어서 rtv뿐임.
    // 추후에 Stencil Buffer, ... 등등도 추가됨.
};
