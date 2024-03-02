#pragma once

#include "framework.h"
#include "opengl_1x.h"



class openglWindow {
protected:
    HINSTANCE   m_instance = 0;
    HWND        m_hWnd = 0;
    int         m_cmdShow;

public:
    openglWindow(HINSTANCE hInstance, int nCmdShow) {
        // ��ʼ��ȫ���ַ���
        m_instance = hInstance;
        m_cmdShow = nCmdShow;
        bool ret = registerInstance();
        ret = initInstance();
    }

    HWND getWindow() {
        return m_hWnd;
    }

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

    static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {


        switch (message)
        {
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��:
            switch (wmId)
            {
            case IDM_ABOUT:
                //DialogBox(m_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
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
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_instance, nullptr);

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