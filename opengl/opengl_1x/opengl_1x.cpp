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

struct pointCoord {
    CELL::float3 coord;
    uint8_t r, g, b, a;
};

class sampleWindow : public openglWindow {
private:

    RECT glRect;
    GLContext glc;

    uint32_t m_textureId[8] = {0};
    uint8_t m_texNum = 1;
    const uint8_t* m_textureSrc[8] = {
        (const uint8_t*)"C:\\Users\\42033\\Pictures\\image.png",
    };

    uint32_t m_vbo[8] = {0};
    uint8_t  m_vboNum = 1;

    uint32_t m_ebo[8] = { 0 };
    uint8_t  m_eboNum = 1;
    uint8_t  m_element[32] = {
        0, 1, 2,
        0, 1, 3,
        1, 2, 3,
        0, 2, 3
    };
    static const uint8_t m_pointCnt = 6;
    pointCoord m_point[m_pointCnt] = {
        { {-1, 0, -1}, 255, 0, 0, 1},
        { {1, 0, -1}, 0, 255, 0, 1},
        { {0, 1, -1}, 0, 0, 255, 1},
        { {0, 0.5, 0}, 255, 255, 255},
        { {0, 0.2, 0}, 255, 255, 255},
        { {0, 0.8, 0}, 255, 255, 255},
    };
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

        //opengl 
        glViewport(glRect.left, glRect.top, glRect.right, glRect.bottom);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();


        for (int i = 0; i < m_vboNum; i++) {
            m_vbo[i] = createVBO();
        }

        for (int i = 0; i < m_eboNum; i++) {
            m_ebo[i] = createEBO();
        }
#if 01
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[0]);
        glVertexPointer(3, GL_FLOAT, sizeof(pointCoord), (float*)0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(pointCoord), (float*)12);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#else
        glVertexPointer(3, GL_FLOAT, sizeof(pointCoord), &m_point[0].s.x);
        glEnableClientState(GL_VERTEX_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(pointCoord), &m_point[0].r);
        glEnableClientState(GL_COLOR_ARRAY);
#endif
    }

    uint32_t createVBO() {
        uint32_t vboId;

        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_point), m_point, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return vboId;
    }

    uint32_t createEBO() {
        uint32_t eboId;

        glGenBuffers(1, &eboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_element), m_element, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return eboId;
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

    void render() override {
        glClearColor(0.3, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 01
        glOrtho(glRect.left, glRect.right, glRect.top, glRect.bottom,  -1000, 1000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(glRect.right / 2, glRect.bottom / 2, 0);
#else
        gluPerspective(45, 16.0 / 9, 1, 101);
#endif

#if 01
        //glBufferSubData(GL_ARRAY_BUFFER, 0, m_pointCnt / 3, m_point);
        
        uint8_t point = 3 + rand() % 2;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[0]);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 5, 1, &point);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 8, 1, &point);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 11, 1, &point);
#endif
        glMatrixMode(GL_MODELVIEW);
        glScaled(100, 100, 100);
        
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, 0);//没有绑定EBO 第四个参数可以直接填数组

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

