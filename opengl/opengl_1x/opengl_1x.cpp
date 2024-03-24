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
#include <map>
#include "freeimage.h"
#include "PixelBuffer.h"

struct pointCoord {
    CELL::float3 coord;
    CELL::float2 uv;
    CELL::float4 color;
};

class sampleWindow : public openglWindow {
private:

    RECT glRect;
    GLContext glc;
    PixelBuffer m_pixelBuffer;

    uint32_t m_textureId[8] = {0};
    uint8_t m_texNum = 1;
    const uint8_t* m_textureSrc[8] = {
        (const uint8_t*)"C:\\ccli\\resource\\image\\num.png",
    };

    uint32_t m_tempTex;

public:
    ~sampleWindow() {
    }
    virtual void openglUninit() override{
        glDeleteTextures(m_texNum, m_textureId);
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
       
        m_pixelBuffer.setup(hWnd, glc.hdc(), glc.hglrc(), glRect.right, glRect.bottom);
        glc.makeCurrent();
        
        glViewport(glRect.left, glRect.top, glRect.right, glRect.bottom);

        glEnable(GL_TEXTURE_2D);
        for (int i = 0; i < m_texNum; i++) {
            m_textureId[i] = createTexture();
            textureImage(i);
        }
        
        m_tempTex = createTexture();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            glRect.right, glRect.bottom, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }

    bool textureImage(int indexs) {
        const char* fileName = (const char*)m_textureSrc[indexs];
        //1 获取图片格式
        FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName, 0);
        if (fifmt == FIF_UNKNOWN)
        {
            return  false;
        }
        //2 加载图片
        FIBITMAP* dib = FreeImage_Load(fifmt, fileName, 0);

        FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType(dib);

        //! 获取数据指针
        FIBITMAP* temp = dib;
        dib = FreeImage_ConvertTo32Bits(dib);
        FreeImage_Unload(temp);

        BYTE* pixels = (BYTE*)FreeImage_GetBits(dib);
        int     width = FreeImage_GetWidth(dib);
        int     height = FreeImage_GetHeight(dib);


        for (int i = 0; i < width * height * 4; i += 4)
        {
            BYTE temp = pixels[i];
            pixels[i] = pixels[i + 2];
            pixels[i + 2] = temp;
        }
        glBindTexture(GL_TEXTURE_2D, m_textureId[indexs]);//将texID绑定  意味着之后的GL_TEXTURE_2D操作，都是对texId进行
#if 1
        // 前三个参数  纹理的类型。
        // 后面的参数  输入图片的参数   边框参数，应该被设置为0
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            return      false;
        }
#else
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
#endif
        FreeImage_Unload(dib);
        return      true;
    }

    uint32_t createTexture() {
        uint32_t texId;

        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);//将texID绑定  意味着之后的GL_TEXTURE_2D操作，都是对texId进行

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//测试放大  搞个小分辨率图片
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//测试缩小  搞个大分辨率图片

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        return texId;
    }
    void renderImg(GLuint tex, bool bRot) {
        glClearColor(0.3, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(glRect.left, glRect.right, glRect.top, glRect.bottom, -1000, 1000);

        pointCoord point[] = {
            { {glRect.left, glRect.bottom, -1}, {0, 0}, {1, 0, 0, 1} },
            { {glRect.left, glRect.top, -1}, {0, 1}, {0, 1, 0, 1} },
            { {glRect.right, glRect.top, -1}, {1, 1}, {0, 0, 1, 1} },
            { {glRect.right, glRect.bottom, -1}, {1, 0}, {1, 1, 1, 1} },
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(pointCoord), &point[0].coord.x);
        glTexCoordPointer(2, GL_FLOAT, sizeof(pointCoord), &point[0].uv.x);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
        glDrawArrays(GL_QUADS, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    void renderRect(){
        glClearColor(0.3, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(glRect.left, glRect.right, glRect.top, glRect.bottom, -1000, 1000);

        pointCoord point[] = {
            { {0, glRect.bottom, -1}, {0, 0}, {1, 0, 0, 1} },
            { {0, glRect.top, -1}, {0, 1}, {0, 1, 0, 1} },
            { {glRect.right, glRect.top, -1}, {1, 1}, {0, 0, 1, 1} },
            { {glRect.right, glRect.bottom, -1}, {1, 0}, {(float)(rand()%255)/255, (float)(rand() % 255) / 255, (float)(rand() % 255) / 255, 1} },
        };

        glVertexPointer(3, GL_FLOAT, sizeof(pointCoord), &point[0].coord.x);
        glColorPointer(4, GL_FLOAT, sizeof(pointCoord), &point[0].color.x);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDrawArrays(GL_QUADS, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    void render() override {
        m_pixelBuffer.makeCurrent();
        //renderRect();
        renderImg(m_textureId[0], false);

        glBindTexture(GL_TEXTURE_2D, m_tempTex);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_pixelBuffer._width, m_pixelBuffer._height);

        glc.makeCurrent();

        renderImg(m_tempTex, false);

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

