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
        glewInit();
        wglSwapIntervalEXT(1);
    }
    void render() override {
        glClearColor(0.5, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(glRect.left, glRect.top, glRect.right, glRect.bottom);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //glOrtho(glRect.left, glRect.right, glRect.bottom, glRect.top, -1, 1);
        
        gluPerspective(45, 16.0 / 9, 1, 101);

        struct pointInfo {
            uint8_t r, g, b, a;
            float x, y, z;
        };
        pointInfo point_1 = { 255,    0,      0  ,0, -10,    -10,     -81 };
        pointInfo point_2 = { 255,    0,      0  ,0, -10,    10,      -81 };
        pointInfo point_3 = { 255,    0,      0  ,0, 10,     -10,     -81 };
        pointInfo point_4 = { 255,    0,     0   ,0, 10,     10,      -81 };
        pointInfo point_5 = { 0,    255,      255,0, -10,    -10,     -91 };
        pointInfo point_6 = { 255,    0,      255,0, -10,    10,      -91 };
        pointInfo point_7 = { 128,    0,      128,0, 10,     -10,     -91 };
        pointInfo point_8 = { 64,    0,      64  ,0, 10,     10,      -91 };
            
        pointInfo trianglePoint[] = {
            point_1, point_3, point_4, point_2,
            point_5, point_6, point_8, point_7,
            point_1, point_2, point_6, point_5,
            point_3, point_4, point_8, point_7,
            point_1, point_5, point_7, point_3,
            point_2, point_6, point_8, point_4, 
        };

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 01
        glInterleavedArrays(GL_C4UB_V3F, sizeof(pointInfo), trianglePoint);
#else
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(pointInfo), &trianglePoint[0].x);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(pointInfo), trianglePoint);
#endif
        glDrawArrays(GL_QUADS, 0, 24);
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

