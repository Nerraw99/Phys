// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved


#include "stdafx.h"

#include "Phys.h"

#include "Circle.hpp"
#include "Line.hpp"

//
// Provides the entry point to the application.
//
int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
    )
{
    // Ignore the return value because we want to continue running even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}


//
// Initialize members.
//
DemoApp::DemoApp() :
    m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pWICFactory(NULL),
    m_pDWriteFactory(NULL),
    m_pRenderTarget(NULL),
    m_pTextFormat(NULL),
    m_pPathGeometry(NULL),
    m_pLinearGradientBrush(NULL),
    m_pBlackBrush(NULL),
    m_pGridPatternBitmapBrush(NULL),
    m_pBitmap(NULL),
    m_pAnotherBitmap(NULL),
	m_bShutdown(false)
{
}

//
// Release resources.
//
DemoApp::~DemoApp()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pTextFormat);
    SafeRelease(&m_pPathGeometry);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pGridPatternBitmapBrush);
    SafeRelease(&m_pBitmap);
    SafeRelease(&m_pAnotherBitmap);
}

//
// Creates the application window and initializes
// device-independent resources.
//
HRESULT DemoApp::Initialize()
{
    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();
    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = DemoApp::WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName  = NULL;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = L"D2DDemoApp";

        RegisterClassEx(&wcex);

        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        // Create the application window.
        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo Application",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}


//
// Create resources which are not bound
// to any device. Their lifetime effectively extends for the
// duration of the app. These resources include the Direct2D,
// DirectWrite, and WIC factories; and a DirectWrite Text Format object
// (used for identifying particular font characteristics) and
// a Direct2D geometry.
//
HRESULT DemoApp::CreateDeviceIndependentResources()
{
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 10;
    HRESULT hr;
    ID2D1GeometrySink *pSink = NULL;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
    if (SUCCEEDED(hr))
    {
        // Create WIC factory.
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory,
            reinterpret_cast<void **>(&m_pWICFactory)
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_pDWriteFactory),
            reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &m_pTextFormat
            );
    }
    if (SUCCEEDED(hr))
    {
        // Center the text horizontally and vertically.
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        // Create a path geometry.
        hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry);
    }
    if (SUCCEEDED(hr))
    {
        // Use the geometry sink to write to the path geometry.
        hr = m_pPathGeometry->Open(&pSink);
    }
    if (SUCCEEDED(hr))
    {
        pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);

        pSink->BeginFigure(
            D2D1::Point2F(0, 0),
            D2D1_FIGURE_BEGIN_FILLED
            );

        pSink->AddLine(D2D1::Point2F(200, 0));

        pSink->AddBezier(
            D2D1::BezierSegment(
                D2D1::Point2F(150, 50),
                D2D1::Point2F(150, 150),
                D2D1::Point2F(200, 200))
            );

        pSink->AddLine(D2D1::Point2F(0, 200));

        pSink->AddBezier(
            D2D1::BezierSegment(
                D2D1::Point2F(50, 150),
                D2D1::Point2F(50, 50),
                D2D1::Point2F(0, 0))
            );

        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

        hr = pSink->Close();
    }

    SafeRelease(&pSink);

    return hr;
}

//
//  This method creates resources which are bound to a particular
//  Direct3D device. It's all centralized here, in case the resources
//  need to be recreated in case of Direct3D device loss (eg. display
//  change, remoting, removal of video card, etc).
//
HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
		D2D1_HWND_RENDER_TARGET_PROPERTIES rt_props;
		rt_props.hwnd = m_hwnd;
		rt_props.pixelSize = size;
		rt_props.presentOptions = D2D1_PRESENT_OPTIONS_IMMEDIATELY;
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
			rt_props,
            &m_pRenderTarget
            );
        if (SUCCEEDED(hr))
        {
            // Create a black brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &m_pBlackBrush
                );
        }
        if (SUCCEEDED(hr))
        {
            ID2D1GradientStopCollection *pGradientStops = NULL;
            // Create a linear gradient.
            static const D2D1_GRADIENT_STOP stops[] =
            {
                {   0.f,  { 0.f, 1.f, 1.f, 0.25f }  },
                {   1.f,  { 0.f, 0.f, 1.f, 1.f }  },
            };

            hr = m_pRenderTarget->CreateGradientStopCollection(
                stops,
                ARRAYSIZE(stops),
                &pGradientStops
                );
            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(100, 0),
                        D2D1::Point2F(100, 200)),
                    D2D1::BrushProperties(),
                    pGradientStops,
                    &m_pLinearGradientBrush
                    );
                pGradientStops->Release();
            }
        }

        // Create a bitmap from an application resource.
        hr = LoadResourceBitmap(
            m_pRenderTarget,
            m_pWICFactory,
            L"SampleImage",
            L"Image",
            100,
            0,
            &m_pBitmap
            );
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadBitmapFromFile(
                m_pRenderTarget,
                m_pWICFactory,
                L".\\sampleImage.jpg",
                100,
                0,
                &m_pAnotherBitmap
                );
        }
        if (SUCCEEDED(hr))
        {
            hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
        }
    }

    return hr;
}

