#include "dx11.h"
#include "atltypes.h"
#include "ps_test.h"
#include "vs_test.h"
#include <DirectXMath.h>
#include <minwindef.h>

using namespace DirectX;

struct Vertex {
	float pos[3];
	float nor[3];
};

Vertex g_VertexList[]{
		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },

		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },

		{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f } },

		{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f } },

		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f } },

		{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f } },
};

WORD g_IndexList[]{
	 0,  1,  2,     3,  2,  1,
	 4,  5,  6,     7,  6,  5,
	 8,  9, 10,    11, 10,  9,
	12, 13, 14,    15, 14, 13,
	16, 17, 18,    19, 18, 17,
	20, 21, 22,    23, 22, 21,
};

D3D11_INPUT_ELEMENT_DESC g_VertexDesc[]{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct ConstantBuffer {
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT4 light;
	XMFLOAT4 attenuation;
};


CD3D11::CD3D11() {
	m_pDevice = NULL;
	m_pImmediateContext = NULL;
	m_pSwapChain = NULL;
	m_pRenderTargetView = NULL;
	m_pDeapthStencilTexture = NULL;
	m_pDepthStencilView = NULL;
	m_pVertexBuffer = NULL;
	m_pInputLayout = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pIndexBuffer = NULL;
	m_pConstantBuffer = NULL;
}

CD3D11::~CD3D11() {
	Release();
}

HRESULT CD3D11::Create(HWND hwnd) {
	Release();
	HRESULT hr;

	UINT flags = 0;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG
	D3D_FEATURE_LEVEL pLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL level;

	CRect rect;
	::GetClientRect(hwnd, &rect);
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = (FLOAT)rect.Width();
	m_Viewport.Height = (FLOAT)rect.Height();
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	DXGI_SWAP_CHAIN_DESC scDesc;
	::ZeroMemory(&scDesc, sizeof(scDesc));
	scDesc.BufferCount = 1;
	scDesc.BufferDesc.Width = rect.Width();
	scDesc.BufferDesc.Height = rect.Height();
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = hwnd;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Windowed = TRUE;

	D3D11_TEXTURE2D_DESC txDesc;
	ZeroMemory(&txDesc, sizeof(txDesc));
	txDesc.Width = rect.Width();
	txDesc.Height = rect.Height();
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format = txDesc.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;

	D3D11_BUFFER_DESC vbDesc;
	vbDesc.ByteWidth = sizeof(Vertex) * 24;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vrData;
	vrData.pSysMem = g_VertexList;
	vrData.SysMemPitch = 0;
	vrData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC ibDesc;
	ibDesc.ByteWidth = sizeof(WORD) * 6 * 6;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA irData;
	irData.pSysMem = g_IndexList;
	irData.SysMemPitch = 0;
	irData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(ConstantBuffer);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		pLevels,
		1,
		D3D11_SDK_VERSION,
		&scDesc,
		&m_pSwapChain,
		&m_pDevice,
		&level,
		&m_pImmediateContext
	);
	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* pBackBuffer;

	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateTexture2D(&txDesc, NULL, &m_pDeapthStencilTexture);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateDepthStencilView(m_pDeapthStencilTexture, &dsDesc, &m_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateBuffer(&vbDesc, &vrData, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateInputLayout(g_VertexDesc, ARRAYSIZE(g_VertexDesc),
		g_vs_main, sizeof(g_vs_main),
		&m_pInputLayout);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateVertexShader(g_vs_main, sizeof(g_vs_main), NULL, &m_pVertexShader);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, &m_pPixelShader);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateBuffer(&ibDesc, &irData, &m_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMVECTOR eye = XMVectorSet(2.0f, 2.0f, -2.0f, 0.0f);
	XMVECTOR focus = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR light = XMVector3Normalize(XMVectorSet(0.0f, 2.0f, -1.5f, 0.0f));
	XMVECTOR attenuation = XMVectorSet(1.0f, 0.0f, 0.2f, 0.0f);
	XMMATRIX viewMatrix = XMMatrixLookAtLH(eye, focus, up);

	float fov = XMConvertToRadians(45.0f);
	float aspect = m_Viewport.Width / m_Viewport.Height;
	float nearZ = 0.1f;
	float farZ = 100.0f;
	XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);

	ConstantBuffer cb;
	XMStoreFloat4x4(&cb.world, XMMatrixTranspose(worldMatrix));
	XMStoreFloat4x4(&cb.view, XMMatrixTranspose(viewMatrix));
	XMStoreFloat4x4(&cb.projection, XMMatrixTranspose(projMatrix));
	XMStoreFloat4(&cb.light, light);
	XMStoreFloat4(&cb.attenuation, attenuation);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);

	return hr;

}

void CD3D11::Release() {
	if (m_pImmediateContext) {
		m_pImmediateContext->ClearState();
	}
	
	if (NULL != m_pRenderTargetView) {
		m_pRenderTargetView->Release();
		m_pRenderTargetView = NULL;
	}
	if (NULL != m_pSwapChain) {
		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	}
	if (NULL != m_pImmediateContext) {
		m_pImmediateContext->Release();
		m_pImmediateContext = NULL;
	}
	if (NULL != m_pDevice) {
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	if (NULL != m_pDeapthStencilTexture) {
		m_pDeapthStencilTexture->Release();
		m_pDeapthStencilTexture = NULL;
	}
	if (NULL != m_pDepthStencilView) {
		m_pDepthStencilView->Release();
		m_pDepthStencilView = NULL;
	}
	if (NULL != m_pVertexBuffer) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
	if (NULL != m_pInputLayout) {
		m_pInputLayout->Release();
		m_pInputLayout = NULL;
	}
	if (NULL != m_pVertexShader) {
		m_pVertexShader->Release();
		m_pInputLayout = NULL;
	}
	if (NULL != m_pPixelShader) {
		m_pPixelShader->Release();
		m_pPixelShader = NULL;
	}
	if (NULL != m_pConstantBuffer) {
		m_pConstantBuffer->Release();
		m_pConstantBuffer = NULL;
	}
}

void CD3D11::Render() {
	if (NULL == m_pDevice) return;
	if (NULL == m_pImmediateContext) return;
	if (NULL == m_pRenderTargetView) return;
	if (NULL == m_pSwapChain) return;
	if (NULL == m_pDeapthStencilTexture) return;
	if (NULL == m_pDepthStencilView) return;
	if (NULL == m_pInputLayout) return;
	if (NULL == m_pPixelShader) return;
	if (NULL == m_pVertexBuffer) return;
	if (NULL == m_pVertexShader) return;
	if (NULL == m_pConstantBuffer) return;


	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	UINT strides = sizeof(Vertex);
	UINT offsets = 0;
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &strides, &offsets);
	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pImmediateContext->RSSetViewports(1, &m_Viewport);
	m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_pImmediateContext->DrawIndexed(42, 0, 0);

	m_pSwapChain->Present(0, 0);
}
