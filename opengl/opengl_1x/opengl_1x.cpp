// opengl_1x.cpp : 定义应用程序的入口点。
//

/*
    glew 找到OpenGL扩展的函数
    垂直同步： 开：根据屏幕刷新率来刷新。 关：根据代码来，可以刷很快
*/
#include "openglWindow.h"
#include "CELLMath.hpp"
#include "GLContext.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>

class sampleWindow : public openglWindow {
private:
    CELL::tspline<float> m_tspline;
    RECT glRect;
    GLContext glc;
    std::vector<CELL::float3> m_point;
    std::vector<CELL::float3> m_tsplinePoint;

    int m_moveIdx = -1;
    POINT rawPoint;

    //RGB image
    uint8_t m_colorRGB[7 * 3] = {
        255, 0, 0,
        0, 255, 0,
        0, 0, 255,
        255, 255, 0,
        255, 0, 255,
        0, 255, 255,
        7, 77, 7,
    };
    int m_rgbWidth = 160;
    int m_rgbHeight = 160;
    std::vector<uint8_t> m_rgbData;

public:
    sampleWindow(HINSTANCE hInstance, int nCmdShow) :openglWindow(hInstance, nCmdShow) {
        HWND hWnd = m_hWnd;
        RECT cliRect;
        GetClientRect(hWnd, &cliRect);

        m_point = {
            {30, 10, 0},
            {100, 100, 0},
            {200, 10, 0},
            {300, 120, 0},
            {400, 200, 0},
            {500, 100, 0},
            {600, 300, 0},
        };
        for (int i = 0; i < m_point.size(); i++) {
            m_tspline.addPoint(m_point.at(i));
        }

        for (float i = 0; i < 1; i += 0.01) {
            CELL::float3 point = m_tspline.interpolate(i);
            m_tsplinePoint.emplace_back(point);
        }
        glRect.left = cliRect.left + 10;
        glRect.right = cliRect.right - 10;
        glRect.top = cliRect.top - 10;
        glRect.bottom = cliRect.bottom + 10;

        glc.setup(hWnd, GetDC(hWnd));
        glewInit();
        wglSwapIntervalEXT(1);

        //生成rgb图片
        int index = 0;
        int rowSize = m_rgbWidth / 7;
        m_rgbData.resize(m_rgbWidth * m_rgbHeight * 3);
        for (int row = 0; row < m_rgbWidth; row++) {
            for (int col = 0; col < m_rgbHeight; col++) {
                index = row / rowSize;
                index = index > 6 ? 6 : index;
                m_rgbData[row * m_rgbWidth * 3 + col * 3] = m_colorRGB[index*3];
                m_rgbData[row * m_rgbWidth * 3 + col * 3 + 1] = m_colorRGB[index * 3 + 1];
                m_rgbData[row * m_rgbWidth * 3 + col * 3 + 2] = m_colorRGB[index * 3 + 2];
            }
        }
    }
    void render() override {
        glClearColor(0.5, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(glRect.left, glRect.top, glRect.right, glRect.bottom);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(glRect.left, glRect.right, glRect.bottom, glRect.top, -1, 1);

        glLineWidth(5);
        glEnable(GL_LINE_STIPPLE);
        //32bit  1画 0不画
        glLineStipple(1, 0xffff);

        //反锯齿的应用  划线要开启blender,目前不理解
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor3f(1, 1, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(CELL::float3), m_tsplinePoint.data());
        glDrawArrays(GL_LINE_STRIP, 0, m_tsplinePoint.size());

        glColor3f(1, 0, 0);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glPointSize(8);
        glVertexPointer(3, GL_FLOAT, sizeof(CELL::float3), m_point.data());
        glDrawArrays(GL_POINTS, 0, m_point.size());

        glRasterPos2i(500, 200);
        glDrawPixels(m_rgbWidth, m_rgbHeight, GL_RGB, GL_UNSIGNED_BYTE, m_rgbData.data());

        glc.swapBuffer();
    }

    LRESULT events(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam) override {
        switch (msgId)
        {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;
        case  WM_LBUTTONDOWN:
        {
            POINT   pt = { LOWORD(lParam),HIWORD(lParam) };
            rawPoint = pt;
            //判断是否是m_point中的点。
            for (int i = 0; i < m_point.size(); i++) {
                CELL::float3 point = m_point.at(i);
                int rangeSize = 10;
                int x0 = point.x - rangeSize, x1 = point.x + rangeSize;
                int y0 = point.y - rangeSize, y1 = point.y + rangeSize;

                if (pt.x >= x0 && pt.x <= x1 && pt.y >= y0 && pt.y <= y1) {
                    m_moveIdx = i;
                }
            }
        }
        break;
        case WM_LBUTTONUP:
        {
            m_moveIdx = -1;
        }
        break;
        case WM_MOUSEMOVE:
        {
            if (m_moveIdx >= 0) {
                POINT   pt = { LOWORD(lParam),HIWORD(lParam) };

                POINT   off = { pt.x - rawPoint.x, pt.y - rawPoint.y };

                rawPoint = pt;

                m_point[m_moveIdx].x += off.x;
                m_point[m_moveIdx].y += off.y;

                m_tspline.getPoint(m_moveIdx).x += off.x;
                m_tspline.getPoint(m_moveIdx).y += off.y;

                m_tspline.recalcTangents();

                m_tsplinePoint.clear();

                for (float t = 0; t < 1.0f; t += 0.01f)
                {
                    CELL::float3  pos = m_tspline.interpolate(t);
                    m_tsplinePoint.emplace_back(pos);
                }
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, msgId, wParam, lParam);
        }
        return  0;
    }
};


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    sampleWindow s(hInstance, nCmdShow);
    s.loop();

    return 0;
    
}

