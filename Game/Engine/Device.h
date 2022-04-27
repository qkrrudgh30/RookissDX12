// Device.h

#pragma once

// Device는 마치 "인력 사무소"와 같음.
// GPU에 맡긴 일감을 수행할 각종 개체의 생성을 담당함.
// dx9에서는 Device 클래스가 모든 기능을 담당함.
class Device
{
public:
    void Init();

    ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
    ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
    // DX는 GPU를 제어하고 프로그래밍하는데 쓰는 저수준 Graphic API.
    // DX를 개발한 MS 사와 GPU 제조사들이 협력하는 이유임.
    // 그래서 우리는 DX만 알면됨. GPU는 몰라도 됨.
    // COM은 DX만 알아도 GPU 제어를 가능하게끔 해주는 기술 중에 하나.
    // COM(Component Object Model): DX의 프로그래밍 언어 독립성과 
    //                              하위 호환성을 가능하게 하는 기술
    // COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐.
    // ComPtr: 일종의 스마트 포인터
    ComPtr<ID3D12Debug>			_debugController;
    ComPtr<IDXGIFactory>		_dxgi;            // [중요]화면 관련 기능들
    ComPtr<ID3D12Device>		_device;          // [중요]각종 객체 생성
};
