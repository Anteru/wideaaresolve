/**
Copyright 2011, Mattaeus G. Chajdas. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
	  conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
	  of conditions and the following disclaimer in the documentation and/or other materials
	  provided with the distribution.

THIS SOFTWARE IS PROVIDED BY MATTHAEUS G. CHAJDAS ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHAEUS G. CHAJDAS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Mattaeus G. Chajdas.
*/

#include "DXUT.h"
#include "DXUTCamera.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include <xnamath.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

struct IntermediateRenderTarget
{
	struct {
		ID3D11RenderTargetView*		rtv;
		ID3D11Texture2D*			texture;
		ID3D11ShaderResourceView*	srv;
	} colorTarget;

	struct {
		ID3D11DepthStencilView*	dsv;
		ID3D11Texture2D*		texture;
	} depthTarget;

	void Bind (ID3D11DeviceContext* context)
	{
		context->OMSetRenderTargets (1, &colorTarget.rtv, depthTarget.dsv);
	}
	
	void Create (ID3D11Device* device, int width, int height, const int sampleCount)
	{
		D3D11_TEXTURE2D_DESC desc;
		::ZeroMemory (&desc, sizeof (desc));

		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = sampleCount;
		desc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
		desc.MipLevels = 1;
		desc.ArraySize = 1;

		device->CreateTexture2D (&desc, nullptr, &colorTarget.texture);
		device->CreateRenderTargetView (colorTarget.texture, nullptr, &colorTarget.rtv);
		device->CreateShaderResourceView (colorTarget.texture, nullptr, &colorTarget.srv);

		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		device->CreateTexture2D (&desc, nullptr, &depthTarget.texture);
		device->CreateDepthStencilView (depthTarget.texture, nullptr, &depthTarget.dsv);
	}

	void Destroy ()
	{
		SAFE_RELEASE (colorTarget.texture);
		SAFE_RELEASE (colorTarget.rtv);
		SAFE_RELEASE (colorTarget.srv);

		SAFE_RELEASE (depthTarget.texture);
		SAFE_RELEASE (depthTarget.dsv);
	}

	~IntermediateRenderTarget ()
	{
		Destroy ();
	}
};

struct Filters
{
	std::map<std::pair<int, std::string>, ID3D11PixelShader*> filters;

	ID3D11PixelShader*	copy;
	ID3D11Texture2D*	resolveTexture;
	ID3D11ShaderResourceView* resolveView;

	std::vector<ID3D11PixelShader*>	shaders;

	ID3D11Buffer*		quadVertexBuffer;
	ID3D11VertexShader*	quadVertexShader;
	ID3D11InputLayout*	quadLayout;

	struct VF_Quad
	{
		XMVECTOR position;
		XMFLOAT2 uv;
	};

	ID3D11PixelShader* CreateShader (ID3D11Device* device, const int sampleCount, 
		const char* filter)
	{
		ID3DXBuffer* blob;

		D3DXMACRO filterMacro;

		if (::strcmp (filter, "COPY") == 0) {
			filterMacro.Definition = "1";
			filterMacro.Name = "COPY";
		} else { 
			filterMacro.Definition = filter;
			filterMacro.Name = "FILTER";
		}

		char MSAASampleCountString [2] = {0};
		::sprintf (MSAASampleCountString, "%d", sampleCount);

		D3DXMACRO sampleCountMacro;
		sampleCountMacro.Name = "MSAA_SAMPLES";
		sampleCountMacro.Definition = MSAASampleCountString;

		const D3DXMACRO defines [] = { filterMacro, sampleCountMacro, {nullptr, nullptr}};

		ID3DXBuffer* error = nullptr;
		D3DXCompileShaderFromFile (L"filter.hlsl", defines,
			nullptr, "PS_Main", "ps_5_0",
			0, &blob, &error, nullptr);

		if (error) {
			OutputDebugStringA (static_cast<const char*> (error->GetBufferPointer ()));
		}

		SAFE_RELEASE (error);

		ID3D11PixelShader* shader;
		device->CreatePixelShader (blob->GetBufferPointer (),
			blob->GetBufferSize (), nullptr, &shader);

		shaders.push_back (shader);

		SAFE_RELEASE(blob);

		return shader;
	}

	void CreateResolutionDependent (ID3D11Device* device, int width, int height)
	{
		D3D11_TEXTURE2D_DESC desc;
		::ZeroMemory (&desc, sizeof (desc));

		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		desc.Width = width;
		desc.Height = height;

		device->CreateTexture2D (&desc, nullptr, &resolveTexture);
		device->CreateShaderResourceView (resolveTexture, nullptr, &resolveView);

	}
	
