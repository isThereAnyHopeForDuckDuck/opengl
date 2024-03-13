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

        //摄像机位置在 0 0 0 点，方向是  -Z  这是前提
        // 这种情况下  gluPerspective 的后2个参数 1 101，意味着 视景体的Z坐标范围是  [-1, -101]
        // 用下面这几组坐标体会一下
        //CELL::float3 trianglePoint[] = {{-1, -1, -0.99},{1, -1, -0.99}, {0, 1, -0.99}};
        //CELL::float3 trianglePoint[] = { {-1, -1, -101.01},{1, -1, -101.01}, {0, 1, -101.01} };
        CELL::float3 trianglePoint[] = { {-1, -1, -101},{1, -1,-101}, {0, 1, -101} };

        glColor3f(1, 0, 0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(CELL::float3), trianglePoint);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

