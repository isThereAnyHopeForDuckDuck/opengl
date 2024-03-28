#pragma once
#include "GL/glew.h"
#include "GL/wglew.h"

#include "framework.h"
#include "opengl_1x.h"

//#define GWL_USERDATA (-21)
#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif



class openglWindow {
protected:
    HINSTANCE   m_instance = 0;
    HWND        m_hWnd = 0;
    int         m_cmdShow;

    int         m_width = 1280, m_height = 720;
public:
    openglWindow(HINSTANCE hInstance, int nCmdShow) {
        // 初始化全局字符串
        m_instance = hInstance;
        m_cmdShow = nCmdShow;
        bool ret = registerInstance();
        ret = initInstance();
    }

    HWND getWindow() {
        return m_hWnd;
    }
    virtual void openglUninit() {}
    virtual void onResize() {}
	void loop() {
        MSG msg = { 0 };

        while (WM_QUIT != msg.message) {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else {
                render();
            }
        }
        //释放资源
        openglUninit();
	}

    static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
    virtual LRESULT events(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam)
    {
        switch (msgId)
        {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;
        case WM_SIZE: {
            m_width = LOWORD(lParam);
            m_height = HIWORD(lParam);
            onResize();
            break;
        }
        default:
            return DefWindowProc(hWnd, msgId, wParam, lParam);
        }
        return  0;
    }
    static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        if (message == WM_CREATE)
        {
            LPCREATESTRUCT  pData = (LPCREATESTRUCT)lParam;
            openglWindow* pTHis = (openglWindow*)pData->lpCreateParams;
            //SetWindowLong(hWnd, GWL_USERDATA, (LONG)pTHis);
            SetProp(hWnd, L"MyWindowData", (HANDLE)pTHis);
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        else
        {
            //openglWindow* pTHis = (openglWindow*)GetWindowLong(hWnd, GWL_USERDATA);
            openglWindow* pTHis = (openglWindow*)GetProp(hWnd, L"MyWindowData");
            if (pTHis)
            {
                return  pTHis->events(hWnd, message, wParam, lParam);
            }
            else
            {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
    }
protected:
    virtual void render() {}

private:
    bool registerInstance() {
        WNDCLASSEXW wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = m_instance;
        wcex.hIcon = LoadIcon(m_instance, MAKEINTRESOURCE(IDI_OPENGL1X));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OPENGL1X);
        wcex.lpszClassName = L"szWindowClass";
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        return RegisterClassExW(&wcex);
    }

    bool initInstance() {
        HWND hWnd = CreateWindowW(L"szWindowClass", L"szTitle", WS_OVERLAPPEDWINDOW,
            0, 0, m_width, m_height, nullptr, nullptr, m_instance, this);

        if (!hWnd)
        {
            return FALSE;
        }
        m_hWnd = hWnd;

        ShowWindow(hWnd, m_cmdShow);
        UpdateWindow(hWnd);

        return TRUE;
    }
};