	void Create (ID3D11Device* device)
	{
#define MAKE_FILTER(sampleCount, name) filters [std::make_pair (sampleCount, name)] = CreateShader (device, sampleCount, name)
		static const int sampleCounts [] = {1, 2, 4, 8};

		for (int i = 0; i < 4; ++i) {
			const int sampleCount = sampleCounts [i];
			MAKE_FILTER(sampleCount, "Lanczos2D");
			MAKE_FILTER(sampleCount, "Gaussian2D");
			MAKE_FILTER(sampleCount, "Box2D");
		}

#undef MAKE_FILTER

		copy = CreateShader (device, 1, "COPY");

		{
			ID3DXBuffer* blob;
			static const D3DXMACRO defines [] = 
			{
				{"MSAA_SAMPLES", "1"},
				{"COPY", "1"},
				{nullptr, nullptr}
			};

			D3DXCompileShaderFromFile (L"filter.hlsl", defines,
			nullptr, "VS_Main", "vs_5_0",
				0, &blob, nullptr, nullptr);

			device->CreateVertexShader (blob->GetBufferPointer (),
				blob->GetBufferSize (), nullptr, &quadVertexShader);
			
			{
				static const D3D11_INPUT_ELEMENT_DESC desc [] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				device->CreateInputLayout (desc, 2,
					blob->GetBufferPointer (), blob->GetBufferSize (),
					&quadLayout);
			}

			static const VF_Quad quad [] = {
				{ XMVectorSet (-1, -1, 0, 1), XMFLOAT2 (0, 0) },
				{ XMVectorSet (-1, 1, 0, 1), XMFLOAT2 (0, 1) },
				{ XMVectorSet (1, -1, 0, 1), XMFLOAT2 (1, 0) },
				
				{ XMVectorSet (1, 1, 0, 1), XMFLOAT2 (1, 1) },
				{ XMVectorSet (1, -1, 0, 1), XMFLOAT2 (1, 0) },
				{ XMVectorSet (-1, 1, 0, 1), XMFLOAT2 (0, 1) }
			};

			{
				D3D11_BUFFER_DESC desc;
				::ZeroMemory (&desc, sizeof (desc));

				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				desc.ByteWidth = sizeof (VF_Quad) * 6;
				desc.Usage = D3D11_USAGE_IMMUTABLE;

				D3D11_SUBRESOURCE_DATA data;
				::ZeroMemory (&data, sizeof (data));
				data.pSysMem = quad;

				device->CreateBuffer (&desc, &data, &quadVertexBuffer);
			}

			SAFE_RELEASE (blob);
		}
	}

	void Resolve (ID3D11DeviceContext* context,
		ID3D11Texture2D* source,
		ID3D11ShaderResourceView* sourceView,
		const std::string& filter, const int sampleCount)
	{
		auto shader = copy;

		if (filter[0] == 'H') {
			context->ResolveSubresource (resolveTexture, D3D11CalcSubresource (0,0,0),
				source, D3D11CalcSubresource (0,0,0), DXGI_FORMAT_R8G8B8A8_UNORM);
			sourceView = resolveView;
		} else {
			shader = filters [std::make_pair (sampleCount, filter)];
		}

		context->IASetInputLayout (quadLayout);
		context->IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		const ::UINT stride = sizeof (VF_Quad);
		const ::UINT offset = 0;
		context->IASetVertexBuffers (0, 1, &quadVertexBuffer, &stride, &offset);

		context->PSSetShaderResources (0, 1, &sourceView);

		context->VSSetShader (quadVertexShader, nullptr, 0);
		context->PSSetShader (shader, nullptr, 0);

		context->Draw (6, 0);

		ID3D11ShaderResourceView* nullView = nullptr;
		context->PSSetShaderResources (0, 1, &nullView);
	}

	void DestroyResolutionDependent ()
	{
		SAFE_RELEASE (resolveTexture);
		SAFE_RELEASE (resolveView);
	}

	void Destroy ()
	{
		std::for_each (begin (shaders), end (shaders), [] (ID3D11PixelShader* s)
			-> void { SAFE_RELEASE (s); } );

		shaders.clear ();

		SAFE_RELEASE (quadVertexBuffer);
		SAFE_RELEASE (quadVertexShader);
		SAFE_RELEASE (quadLayout);
	}
};