//
// Creates a pattern brush.
//
HRESULT DemoApp::CreateGridPatternBrush(
    ID2D1RenderTarget *pRenderTarget,
    ID2D1BitmapBrush **ppBitmapBrush
    )
{
    HRESULT hr = S_OK;

    // Create a compatible render target.
    ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
    hr = pRenderTarget->CreateCompatibleRenderTarget(
        D2D1::SizeF(10.0f, 10.0f),
        &pCompatibleRenderTarget
        );
    if (SUCCEEDED(hr))
    {
        // Draw a pattern.
        ID2D1SolidColorBrush *pGridBrush = NULL;
        hr = pCompatibleRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f)),
            &pGridBrush
            );
        if (SUCCEEDED(hr))
        {
            pCompatibleRenderTarget->BeginDraw();
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.1f, 1.0f, 10.0f), pGridBrush);
            pCompatibleRenderTarget->EndDraw();

            // Retrieve the bitmap from the render target.
            ID2D1Bitmap *pGridBitmap = NULL;
            hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);
            if (SUCCEEDED(hr))
            {
                // Choose the tiling mode for the bitmap brush.
                D2D1_BITMAP_BRUSH_PROPERTIES brushProperties =
                    D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP);

                // Create the bitmap brush.
                hr = m_pRenderTarget->CreateBitmapBrush(pGridBitmap, brushProperties, ppBitmapBrush);

                pGridBitmap->Release();
            }

            pGridBrush->Release();
        }

        pCompatibleRenderTarget->Release();
    }

    return hr;
}

//
//  Discard device-specific resources which need to be recreated
//  when a Direct3D device is lost
//
void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBitmap);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pAnotherBitmap);
    SafeRelease(&m_pGridPatternBitmapBrush);
}

//
// The main window message loop.
//
void DemoApp::RunMessageLoop()
{
    MSG msg;

	Circle testCircle( Vector2( 10.0, 10.0 ), 10.0 );
	Circle testCircle2( Vector2( 50.0, 10.0 ), 20.0 );

	Line testLine( Vector( -200.0, 0.0 ), Vector( 200.0, 0.0 ), Vector( 0.0, -50.0 ) );
	Line testLine2( Vector( 200.0, 0.0 ), Vector( 400.0, 200.0 ), Vector( 0.0, -50.0 ) );
	Line testLine3( Vector( 0.0, 200.0 ), Vector( 0.0, 0.0 ), Vector( 0.0, -50.0 ) );

	testCircle.SetVelocity( Vector( 20.0, 20.0 ) );
	testLine.SetStatic( true );
	testLine2.SetStatic( true );
	testLine3.SetStatic( true );

	testCircle2.SetMass( 5.0 );
	
	//m_Physics.AddObject( &testCircle );
	//m_Physics.AddObject( &testCircle2 );
	m_Physics.AddObject( &testLine );
	m_Physics.AddObject( &testLine2 );
	m_Physics.AddObject( &testLine3 );

	srand(1);

	Scalar minRadius = 2.0;
	Scalar maxRadius = 15.0;

	int iNumColumns = 7;
	int iNumRows = 7;

	Scalar xStart = 50.0;
	Scalar yStart = 0.0;

	Scalar xSpan = 200.0;
	Scalar ySpan = 200.0;

	for ( int x = 0; x < iNumColumns; x++ )
	{
		for ( int y = 0; y < iNumRows; y++ )
		{
			Scalar scX = xStart + ( (Scalar) x / iNumColumns ) * xSpan;
			Scalar scY = yStart + ( (Scalar) y / iNumRows ) * ySpan;

			Scalar scRadius = minRadius * 0.1 + maxRadius * 0.9 * (float)rand()/(float)RAND_MAX;

			Circle *pNewCircle = new Circle( Vector2( scX, scY ), scRadius );

			pNewCircle->SetMass( 3.14 * scRadius * scRadius );

			m_Physics.AddObject( pNewCircle );
		}
	}

	//Circle testCircle( Vector2( 10.0, 10.0 ), 10.0 );
	//testCircle.SetVelocity( Vector( 10.0, 0.0 ) );

	//Circle testCircle2( Vector2( 50.0, 10.0 ), 10.0 );
	//testCircle2.SetVelocity( Vector( 0.0, 0.0 ) );

	//testCircle2.SetMass( 2.0 );

	//m_Physics.AddObject( &testCircle );
	//m_Physics.AddObject( &testCircle2 );

	do
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		static LARGE_INTEGER liPerfCountFrequency;
		QueryPerformanceFrequency( &liPerfCountFrequency );

		LARGE_INTEGER liCurrentPerfCount;
		QueryPerformanceCounter( &liCurrentPerfCount );

		static LARGE_INTEGER liPreviousPerfCount = liCurrentPerfCount;

		double fDuration = (double) ( liCurrentPerfCount.QuadPart - liPreviousPerfCount.QuadPart ) / liPerfCountFrequency.QuadPart;

		liPreviousPerfCount = liCurrentPerfCount;

		//static DWORD sdwPreviousTickCount = GetTickCount();
		//DWORD dwCurrentTickCount = GetTickCount();
		//DWORD dwTickDiff = dwCurrentTickCount - sdwPreviousTickCount;
		//float fFrameRate = 1000.0 / dwTickDiff;

		//sdwPreviousTickCount = dwCurrentTickCount;

		m_Physics.Update( fDuration );
		//m_Physics.Update( 0.0005 );

		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
		OnRender();
		EndPaint(m_hwnd, &ps);

	}
	while ( !m_bShutdown );
}


