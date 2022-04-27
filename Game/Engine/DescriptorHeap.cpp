// DescriptorHeap.cpp

#include "pch.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"

// .cpp ���Ͽ����� ���� ������ �ص� �ƹ��� �ǹ̰� ����. #include ���� �߰�. 
void DescriptorHeap::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
    _swapChain = swapChain;

    // Descriptor (DX12) = View (~DX11)
    // �������� ������ ���ҽ����� ������ View�� �־���. ex. RenderTargetView, DepthStencilView, ...
    // �׸��� �� ���ε��� ���� �߾���.
    // DX12���ʹ� DescriptorHeap�� �̿��ؼ� ��� �並 �����ϰ� ������.
    // [������ ��]���� RTV ����
    // DX11�� RTV(RenderTargetView), DSV(DepthStencilView), 
    // CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)�� ����!

    // �������� SwapChain���� ���ҽ�(_renderTargets)�� ���� ����.
    // �̰� �����ϴ� ��ȼ� �ϳ��� ������� ��.
    _rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;    // Ÿ���� rtv��.
    rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT; // ��ȼ��� 2�� �ʿ��ϴ�.
    rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvDesc.NodeMask = 0;

    // DescriptorHeap�� ������ �迭����. ��, ���� ������ �����ͳ��� �迭�� ����
    // RTV ��� : [ ] [ ]
    device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));
    // rtv 2�� ¥���� DescriptorHeap�� �������.

    // �� ĭ�� ä������.
    // rtvHeapBegin�� �� �ٷ� ���� ���� ���� ���� �ּҸ� ����Ŵ.
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

    for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
    {
        // CD3DX12�� d3dx12.h�� ���� Ŭ������ ���ǵ� �༮���� ����.
        _rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);
        /*
            Ctrl + Ŭ������ ���Ǹ� �i�ư���
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

            4. �ᱹ �Ʒ� �ڵ�� �Ͱ��
            _rtvHandle[i] = handle.ptr = base.ptr + offsetScaledByIncrementSize;

            5.
            _rtvHandle[i] = handle.ptr = rtvHeapBegin.ptr + (i * _rtvHeapSize);

        */

        device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, _rtvHandle[i]); // ���� ���ҽ��� SwapChain���� ����.(_renderTargets)
        // ��, _rtvHandle[i]�� RenderTarget���� ��ĭ�� ä���� ��� ��.
    }

    // ��������� �Ϸ�Ǹ�, GPU���� ��ȼ��� �Ѱ��� �غ� �� ����.
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetBackBufferView()
{
    return GetRTV(_swapChain->GetCurrentBackBufferIndex()); // ����� ���ҽ��� rtv�� ��ȯ��.
}