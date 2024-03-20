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
#include "freeimage.h"

class sampleWindow : public openglWindow {
private:

    RECT glRect;
    GLContext glc;

    uint32_t m_textureId[8] = {0};
    const uint8_t* m_textureSrc[8] = {
        (const uint8_t*)"D:\\lr\\code\\res\\1.bmp",
        (const uint8_t*)"D:\\lr\\code\\res\\2.bmp",
        (const uint8_t*)"D:\\lr\\code\\res\\3.bmp",
        (const uint8_t*)"D:\\lr\\code\\res\\4.bmp",
        (const uint8_t*)"D:\\lr\\code\\res\\5.bmp",
        (const uint8_t*)"D:\\lr\\code\\res\\5.bmp",
    };
    uint8_t* m_textureRandom;
    uint32_t m_randomTextureWidth = 128, m_randomTextureHeight = 160;
public:
    ~sampleWindow() {
    }
    virtual void openglUninit() override{
        glDeleteTextures(6, m_textureId);
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
        
        for (int i = 0; i < 6; i++) {
            m_textureId[i] = createTexture();
            textureImage(i);
        }

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
        glBindTexture(GL_TEXTURE_2D, m_textureId[0]);//将texID绑定  意味着之后的GL_TEXTURE_2D操作，都是对texId进行
#if 1
        // 前三个参数  纹理的类型。
        // 后面的参数  输入图片的参数   边框参数，应该被设置为0
        if (indexs == 0) {
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, indexs, GL_RGBA,
                width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                return      false;
            }
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//测试缩小  搞个大分辨率图片

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        return texId;
    }

    void render() override {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
#if 01
        glOrtho(glRect.left, glRect.right, glRect.top, glRect.bottom,  -1000, 1000);
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
            {0, 0,      -200, -200, -200},
            {0, 1,     -200, 200, -200},
            {1, 1,    200, 200, -200},
            {1, 0,     200, -200, -200},

            {0, 0,      210, 0, 210},
            {0, 1,      210, 200, 210},
            {1, 1,      410, 200, 210},
            {1, 0,      410, 0, 210},

            {0, 0,      10 ,- 10, -10, },
            {0, 1,      10 ,- 10, 10,  },
            {1, 1,      10, 10, 10,   },
            {1, 0,      10, 10, -10,  },

            {0, 0,      -10 ,-10, -10, },
            {0, 1,      -10 ,-10, 10,  },
            {1, 1,      -10, 10, 10,   },
            {1, 0,      -10, 10, -10,  },

            {0, 0,      -10,-10 , -10, },
            {0, 1,      -10,-10 , 10,  },
            {1, 1,      10, -10, 10,   },
            {1, 0,      10, -10, -10,  },

            {0, 0,      -10, 10 , -10, },
            {0, 1,      -10, 10 , 10,  },
            {1, 1,      10,  10, 10,   },
            {1, 0,      10,  10, -10,  },
        };
#if 0
        glInterleavedArrays(GL_C4UB_V3F, sizeof(pointInfo), renderPoint);
#else
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(pointInfo), &renderPoint[0].x);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(pointInfo), &renderPoint[0].u);
#endif

        glBindTexture(GL_TEXTURE_2D, m_textureId[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glDrawArrays(GL_QUADS, 0, 4);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glDrawArrays(GL_QUADS, 4, 4);

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

