// CommandQueue.cpp

#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

CommandQueue::~CommandQueue()
{
    ::CloseHandle(_fenceEvent);
}

void CommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain, shared_ptr<DescriptorHeap> descHeap)
{
    _swapChain = swapChain;
    _descHeap = descHeap;

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    // �ᱹ device�� �η� �繫�ұ� ������ device�� ���ؼ� ���� �������(�η� �繫���� �η��� ����ϴ� ����).
    // ��, new CommandQueue()�ϴ°� �ƴ϶�, COM ��ü�� Ư�� �Լ��� ���ؼ� ����.
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));
    // - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU�� ���� �����ϴ� ��� ���

    // GPU�� �ϳ��� �ý��ۿ����� 0����
    // DIRECT or BUNDLE
    // Allocator
    // �ʱ� ���� (�׸��� ����� nullptr ����)
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    /*
        Command List�� Vector�� �����ϰ� ������.
        Vector.clear() �ϸ�, Size�� �پ������ Capacity�� ���پ��.
        ���������� Command List�� cmdList�� clear�� ���� �ִµ�,
        �׶� Allocator�� Vectoró�� clear �ɶ��� ��Ģ���� �͵��� ������.
    */

    // CommandList�� Close / Open ���°� �ִµ�
    // Open ���¿��� Command�� �ִٰ� Close�� ���� �����ϴ� ����
    _cmdList->Close();

    // CreateFence
    // - CPU�� GPU�� ����ȭ �������� ���δ�
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    // Dx������ ���̴� �����̶�� ����, �ý��� ���α׷��ֿ��� ���� �����ϴ� Event ����� ����.
    // �̺�Ʈ == ��ȣ��. �ʷϺ� �Ǹ� �ȳʰԲ� �ʷϺҷ� �������.
    // �ٵ� �̺�Ʈ�� �����, �Ҹ굵 ��������ؼ� ~CommandList()���� �Ҹ������.
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
    // å�� �ڵ带 �״�� �ܾ��.
    // Advance the fence value to mark commands up to this fence point.
    _fenceValue++;
    // �ϰ����� ��� ��û�� �ڿ�, �ϰ����� ��� ���� ��Ͽ� ��ȣ�� �ű����.

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    _cmdQueue->Signal(_fence.Get(), _fenceValue);
    // ���� ��� ��ȣ�� CommandQueue�� �˷���.

    // Wait until the GPU has completed commands up to this fence point.
    if (_fence->GetCompletedValue() < _fenceValue)
    {
        // Fire event when GPU hits current fence.  
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        // �ش� ���� ��� ��ȣ�� �����ٸ�, _fenceEvent�� �������.

        // Wait until the GPU hits current fence event is fired.
        ::WaitForSingleObject(_fenceEvent, INFINITE);
        // _fenceEvent�� �ʷϺҷ� �ٲ𶧱��� CPU�� ��ٸ�.
        // ��, GPU�� �ش� ���� ��� ��ȣ�� ���� �� ���� ��ٸ�.
        // �ٵ� �̷� ����� ȿ�������� ����. GPU�� �� ���Ҷ����� CPU�� ���? ��ȿ����.
        // �ʱ� �ڵ�� �׷�. ���߿��� CPU�� ���� ai �����Ŷ� �����.
    }
}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);
    // Vector.clear()�� ������.

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _swapChain->GetCurrentBackBufferResource().Get(),
        D3D12_RESOURCE_STATE_PRESENT,        // ���� ȭ�� ��� ���۸�
        D3D12_RESOURCE_STATE_RENDER_TARGET); // ����۷� transition�ϰڴ�.
    // ���� ����� ���ҽ��� �������� ȭ�� ��� ���ۿ��µ�, ����۷� transition�ض�.

    // �ٵ� ���� ��ٷ� ����Ǵ°� �ƴ�. barrier�� �Ǿ CommandList�� �߰���.
    _cmdList->ResourceBarrier(1, &barrier);

    // Set the viewport and scissor rect. This needs to be reset whenever the command list is reset.
    // command list�� ���������� ������ ���� ����Ʈ�� ������Ʈ�� set�ؾ� ��.
    _cmdList->RSSetViewports(1, vp);
    _cmdList->RSSetScissorRects(1, rect);

    // Specify the buffers we are going to render to.
    // � ���ۿ� �׸��� �׷����ϴ��� �ٽ� �� �� ����ؾ� ��. �װ� ���ִ� �ڵ��.
    D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _descHeap->GetBackBufferView(); // ������� �並 �����´�����
    _cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr); // clear�ϰ�
                                                   // �� ������ ������ �� ���� �Ǿ����� Ȯ�� ����.
    _cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);                    // �ش� ����� �信 �׷��޶�� ��û.
}

void CommandQueue::RenderEnd()
{
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _swapChain->GetCurrentBackBufferResource().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, // ���� �����
        D3D12_RESOURCE_STATE_PRESENT); // ȭ�� ���
    // ���� ����� ���ҽ��� ������ ����ۿ��µ�, ������ ȭ�� ��� ���۷� transition�����.

    _cmdList->ResourceBarrier(1, &barrier);

    // ������ �ϰ� �� �־��ٴ� ǥ��.
    _cmdList->Close();

    // ���⼭ Ŀ�ǵ� ����Ʈ ����
    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // ȭ�� ä���
    _swapChain->Present();

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    // ������ ��û�� �ϰ����� �� ó�� �� ������ CPU�� ��� ���
    WaitSync();

    // swapchain�� ���� ����� �ε����� �ٲ�.
    _swapChain->SwapIndex();
}