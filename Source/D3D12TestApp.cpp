﻿// D3D12TestApp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "D3D12TestApp.h"

#include "GfxLib.h"

#include <crtdbg.h>

using namespace DirectX;


#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND	g_hWnd;

#if 0
namespace GlobalRootSignatureParams {
	enum Value {
		OutputViewSlot = 0,
		AccelerationStructureSlot,
		SamplerStateSlot,
		SkyTextures,
		Count
	};
}

namespace LocalRootSignatureParams {
	enum Value {
		ViewportConstantSlot = 0,
		Count
	};
}

struct Viewport
{
	float left;
	float top;
	float right;
	float bottom;
};

struct RayGenConstantBuffer
{
	Viewport	viewport;
	Viewport	stencil;
	XMMATRIX	mtxCamera;
};

struct ModelConstantBuffer
{
	uint32_t  isIndex16bit;
	uint32_t  primitiveOffset;
	uint32_t	padd[2];
};

struct RtVertex { float v1, v2, v3; };

// 頂点属性
struct RtAttrib {

	XMFLOAT3	Normal;
	XMFLOAT3	BaseColor;
	XMFLOAT2	Uv;

};

#endif

namespace {


	//	ファイル名の位置を取得
	uint32_t	FindFileName(const wchar_t* filepath)
	{

		uint32_t idx = 0;
		uint32_t lastidx = 0;

		while (filepath[idx] != 0)
		{
			if (filepath[idx] == L'/' || filepath[idx] == L'\\') {
				lastidx = idx + 1;
			}


			++idx;
		}

		return lastidx;


	}

};


#if 0
namespace Config
{

	enum {

		MaxRecursionDepth = 4,

	};

}

enum {


	TRACE_TYPE_NUM = 2,

};



const wchar_t* c_hitGroupName[] = {
	L"MyHitGroup",
	L"MyHitGroup_Shadow",
};
const wchar_t* c_raygenShaderName = L"MyRaygenShader";
const wchar_t* c_closestHitShaderName[] = {
	L"MyClosestHitShader",
	L"MyClosestHitShader_Shadow",
};
const wchar_t* c_missShaderName[] = {
	L"MyMissShader",
	L"MyMissShader_Shadow",
};

#endif


struct GFX{
	GfxLib::CoreSystem		gfxCore;
	GfxLib::SwapChain		swapChain;
	GfxLib::GraphicsCommandList		cmdList;
	GfxLib::RootSignature	rootSig;
	GfxLib::PipelineState	pipelineState;
	GfxLib::VertexBuffer	vtxBuff;
	GfxLib::IndexBuffer		idxBuff;
	GfxLib::PixelShader		pixelshader;
	GfxLib::VertexShader	vertexshader;
	//GfxLib::ConstantBuffer	constantBuffer;
	//GfxLib::ConstantBuffer	constantBuffer2;
	GfxLib::DepthStencil	depthStencil;
	GfxLib::Texture2D		texture2D;
	GfxLib::DescriptorHeap	descHeap_Sampler;
	//GfxLib::DescriptorHeap	descHeap_CBV;
	GfxLib::DepthStencilState	depthStencilState;
	GfxLib::BlendState		blendState;
	GfxLib::BlendState		blendState2;
	GfxLib::RasterizerState	rasterizerState;
	GfxLib::InputLayout		inputLayout;
	GfxLib::Sampler			sampsLinear;

	GfxLib::FontSystem		fontSystem;
	GfxLib::FontData		fontData;


	//	Ray Tracing
#if 0
	GfxLib::RootSignature	globalRootSig;
	GfxLib::RootSignature	localRootSig;
	GfxLib::RootSignature	localRootSigMiss;
	GfxLib::RootSignature	localRootSigRayGen;
	RayGenConstantBuffer	m_rayGenCB;
	GfxLib::Shader			m_rtShaderLib;
	GfxLib::Shader			m_rtShaderLibModel;
	GfxLib::StateObject		m_rtStateObject;
#endif


	GfxLib::RayTracingRenderer	m_RTRender;

	//GfxLib::RtGeometry		m_rtGeometry;
	GfxLib::RtModel			m_rtModel;
	GfxLib::TextureContainer	m_TexContainer;
	//GfxLib::StructuredBuffer	m_rtGeomAttrib;
	//GfxLib::Texture2D		m_rtOutput;
	GfxLib::RtSceneTargets		m_rtTarget;
	GfxLib::TopLevelAccelerationStructure	m_rtTLAS;
	GfxLib::BottomLevelAccelerationStructure	m_rtBLAS;
	GfxLib::Buffer			m_rtScratch;
	GfxLib::Texture2D		m_texSky;
	GfxLib::Texture2D		m_texSkyRem;
	GfxLib::Texture2D		m_texSkyIem;


	double		m_AppTime;	//	アプリが起動してからの時間

	float		m_camAngX, m_camAngY;
	float		m_camDist;
	GfxLib::Float3	m_camCenter;
	POINT	m_lastMouseDown;
	bool	m_MouseMove;

	bool	Initialize();
	void	Finalize();

	void	Update();
	void	Render();

	void	DoRayTracing(GfxLib::GraphicsCommandList &cmdList);

	void	OnLButtonDown(POINT pt);
	void	OnLButtonUp(POINT pt);
	void	OnMouseMove(POINT pt);
	void	OnWheel(int wheel);
	void	OnKeyDown(WPARAM wParam,LPARAM lParam);

	//void	CreateRayTracingRootSignature();
	//void	CreateRayTracingPipelineStateObject();
	void	CreateRayTracingGeometry();
	void	CreateRayTracingResources();
	void	BuildAccelerationStructures();
	void	CreateRayTracingOutputResource();	//	出力リソースの作成
};



GFX * g_pGfx;


// その場でPSOを作る
#define		ENABLE_ONTHEFLY_PSO

//GfxLib::DescriptorHeap  descHeap_CbvSrv;


LARGE_INTEGER			g_lastUpdateTime;
float					g_accTime = 0;


struct CBData
{
	DirectX::XMMATRIX	world;
	DirectX::XMMATRIX	view;
	DirectX::XMMATRIX	proj;
};


void Update();
void Render();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化しています。
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_D3D12TESTAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーションの初期化を実行します:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D12TESTAPP));


	g_pGfx = new GFX();

	if (!g_pGfx->Initialize()) {
		return FALSE;
	}

	QueryPerformanceCounter(&g_lastUpdateTime);

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}

	//	Executeなしで、確保が行われた時のテスト
	/*
	cmdList.Reset();
	cmdList.AllocateDescriptorBuffer(2);
	void *cpuAddress;
	cmdList.AllocateGpuBuffer(cpuAddress, 128, 16);
	*/

	g_pGfx->Finalize();

	delete g_pGfx;
	g_pGfx = nullptr;

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D12TESTAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_D3D12TESTAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);



   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウの描画
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
		{	
			int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = (int16_t)LOWORD(lParam);
			pt.y = (int16_t)HIWORD(lParam);

			if (g_pGfx) g_pGfx->OnLButtonDown(pt);

			SetCapture(hWnd);
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT pt;
			pt.x = (int16_t)LOWORD(lParam);
			pt.y = (int16_t)HIWORD(lParam);

			if (g_pGfx) g_pGfx->OnLButtonUp(pt);

			ReleaseCapture();
		}
		break;
	case WM_MOUSEMOVE:
		{
			POINT pt;
			pt.x = (int16_t)LOWORD(lParam);
			pt.y = (int16_t)HIWORD(lParam);

			if (g_pGfx) g_pGfx->OnMouseMove(pt);
		}
		break;
	case WM_MOUSEWHEEL:
		{
			g_pGfx->OnWheel(GET_WHEEL_DELTA_WPARAM(wParam));
		}
		break;
	case WM_KEYDOWN:
		{
			g_pGfx->OnKeyDown(wParam, lParam);
		}
		break;
	case WM_CLOSE:
		{

			return DefWindowProc(hWnd, message, wParam, lParam);
		}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void Update()
{
	g_pGfx->Update();
}


