﻿// D3D12TestApp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "D3D12TestApp.h"

#include "GfxLib.h"


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
//GfxLib::DescriptorHeap  descHeap_CbvSrv;


LARGE_INTEGER			g_lastUpdateTime;
float					g_accTime = 0;

using namespace DirectX;

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



	if (!gfxCore.Initialize()) {
		return FALSE;
	}


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
		rootSigDesc.AddParam_DescriptorTable(aCbvSrvRange, _countof(aCbvSrvRange) );
		rootSigDesc.AddParam_DescriptorTable(&GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Sampler,1,0 }, 1);

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
		descHeap_Sampler.InitializeSampler(1);

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

		

	}

	{
		// RenderTarget

		//GfxLib::RenderTarget renderTarget;

		//bool b = renderTarget.Initialize(GfxLib::Format::R8G8B8A8_UNORM, 128, 128, 1, true);


		depthStencil.Initialize(GfxLib::Format::D32_FLOAT, swapChain.GetWidth(), swapChain.GetHeight() ,1,true);

		
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

		Vertex vertices[] = {
			{ { 0.f,1.f,0.f },	{ 0.f,0.f,-1.f },	{ 0.f,1.f },	{ 1.f,1.f,1.f,1.f }, },
			{ { 1.f,-1.f,0.f },	{ 0.f,0.f,-1.f },	{ 1.f,0.f },	{ 0.f,1.f,1.f,1.f }, },
			{ { -1.f,-1.f,0.f },{ 0.f,0.f,-1.f },	{ 0.f,0.f },	{ 1.f,0.f,0.f,1.f }, },

		};

		

		vtxBuff.Initialize(vertices, sizeof(Vertex), _countof(vertices));


		uint16_t indices[] = {
			0,1,2
		};

		idxBuff.Initialize(indices, sizeof(indices), GfxLib::Format::R16_UINT);

		
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

#if 1
	{
				
		
		D3D12_INPUT_ELEMENT_DESC inputElement[] = {
			{ "POSITION",	0	,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
			{ "NORMAL",		0	,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
			{ "TEXCOORD",	0	,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
			{ "VTX_COLOR",	0	,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0	},
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
		il2.Initialize(_countof(inputElement2), inputElement2 );



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
		desc.pRootSignature		= rootSig.GetD3DRootSignature();
		desc.VS					= vertexshader.GetD3D12ShaderBytecode();	//	TODO
		desc.PS					= pixelshader.GetD3D12ShaderBytecode();
		desc.RasterizerState	= descRS;
		desc.BlendState			= descBS;
		desc.DepthStencilState.DepthEnable = TRUE;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask			= UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets	= 1;
		desc.RTVFormats[0]		= DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat			= DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count	= 1;


		pipelineState.Initialize(desc);

	}
#endif

#endif

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

	gfxCore.Finalize();


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
	LARGE_INTEGER nowTime;
	QueryPerformanceCounter(&nowTime);

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER deltaTime;
	deltaTime.QuadPart = nowTime.QuadPart - g_lastUpdateTime.QuadPart;


	float deltaTimeInSec = float(deltaTime.QuadPart / (double)freq.QuadPart);

	g_lastUpdateTime = nowTime;


	g_accTime += deltaTimeInSec;

	gfxCore.Update();

}


void Render()
{

	if (gfxCore.Begin()) {


		cmdList.Reset();
		//cmdList2.Reset();

		ID3D12GraphicsCommandList *d3dCmdList = cmdList.GetD3DCommandList();
		{
			swapChain.Begin(cmdList);
			//


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
			d3dCmdList->ClearRenderTargetView(rt->GetRTVDescriptorHandle(), clearColorF, 0, nullptr);
			d3dCmdList->ClearDepthStencilView(depthStencil.GetDSVDescriptorHandle(), D3D12_CLEAR_FLAG_DEPTH , GFX_DEFAULT_DEPTH_CLEAR_VALUE, 0, 0 , nullptr);

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
				cbdata.view = XMMatrixTranspose(XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 5.f, 0.f), XMVectorZero(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
				cbdata.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(3.14159f / 4.f, 1.f, 0.1f, 100.f));
#if 0
				void* cpuAddr = nullptr;
				D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = cmdList.AllocateGpuBuffer(cpuAddr,
					GfxLib::UpperBounds((uint32_t)sizeof(CBData),256) , D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);


				memcpy(cpuAddr, &cbdata, sizeof(CBData));

				descBuff.SetConstantBuffer(0, gpuAddr, sizeof(CBData));
#endif

				descBuff.SetConstantBuffer( 0, &cbdata, sizeof(CBData));
			}



			//descBuff.CopyHandle(0, constantBuffer.GetCbvDescHandle());
			descBuff.CopyHandle(1, texture2D.GetSrvDescHandle());

			// Rootsignatureおよび、DescriptorHeap対応
			ID3D12DescriptorHeap *heap[] = {
				//cbvsrvHeap->GetD3DDescriptorHeap() ,
				descBuff.GetD3DDescriptorHeap(),
				descHeap_Sampler.GetD3DDescriptorHeap() ,
			};
			//d3dCmdList->SetDescriptorHeaps(1, &heap );

			d3dCmdList->SetGraphicsRootSignature(rootSig.GetD3DRootSignature());

			// デスクリプタテーブルを設定する
			d3dCmdList->SetDescriptorHeaps( _countof(heap) , heap);
			d3dCmdList->SetGraphicsRootDescriptorTable(0, descBuff.GetGPUDescriptorHandle() );
			d3dCmdList->SetGraphicsRootDescriptorTable(1, descHeap_Sampler.GetD3DDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

			//
			//d3dCmdList->SetGraphicsRootShaderResourceView(2, texture2D.GetD3DResource()->GetGPUVirtualAddress());

			d3dCmdList->SetPipelineState(pipelineState.GetD3DPipelineState());

			// 頂点バッファの設定、プリミティブトポロジの設定
			d3dCmdList->IASetVertexBuffers(0, 1, &vtxBuff.GetVertexBufferView());
			d3dCmdList->IASetIndexBuffer(&idxBuff.GetIndexBufferView());
			d3dCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			d3dCmdList->DrawInstanced(3, 1, 0, 0);


			// 2回目の描画

			{

				uint32_t descHeapStartIndex = 0;

				//D3D12_CPU_DESCRIPTOR_HANDLE aHandle[2] = { constantBuffer2.GetCbvDescHandle(), texture2D.GetSrvDescHandle() };
				GfxLib::DescriptorBuffer descBuff = cmdList.AllocateDescriptorBuffer(2);

				//DescBuff.CopyHandle(0, aHandle, 2);



				{
					//void* cpuAddr = nullptr;
					//D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = cmdList.AllocateGpuBuffer(cpuAddr,
					//	GfxLib::UpperBounds((uint32_t)sizeof(CBData), 256), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);


					CBData cbdata;
					
					cbdata.world = XMMatrixTranspose(XMMatrixRotationY(XM_PI / 4 * g_accTime ));
					cbdata.view = XMMatrixTranspose(XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 5.f, 0.f), XMVectorZero(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
					cbdata.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(3.14159f / 4.f, 1.f, 0.1f, 100.f));


					//memcpy(cpuAddr, &cbdata, sizeof(CBData));

					//descBuff.SetConstantBuffer(0, gpuAddr, sizeof(CBData));
					descBuff.SetConstantBuffer(0, &cbdata, sizeof(cbdata));
				}

				descBuff.CopyHandle(1, texture2D.GetSrvDescHandle());


				ID3D12DescriptorHeap *heap[] = {
					descBuff.GetD3DDescriptorHeap() ,
					descHeap_Sampler.GetD3DDescriptorHeap() ,
				};

				d3dCmdList->SetDescriptorHeaps(_countof(heap) , heap );
				d3dCmdList->SetGraphicsRootDescriptorTable(0, descBuff.GetGPUDescriptorHandle());
				d3dCmdList->SetGraphicsRootDescriptorTable(1, descHeap_Sampler.GetD3DDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());




				d3dCmdList->DrawInstanced(3, 1, 0, 0);
				
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
		swapChain.Present( 1 , 0 );


		gfxCore.End();
	}



}