struct VF_PosNormalColor
{
	XMVECTOR	position;
	XMVECTOR	normal;
	XMCOLOR		color;
};

struct DemoDataSet
{
	ID3D11Buffer*		vertexBuffer;
	ID3D11InputLayout*	inputLayout;
	ID3D11VertexShader*	vertexShader;
	ID3D11PixelShader*	pixelShader;
	ID3D11Buffer*		shaderConstantBuffer;

	::UINT				vertexCount;

#pragma pack(push, 1)
	struct ConstantBuffer
	{
		XMMATRIX	worldViewProjection;
		XMMATRIX	view;
	};
#pragma pack(pop, 1)

	void AddQuad (const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c,
		const XMVECTOR& d, const XMCOLOR& color, std::vector<VF_PosNormalColor>& output)
	{
		const auto normal = XMVector3Normalize (XMVector3Cross (b-a, c-a));

		VF_PosNormalColor tmp;
		tmp.color = color;
		tmp.normal = normal;

		tmp.position = a;
		output.push_back (tmp);
		tmp.position = b;
		output.push_back (tmp);
		tmp.position = c;
		output.push_back (tmp);
		tmp.position = b;
		output.push_back (tmp);
		tmp.position = d;
		output.push_back (tmp);
		tmp.position = c;
		output.push_back (tmp);
	}

