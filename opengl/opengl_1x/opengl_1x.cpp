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

        glViewport(glRect.left, glRect.top, glRect.right, glRect.bottom);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glEnable(GL_DEPTH_TEST);
    }
    void render() override {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


#if 01
        glOrtho(glRect.left, glRect.right, glRect.top, glRect.bottom,  -100, 100);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(glRect.right / 2, glRect.bottom / 2, 0);
#else
        gluPerspective(45, 16.0 / 9, 1, 101);
#endif

        struct pointInfo {
            uint8_t r, g, b, a;
            float x, y, z;
        };
            
        pointInfo renderPoint[] = {
            {255, 0, 0, 0,      -100, 0, 0},
            {255, 0, 0, 0,      100, 0, 0},

            {0, 255, 0, 0,      0, 100, 0},
            {0, 255, 0, 0,      0, -100, 0},

            {0, 0, 255, 0,      0, 0, -100},
            {0, 0, 255, 0,      0, 0, 100},

            //point
            {255, 255, 255, 0,      0, 0, 100},
            {255, 255, 255, 0,      0, 100, 0},
            {255, 255, 255, 0,      100, 0, 0},
        };
#if 01
        glInterleavedArrays(GL_C4UB_V3F, sizeof(pointInfo), renderPoint);
#else
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(pointInfo), &renderPoint[0].x);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(pointInfo), &renderPoint[0].r);
#endif
        static float angle = 0;
        angle += 0.1;

        glRotatef(10, 1, 0, 0);
        glRotatef(angle, 0, 0, 1);
        glDrawArrays(GL_LINES, 0, 6);
        glPointSize(10);
        glDrawArrays(GL_POINTS, 6, 3);

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

