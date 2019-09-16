#include <d3d11.h>

class CD3D11 {
public:
	CD3D11_VIEWPORT m_Viewport;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDeapthStencilTexture;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11Buffer* m_pConstantBuffer;

public:
	CD3D11();
	virtual ~CD3D11();
	virtual HRESULT Create(HWND hwnd);
	virtual void Release();
	virtual void Render();
};