	void Create (ID3D11Device* device)
	{
		ID3D11Texture2D* intermediateTexture;

		D3DX11_IMAGE_LOAD_INFO info;
		::memset (&info, D3DX11_DEFAULT, sizeof (info));
		info.pSrcInfo = nullptr;
		info.CpuAccessFlags = D3D11_CPU_ACCESS_READ;
		info.Usage = D3D11_USAGE_STAGING;
		info.BindFlags = 0;

		D3DX11CreateTextureFromFile (device, L"hm.png", &info, nullptr,
			reinterpret_cast<ID3D11Resource**>(&intermediateTexture), nullptr);

		D3D11_TEXTURE2D_DESC desc;
		intermediateTexture->GetDesc (&desc);

		// Get the width, height and the data
		std::vector<XMCOLOR>	data (desc.Width * desc.Height);

		// Input must be RGBA8
		assert (desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM);

		ID3D11DeviceContext* context;
		device->GetImmediateContext (&context);
		D3D11_MAPPED_SUBRESOURCE mapping;
		context->Map (intermediateTexture, D3D11CalcSubresource (0, 0, 0),
			D3D11_MAP_READ, 0, &mapping);
		::memcpy (&data[0], mapping.pData, sizeof (XMCOLOR) * data.size ());
		context->Unmap (intermediateTexture, D3D11CalcSubresource (0, 0, 0));
		SAFE_RELEASE (context);

		SAFE_RELEASE(intermediateTexture);

		std::vector<VF_PosNormalColor>	vertexBufferData;

		static const float CAP_FACTOR = 3.0f;
		static const float SPACING = 2.0f;
		static const float BORDER = 0.1f;

		// We have the data, build the grid now
		for (int y = 0; y < desc.Height; ++y) {
			for (int x = 0; x < desc.Width; ++x) {
				const auto value = data [y * desc.Width + x];

				const float height = 8.0f + 32.0f * static_cast<float>(255 - value.g) / 255.0f;

				const float r0 = CAP_FACTOR * static_cast<float> (std::rand ()) / RAND_MAX;
				const float r1 = CAP_FACTOR * static_cast<float> (std::rand ()) / RAND_MAX;
				const float r2 = CAP_FACTOR * static_cast<float> (std::rand ()) / RAND_MAX;

				const XMVECTOR	x0y0z0		= XMVectorSet (-x*SPACING + BORDER, 0,					y*SPACING + BORDER, 1);
				const XMVECTOR	x0y1z0		= XMVectorSet (-x*SPACING + BORDER, r0 + height,		y*SPACING + BORDER, 1);
				const XMVECTOR	x0y0z1		= XMVectorSet (-x*SPACING + BORDER, 0,					(y+1)*SPACING - BORDER, 1);
				const XMVECTOR	x0y1z1		= XMVectorSet (-x*SPACING + BORDER, r1 + height,		(y+1)*SPACING - BORDER, 1);
				const XMVECTOR	x1y0z0		= XMVectorSet (-(x-1)*SPACING - BORDER, 0,				y*SPACING + BORDER, 1);
				const XMVECTOR	x1y1z0		= XMVectorSet (-(x-1)*SPACING - BORDER, r2 + height,	y*SPACING + BORDER, 1);

				const XMVECTOR	x1y0z1		= XMVectorSet (-(x-1)*SPACING - BORDER, 0, (y+1)*SPACING - BORDER, 1);
				const auto plane			= XMPlaneNormalize (XMPlaneFromPoints (x0y1z0, x0y1z1, x1y1z0));
				const auto x1y1z1			= XMPlaneIntersectLine (plane, x1y0z1, x1y0z1 + XMVectorSet (0, 256.0f, 0, 0));

				AddQuad (x0y1z0, x0y1z1, x1y1z0, x1y1z1, value, vertexBufferData);

				AddQuad (x0y1z0, x0y0z0, x0y1z1, x0y0z1, value, vertexBufferData);

				AddQuad (x1y1z0, x1y1z1, x1y0z0, x1y0z1, value, vertexBufferData);

				AddQuad (x0y0z0, x0y1z0, x1y0z0, x1y1z0, value, vertexBufferData);

				AddQuad (x0y0z1, x1y0z1, x0y1z1, x1y1z1, value, vertexBufferData);
			}
		}

		{
			D3D11_BUFFER_DESC desc;
			::ZeroMemory (&desc, sizeof (desc));
			desc.ByteWidth = sizeof (VF_PosNormalColor) * vertexBufferData.size ();
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			
			D3D11_SUBRESOURCE_DATA bufferDataPointer;
			::ZeroMemory (&bufferDataPointer, sizeof (bufferDataPointer));
			bufferDataPointer.pSysMem = &vertexBufferData [0];

			device->CreateBuffer (&desc, &bufferDataPointer, &vertexBuffer);

			vertexCount = static_cast< ::UINT> (vertexBufferData.size ());
		}

		{
			D3D11_BUFFER_DESC desc;
			::ZeroMemory (&desc, sizeof (desc));
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof (ConstantBuffer);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_DYNAMIC;

			device->CreateBuffer (&desc, nullptr, &shaderConstantBuffer);
		}

		ID3DXBuffer*	vertexShaderBlob, * pixelShaderBlob;
		
		D3DXCompileShaderFromFile (L"scene.hlsl", nullptr,
			nullptr, "VS_Main", "vs_5_0",
			0, &vertexShaderBlob, nullptr, nullptr);
		D3DXCompileShaderFromFile (L"scene.hlsl", nullptr,
			nullptr, "PS_Main", "ps_5_0",
			0, &pixelShaderBlob, nullptr, nullptr);

		device->CreateVertexShader (vertexShaderBlob->GetBufferPointer (),
			vertexShaderBlob->GetBufferSize (), nullptr, &vertexShader);
		device->CreatePixelShader (pixelShaderBlob->GetBufferPointer (),
			pixelShaderBlob->GetBufferSize (), nullptr, &pixelShader);

		{
			static const D3D11_INPUT_ELEMENT_DESC desc [] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			device->CreateInputLayout (desc, 3,
				vertexShaderBlob->GetBufferPointer (), vertexShaderBlob->GetBufferSize (),
				&inputLayout);
		}

		SAFE_RELEASE (vertexShaderBlob);
		SAFE_RELEASE (pixelShaderBlob);
	}

	void Destroy ()
	{
		SAFE_RELEASE (vertexBuffer);
		SAFE_RELEASE (inputLayout);
		SAFE_RELEASE (pixelShader);
		SAFE_RELEASE (vertexShader);
		SAFE_RELEASE (shaderConstantBuffer);
	}

	void Draw (ID3D11DeviceContext* context, const XMMATRIX& worldViewProjection, const XMMATRIX& view)
	{
		ConstantBuffer cb;
		cb.view = view;
		cb.worldViewProjection = worldViewProjection;

		D3D11_MAPPED_SUBRESOURCE mapping;
		context->Map (shaderConstantBuffer, D3D11CalcSubresource (0, 0, 0),
			D3D11_MAP_WRITE_DISCARD, 0, &mapping);
		::memcpy (mapping.pData, &cb, sizeof (cb));
		context->Unmap (shaderConstantBuffer, D3D11CalcSubresource (0, 0, 0));

		context->IASetInputLayout (inputLayout);
		context->IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const ::UINT stride [] = { sizeof (VF_PosNormalColor) };
		const ::UINT offset [] = { 0 };
		context->IASetVertexBuffers (0, 1, &vertexBuffer, stride, offset);

		context->VSSetConstantBuffers (0, 1, &shaderConstantBuffer);
		context->VSSetShader (vertexShader, nullptr, 0);

		context->PSSetConstantBuffers (0, 1, &shaderConstantBuffer);
		context->PSSetShader (pixelShader, nullptr, 0);

		context->Draw (vertexCount, 0);
	}
};


