// opengl_1x.cpp : 定义应用程序的入口点。
//


#include "openglWindow.h"

#include "GLContext.h"
#define _USE_MATH_DEFINES
#include <math.h>



class sampleWindow : public openglWindow {
private:
    RECT glRect;
    GLContext glc;
public:
    sampleWindow(HINSTANCE hInstance, int nCmdShow) :openglWindow(hInstance, nCmdShow) {
        HWND hWnd = m_hWnd;
        RECT cliRect;
        GetClientRect(hWnd, &cliRect);

        glRect.left = cliRect.left + 10;
        glRect.right = cliRect.right - 10;
        glRect.top = cliRect.top - 10;
        glRect.bottom = cliRect.bottom + 10;

        glc.setup(hWnd, GetDC(hWnd));
    }
    void render() override {
        glClearColor(0.5, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(glRect.left, glRect.top, glRect.right, glRect.bottom);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(glRect.left, glRect.right, glRect.bottom, glRect.top, -1, 1);

#if 1
        //固定管线编程，不能自己写shader，但是顶点数组还是有的
        //但是，这个顶点数组中的数据，真就只是顶点，不能指定多种数据
        struct openglPoint {
            float m_x;
            float m_y;
            float m_z;

            float m_r;
            float m_g;
            float m_b;
        };
        
        openglPoint pointData[16] = { 0 };
        
        auto bezeir = [](openglPoint p0, openglPoint p1, openglPoint p2, openglPoint p3, float t)->openglPoint {
            openglPoint pointData;

            pointData.m_x = (1 - t) * (1 - t) * (1 - t) * p0.m_x + 3 * t * (1 - t) * (1 - t) * p1.m_x + 3 * t * t * (1 - t) * p2.m_x + t * t * t * p3.m_x;
            pointData.m_y = (1 - t) * (1 - t) * (1 - t) * p0.m_y + 3 * t * (1 - t) * (1 - t) * p1.m_y + 3 * t * t * (1 - t) * p2.m_y + t * t * t * p3.m_y;
            pointData.m_z = 0;

            return pointData;
        };
        
        openglPoint p0 = { 20, 20, 0, 0, 0, 0 };
        openglPoint p1 = { 400, 10, 0, 0, 0, 0 };
        openglPoint p2 = { 600, 300, 0, 0, 0, 0 };
        openglPoint p3 = { 800, 200, 0, 0, 0, 0 };
        int cnt = 15;
        for (int i = 0; i < cnt; i++) {
            pointData[i] = bezeir(p0, p1, p2, p3, (float)i/cnt);
        }
        //opengl 1x 的顶点数组，只能放顶点，但是它也提供了颜色数组，放颜色。
        //在opengl 3x 中，顶点数组，放了 顶点 颜色 纹理坐标 这里有没有提供呢
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(openglPoint), pointData);
        glColorPointer(3, GL_FLOAT, sizeof(openglPoint), &pointData[0].m_r);

        glDrawArrays(GL_LINE_STRIP, 0, cnt);        
#else

#endif
        glc.swapBuffer();
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

