#pragma once
#include "CAsset.h"

class CTexture :
    public CAsset
{
private:
    ScratchImage                        m_Image;
    ComPtr<ID3D11Texture2D>			    m_Tex2D;

    ComPtr<ID3D11RenderTargetView>      m_RTV;
    ComPtr<ID3D11DepthStencilView>      m_DSV;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    D3D11_TEXTURE2D_DESC                m_Desc;

    UINT                                m_RecentBindingRegisterNum;

public:
    ComPtr<ID3D11Texture2D>             GetTex2D() { return m_Tex2D; }
    ComPtr<ID3D11RenderTargetView>      GetRTV() { return m_RTV; }
    ComPtr<ID3D11DepthStencilView>      GetDSV() { return m_DSV; }
    ComPtr<ID3D11ShaderResourceView>    GetSRV() { return m_SRV; }
    ComPtr<ID3D11UnorderedAccessView>   GetUAV() { return m_UAV; }

    const D3D11_TEXTURE2D_DESC          GetDesc() { return m_Desc; }

    tPixel* GetPixels() { return (tPixel*)m_Image.GetPixels(); }

    UINT Width() { return m_Desc.Width; }
    UINT Height() { return m_Desc.Height; }

public:
    // _Flags : D3D11_BIND_FLAG
    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _PixelFormat, UINT _Flags, D3D11_USAGE _Usage = D3D11_USAGE_DEFAULT);
    int Create(ComPtr<ID3D11Texture2D> _Tex2D);

    // GracphiShader - t register
    void Binding(UINT _registerNum);
    static void Clear(UINT _registerNum);

    // ComputeShader - t register
    void Binding_CS_SRV(UINT _RegisterNum);
    void Clear_CS_SRV();
    
    // ComputeShader - u register
    void Binding_CS_UAV(UINT _RegisterNum);
    void Clear_CS_UAV();


    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override;

public:
    CTexture();
    ~CTexture();
    virtual CTexture* Clone() { return nullptr; }
};