void Render()
{

	g_pGfx->Render();

}


bool	GFX::Initialize()
{


	if (!gfxCore.Initialize()) {
		return FALSE;
	}


	m_camAngX = 0;
	m_camAngY = 0;
	m_camCenter = { 0.f,0.f,0.f };
	m_MouseMove = false;
	m_camDist = 5;
	m_AppTime = 0;

	m_lastMouseDown = POINT{ 0,0 };

	// メイン メッセージ ループ:
	/*
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	{
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}
	}
	*/

	/*{
	D3D12_CPU_DESCRIPTOR_HANDLE	aHandle[32];

	GfxLib::DescriptorAllocator* allocator = GfxLib::CoreSystem::GetInstance()->GetDescriptorAllocator();


	for (uint32_t i = 0; i < _countof(aHandle); ++i) {

	aHandle[i] = allocator->Allocate(GfxLib::DescriptorHeapType::RTV);

	}


	std::random_shuffle(aHandle, aHandle + _countof(aHandle)-1);

	for (uint32_t i = 0; i < 8; ++i) {
	allocator->Free(GfxLib::DescriptorHeapType::RTV, aHandle[i]);
	}
	for (uint32_t i = 0; i < 8; ++i) {
	aHandle[i] = allocator->Allocate(GfxLib::DescriptorHeapType::RTV );
	}


	for (uint32_t i = 0; i < _countof(aHandle); ++i) {

	allocator->Free(GfxLib::DescriptorHeapType::RTV, aHandle[i]);

	}

	int i;
	i = 0;

	}*/

	cmdList.Initialize(&gfxCore.GetCommandQueue());
	swapChain.Initialize(g_hWnd);


#if 1
	/*
	http://www.project-asura.com/program/d3d12/d3d12_002.html
	を参考にしたテストコード
	*/
	{
		// Root Signature test
		GfxLib::RootSignatureDesc	rootSigDesc;


		//GfxLib::DESCRIPTOR_RANGE	range = { GfxLib::DescriptorRangeType::Cbv,1,0 };
		//rootSigDesc.AddParam_DescriptorTable(&range, 1);
		// 0 から始まる、一つのCBV
		GfxLib::DESCRIPTOR_RANGE aCbvSrvRange[] = {
			{ GfxLib::DescriptorRangeType::Cbv,1,0 },
			{ GfxLib::DescriptorRangeType::Srv,1,0 },
		};
		rootSigDesc.AddParam_DescriptorTable(aCbvSrvRange, _countof(aCbvSrvRange));
		rootSigDesc.AddParam_DescriptorTable(&GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Sampler,1,0 }, 1);

		rootSigDesc.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		/*
		rootSigDesc.AddParam_DescriptorTable(
		std::array<GfxLib::DESCRIPTOR_RANGE,2>{
		GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Cbv,1, 1 },
		//GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Uav,1, 1 },
		GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Sampler,1, 1 },
		//GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Sampler,1, 0 }
		}
		);
		//*/


		rootSig.Initialize(rootSigDesc);
	}

	{
		// サンプラのデスクリプタテーブル
		descHeap_Sampler.InitializeSampler(1, false);

		D3D12_SAMPLER_DESC	sampsDesc = {
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0.f,
			1,
			D3D12_COMPARISON_FUNC_ALWAYS,
			{ 0.f,0.f,0.f,0.f },
			0,
			13,
		};
		gfxCore.GetD3DDevice()->CreateSampler(&sampsDesc, descHeap_Sampler.GetCPUDescriptorHandleByIndex(0));


		//descHeap_CBV.InitializeCBV_SRV_UAV(10,false);
	}

	{
		// RenderTarget

		//GfxLib::RenderTarget renderTarget;

		//bool b = renderTarget.Initialize(GfxLib::Format::R8G8B8A8_UNORM, 128, 128, 1, true);


		depthStencil.Initialize(GfxLib::Format::D32_FLOAT, swapChain.GetWidth(), swapChain.GetHeight(), 1, true);


	}


#if 1

	{
		//	Create Vertex Buffer and index buffer
		struct Vertex {
			DirectX::XMFLOAT3	Position;
			DirectX::XMFLOAT3	Normal;
			DirectX::XMFLOAT2	TexCoord;
			DirectX::XMFLOAT4	Color;
		};

		/*
		Vertex vertices[] = {
			{ { 0.f,1.f,0.f },{ 0.f,0.f,-1.f },{ 0.5f,0.1f },{ 1.f,1.f,1.f,1.f }, },
			{ { 1.f,-1.f,0.f },{ 0.f,0.f,-1.f },{ 0.1f,0.9f },{ 1.f,1.f,1.f,1.f }, },
			{ { -1.f,-1.f,0.f },{ 0.f,0.f,-1.f },{ 0.9f,0.9f },{ 1.f,1.f,1.f,1.f }, },
		};
		*/

		Vertex vertices[] = {
			{ { 0.f,0.f,0.f },{ 0.f,0.f,-1.f },{ 0.f,0.f },{ 1.f,1.f,1.f,1.f }, },
			{ { 1.f,0.f,0.f },{ 0.f,0.f,-1.f },{ 1.f,0.f },{ 1.f,1.f,1.f,1.f }, },
			{ { 0.f,1.f,0.f },{ 0.f,0.f,-1.f },{ 0.f,1.f },{ 1.f,1.f,1.f,1.f }, },
			{ { 1.f,1.f,0.f },{ 0.f,0.f,-1.f },{ 1.f,1.f },{ 1.f,1.f,1.f,1.f }, },
		};


		vtxBuff.Initialize(vertices, sizeof(Vertex), _countof(vertices));


		uint16_t indices[] = {
			0,1,2,1,2,3
		};

		idxBuff.Initialize(indices, GfxLib::Format::R16_UINT , _countof(indices));


	}
#endif
	{
		// Create Texture2D


		/*
		bool b = texture2D.Initialize(GfxLib::Format::R8G8B8A8_UNORM, 128, 128, 1);


		if (b) {

		uint32_t  bits[128][128];


		for (uint32_t y = 0; y < _countof(bits); ++y) {
		for (uint32_t x = 0; x < _countof(bits[0]); ++x) {

		bits[y][x] = ( 0xff << 24 ) | ( 0xff << 16 ) | ( (x*255/128) << 8 ) | ( (y*255/128) );

		}
		}

		texture2D.WriteToSubresource(0, nullptr, bits, 4 * 128, 0);
			float3 origin = float3(
		lerp(g_rayGenCB.viewport.left, g_rayGenCB.viewport.right, lerpValues.x),
		lerp(g_rayGenCB.viewport.top, g_rayGenCB.viewport.bottom, lerpValues.y),
		0.0f);

		}

		*/

		D3D12_SUBRESOURCE_DATA	subData;


		uint32_t  bits[128][128];


		for (uint32_t y = 0; y < _countof(bits); ++y) {
			for (uint32_t x = 0; x < _countof(bits[0]); ++x) {

				bits[y][x] = (0xff << 24) | (0xff << 16) | ((x * 255 / 128) << 8) | ((y * 255 / 128));

			}
		}
		subData.pData = bits;
		subData.RowPitch = sizeof(bits[0]);
		subData.SlicePitch = 0;


		//bool b = texture2D.Initialize(GfxLib::Format::R8G8B8A8_UNORM, 128, 128, 1, 1, &subData);
		bool b = texture2D.InitializeFromFile(L"fire.dds");

		b = b;


	}


	{

		pixelshader.CreateFromFile(L"../x64/debug/SimplePS.cso");
		vertexshader.CreateFromFile(L"../x64/debug/SimpleVS.cso");


		/*
		constantBuffer.Initialize(sizeof(CBData));
		constantBuffer2.Initialize(sizeof(CBData));

		CBData cbdata;

		cbdata.world = XMMatrixIdentity();
		cbdata.view = XMMatrixTranspose( XMMatrixLookAtLH( XMVectorSet(0.f,0.f,5.f,0.f ) , XMVectorZero(), XMVectorSet(0.f,1.f,0.f,0.f) ) );
		cbdata.proj = XMMatrixTranspose( XMMatrixPerspectiveFovLH(3.14159f / 4.f, 1.f, 0.1f, 100.f) );

		constantBuffer.SetData(&cbdata, sizeof(cbdata));


		cbdata.world = XMMatrixTranspose( XMMatrixRotationY( XM_PI / 4 ) );

		constantBuffer2.SetData(&cbdata, sizeof(cbdata));
		*/
	}



	{
		// CBV - SRV  DescriptorTable

		//descHeap_CbvSrv.InitializeCBV_SRV_UAV(2);

		// 0番目にCBV , 1番目にSRV
		//gfxCore.GetD3DDevice()->CopyDescriptorsSimple(1, descHeap_CbvSrv.GetCPUDescriptorHandleByIndex(0), constantBuffer.GetCbvDescHandle() , D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		//gfxCore.GetD3DDevice()->CopyDescriptorsSimple(1, descHeap_CbvSrv.GetCPUDescriptorHandleByIndex(1), texture2D.GetSrvDescHandle() , D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );


	}
	{

		fontSystem.Initialize();
		fontData.Initialize(L"Media\\Texture\\Font.dds");
	}