IntermediateRenderTarget*	g_IntermediateTarget					= nullptr;
int							g_SampleCount							= 1;
std::string					g_Filter								= "H";

CFirstPersonCamera          g_Camera;
DemoDataSet*				g_DemoData								= nullptr;
Filters*					g_Filters								= nullptr;

CDXUTDialogResourceManager  g_DialogResourceManager; 
CDXUTTextHelper*            g_pTxtHelper = nullptr;
CDXUTDialog                 g_HUD;                  
CDXUTDialog                 g_SampleUI;

static const ::UINT			IDC_CHANGE_FILTER = 1;

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
									   DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
									  void* pUserContext )
{
	g_IntermediateTarget	= new IntermediateRenderTarget ();
	g_Filters				= new Filters ();
	g_Filters->Create (pd3dDevice);
	g_DemoData				= new DemoDataSet ();
	g_DemoData->Create (pd3dDevice);

	D3DXVECTOR3 eye (-64, 256, 128), lookAt (-128, 0, 64);
	g_Camera.SetViewParams (&eye, &lookAt);
	
	ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	HRESULT hr;
	V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
	g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15 );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
										  const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	g_IntermediateTarget->Create (pd3dDevice, pBackBufferSurfaceDesc->Width,
		pBackBufferSurfaceDesc->Height, g_SampleCount);

	const float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams (D3DX_PI / 4, fAspectRatio, 0.1f, 1000.0f );
	g_Camera.SetScalers (0.01f, 192.0f );
	g_Camera.SetDrag (true);

	g_Filters->CreateResolutionDependent (pd3dDevice, pBackBufferSurfaceDesc->Width,
		pBackBufferSurfaceDesc->Height);

	HRESULT hr;

	V_RETURN( g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

	g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
	g_HUD.SetSize( 170, 170 );
	g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
	g_SampleUI.SetSize( 170, 300 );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	g_Camera.FrameMove( fElapsedTime );
}

