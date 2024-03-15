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
        pointInfo point_1 = { 255,    0,      0  ,0, -1,    -1,     -1 };
        pointInfo point_2 = { 255,    0,      0  ,0, -1,    1,      -1 };
        pointInfo point_3 = { 255,    0,      0  ,0, 1,     -1,     -1 };
        pointInfo point_4 = { 255,    0,     0   ,0, 1,     1,      -1 };
        pointInfo point_5 = { 0,    255,      255,0, -1,    -1,     1 };
        pointInfo point_6 = { 255,    0,      255,0, -1,    1,      1 };
        pointInfo point_7 = { 128,    0,      128,0, 1,     -1,     1 };
        pointInfo point_8 = { 64,    0,      64  ,0, 1,     1,      1 };
            
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
        static float angle = 0;
        static float angle_moon = 0;
        static float angle_earth = 0;
        angle += 1;
        angle_moon += 7.8;
        angle_earth += 1.2;
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity(); 
        /*多物体绘制原理分析
        glTranslatef glRotatef 起始坐标系没动，是摄像机动了
        */
        glTranslatef(0, 0, -50);
        glRotatef(45, 1, 0, 0);
        glRotatef(45, 0, 0, 1);
        glRotatef(angle, 1, 1, -1);
        glDrawArrays(GL_QUADS, 0, 24);
        /*
        到这里 摄像机在X Z上都转了45度 所以方块看起来是立起来的。
        glRotatef(angle, 1, 1, -1); 给的旋转轴  其实是相对坐标系给的。
        glDrawArrays(GL_QUADS, 0, 24);  产生了第一张图
        */
        glRotatef(angle_earth, 1, 1, -1);
        glTranslatef(10, -10, 0);
        /*
        因为Z旋转了45度 所以想要和第一个方块在水平线，得平移 15 -15  到4象限  严格来说这样不准确  之后再出一个详细完整版
        产生了第2张图
        */
        glScalef(0.5, 0.5 , 0.5);
        glRotatef(angle, 1, 1, -1);
        glDrawArrays(GL_QUADS, 0, 24);

        glRotatef(angle_moon, 1, 1, -1);
        glTranslatef(5, -5, 0);
        glRotatef(angle, 1, 1, -1);
        glScalef(0.5, 0.5, 0.5);
        glDrawArrays(GL_QUADS, 0, 24);
        /*
        * 产生了第3张图
        */
        /*绕某个物体转，其实只要固定两个物体的位置，其中一个自转就行*/
        glc.swapBuffer();//3图叠加
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

