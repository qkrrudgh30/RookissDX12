// Device.h

#pragma once

// Device�� ��ġ "�η� �繫��"�� ����.
// GPU�� �ñ� �ϰ��� ������ ���� ��ü�� ������ �����.
// dx9������ Device Ŭ������ ��� ����� �����.
class Device
{
public:
    void Init();

    ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
    ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
    // DX�� GPU�� �����ϰ� ���α׷����ϴµ� ���� ������ Graphic API.
    // DX�� ������ MS ��� GPU ��������� �����ϴ� ������.
    // �׷��� �츮�� DX�� �˸��. GPU�� ���� ��.
    // COM�� DX�� �˾Ƶ� GPU ��� �����ϰԲ� ���ִ� ��� �߿� �ϳ�.
    // COM(Component Object Model): DX�� ���α׷��� ��� �������� 
    //                              ���� ȣȯ���� �����ϰ� �ϴ� ���
    // COM ��ü(COM �������̽�)�� ���. ���λ����� �츮���� ������.
    // ComPtr: ������ ����Ʈ ������
    ComPtr<ID3D12Debug>			_debugController;
    ComPtr<IDXGIFactory>		_dxgi;            // [�߿�]ȭ�� ���� ��ɵ�
    ComPtr<ID3D12Device>		_device;          // [�߿�]���� ��ü ����
};
