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
#include "FrameBufferObject.h"
#include "time.h"
struct pointCoord {
    CELL::float3 coord;
    CELL::float2 uv;
    CELL::float4 color;
};

class sampleWindow : public openglWindow {
private:

    //RECT glRect;
    GLContext glc;
    PixelBuffer m_pixelBuffer;
    FrameBufferObject m_fbo;
    uint32_t m_pbo[2];
    uint8_t m_toPbo = 0, m_toCpu = 1;

    int m_x, m_y, m_w, m_h;

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
    virtual void onResize() override{
        RECT cliRect;
        GetClientRect(m_hWnd, &cliRect);
        //这个坐标是图片坐标
        m_x = cliRect.left;
        m_y = cliRect.top;
        m_w = cliRect.right;
        m_h = cliRect.bottom;
        glViewport(m_x, m_y, m_w, m_h);
    }
    sampleWindow(HINSTANCE hInstance, int nCmdShow) :openglWindow(hInstance, nCmdShow) {

        HWND hWnd = m_hWnd;

        glc.setup(hWnd, GetDC(hWnd));
        glewInit();
        wglSwapIntervalEXT(1);
       
        //m_pixelBuffer.setup(hWnd, glc.hdc(), glc.hglrc(), glRect.right, glRect.bottom);
        m_fbo.setup(640, 480);
        glc.makeCurrent();
        
        RECT cliRect;
        GetClientRect(hWnd, &cliRect);
        //这个坐标是图片坐标
        m_x = cliRect.left;
        m_y = cliRect.top;
        m_w = cliRect.right;
        m_h = cliRect.bottom;
        glViewport(m_x, m_y, m_w, m_h);

        glEnable(GL_TEXTURE_2D);
        for (int i = 0; i < m_texNum; i++) {
            m_textureId[i] = createTexture();
            textureImage(i);
        }
        
        m_tempTex = createTexture();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            640, 480, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


        glGenBuffers(2, m_pbo);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[0]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 1920 * 1080 * 4, 0, GL_STREAM_READ);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[1]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 1920 * 1080 * 4, 0, GL_STREAM_READ);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

    bool    save(int w, int h, char* data, size_t length)
    {

        FIBITMAP* bitmap = FreeImage_Allocate(w, h, 32, 0, 0, 0);

        BYTE* pixels = (BYTE*)FreeImage_GetBits(bitmap);

        memcpy(pixels, data, w * h * 4);
        bool    bSuccess = FreeImage_Save(FIF_PNG, bitmap, "C:\\ccli\\resource\\image\\pbo.png", PNG_DEFAULT);

        FreeImage_Unload(bitmap);
        return  bSuccess;
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
    void renderImg(GLuint tex, int x, int y, int w, int h, float grey) {
        glClearColor(grey, grey, grey, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(m_x, m_w, m_y, m_h, -1000, 1000);

        x += m_x;
        y += m_y;
        pointCoord point[] = {
            { {x, y, -1},               {0, 0}, {1, 0, 0, 1} },
            { {x, y + h, -1},         {0, 1}, {0, 1, 0, 1} },
            { {x + w, y + h, -1},   {1, 1}, {0, 0, 1, 1} },
            { {x + w, y, -1},         {1, 0}, {1, 1, 1, 1} },
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
    void renderRect(int x, int y, int w, int h){
        glClearColor(0.3, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(m_x, m_w, m_y, m_h, -1000, 1000);

        x += m_x;
        y += m_y;
        pointCoord point[] = {
            { {x, y, -1},               {0, 0}, {1, 0, 0, 1} },
            { {x, y+h, -1},         {0, 1}, {0, 1, 0, 1} },
            { {x+w, y+h, -1},   {1, 1}, {0, 0, 1, 1} },
            { {x+w, y, -1},         {1, 0}, {1, 1, 1, 1} },
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
        glc.makeCurrent();

        clock_t start_time, end_time;
        double bindTime, readTime, mapTime;
        
        renderRect(10, 10, m_w-20, m_h-20);

        //glFinish();

        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[m_toPbo]);
        glReadPixels(10, 10, m_w-20, m_h-20, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[m_toCpu]);
        start_time = clock(); //获取开始执行时间
        uint8_t* data = (uint8_t*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
        end_time = clock(); //获取结束时间
        mapTime = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        if (data) {
            save(m_w - 20, m_h - 20, (char *)data, (m_w - 20) * (m_h - 20));
        }
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        std::swap(m_toCpu, m_toPbo);
        /*
        *   map确实比较耗时间  (map 是为了等glReadPixels的操作，所以才比较耗时，使用glFinsh()阻塞执行glReadPixels的操作
                就可以看出来)
        *   glReadPixels 帧缓冲-> PBO ， 异步的
        *   glMapBuffer PBO->CPU  ， 单线程下 ,可以保证得到的数据，是完整的
        *   
        *   乒乓球操作的思路
        *   1. glReadPixels是比较耗时的，所以调用完glReadPixels之后，就执行其他操作
        *   2. 对上一个glReadPixels的结果，进行map操作，此时，glReadPixels操作大概率已经完成，如果没有，阻塞时间也会很短
        *       这样就需要2个PBO
        *   3.这种方式，map几乎没有阻塞，说明在map时，glReadPixels的操作已经完成了
        *   其实时间还是一样的，只是没有阻塞了，时间用在别的地方。
        */
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