void RenderText()
{
	D3D11_TEXTURE2D_DESC desc;
	g_IntermediateTarget->colorTarget.texture->GetDesc (&desc);

	::WCHAR intermediateBufferDesc [1024] = { 0 };
	::wsprintf (intermediateBufferDesc, L"Backbuffer: %dx%d @ %d sample%c",
		desc.Width, desc.Height, desc.SampleDesc.Count, desc.SampleDesc.Count > 1 ? L's' : L' ');

	g_pTxtHelper->Begin();
	g_pTxtHelper->SetInsertionPos( 5, 5 );
	g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
	g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
	g_pTxtHelper->DrawTextLine (intermediateBufferDesc);
	g_pTxtHelper->DrawTextLine (L"Hold shift to slow down movement");
	g_pTxtHelper->End();
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
								  double fTime, float fElapsedTime, void* pUserContext )
{
	// Render to intermediate target
	g_IntermediateTarget->Bind (pd3dImmediateContext);
	static const float clearColor [] = { 0.042f, 0.042f, 0.042f, 0 };
	pd3dImmediateContext->ClearRenderTargetView (
		g_IntermediateTarget->colorTarget.rtv, clearColor);
	pd3dImmediateContext->ClearDepthStencilView (
		g_IntermediateTarget->depthTarget.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	// Render our geometry
	const XMMATRIX view (*g_Camera.GetViewMatrix ());
	const XMMATRIX proj (*g_Camera.GetProjMatrix ());
	const XMMATRIX world (*g_Camera.GetWorldMatrix ());

	const auto worldViewProjection = view * proj;

	g_DemoData->Draw (pd3dImmediateContext, worldViewProjection, view);

	// Render to frame buffer	
	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	
	pd3dImmediateContext->OMSetRenderTargets (1, &pRTV, pDSV);

	pd3dImmediateContext->ClearRenderTargetView( pRTV, clearColor );
	pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	g_Filters->Resolve (pd3dImmediateContext,
		g_IntermediateTarget->colorTarget.texture,
		g_IntermediateTarget->colorTarget.srv,
		g_Filter,
		g_SampleCount);

	DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
		g_HUD.OnRender( fElapsedTime );
		g_SampleUI.OnRender( fElapsedTime );
		RenderText();
	DXUT_EndPerfEvent();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
	if (g_IntermediateTarget) { g_IntermediateTarget->Destroy (); }
	if (g_Filters) { g_Filters->DestroyResolutionDependent (); }

	
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	g_DemoData->Destroy ();
	delete g_DemoData;
	g_Filters->Destroy ();
	delete g_Filters;
	delete g_IntermediateTarget;

	g_DialogResourceManager.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_DELETE( g_pTxtHelper );
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
						  bool* pbNoFurtherProcessing, void* pUserContext )
{	
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;
	*pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

	return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if (nChar == VK_SHIFT) {
		if (bKeyDown) {
			g_Camera.SetScalers (0.01f, 4.0f );
		} else {
			g_Camera.SetScalers (0.01f, 192.0f );
		}
	}
}


//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
					   bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
					   int xPos, int yPos, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved( void* pUserContext )
{
	return true;
}

void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	switch( nControlID )
	{
	case IDC_CHANGE_FILTER:
		{
			DXUTComboBoxItem* pItem = ( ( CDXUTComboBox* )pControl )->GetSelectedItem();
			const char* selectedFilter = static_cast<const char*> (pItem->pData);

			const int oldSampleCount = g_SampleCount;
			switch (selectedFilter [0]) {
			case 'H':
				g_Filter = "HW";
				break;
			case 'B':
				g_Filter = "Box2D";
				break;
			case 'L':
				g_Filter = "Lanczos2D";
				break;
			case 'G':
				g_Filter = "Gaussian2D";
				break;
			}

			switch (selectedFilter [1]) {
			case '2':
				g_SampleCount = 2;
				break;
			case '1':
				g_SampleCount = 1;
				break;
			case '4':
				g_SampleCount = 4;
				break;
			case '8':
				g_SampleCount = 8;
				break;
			}

			if (g_SampleCount != oldSampleCount) {
				OnD3D11ReleasingSwapChain (nullptr);
				OnD3D11ResizedSwapChain (DXUTGetD3D11Device (),
					DXUTGetDXGISwapChain (), DXUTGetDXGIBackBufferSurfaceDesc (),
					nullptr);
			}
		}
	}
}
//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// DXUT will create and use the best device (either D3D9 or D3D11) 
	// that is available on the system depending on which D3D callbacks are set below

	// Set general DXUT callbacks
	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackKeyboard( OnKeyboard );
	DXUTSetCallbackMouse( OnMouse );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
	DXUTSetCallbackDeviceRemoved( OnDeviceRemoved );

	// Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
	DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
	DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
	DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
	DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
	DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
	DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

	// Perform any application-level initialization here
	// UI Init
	g_HUD.Init( &g_DialogResourceManager );
	g_SampleUI.Init( &g_DialogResourceManager );

	g_HUD.SetCallback( OnGUIEvent );
	int iY = 4;
	int iYo = 26;
	CDXUTComboBox* filterCombo;
	g_HUD.AddComboBox (IDC_CHANGE_FILTER, 0, iY, 170, 22, 0, false, &filterCombo);
	filterCombo->AddItem (L"None", "H1");
	filterCombo->AddItem (L"HW 2x", "H2");
	filterCombo->AddItem (L"Box 2x", "B2");
	filterCombo->AddItem (L"Gauss 2x", "G2");
	filterCombo->AddItem (L"Lanczos 2x", "L2");
	filterCombo->AddItem (L"HW 4x", "H4");
	filterCombo->AddItem (L"Box 4x", "B4");
	filterCombo->AddItem (L"Gauss 4x", "G4");
	filterCombo->AddItem (L"Lanczos 4x", "L4");
	filterCombo->AddItem (L"HW 8x", "H8");
	filterCombo->AddItem (L"Box 8x", "B8");
	filterCombo->AddItem (L"Gauss 8x", "G8");
	filterCombo->AddItem (L"Lanczos 8x", "L8");

	g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;

	DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
	DXUTCreateWindow( L"WideFilter" );

	// Only require 10-level hardware
	DXUTCreateDevice( D3D_FEATURE_LEVEL_11_0, true, 1280, 720);
	DXUTMainLoop(); // Enter into the DXUT ren  der loop

	// Perform any application-level cleanup here

	return DXUTGetExitCode();
}