#if 1
	{


		D3D12_INPUT_ELEMENT_DESC inputElement[] = {
			{ "POSITION",	0	,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
			{ "NORMAL",		0	,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
			{ "TEXCOORD",	0	,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
			{ "VTX_COLOR",	0	,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
		};


		GfxLib::InputLayout il;
		il.Initialize(_countof(inputElement), inputElement);



char elemstr[] = { "POSITION" };

D3D12_INPUT_ELEMENT_DESC inputElement2[] = {
	{ elemstr,	0	,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
	{ "NORMAL",		0	,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
	{ "TEXCOORD",	0	,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
	{ "VTX_COLOR",	0	,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
};

GfxLib::InputLayout il2;
il2.Initialize(_countof(inputElement2), inputElement2);



//	ラスタライザステートの設定
D3D12_RASTERIZER_DESC descRS = {};

descRS.FillMode = D3D12_FILL_MODE_SOLID;
descRS.CullMode = D3D12_CULL_MODE_NONE;
descRS.FrontCounterClockwise = FALSE;
descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
descRS.DepthClipEnable = TRUE;
descRS.MultisampleEnable = FALSE;
descRS.AntialiasedLineEnable = FALSE;
descRS.ForcedSampleCount = 0;
descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

// レンダーターゲットのブレンド設定
D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
	FALSE,	FALSE,
	D3D12_BLEND_ONE,	D3D12_BLEND_ZERO,	D3D12_BLEND_OP_ADD,
	D3D12_BLEND_ONE,	D3D12_BLEND_ZERO,	D3D12_BLEND_OP_ADD,
	D3D12_LOGIC_OP_NOOP,
	D3D12_COLOR_WRITE_ENABLE_ALL,
};

// ブレンドステートの設定
D3D12_BLEND_DESC descBS;
descBS.AlphaToCoverageEnable = FALSE;
descBS.IndependentBlendEnable = FALSE;
for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
	descBS.RenderTarget[i] = descRTBS;
};

// パイプラインステートの設定
D3D12_GRAPHICS_PIPELINE_STATE_DESC	desc = {};

desc.InputLayout = { inputElement,_countof(inputElement) };
desc.pRootSignature = rootSig.GetD3DRootSignature();
desc.VS = vertexshader.GetD3D12ShaderBytecode();	//	TODO
desc.PS = pixelshader.GetD3D12ShaderBytecode();
desc.RasterizerState = descRS;
desc.BlendState = descBS;
desc.DepthStencilState.DepthEnable = TRUE;
desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
desc.DepthStencilState.StencilEnable = FALSE;
desc.SampleMask = UINT_MAX;
desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
desc.NumRenderTargets = 1;
desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
desc.SampleDesc.Count = 1;


{
	D3D12_SAMPLER_DESC desc = GfxLib::Sampler::GetDefaultDesc();
	sampsLinear.Initialize(desc);
}


#ifndef ENABLE_ONTHEFLY_PSO
pipelineState.Initialize(desc);
#endif

//	OnTheFly PSO
depthStencilState.Initialize(desc.DepthStencilState);

blendState.Initialize(descBS);

// AlphaBlend
descRTBS.BlendEnable = true;
descRTBS.SrcBlend = D3D12_BLEND_SRC_ALPHA;
descRTBS.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

descBS.RenderTarget[0] = descRTBS;
blendState2.Initialize(descBS);


rasterizerState.Initialize(descRS);

inputLayout.Initialize(_countof(inputElement), inputElement);

	}
#endif

#endif


	{
	// RayTracing
#if 0
		m_rtShaderLib.CreateFromFile(L"Media/Shader/GfxRayTracing.cso");
		m_rtShaderLibModel.CreateFromFile(L"Media/Shader/GfxRayTracingModel.cso");
#endif

		float aspect = swapChain.GetHeight() / (float)swapChain.GetWidth();

		float border = 0.1f;

#if 0
		m_rayGenCB.viewport = { -1.0f,  aspect, 1.0f, -aspect };
		m_rayGenCB.stencil = { -1.0f+border, -1.0f+border, 1.0f-border, 1.0f-border };

		XMMATRIX mtxCamera = XMMatrixIdentity();
		mtxCamera = XMMatrixRotationRollPitchYaw(m_camAngY,m_camAngX,0.f);
		mtxCamera.r[3] = XMVectorSet(0.f,2.f,-5.f,0.f);

		m_rayGenCB.mtxCamera = XMMatrixTranspose(mtxCamera);
#endif

		m_RTRender.Initialize();


		m_TexContainer.Initialize();

		//CreateRayTracingRootSignature();
		
		//CreateRayTracingPipelineStateObject();

		CreateRayTracingGeometry();

		CreateRayTracingResources();

		BuildAccelerationStructures();

		CreateRayTracingOutputResource();
	}

	return true;

}


#if 0
void	GFX::CreateRayTracingRootSignature()
{


		GfxLib::RootSignatureDesc	rootSigDesc;


		rootSigDesc.AddParam_DescriptorTable(&GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Uav,1,0 }, 1);
		rootSigDesc.AddParam_Srv(0);
		rootSigDesc.AddParam_DescriptorTable(&GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Sampler,1,0 }, 1);
		rootSigDesc.AddParam_DescriptorTable(&GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Srv,3,1 }, 1);

		globalRootSig.Initialize(rootSigDesc);

		rootSigDesc.Clear();

		

		//rootSigDesc.AddParam_32BitConstants(sizeof(m_rayGenCB) / sizeof(uint32_t), 0);
		rootSigDesc.AddParam_Cbv(16);
		GfxLib::DESCRIPTOR_RANGE	ranges = {GfxLib::DescriptorRangeType::Srv , 1,16,1};
		rootSigDesc.AddParam_DescriptorTable(&ranges, 1);

		GfxLib::DESCRIPTOR_RANGE	ranges2 = { GfxLib::DescriptorRangeType::Srv , 1,16,2 };
		rootSigDesc.AddParam_DescriptorTable(&ranges2, 1);


		rootSigDesc.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

		localRootSig.Initialize(rootSigDesc);

		rootSigDesc.Clear();

		{
			GfxLib::DESCRIPTOR_RANGE	ranges = { GfxLib::DescriptorRangeType::Srv , 1,16,0 };
			rootSigDesc.AddParam_DescriptorTable(&ranges, 1);
		}
		rootSigDesc.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

		localRootSigMiss.Initialize(rootSigDesc);


		// RayGen
		rootSigDesc.Clear();

		rootSigDesc.AddParam_Cbv(0);
		rootSigDesc.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

		localRootSigRayGen.Initialize(rootSigDesc);
}
#endif


#if 0
void	GFX::CreateRayTracingPipelineStateObject()
{

	GfxLib::RtPipelineStateDesc	stateDesc;


	{
		auto *subobj = stateDesc.CreateSubObject<GfxLib::PipelineState_DxilLibrary>();

		D3D12_SHADER_BYTECODE dxillib = m_rtShaderLib.GetD3D12ShaderBytecode();
		subobj->SetDXILLibrary(dxillib);
		subobj->AddExport(c_raygenShaderName);
		for( auto s : c_missShaderName )		subobj->AddExport(s);
	}

	{
		auto* subobj = stateDesc.CreateSubObject<GfxLib::PipelineState_DxilLibrary>();

		D3D12_SHADER_BYTECODE dxillib = m_rtShaderLibModel.GetD3D12ShaderBytecode();
		subobj->SetDXILLibrary(dxillib);
		for (auto s : c_closestHitShaderName)	subobj->AddExport(s);
	}

	for ( uint32_t i = 0 ; i < TRACE_TYPE_NUM; ++i )
	{
		/*
			HitGroupはAnyHit、ClosestHit、ClosestHitで同じグループに入る
		*/

		auto* hitGroup = stateDesc.CreateSubObject<GfxLib::PipelineState_HitGroup>();

		hitGroup->SetClosestHitShaderImport(c_closestHitShaderName[i]);
		hitGroup->SetHitGroupExport(c_hitGroupName[i]);
		hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	}


	{
		auto* shaderConfig = stateDesc.CreateSubObject<GfxLib::PipelineState_RaytracingShaderConfig>();

		UINT payloadSize = 4 * sizeof(float);   // float4 color
		UINT attributeSize = 2 * sizeof(float); // float2 barycentrics
		shaderConfig->Config(payloadSize, attributeSize);
	}

	{
		//	LocalRootSignature Associate

		auto localRootSignature = stateDesc.CreateSubObject<GfxLib::PipelineState_LocalRootSignature>();
		localRootSignature->SetRootSignature(localRootSig.GetD3DRootSignature());
		// Shader association
		auto rootSignatureAssociation = stateDesc.CreateSubObject<GfxLib::PipelineState_SubobjectToExportsAssociation>();
		rootSignatureAssociation->SetRootSignature(localRootSignature);
		for(auto s : c_closestHitShaderName ) rootSignatureAssociation->AddExport(s);
	}

	{
		//	LocalRootSignature Associate [raygen]

		auto localRootSignature = stateDesc.CreateSubObject<GfxLib::PipelineState_LocalRootSignature>();
		localRootSignature->SetRootSignature(localRootSigRayGen.GetD3DRootSignature());
		// Shader association
		auto rootSignatureAssociation = stateDesc.CreateSubObject<GfxLib::PipelineState_SubobjectToExportsAssociation>();
		rootSignatureAssociation->SetRootSignature(localRootSignature);
		rootSignatureAssociation->AddExport(c_raygenShaderName);
	}


	{
		//	Local Root Signature for [miss]

		auto localRootSignature = stateDesc.CreateSubObject<GfxLib::PipelineState_LocalRootSignature>();
		localRootSignature->SetRootSignature(localRootSigMiss.GetD3DRootSignature());
		// Shader association
		auto rootSignatureAssociation = stateDesc.CreateSubObject<GfxLib::PipelineState_SubobjectToExportsAssociation>();
		rootSignatureAssociation->SetRootSignature(localRootSignature);
		for (auto s : c_missShaderName ) rootSignatureAssociation->AddExport(s);
	}

	{
		//	Global Root Signature

		auto globalRootSignature = stateDesc.CreateSubObject<GfxLib::PipelineState_GlobalRootSignature>();
		globalRootSignature->SetRootSignature(globalRootSig.GetD3DRootSignature());
	}

	{
		// Pipeline Config
		auto pipelineConfig = stateDesc.CreateSubObject<GfxLib::PipelineState_PipelineConfig>();
		// PERFOMANCE TIP: Set max recursion depth as low as needed 
		// as drivers may apply optimization strategies for low recursion depths. 
		UINT maxRecursionDepth = Config::MaxRecursionDepth; // ~ primary rays only. 
		pipelineConfig->Config(maxRecursionDepth);

	}

	{
		//	


		stateDesc.Resolve();


		D3D12_STATE_OBJECT_DESC desc = {};

		desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
		desc.pSubobjects = stateDesc.GetStateSubObjects();
		desc.NumSubobjects = stateDesc.GetNumStateSubObjects();


		m_rtStateObject.Initialize(desc);


	}

}
#endif




void	GFX::CreateRayTracingGeometry()
{


	GfxLib::InterModelData interModelData;

	//std::wstring objfilepath = L"Media/Model/cube/cube.obj";
	float scale = 1.f;
	//std::wstring objfilepath = L"Media/Model/bunny/bunny.obj";
	//std::wstring objfilepath = L"Media/Model/bmw/bmw.obj";
	//std::wstring objfilepath = L"Media/Model/teapot/teapot.obj";
	//std::wstring objfilepath = L"Media/Model/sibenik/sibenik.obj";
	std::wstring objfilepath = L"Media/Model/sponza/sponza.obj";
	//std::wstring objfilepath = L"Media/Model/CornellBox/CornellBox-Sphere.obj";
	//std::wstring objfilepath = L"Media/Model/CornellBox/CornellBox-Original.obj";
	//std::wstring objfilepath = L"Media/Model/CornellBox/CornellBox-Water.obj";
	//std::wstring objfilepath = L"Media/Model/CornellBox/CornellBox-Glossy-Floor.obj";
	//float scale = 2.f;
	//std::wstring objfilepath = L"Media/Model/fireplace_room/fireplace_room.obj";




	//if (!interModelData.InitializeFromObjFile(L"Media/Model/cube/cube.obj")) {
	//if (!interModelData.InitializeFromObjFile(L"Media/Model/cube/cube2.obj")) {
	//if (!interModelData.InitializeFromObjFile(L"Media/Model/teapot/teapot.obj", 0.01f)) {
	//if (!interModelData.InitializeFromObjFile(L"Media/Model/bunny/bunny.obj", 2.f)) {
	//if (!interModelData.InitializeFromObjFile(L"Media/Model/bmw/bmw.obj", 0.01f)) {
	//if (!interModelData.InitializeFromObjFile(L"Media/Model/cube/cube.obj", 1.f)) {

	if (!interModelData.InitializeFromObjFile(objfilepath.c_str(),scale)) {

		// ?

		return;

	}




	uint32_t filenameidx = FindFileName(objfilepath.c_str());

	// .mtlのパスに変換する
	std::wstring objbasepath(objfilepath.c_str(), filenameidx);
	

	m_TexContainer.SetBasePath(objbasepath.c_str());

	m_rtModel.Initialize(interModelData, m_TexContainer);

#if 0

	//const float scale = 0.01f;	//	bmw
	//const float scale = 2.f;	//	bunny
	//const float scale = 0.01f;	//	teapot
	const float scale = 1.f;

	const auto& triangles = interModelData.GetSubMeshes().at(0)->GetTriangle();
	const auto& vertices = interModelData.GetVertex();



	uint32_t indices_size = (uint32_t)triangles.size() * 3;

	uint32_t vertices_size = (uint32_t)vertices.size();
	RtVertex* positions = new RtVertex[vertices_size];
	RtAttrib* attribs = new RtAttrib[vertices_size];



	uint32_t i = 0;


	i = 0;
	for (auto& vert : vertices) {

		RtVertex &vtx = positions[i];
		RtAttrib& attr = attribs[i];

		vtx.v1 = vert.pos.x * scale;
		vtx.v2 = vert.pos.y * scale;
		vtx.v3 = vert.pos.z * scale;


		attr.BaseColor = XMFLOAT3{ 1,1,1 };
		attr.Normal = vert.norm;
		attr.Uv = XMFLOAT2{ vert.uv.x,vert.uv.y };


		++i;

	}


	m_rtGeomAttrib.InitializeImmutable(attribs, sizeof(RtAttrib), vertices_size, GfxLib::ResourceStates::ShaderResource);


	if (vertices_size <= 0xffff) {
		uint16_t* indices = new uint16_t[indices_size];
		int32_t i = 0;
		for (auto& tri : triangles) {

			indices[i++] = tri.v0;
			indices[i++] = tri.v1;
			indices[i++] = tri.v2;

		}
		m_rtGeometry.Initialize(positions, sizeof(RtVertex), vertices_size, indices, GfxLib::Format::R16_UINT, indices_size);
		delete[] indices;
	} else {

		uint32_t* indices = new uint32_t[indices_size];
		int32_t i = 0;
		for (auto& tri : triangles) {

			indices[i++] = tri.v0;
			indices[i++] = tri.v1;
			indices[i++] = tri.v2;

		}
		m_rtGeometry.Initialize(positions, sizeof(RtVertex), vertices_size, indices, GfxLib::Format::R32_UINT, indices_size);
		delete[] indices;
	}

	delete[] positions;
	delete[] attribs;
#endif

#if 0
	uint16_t indices[] =
	{
		0, 1, 2,
		2, 1, 3,

		4,8,6,
		5,8,4,
		7,8,5,
		6,8,7,
	};

	float depthValue = 1.0;
	float offset = 0.7f;
	RtVertex vertices[] =
	{
		// The sample raytraces in screen space coordinates.
		// Since DirectX screen space coordinates are right handed (i.e. Y axis points down).
		// Define the vertices in counter clockwise order ~ clockwise in left handed.
		/*
		{ 0, -offset, depthValue },
		{ -offset, offset, depthValue },
		{ offset, offset, depthValue },
		*/


		{	-10.f,0.f, 10.f	},
		{	 10.f,0.f, 10.f	},
		{	-10.f,0.f,-10.f	},
		{	 10.f,0.f,-10.f	},

		{	-1.f,0.f, 1.f	},
		{	 1.f,0.f, 1.f	},
		{	-1.f,0.f,-1.f	},
		{	 1.f,0.f,-1.f	},

		{	 0.f,1.f,0.f	},
	};

	m_rtGeometry.Initialize(vertices, sizeof(RtVertex), _countof(vertices), indices , GfxLib::Format::R16_UINT, _countof(indices));



	RtAttrib	vtxAttrib[] =
	{

		{	{0.f,1.f,0.f},	{1.f,1.f,1.f},	{},	},
		{	{0.f,1.f,0.f},	{1.f,1.f,1.f},	{},	},
		{	{0.f,1.f,0.f},	{1.f,1.f,1.f},	{},	},
		{	{0.f,1.f,0.f},	{1.f,1.f,1.f},	{},	},

		{	{-0.57735f,0.57735f, 0.57735f},	{1.f,1.f,0.f},	{},	},
		{	{ 0.57735f,0.57735f, 0.57735f},	{0.f,1.f,1.f},	{},	},
		{	{-0.57735f,0.57735f,-0.57735f},	{0.f,0.3f,0.5f},{},	},
		{	{ 0.57735f,0.57735f,-0.57735f},	{0.8f,0.f,0.7f},{},	},

		{	{0.f,1.f,0.f},	{1.f,0.f,0.f},	{},	},


	};


	m_rtGeomAttrib.InitializeImmutable(vtxAttrib,sizeof(RtAttrib), _countof(vtxAttrib), GfxLib::ResourceStates::ShaderResource);
#endif
}


void	GFX::CreateRayTracingResources()
{

	m_texSky.InitializeFromFile(L"Media\\Texture\\sky_bg.dds");


	DXGI_FORMAT fmt = m_texSky.GetFormat();

	m_texSkyRem.InitializeFromFile(L"Media\\Texture\\sky_rem.dds");
	m_texSkyIem.InitializeFromFile(L"Media\\Texture\\sky_iem.dds");


}


void	GFX::BuildAccelerationStructures()
{

	//m_rtBLAS.Initialize(&m_rtGeometry, 1);
	m_rtBLAS.Initialize( &m_rtModel.GetGeomDescs().at(0), m_rtModel.GetGeomDescs().size());

	GfxLib::BottomLevelAccelerationStructure* tlas[] = {
		&m_rtBLAS,
	};
	m_rtTLAS.Initialize(tlas, _countof(tlas));


	m_rtBLAS.SetTransform(XMMatrixRotationY(XM_PI));


	size_t scratchsize = std::max( m_rtBLAS.GetScratchDataSizeInBytes(), m_rtTLAS.GetScratchDataSizeInBytes() );


	m_rtScratch.Initialize(scratchsize,D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS , D3D12_RESOURCE_STATE_UNORDERED_ACCESS );

	auto* coreSystem = GfxLib::CoreSystem::GetInstance();
	auto* cmdList = coreSystem->GetResourceInitCommandList();


	//*
	// ASのビルド
	m_rtBLAS.Build(*cmdList, &m_rtScratch);

	cmdList->ResourceUavBarrier(&m_rtScratch);

	m_rtTLAS.Build(*cmdList,&m_rtScratch);


	coreSystem->FlushResourceInitCommandList();
	coreSystem->WaitGpuFinish();
	//*/

}



void	GFX::CreateRayTracingOutputResource()
{

	//m_rtOutput.InitializeUAV(GfxLib::Format::R8G8B8A8_UNORM,swapChain.GetWidth(),swapChain.GetHeight(),1,GfxLib::ResourceStates::ShaderResource);


	GfxLib::RtSceneTargets::INITIALIZE_PARAM	initParam = { swapChain.GetWidth(),swapChain.GetHeight() };

	m_rtTarget.Initialize(initParam);


}


void	GFX::Finalize()
{


	gfxCore.WaitGpuFinish();

	fontData.Finalize();
	fontSystem.Finalize();

	swapChain.Finalize();
	cmdList.Finalize();
	//cmdList2.Finalize();
	rootSig.Finalize();
	idxBuff.Finalize();
	vtxBuff.Finalize();
	pipelineState.Finalize();
	pixelshader.Finalize();
	vertexshader.Finalize();
	//constantBuffer2.Finalize();
	//constantBuffer.Finalize();
	depthStencil.Finalize();
	texture2D.Finalize();
	descHeap_Sampler.Finalize();
	//descHeap_CBV.Finalize();
	depthStencilState.Finalize();
	rasterizerState.Finalize();
	blendState.Finalize();
	blendState2.Finalize();
	inputLayout.Finalize();
	sampsLinear.Finalize();


	m_RTRender.Finalize();
	m_TexContainer.Finalize();

#if 0
	globalRootSig.Finalize();
	localRootSig.Finalize();
	localRootSigRayGen.Finalize();
	localRootSigMiss.Finalize();
	m_rtShaderLib.Finalize();
	m_rtShaderLibModel.Finalize();
	m_rtStateObject.Finalize();
#endif
	//m_rtGeometry.Finalize();
	m_rtModel.Finalize();
	m_rtTarget.Finalize();
	m_rtBLAS.Finalize();
	m_rtTLAS.Finalize();
	m_rtScratch.Finalize();
	//m_rtGeomAttrib.Finalize();
	m_texSky.Finalize();
	m_texSkyRem.Finalize();
	m_texSkyIem.Finalize();


	gfxCore.Finalize();


	return ;
}


void	GFX::Update()
{

	LARGE_INTEGER nowTime;
	QueryPerformanceCounter(&nowTime);

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER deltaTime;
	deltaTime.QuadPart = nowTime.QuadPart - g_lastUpdateTime.QuadPart;


	float deltaTimeInSec = float(deltaTime.QuadPart / (double)freq.QuadPart);

	g_lastUpdateTime = nowTime;


	g_accTime += deltaTimeInSec;


	m_AppTime += deltaTimeInSec;


	gfxCore.Update();


	{ // Camera Update

		XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(m_camAngY, m_camAngX, 0.f);

		BYTE KeyState[256] = {};
		GetKeyboardState(KeyState);

		float speed = m_camDist*0.01f;

		GfxLib::Float3 Forward = mtxRotate.r[2];
		GfxLib::Float3 Right = mtxRotate.r[0];
		GfxLib::Float3 Up = { 0.f,1.f,0.f };

		//	Forward

		if (KeyState['W'] & 0xf0) {

			m_camCenter += Forward* speed;

		}

		if (KeyState['S'] & 0xf0) {

			m_camCenter -= Forward * speed;

		}


		if (KeyState['D'] & 0xf0) {

			m_camCenter += Right * speed;

		}

		if (KeyState['A'] & 0xf0) {

			m_camCenter -= Right * speed;

		}


		if (KeyState[VK_UP] & 0xf0) {

			m_camCenter += Up * speed;

		}

		if (KeyState[VK_DOWN] & 0xf0) {

			m_camCenter -= Up * speed;

		}

	}






}

void	GFX::Render()
{


	if (gfxCore.Begin()) {


		cmdList.Reset();

		//cmdList2.Reset();

		ID3D12GraphicsCommandList *d3dCmdList = cmdList.GetD3DCommandList();
		{
			swapChain.Begin(cmdList);
			//


			cmdList.ReserveDescriptorBuffers(32, 32);


			//auto handle = swapChain.GetCurrentRenderTargetHandle();
			//d3dCmdList->OMSetRenderTargets(1, &handle, false, &depthStencil.GetDSVDescriptorHandle() );	//	MEMO 3つめの引数で、連続したハンドルヒープを使うことが可能
			GfxLib::RenderTarget *rt = swapChain.GetCurrentRenderTarget();
			cmdList.OMSetRenderTargets(1, &rt, &depthStencil);

			D3D12_VIEWPORT vp = {};
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			vp.MinDepth = 0.f;
			vp.MaxDepth = 1.f;
			vp.Width = (FLOAT)swapChain.GetWidth();
			vp.Height = (FLOAT)swapChain.GetHeight();

			d3dCmdList->RSSetViewports(1, &vp);

			D3D12_RECT rect = { 0,0, (LONG)swapChain.GetWidth(),(LONG)swapChain.GetHeight() };
			d3dCmdList->RSSetScissorRects(1, &rect);

			// RenderTargetおよび、DepthStencilのクリア
			const float clearColorF[] = { 0.f , 1.f , 0.5f , 1.f };
			cmdList.ClearRenderTargetView(*rt, clearColorF, 0, nullptr);
			cmdList.ClearDepthStencilView(depthStencil, D3D12_CLEAR_FLAG_DEPTH, GFX_DEFAULT_DEPTH_CLEAR_VALUE, 0, 0, nullptr);

			/*
			uint32_t descHeapStartIndex = 0;
			GfxLib::DescriptorHeap *cbvsrvHeap = gfxCore.RequireAdhocDescriptorHeap(2, descHeapStartIndex );

			// CopyHandle
			// todo: Copy付きRequire
			gfxCore.GetD3DDevice()->CopyDescriptorsSimple(1,
			cbvsrvHeap->GetCPUDescriptorHandleByIndex(descHeapStartIndex),
			constantBuffer.GetCbvDescHandle(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			gfxCore.GetD3DDevice()->CopyDescriptorsSimple(1,
			cbvsrvHeap->GetCPUDescriptorHandleByIndex(descHeapStartIndex+1),
			texture2D.GetSrvDescHandle(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			*/
			uint32_t descHeapStartIndex = 0;

			GfxLib::DescriptorBuffer descBuff = cmdList.AllocateDescriptorBuffer(2);

			{


				CBData cbdata;

				cbdata.world = XMMatrixIdentity();
				cbdata.view = XMMatrixTranspose(XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, m_camDist, 0.f), XMVectorZero(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
				cbdata.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(3.14159f / 4.f, 1.f, 0.1f, 100.f));


				descBuff.SetConstantBuffer(0, &cbdata, sizeof(CBData));
			}



			//descBuff.CopyHandle(0, constantBuffer.GetCbvDescHandle());
			descBuff.CopyHandle(1, texture2D.GetSrvDescHandle());


			GfxLib::DescriptorBuffer sampsBuff = cmdList.AllocateDescriptorBuffer_Sampler(1);

			sampsBuff.CopyHandle(0, descHeap_Sampler.GetCPUDescriptorHandleByIndex(0));

			/*
			// Rootsignatureおよび、DescriptorHeap対応
			ID3D12DescriptorHeap *heap[] = {
				//cbvsrvHeap->GetD3DDescriptorHeap() ,
				descBuff.GetD3DDescriptorHeap(),
				sampsBuff.GetD3DDescriptorHeap() ,
			};
			*/
			//d3dCmdList->SetDescriptorHeaps(1, &heap );

			//d3dCmdList->SetGraphicsRootSignature(rootSig.GetD3DRootSignature());
			cmdList.SetRootSignature(&rootSig);

			// デスクリプタテーブルを設定する
			//d3dCmdList->SetDescriptorHeaps(_countof(heap), heap);

			//d3dCmdList->SetGraphicsRootDescriptorTable(0, descBuff.GetGPUDescriptorHandle());
			//d3dCmdList->SetGraphicsRootDescriptorTable(1, sampsBuff.GetGPUDescriptorHandle());
			cmdList.SetGraphicsRootDescriptorTable(0, descBuff);
			cmdList.SetGraphicsRootDescriptorTable(1, sampsBuff);

			//
			//d3dCmdList->SetGraphicsRootShaderResourceView(2, texture2D.GetD3DResource()->GetGPUVirtualAddress());

#ifdef ENABLE_ONTHEFLY_PSO


			cmdList.OMSetDepthStencilState(&depthStencilState);
			cmdList.OMSetBlendState(&blendState);
			cmdList.RSSetState(&rasterizerState);
			cmdList.IASetInputLayout(&inputLayout);
			cmdList.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

			cmdList.PSSetShader(&pixelshader);
			cmdList.VSSetShader(&vertexshader);

			cmdList.FlushPipeline();


#else

			d3dCmdList->SetPipelineState(pipelineState.GetD3DPipelineState());


#endif

			// 頂点バッファの設定、プリミティブトポロジの設定
			cmdList.IASetVertexBuffers(0, 1, &vtxBuff.GetVertexBufferView());
			cmdList.IASetIndexBuffer(&idxBuff.GetIndexBufferView());
			cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//d3dCmdList->DrawInstanced(3, 1, 0, 0);


			// 2回目の描画

			{

				uint32_t descHeapStartIndex = 0;

				//D3D12_CPU_DESCRIPTOR_HANDLE aHandle[2] = { constantBuffer2.GetCbvDescHandle(), texture2D.GetSrvDescHandle() };
				GfxLib::DescriptorBuffer descBuff = cmdList.AllocateDescriptorBuffer(2);

				//DescBuff.CopyHandle(0, aHandle, 2);



				{

					CBData cbdata;

					/*
					cbdata.world = XMMatrixTranspose(XMMatrixRotationY(XM_PI / 4 * g_accTime));
					cbdata.view = XMMatrixTranspose(XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 5.f, 0.f), XMVectorZero(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
					cbdata.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(3.14159f / 4.f, 1.f, 0.1f, 100.f));
					*/

					cbdata.world = XMMatrixIdentity();
					cbdata.view = XMMatrixIdentity();
					//cbdata.proj = XMMatrixIdentity();
					cbdata.proj = XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0.f,1.f,1.f,0.f,0.f,1.f));


					descBuff.SetConstantBuffer(0, &cbdata, sizeof(cbdata));
				}

				// Raytracing Output
				//descBuff.CopyHandle(1, texture2D.GetSrvDescHandle());
				descBuff.CopyHandle(1, m_rtTarget.GetResultSrv());

				GfxLib::DescriptorBuffer sampsBuff = cmdList.AllocateDescriptorBuffer_Sampler(1);
				sampsBuff.CopyHandle(0, descHeap_Sampler.GetCPUDescriptorHandleByIndex(0));

				/*
				ID3D12DescriptorHeap *heap[] = {
					descBuff.GetD3DDescriptorHeap() ,
					sampsBuff.GetD3DDescriptorHeap() ,
				};
				*/

				//d3dCmdList->SetDescriptorHeaps(_countof(heap), heap);

				//d3dCmdList->SetGraphicsRootDescriptorTable(0, descBuff.GetGPUDescriptorHandle());
				//d3dCmdList->SetGraphicsRootDescriptorTable(1, sampsBuff.GetGPUDescriptorHandle());

				cmdList.SetGraphicsRootDescriptorTable(0, descBuff);
				cmdList.SetGraphicsRootDescriptorTable(1, sampsBuff);


				cmdList.OMSetBlendState(&blendState);
				//cmdList.FlushPipeline();

				cmdList.DrawIndexedInstanced(6, 1, 0, 0, 0);

			}


			{
				//	TrayTracing


				DoRayTracing(cmdList);


			}


			{
				GfxLib::FontRenderer fontRender(&cmdList, &fontSystem, &fontData, vp);


				// Todo : フォント内部の処理とする
				GfxLib::DescriptorBuffer descBuff = cmdList.AllocateDescriptorBuffer(2);
				descBuff.CopyHandle(1, fontData.GetTexture().GetSrvDescHandle());


				cmdList.SetGraphicsRootDescriptorTable(0, descBuff);


				fontRender.Begin();

				fontRender.DrawFormatText(0, 0, L"ABCDEF");

				fontRender.End();

			}

			/*
			void *cpuAddr;
			D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = cmdList.AllocateGpuBuffer(cpuAddr, 32, 256);
			gpuAddr = cmdList.AllocateGpuBuffer(cpuAddr, 32, 256);
			gpuAddr = cmdList.AllocateGpuBuffer(cpuAddr, 4 * 1024 * 1024 - 512, 256);
			gpuAddr = cmdList.AllocateGpuBuffer(cpuAddr, 4 * 1024 * 1024, 256);
			*/

			swapChain.End(cmdList);
		}

		// コマンドの書き込みを終了する
		//d3dCmdList->Close();

		//cmdList2.GetD3DCommandList()->Close();

		// コマンドリストをキューイング
		//ID3D12CommandList *c = d3dCmdList;
		//gfxCore.GetCommandQueue().GetD3DCommandQueue()->ExecuteCommandLists(1, &c);
		GfxLib::CommandList *c = &cmdList;
		gfxCore.GetCommandQueue().ExecuteCommandLists(1, &c);

		//	プリゼンテーション
		swapChain.Present(1, 0);


		gfxCore.End();
	}


}