//
//  Called whenever the application needs to display the client
//  window. This method draws a bitmap a couple times, draws some
//  geometries, and writes "Hello, World"
//
//  Note that this function will not render anything if the window
//  is occluded (e.g. when the screen is locked).
//  Also, this function will automatically discard device-specific
//  resources if the Direct3D device disappears during function
//  invocation, and will recreate the resources the next time it's
//  invoked.
//

HRESULT DemoApp::OnRender()
{
    HRESULT hr;

    hr = CreateDeviceResources();

    if (SUCCEEDED(hr) && !(m_pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
    {
		static LARGE_INTEGER liPerfCountFrequency;
		QueryPerformanceFrequency( &liPerfCountFrequency );

		static LARGE_INTEGER liPreviousPerfCount;
		LARGE_INTEGER liCurrentPerfCount;
		QueryPerformanceCounter( &liCurrentPerfCount );

		double fDuration = (double) ( liCurrentPerfCount.QuadPart - liPreviousPerfCount.QuadPart ) / liPerfCountFrequency.QuadPart;
		double fFrameRate = 1.0 / fDuration;

		liPreviousPerfCount = liCurrentPerfCount;

		//static DWORD sdwPreviousTickCount = GetTickCount();
		//DWORD dwCurrentTickCount = GetTickCount();
		//DWORD dwTickDiff = dwCurrentTickCount - sdwPreviousTickCount;
		//float fFrameRate = 1000.0 / dwTickDiff;

		//sdwPreviousTickCount = dwCurrentTickCount;

		static double fFrameRateInterpolated = fFrameRate;
		fFrameRateInterpolated = fFrameRate; //( fFrameRateInterpolated * 0.999f ) + ( fFrameRate * 0.001f );

		WCHAR sc_framerate[256];
		swprintf( sc_framerate, 255, L"Frame Rate: %f", fFrameRateInterpolated );

		static uint32 s_uLastCollisions = 0;

		uint32 uNumCollisions = m_Physics.GetNumCollisions();

		WCHAR sc_collisions[256];
		swprintf( sc_collisions, 255, L"Collisions: %d", uNumCollisions );

		WCHAR sc_collisionsThisFrame[256];
		swprintf( sc_collisionsThisFrame, 255, L"Collisions This Frame: %d", uNumCollisions - s_uLastCollisions );

		s_uLastCollisions = uNumCollisions;

        // Retrieve the size of the render target.
        D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF( 0.8f, 0.8f, 0.8f, 1.0f )));

		// Fill a rectangle
		D2D1_RECT_F rect;
		rect.bottom = 0.0;
		rect.top = 1000.0;
		rect.left = 0.0;
		rect.right = 1000.0;

		ID2D1SolidColorBrush *pBackgroundBrush;

		D2D1::ColorF color( 1.0, 1.0, 1.0, 1.0 );

		hr = m_pRenderTarget->CreateSolidColorBrush(
			color,
			&pBackgroundBrush
			);

		//m_pRenderTarget->FillRectangle( &rect, pBackgroundBrush );

		SafeRelease( &pBackgroundBrush );
		
		//if ( m_pGridPatternBitmapBrush )
		//{
		//	// Paint a grid background.
		//	m_pRenderTarget->FillRectangle(
		//		D2D1::RectF(0.0, 0.0, renderTargetSize.width, renderTargetSize.height),
		//		m_pGridPatternBitmapBrush
		//		);
		//}

		//if ( m_pBitmap )
		//{
		//	D2D1_SIZE_F size = m_pBitmap->GetSize();

		//	// Draw a bitmap in the upper-left corner of the window.
		//	m_pRenderTarget->DrawBitmap(
		//		m_pBitmap,
		//		D2D1::RectF(0.0, 0.0, size.width, size.height)
		//		);


		//	// Draw a bitmap at the lower-right corner of the window.
		//	size = m_pAnotherBitmap->GetSize();
		//	m_pRenderTarget->DrawBitmap(
		//		m_pAnotherBitmap,
		//		D2D1::RectF(
		//			renderTargetSize.width - size.width,
		//			renderTargetSize.height - size.height,
		//			renderTargetSize.width,
		//			renderTargetSize.height)
		//		);
		//}

        // Set the world transform to a 45 degree rotation at the center of the render target
        // and write "Hello, World".
 /*       m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Rotation(
                45,
                D2D1::Point2F(
                    renderTargetSize.width / 2,
                    renderTargetSize.height / 2))
            );*/

       m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Translation( 0, 0 )
            );

        //m_pRenderTarget->DrawText(
        //    sc_helloWorld,
        //    ARRAYSIZE(sc_helloWorld) - 1,
        //    m_pTextFormat,
        //    D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
        //    m_pBlackBrush
        //    );

		m_pRenderTarget->DrawText(
			sc_framerate,
            wcslen(sc_framerate),
            m_pTextFormat,
            D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
            m_pBlackBrush
            );

		m_pRenderTarget->DrawText(
			sc_collisions,
            wcslen(sc_collisions),
            m_pTextFormat,
            D2D1::RectF(0, 15, renderTargetSize.width, renderTargetSize.height),
            m_pBlackBrush
            );

		m_pRenderTarget->DrawText(
			sc_collisionsThisFrame,
            wcslen(sc_collisionsThisFrame),
            m_pTextFormat,
            D2D1::RectF(0, 30, renderTargetSize.width, renderTargetSize.height),
            m_pBlackBrush
            );


        //
        // Reset back to the identity transform
        //
        m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Translation(0, renderTargetSize.height - 200)
            );

        // Fill the hour glass geometry with a gradient.
        //m_pRenderTarget->FillGeometry(m_pPathGeometry, m_pLinearGradientBrush);

        //m_pRenderTarget->SetTransform(
        //    D2D1::Matrix3x2F::Translation(renderTargetSize.width - 200, 0)
        //    );

        //// Fill the hour glass geometry with a gradient.
        //m_pRenderTarget->FillGeometry(m_pPathGeometry, m_pLinearGradientBrush);

		m_Physics.Draw( m_pRenderTarget );

        hr = m_pRenderTarget->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            DiscardDeviceResources();
        }
    }

    return hr;
}

