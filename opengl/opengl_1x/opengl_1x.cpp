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
        }
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 3*sizeof(float), pointData);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 1 + 7);
        //一个圆心 2个点 就是一个三角形 每加一个点，就是一个新的三角形。 这里一共 6个三角形
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

