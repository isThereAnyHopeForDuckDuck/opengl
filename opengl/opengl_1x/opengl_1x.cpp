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

    uint32_t m_textureId;
    uint8_t* m_textureRandom;
    uint32_t m_randomTextureWidth = 128, m_randomTextureHeight = 160;
public:
    ~sampleWindow() {
        delete[] m_textureRandom;
    }
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

        //opengl 
        glViewport(glRect.left, glRect.top, glRect.right, glRect.bottom);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_TEXTURE_2D);
        m_textureId = createTexture();

        //random 
        m_textureRandom = new uint8_t[m_randomTextureWidth * m_randomTextureHeight * 3];

        for (int i = 0; i < m_randomTextureWidth * m_randomTextureHeight * 3; i++) {
            m_textureRandom[i] = i % 255;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_randomTextureWidth, m_randomTextureHeight, 0, 
            GL_RGB, GL_UNSIGNED_BYTE, m_textureRandom);
    }

    uint32_t createTexture() {
        uint32_t texId;

        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);//将texID绑定  意味着之后的GL_TEXTURE_2D操作，都是对texId进行

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        return texId;
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
            float u, v;
            float x, y, z;
        };
            
        pointInfo renderPoint[] = {
            {0, 0,      -100, -100, 0},
            {0, 1,      -100, 100, 0},
            {1, 1,      100, 100, 0},
            {1, 0,      100, -100, 0},
        };
#if 0
        glInterleavedArrays(GL_C4UB_V3F, sizeof(pointInfo), renderPoint);
#else
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(pointInfo), &renderPoint[0].x);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(pointInfo), &renderPoint[0].u);
#endif
        glBindTexture(GL_TEXTURE_2D, m_textureId);

        glDrawArrays(GL_QUADS, 0, 4);
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

