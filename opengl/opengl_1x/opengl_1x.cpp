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
        float cx = glRect.right / 2, cy = glRect.bottom / 2, cz = 0;
        float r = glRect.right / 6;

        openglPoint pointData[16] = { 0 };
        pointData[0].m_x = cx;
        pointData[0].m_y = cy;
        pointData[0].m_z = cz;
        for (int i = 1; i <= 7; i++) {
            int radian = (i-1) * 60;

            pointData[i].m_x = (float)cos((double)radian * M_PI / 180) * r + cx;;
            pointData[i].m_y = (float)sin((double)radian * M_PI / 180) * r + cy;;
            pointData[i].m_z = cz;

            pointData[i].m_r = i*0.1;
            pointData[i].m_g = i * 0.15;
            pointData[i].m_b = i*0.17;
        }
        //opengl 1x 的顶点数组，只能放顶点，但是它也提供了颜色数组，放颜色。
        //在opengl 3x 中，顶点数组，放了 顶点 颜色 纹理坐标 这里有没有提供呢
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(openglPoint), pointData);
        glColorPointer(3, GL_FLOAT, sizeof(openglPoint), &pointData[0].m_r);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 1 + 7);
        
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

