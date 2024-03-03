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
        
        openglPoint pointData[] = { 
            {    20, 20, 0,      1, 0, 0},
            {    20, 100, 0,     0, 1, 0 },
            {    100, 20, 0,     1, 0, 0 },
            {    100, 100, 0,    0, 0, 1 },
            
            {    200, 120, 0,     1, 0, 0 },
            {    200, 200, 0,    0, 0, 1 },

            {    300, 120, 0,     1, 0, 0 },
            {    300, 200, 0,    0, 0, 1 },
        }; 

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(openglPoint), pointData);
        glColorPointer(3, GL_FLOAT, sizeof(openglPoint), &pointData[0].m_r);
        //多边形  用的三角形扇绘制
        //四边形，用的三角形绘制
        glDrawArrays(GL_POLYGON, 0, sizeof(pointData)/sizeof(openglPoint));
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(pointData) / sizeof(openglPoint));

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

