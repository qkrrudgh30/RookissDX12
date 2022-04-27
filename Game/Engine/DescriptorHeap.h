// DescriptorHeap.h

#pragma once

// DescriptorHeap�� ��ȼ��� ����.
// ���ָ� �ñ� �� �̷� ���� �������� ���� �Ѱ���� �ϴµ�,
// �ƹ� ���·γ� ��û�ϸ� �� �˾ƸԴ´�.
// ~~�� ~������ �׷��ּ���. ��� ��û�ϴ� ǥ�� ���. 
//   ex. Ư�� ���� _renderTargets[0]�� ��ȼ� _rtvHandle[0]������ �׷��ּ���.
// ���� ���ҽ��� � �뵵�� ����ϴ��� �Ĳ��ϰ� ��� �Ѱ��ִ� �뵵
// Dx11������ View��� �̸����� �Ҹ�.
// ����Ǿ���, GPU���� ������ ��û�� �� ���ҽ��� ���� �����°� �ƴϰ�
// �ش� ���ҽ��� �� �� �� Wrapping�ذ����� �ش� ���ҽ��� � �뵵�� ����Ұ��� �����ؼ� ����.
class DescriptorHeap
{
public:
    void Init(ComPtr<ID3D12Device> device, shared_ptr<class SwapChain> swapChain);

    D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(int32 idx) { return _rtvHandle[idx]; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView();

private:
    // rtv == Render Target View
    // DescHeap [ [View] [View] ]
    //               ��
    //            [resource]
    // DH �ȿ��� ���� ���� View�� �ְ�, �� View�� � ���ҽ��� �����ϴµ� ���ǰ� ����.
    ComPtr<ID3D12DescriptorHeap>    _rtvHeap;
    uint32                          _rtvHeapSize = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE     _rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

    shared_ptr<class SwapChain>     _swapChain; // ���߿� swapChain�� Ȱ���ϰڴٴ� �Ͻ�.

    // ������ ����� rtv �ۿ� ��� rtv����.
    // ���Ŀ� Stencil Buffer, ... �� �߰���.
};