void	GFX::DoRayTracing(GfxLib::GraphicsCommandList& cmdList)
{

	//cmdList.ResourceTransitionBarrier(&m_rtOutput, GfxLib::ResourceStates::ShaderResource, GfxLib::ResourceStates::UnorderedAccess);


	GfxLib::RayTracing::SceneInfo sceneInfo = {};


	XMMATRIX mtxCamera = XMMatrixIdentity();
	//mtxCamera = XMMatrixRotationRollPitchYaw(m_camAngY, m_camAngX, 0.f);
	mtxCamera.r[3] = XMVectorSet(0.f, 0.f, -m_camDist, 1.f);

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(m_camAngY, m_camAngX, 0.f);

	mtxCamera = XMMatrixMultiply(mtxCamera, mtxRotate);

	mtxCamera.r[3] += m_camCenter.ToXMVECTOR(0.f);

	sceneInfo.mtxCamera = mtxCamera;


	sceneInfo.texSky = &m_texSky;
	sceneInfo.texSkyRem = &m_texSkyRem;
	sceneInfo.texSkyIem = &m_texSkyIem;



	float aspect = swapChain.GetHeight() / (float)swapChain.GetWidth();
	sceneInfo.vp = { -1.0f,  aspect, 1.0f, -aspect };

	sceneInfo.Width = swapChain.GetWidth();
	sceneInfo.Height = swapChain.GetHeight();


	sceneInfo.BLAS = &m_rtBLAS;
	sceneInfo.TLAS = &m_rtTLAS;


	sceneInfo.rtModel = &m_rtModel;

	sceneInfo.globalTime = m_AppTime;

	m_RTRender.Render(cmdList, m_rtTarget, sceneInfo);


#if 0
	const void* rayGenShaderIdentifier = m_rtStateObject.GetShaderIdentifier(c_raygenShaderName);
	const void* missShaderIdentifier0 = m_rtStateObject.GetShaderIdentifier(c_missShaderName[0]);
	const void* missShaderIdentifier1 = m_rtStateObject.GetShaderIdentifier(c_missShaderName[1]);

	const uint32_t shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	const uint32_t instanceCount = 1;


	XMMATRIX mtxCamera = XMMatrixIdentity();
	//mtxCamera = XMMatrixRotationRollPitchYaw(m_camAngY, m_camAngX, 0.f);
	mtxCamera.r[3] = XMVectorSet(0.f, 0.f, -5.f, 0.f);

	XMMATRIX mtxRotate  = XMMatrixRotationRollPitchYaw(m_camAngY, m_camAngX, 0.f);

	mtxCamera = XMMatrixMultiply(mtxCamera, mtxRotate);

	m_rayGenCB.mtxCamera = XMMatrixTranspose(mtxCamera);

	//  CommandList上に、シェーダテーブルを構築する

	struct RootArguments {
		//RayGenConstantBuffer cb;
		D3D12_GPU_VIRTUAL_ADDRESS	cb;			//	RayGenConstantBuffer
		D3D12_GPU_DESCRIPTOR_HANDLE	indexSRV;	//	IndexSRV	
		D3D12_GPU_DESCRIPTOR_HANDLE	vtxSRV;		//	VtxSRV
	} rootArguments;

	struct RootArguments_RayGen {
		//RayGenConstantBuffer cb;
		D3D12_GPU_VIRTUAL_ADDRESS	cb;			//	RayGenConstantBuffer
	} rootArguments_RayGen;

	struct RootArguments_Miss {
		//RayGenConstantBuffer cb;
		D3D12_GPU_DESCRIPTOR_HANDLE	srv;	//	IndexSRV	
	} rootArguments_Miss;


	//rootArguments.cb = m_rayGenCB;

	// LocalRootArguments
#if 0
	{

		ModelConstantBuffer  modelCB = {};

		modelCB.isIndex16bit = m_rtGeometry.GetIndexFormat() == GfxLib::Format::R16_UINT;

		GfxLib::GpuBufferRange cbBuffer = cmdList.AllocateGpuBuffer(sizeof(modelCB), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		memcpy(cbBuffer.GetCpuAddr(), &modelCB, sizeof(modelCB));
		rootArguments.cb = cbBuffer.GetGpuAddr();

		// LocalRootSignatureだから配列じゃなくてよいか？
		GfxLib::DescriptorBuffer	indexSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		indexSrvDesc.CopyHandle(0, m_rtModel.GetIndexBufferSRV());
		rootArguments.indexSRV = indexSrvDesc.GetGPUDescriptorHandle();

		GfxLib::DescriptorBuffer	vtxSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		vtxSrvDesc.CopyHandle(0, m_rtModel.GetGeomAttributeSRV());
		rootArguments.vtxSRV = vtxSrvDesc.GetGPUDescriptorHandle();
	}
#endif

	// LocalRootArguments for RayGen
	{
		GfxLib::GpuBufferRange cbBuffer = cmdList.AllocateGpuBuffer(sizeof(m_rayGenCB), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		memcpy(cbBuffer.GetCpuAddr(), &m_rayGenCB, sizeof(m_rayGenCB));
		rootArguments_RayGen.cb = cbBuffer.GetGpuAddr();




	}

	// LocalRootArguments for miss
	{

		GfxLib::DescriptorBuffer	vtxSrvDesc = cmdList.AllocateDescriptorBuffer(1);
		vtxSrvDesc.CopyHandle(0, m_texSky.GetSrvDescHandle());
		rootArguments_Miss.srv = vtxSrvDesc.GetGPUDescriptorHandle();

	}

	GfxLib::ShaderTable  rayGenShaderTable(cmdList, 1, shaderIdentifierSize+sizeof(rootArguments_RayGen));
	rayGenShaderTable.AddRecord(rayGenShaderIdentifier,&rootArguments_RayGen,sizeof(rootArguments_RayGen));

	GfxLib::ShaderTable  missShaderTable(cmdList, 2, shaderIdentifierSize+sizeof(RootArguments_Miss));
	missShaderTable.AddRecord(missShaderIdentifier0, &rootArguments_Miss, sizeof(rootArguments_Miss));
	missShaderTable.AddRecord(missShaderIdentifier1, &rootArguments_Miss, sizeof(rootArguments_Miss));


	auto& subGroupVec = m_rtModel.GetSubGroups();
	GfxLib::ShaderTable  hitGroupShaderTable(cmdList, TRACE_TYPE_NUM* (uint32_t)subGroupVec.size(), shaderIdentifierSize + sizeof(RootArguments));

	{

		// LocalRootSignatureだから配列じゃなくてよいか？

		// モデル全体のパラメータ


		#if 0
		GfxLib::DescriptorBuffer	indexSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		indexSrvDesc.CopyHandle(0, m_rtGeometry.GetIndexBufferSRV());
		rootArguments.indexSRV = indexSrvDesc.GetGPUDescriptorHandle();

		GfxLib::DescriptorBuffer	vtxSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		vtxSrvDesc.CopyHandle(0, m_rtGeomAttrib.GetSrvDescHandle());
		rootArguments.vtxSRV = vtxSrvDesc.GetGPUDescriptorHandle();

#endif

		GfxLib::DescriptorBuffer	indexSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		indexSrvDesc.CopyHandle(0, m_rtModel.GetIndexBufferSRV());
		rootArguments.indexSRV = indexSrvDesc.GetGPUDescriptorHandle();

		GfxLib::DescriptorBuffer	vtxSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		vtxSrvDesc.CopyHandle(0, m_rtModel.GetGeomAttributeSRV());
		rootArguments.vtxSRV = vtxSrvDesc.GetGPUDescriptorHandle();

		for (auto subgroup : subGroupVec) {
			//RootArguments rootArgMaterial = rootArguments;

			// マテリアル毎のパラメータ

			ModelConstantBuffer  modelCB = {};

			modelCB.isIndex16bit = m_rtModel.GetIndexFormat() == GfxLib::Format::R16_UINT;
			modelCB.primitiveOffset = subgroup->GetStartIndex() / 3;

			GfxLib::GpuBufferRange cbBuffer = cmdList.AllocateGpuBuffer(sizeof(modelCB), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
			memcpy(cbBuffer.GetCpuAddr(), &modelCB, sizeof(modelCB));
			rootArguments.cb = cbBuffer.GetGpuAddr();


			for (uint32_t i = 0; i < TRACE_TYPE_NUM; ++i) {
				
				// TraceType毎にLocalRootSignatureは同じにしている（別でもよい）

				const void* hitGroupShaderIdentifier = m_rtStateObject.GetShaderIdentifier(c_hitGroupName[i]);
				hitGroupShaderTable.AddRecord(hitGroupShaderIdentifier, &rootArguments, sizeof(RootArguments));
			}
		}
	}


	auto DispatchRays = [&](auto* commandList, auto* stateObject, D3D12_DISPATCH_RAYS_DESC* dispatchDesc)
	{
		// Since each shader table has only one shader record, the stride is same as the size.

		dispatchDesc->HitGroupTable = hitGroupShaderTable.GetGpuVirtualAddressRangeAndStride();
		dispatchDesc->MissShaderTable = missShaderTable.GetGpuVirtualAddressRangeAndStride();
		dispatchDesc->RayGenerationShaderRecord = rayGenShaderTable.GetGpuVirtualAddressRange();

		dispatchDesc->Width = swapChain.GetWidth();
		dispatchDesc->Height = swapChain.GetHeight();
		dispatchDesc->Depth = 1;
		commandList->SetPipelineState1(stateObject);
		commandList->DispatchRays(dispatchDesc);
	};


	auto* commandList = cmdList.GetD3DCommandList4();
	commandList->SetComputeRootSignature(globalRootSig.GetD3DRootSignature());
	
	cmdList.ReserveDescriptorBuffers(2, 0);


	/*
	GfxLib::DescriptorBuffer  descBuff = cmdList.AllocateDescriptorBuffer(1);
	commandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, m_raytracingOutputResourceUAVGpuDescriptor);
	commandList->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot, m_topLevelAccelerationStructure->GetGPUVirtualAddress());

	descBuff.CopyHandle(0, m_rtOutput.GetUavDescHandle());
	*/

	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};

	GfxLib::DescriptorBuffer db1 = cmdList.AllocateDescriptorBuffer(1);
	db1.CopyHandle(0, m_rtOutput.GetUavDescHandle());

	GfxLib::DescriptorBuffer db2 = cmdList.AllocateDescriptorBuffer_Sampler(1);
	db2.CopyHandle(0, sampsLinear.GetDescHandle());


	GfxLib::DescriptorBuffer db3 = cmdList.AllocateDescriptorBuffer(3);
	db3.CopyHandle(0, m_texSky.GetSrvDescHandle());
	db3.CopyHandle(1, m_texSkyRem.GetSrvDescHandle());
	db3.CopyHandle(2, m_texSkyIem.GetSrvDescHandle());



	cmdList.GetD3DCommandList()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, db1.GetGPUDescriptorHandle());
	cmdList.GetD3DCommandList()->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot, m_rtTLAS.GetD3DResource()->GetGPUVirtualAddress());
	cmdList.GetD3DCommandList()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::SamplerStateSlot, db2.GetGPUDescriptorHandle());
	cmdList.GetD3DCommandList()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::SkyTextures, db3.GetGPUDescriptorHandle());


	DispatchRays(cmdList.GetD3DCommandList4(), m_rtStateObject.GetD3DStateObject(), &dispatchDesc);

#endif

	//cmdList.ResourceTransitionBarrier(&m_rtOutput, GfxLib::ResourceStates::UnorderedAccess, GfxLib::ResourceStates::ShaderResource);
}


void	GFX::OnLButtonDown(POINT pt)
{
	m_lastMouseDown = pt;
	m_MouseMove = true;

}
void	GFX::OnLButtonUp(POINT pt)
{

	m_MouseMove = false;

}
void	GFX::OnMouseMove(POINT pt)
{
	if (m_MouseMove) {
		m_camAngX += (pt.x - m_lastMouseDown.x) * 0.003f;
		m_camAngY += (pt.y - m_lastMouseDown.y) * 0.003f;

		GFX_INFO(L"MouseMove: (%d,%d)\n",pt.x,pt.y);

		m_lastMouseDown = pt;
	}
}

void	GFX::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_HOME) {


		m_camAngY = m_camAngX = 0.f;
		m_camDist = 5.f;
		m_camCenter = GfxLib::Float3{ 0.f,0.f,0.f };


	}


}


void	GFX::OnWheel(int wheel)
{

	m_camDist += (m_camDist * wheel * -0.001f);

	if (m_camDist <= 0.1f) m_camDist = 0.1f;

}