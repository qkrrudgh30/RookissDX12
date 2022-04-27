// Device.cpp

#include "pch.h"
#include "Device.h"

void Device::Init()
{
    // D3D12 디버그층 활성화. 디버그모드 일때만 받을 수 있는 추가 정보들이 있음.
    // - VC++ 출력창에 상세한 디버깅 메시지 출력
    // - riid : 디바이스의 COM ID
    // - ppDevice : 생성된 장치가 매개변수에 설정
#ifdef _DEBUG
    // IID_PPV_ARGS: 인자의 Com 개체 ID와 이중포인터를 반환해줌.
    ::D3D12GetDebugInterface(IID_PPV_ARGS(&_debugController));
    _debugController->EnableDebugLayer();
#endif

    // DXGI(DirectX Graphics Infrastructure)
    // Direct3D와 함께 쓰이는 API. 모든게 다 device에게 있는건 아님. 아래 기능들은 dxgi에게 있음.
    // - 전체 화면 모드 전환
    // - 지원되는 디스플레이 모드 열거 등
    // CreateDXGIFactory
    // - riid : 디바이스의 COM ID
    // - ppDevice : 생성된 장치가 매개변수에 설정
    ::CreateDXGIFactory(IID_PPV_ARGS(&_dxgi));

    // CreateDevice
    // - 디스플레이 어댑터(그래픽 카드)를 나타내는 객체. 즉, 이제부터 gpu에게 뭘 요청할 때는 항상 _device를 통해서 요청하게 됨.
    // - pAdapter : nullptr 지정하면 시스템 기본 디스플레이 어댑터
    // - MinimumFeatureLevel : 응용 프로그램이 요구하는 최소 기능 수준 (구닥다리 걸러낸다)
    //                        우리는 Dx11은 최소한 지원하는 글카여야 한다고 지정.
    // - riid : 디바이스의 COM ID
    // - ppDevice : 생성된 장치가 매개변수에 설정
    ::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
}