//
//  If the application receives a WM_SIZE message, this method
//  resize the render target appropriately.
//
void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;

        // Note: This method can fail, but it's okay to ignore the
        // error here -- it will be repeated on the next call to
        // EndDraw.
        m_pRenderTarget->Resize(size);
    }
}


//
// The window message handler.
//
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
            );

        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
            case WM_DISPLAYCHANGE:
                {
                    PAINTSTRUCT ps;
                    BeginPaint(hwnd, &ps);
                    pDemoApp->OnRender();
                    EndPaint(hwnd, &ps);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
					pDemoApp->OnShutdown();	
                }
                result = 1;
                wasHandled = true;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

//
// Creates a Direct2D bitmap from a resource in the
// application resource file.
//
HRESULT DemoApp::LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
    )
{
    HRESULT hr = S_OK;
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

    hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                            pSource,
                            destinationWidth,
                            destinationHeight,
                            WICBitmapInterpolationModeCubic
                            );
                    if (SUCCEEDED(hr))
                    {
                        hr = pConverter->Initialize(
                            pScaler,
                            GUID_WICPixelFormat32bppPBGRA,
                            WICBitmapDitherTypeNone,
                            NULL,
                            0.f,
                            WICBitmapPaletteTypeMedianCut
                            );
                    }
                }
            }
        }
        else
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                );
        }
    }
    if (SUCCEEDED(hr))
    {
        //create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

//
// Creates a Direct2D bitmap from the specified
// file name.
//
HRESULT DemoApp::LoadBitmapFromFile(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR uri,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
    )
{
    HRESULT hr = S_OK;

    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    hr = pIWICFactory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
        );
    if (SUCCEEDED(hr))
    {

        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }

    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                            pSource,
                            destinationWidth,
                            destinationHeight,
                            WICBitmapInterpolationModeCubic
                            );
                }
                if (SUCCEEDED(hr))
                {
                    hr = pConverter->Initialize(
                        pScaler,
                        GUID_WICPixelFormat32bppPBGRA,
                        WICBitmapDitherTypeNone,
                        NULL,
                        0.f,
                        WICBitmapPaletteTypeMedianCut
                        );
                }
            }
        }
        else // Don't scale the image.
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                );
        }
    }
    if (SUCCEEDED(hr))
    {
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}
