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
            float m_x0;
            float m_y0;
            float m_z0;

            float m_x1;
            float m_y1;
            float m_z1;

            float m_cx;
            float m_cy;
            float m_cz;
        };
        float cx = glRect.right / 2, cy = glRect.bottom / 2, cz = 0;
        float r = glRect.right / 6;

        openglPoint pointData[361] = { 0 };
        for (int i = 0; i < 361; i++) {
            pointData[i].m_cx = cx;
            pointData[i].m_cy = cy;
            pointData[i].m_cz = cz;

            pointData[i].m_x0 = (float)cos((double)i * M_PI / 180) * r + cx;;
            pointData[i].m_y0 = (float)sin((double)i * M_PI / 180) * r + cy;;
            pointData[i].m_z0 = cz;

            pointData[i].m_x1 = (float)cos((double)(i + 1) * M_PI / 180) * r + cx;
            pointData[i].m_y1 = (float)sin((double)(i + 1) * M_PI / 180) * r + cy;
            pointData[i].m_z1 = cz;
        }
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 3*sizeof(float), pointData);
        glDrawArrays(GL_TRIANGLES, 3, 3*361);

#else
        // 这种方式，相当于每画一个三角形，就向显存传递一次数据
        glBegin(GL_TRIANGLES);
            float x = 0, y = 0, z = 0;
            float cx = glRect.right / 2, cy = glRect.bottom / 2, cz = 0;
            float r = glRect.right / 6;

            for (int i = 0; i < 360; i++) {
                glVertex3f(cx, cy, cz);

                x = (float)cos((double)i * M_PI / 180) * r + cx;;
                y = (float)sin((double)i * M_PI / 180) * r + cy;;
                z = cz;

                glVertex3f(x, y, z);

                x = (float)cos((double)(i + 1) * M_PI / 180) * r + cx;
                y = (float)sin((double)(i + 1) * M_PI / 180) * r + cy;
                z = cz; 
                glVertex3f(x, y, z);
            }
        glEnd();
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

