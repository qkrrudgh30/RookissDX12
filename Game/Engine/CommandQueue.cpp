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

    // 결국 device가 인력 사무소기 때문에 device를 통해서 뭔갈 만들어줌(인력 사무소의 인력을 고용하는 느낌).
    // 즉, new CommandQueue()하는게 아니라, COM 개체의 특정 함수를 통해서 만듦.
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));
    // - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록

    // GPU가 하나인 시스템에서는 0으로
    // DIRECT or BUNDLE
    // Allocator
    // 초기 상태 (그리기 명령은 nullptr 지정)
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    /*
        Command List도 Vector와 유사하게 동작함.
        Vector.clear() 하면, Size는 줄어들지만 Capacity는 안줄어듦.
        마찬가지로 Command List의 cmdList도 clear될 때가 있는데,
        그때 Allocator가 Vector처럼 clear 될때의 규칙같은 것들을 관리함.
    */

    // CommandList는 Close / Open 상태가 있는데
    // Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
    _cmdList->Close();

    // CreateFence
    // - CPU와 GPU의 동기화 수단으로 쓰인다
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    // Dx에서만 쓰이는 개념이라기 보단, 시스템 프로그래밍에서 자주 등장하는 Event 개념과 같음.
    // 이벤트 == 신호등. 초록불 되면 걷너게끔 초록불로 만들어줌.
    // 근데 이벤트를 만들면, 소멸도 시켜줘야해서 ~CommandList()에서 소멸시켜줌.
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
    // 책의 코드를 그대로 긁어옴.
    // Advance the fence value to mark commands up to this fence point.
    _fenceValue++;
    // 일감들을 모두 요청한 뒤에, 일감들이 담긴 외주 목록에 번호를 매길거임.

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    _cmdQueue->Signal(_fence.Get(), _fenceValue);
    // 외주 목록 번호를 CommandQueue에 알려줌.

    // Wait until the GPU has completed commands up to this fence point.
    if (_fence->GetCompletedValue() < _fenceValue)
    {
        // Fire event when GPU hits current fence.  
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        // 해당 외주 목록 번호가 끝났다면, _fenceEvent를 실행시켜.

        // Wait until the GPU hits current fence event is fired.
        ::WaitForSingleObject(_fenceEvent, INFINITE);
        // _fenceEvent가 초록불로 바뀔때까지 CPU는 기다림.
        // 즉, GPU가 해당 외주 목록 번호를 끝낼 때 까지 기다림.
        // 근데 이런 방식은 효율적이지 못함. GPU가 일 다할때까지 CPU가 논다? 비효율적.
        // 초기 코드라서 그럼. 나중에는 CPU가 몬스터 ai 같은거라도 계산함.
    }
}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);
    // Vector.clear()와 유사함.

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _swapChain->GetCurrentBackBufferResource().Get(),
        D3D12_RESOURCE_STATE_PRESENT,        // 기존 화면 출력 버퍼를
        D3D12_RESOURCE_STATE_RENDER_TARGET); // 백버퍼로 transition하겠다.
    // 현재 백버퍼 리소스가 이전에는 화면 출력 버퍼였는데, 백버퍼로 transition해라.

    // 근데 이제 곧바로 실행되는게 아님. barrier가 되어서 CommandList에 추가됨.
    _cmdList->ResourceBarrier(1, &barrier);

    // Set the viewport and scissor rect. This needs to be reset whenever the command list is reset.
    // command list를 리셋햇으면 언제든 간에 뷰포트와 시저렉트를 set해야 함.
    _cmdList->RSSetViewports(1, vp);
    _cmdList->RSSetScissorRects(1, rect);

    // Specify the buffers we are going to render to.
    // 어떤 버퍼에 그림을 그려야하는지 다시 한 번 언급해야 함. 그걸 해주는 코드들.
    D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _descHeap->GetBackBufferView(); // 백버퍼의 뷰를 꺼내온다음에
    _cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr); // clear하고
                                                   // 위 색상을 가지고 잘 빌드 되었는지 확인 가능.
    _cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);                    // 해당 백버퍼 뷰에 그려달라고 요청.
}

void CommandQueue::RenderEnd()
{
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _swapChain->GetCurrentBackBufferResource().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
        D3D12_RESOURCE_STATE_PRESENT); // 화면 출력
    // 현재 백버퍼 리소스는 이전에 백버퍼였는데, 이제는 화면 출력 버퍼로 transition해줘라.

    _cmdList->ResourceBarrier(1, &barrier);

    // 전달할 일감 다 넣었다는 표시.
    _cmdList->Close();

    // 여기서 커맨드 리스트 수행
    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // 화면 채우기
    _swapChain->Present();

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    // 위에서 요청한 일감들이 다 처리 될 때까지 CPU가 잠시 대기
    WaitSync();

    // swapchain의 기존 백버퍼 인덱스가 바뀜.
    _swapChain->SwapIndex();
}