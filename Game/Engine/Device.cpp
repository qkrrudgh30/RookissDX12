// Device.cpp

#include "pch.h"
#include "Device.h"

void Device::Init()
{
    // D3D12 ������� Ȱ��ȭ. ����׸�� �϶��� ���� �� �ִ� �߰� �������� ����.
    // - VC++ ���â�� ���� ����� �޽��� ���
    // - riid : ����̽��� COM ID
    // - ppDevice : ������ ��ġ�� �Ű������� ����
#ifdef _DEBUG
    // IID_PPV_ARGS: ������ Com ��ü ID�� ���������͸� ��ȯ����.
    ::D3D12GetDebugInterface(IID_PPV_ARGS(&_debugController));
    _debugController->EnableDebugLayer();
#endif

    // DXGI(DirectX Graphics Infrastructure)
    // Direct3D�� �Բ� ���̴� API. ���� �� device���� �ִ°� �ƴ�. �Ʒ� ��ɵ��� dxgi���� ����.
    // - ��ü ȭ�� ��� ��ȯ
    // - �����Ǵ� ���÷��� ��� ���� ��
    // CreateDXGIFactory
    // - riid : ����̽��� COM ID
    // - ppDevice : ������ ��ġ�� �Ű������� ����
    ::CreateDXGIFactory(IID_PPV_ARGS(&_dxgi));

    // CreateDevice
    // - ���÷��� �����(�׷��� ī��)�� ��Ÿ���� ��ü. ��, �������� gpu���� �� ��û�� ���� �׻� _device�� ���ؼ� ��û�ϰ� ��.
    // - pAdapter : nullptr �����ϸ� �ý��� �⺻ ���÷��� �����
    // - MinimumFeatureLevel : ���� ���α׷��� �䱸�ϴ� �ּ� ��� ���� (���ڴٸ� �ɷ�����)
    //                        �츮�� Dx11�� �ּ��� �����ϴ� ��ī���� �Ѵٰ� ����.
    // - riid : ����̽��� COM ID
    // - ppDevice : ������ ��ġ�� �Ű������� ����
    ::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
}