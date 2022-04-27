// DescriptorHeap.cpp

#include "pch.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"

// .cpp 파일에서는 전방 선언을 해도 아무런 의미가 없음. #include 구문 추가. 
void DescriptorHeap::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
    _swapChain = swapChain;

    // Descriptor (DX12) = View (~DX11)
    // 예전에는 각각의 리소스마다 각각의 View가 있었음. ex. RenderTargetView, DepthStencilView, ...
    // 그리고 다 따로따로 관리 했었음.
    // DX12부터는 DescriptorHeap을 이용해서 모든 뷰를 묘사하고 관리함.
    // [서술자 힙]으로 RTV 생성
    // DX11의 RTV(RenderTargetView), DSV(DepthStencilView), 
    // CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)를 전부!

    // 기존에는 SwapChain에서 리소스(_renderTargets)를 관리 했음.
    // 이걸 묘사하는 기안서 하나를 만들고자 함.
    _rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;    // 타입은 rtv다.
    rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT; // 기안서는 2개 필요하다.
    rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvDesc.NodeMask = 0;

    // DescriptorHeap은 일종의 배열같음. 즉, 같은 종류의 데이터끼리 배열로 관리
    // RTV 목록 : [ ] [ ]
    device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));
    // rtv 2개 짜리의 DescriptorHeap이 만들어짐.

    // 두 칸을 채워보자.
    // rtvHeapBegin는 곧 바로 전에 만든 힙의 시작 주소를 가르킴.
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

    for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
    {
        // CD3DX12는 d3dx12.h의 핼퍼 클래스에 정의된 녀석들을 뜻함.
        _rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);
        /*
            Ctrl + 클릭으로 정의를 쫒아가면
            1.
            CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &other, INT offsetScaledByIncrementSize)
            {
                InitOffsetted(other, offsetScaledByIncrementSize);
            }

            2.
            inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
            {
                InitOffsetted(*this, base, offsetScaledByIncrementSize);
            }

            3.
            static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
            {
                handle.ptr = base.ptr + offsetScaledByIncrementSize;
            }

            4. 결국 아래 코드로 귀결됨
            _rtvHandle[i] = handle.ptr = base.ptr + offsetScaledByIncrementSize;

            5.
            _rtvHandle[i] = handle.ptr = rtvHeapBegin.ptr + (i * _rtvHeapSize);

        */

        device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, _rtvHandle[i]); // 원본 리소스는 SwapChain에게 있음.(_renderTargets)
        // 즉, _rtvHandle[i]를 RenderTarget으로 한칸씩 채워줘 라는 뜻.
    }

    // 여기까지가 완료되면, GPU에게 기안서를 넘겨줄 준비가 된 상태.
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetBackBufferView()
{
    return GetRTV(_swapChain->GetCurrentBackBufferIndex()); // 백버퍼 리소스의 rtv를 반환함.
}