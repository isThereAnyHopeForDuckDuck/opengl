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

        auto drawLine = []() {
            glBegin(GL_LINES);
            glColor3f(1, 0, 0);
            glVertex3f(-10, 0, 0);
            glVertex3f(10, 0, 0);

            glColor3f(0, 1, 0);
            glVertex3f(0, -10, 0);
            glVertex3f(0, 10, 0);

            glColor3f(0, 0, 1);
            glVertex3f(0, 0, 10);
            glVertex3f(0, 0, -10);
            glEnd();

            glPointSize(8);
            glColor3f(1, 1, 1);
            glBegin(GL_POINTS);
            glVertex3f(10, 0, 0);
            glVertex3f(0, 10, 0);
            glVertex3f(0, 0, 10);
            glEnd();
        };

        auto drawRect = []() {
            glBegin(GL_LINES);
            glColor3f(1, 0, 0);
            glVertex3f(-10, 10, 0);
            glVertex3f(10, 10, 0);

            glColor3f(0, 1, 0);
            glVertex3f(-10, -10, 0);
            glVertex3f(10, -10, 0);

            glColor3f(0, 0, 1);
            glVertex3f(-10, -10, 0);
            glVertex3f(-10, 10, 0);

            glColor3f(0, 0, 1);
            glVertex3f(10, -10, 0);
            glVertex3f(10, 10, 0);
            glEnd();
        };

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity(); 

        // 关于平移
        // 1. glTranslatef的三个参数，可以理解为一个向量，也可以理解为3个维度的平移量
        //      因为直接平移到这个三维向量代表的点 和 X Y Z 分别移动，最后的效果是一样的。
        // 2. 平移的方向 从Z轴正方向往负方向看
        //      x+ 场景往右     意味着，摄像机往X-
        //      y+ 场景往上     意味着，摄像机往Y-
        //      z+ 场景靠近     意味着，摄像机往Z-
#if 0
        glTranslatef(1, 1, -50);
#else
        glTranslatef(0, 0, -50);
#endif
        
        // 关于旋转
        // 1. glRotatef的后3个参数，表示一个向量，作为旋转的中心轴。  101 和 100+001 是不同的
        //      因为旋转是以3维向量为旋转轴，这个向量必然穿过原点，所以旋转轴一定是穿过原点的。
        // 2. 旋转，其实不是坐标轴旋转，而是摄像机旋转 也就是GL_MODELVIEW矩阵
        //      旋转的方向，是从旋转轴正方形往负方向看，逆时针旋转
        // 3. 自转 自转就是旋转 只不过维护了一个角度变量，每次进来，都递增这个变量，达到自转的目的
        //      如果这个变量一直不变，画面其实是静止的。
        //      每次绘制，都比前一次绘制多转一点角度，从而视觉上达到自转的目的
        // 4. 公转 
        //      因为场景在自转，如果另外一个物体，相对于场景旋转轴上的物体位置是固定的，那么就会随着场景转，达到公转的视觉效果
        //      为了让公转更真实，可以将物体额外做一个旋转
#if 0
        glRotatef(45, 1, 0, 1);
#else
        //glRotatef(45, 1, 0, 0);
        glRotatef(45, 0, 0, 1);
#endif
        //动态的效果  其实是一张一张图叠加起来的
        // 我先用Z轴旋转45° 画一个正方形
        //生成第一张图
        drawRect();
        
        //生成第2张图
        glScalef(0.5, 0.5, 0.5);
        glRotatef(-90, 0, 0, 1);
        glRotatef(45, 1, 0, 0);
        drawRect();//到这里可以看到，之前画的图并没有受影响，只是把第二张图叠上去了。

        //生成第3张图
        glLoadIdentity();//我在这清空一下
        glTranslatef(0, 0, -50);
        glScalef(1.5, 1.5, 1.5);
        glRotatef(15, 0, 0, 1);
        drawRect();//可以看到 其实是3图叠加在一起了。  可以理解为 画完一个东西后，一张图生成了，画布就清空了。

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

