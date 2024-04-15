#include <glad/glad.h>

#include <memory>

#include "openglWindow.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct glPoint {
    float x, y, z;
    float r, g, b, a;
    float u, v;
};

class glWin : public openglWindow {

public:

    bool	onInit() override {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        glViewport(0, 0, 1280, 720);

        //vbo
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_pointData), m_pointData, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //ebo
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //shader
        m_shader = std::make_shared<Shader>("D:\\lr\\code\\CG\\learnOpenGL\\learnOpenGL-0\\vertex.glsl", "D:\\lr\\code\\CG\\learnOpenGL\\learnOpenGL-0\\frag.glsl");

        //tex
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);

        for (int i = 0; i < sizeof(m_tex) / sizeof(float); i++) {
            uint8_t* data = stbi_load(m_texPath[i], &width, &height, &channels, 0);
            if (nullptr == data) {
                std::cout << __LINE__ << " stbi load err " << std::endl;
            }

            glGenTextures(1, &m_tex[i]);
            glBindTexture(GL_TEXTURE_2D, m_tex[i]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            if (channels == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            else {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);
        }

        for (int i = 0; i < 6; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, m_tex[i]);
        }

        //vao
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        //顶点属性位置
        //维度
        //顶点数据类型
        //是否标准化
        //stride
        //offset
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glPoint), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glPoint), (void*)(3*sizeof(float)) );
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glPoint), (void*)(7 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //matrix
        m_unitMat = glm::mat4(1.0f);

        glEnable(GL_DEPTH_TEST);

        return true;
    }

    void	onUninit() override {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

    void doRender() override {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        
        glBindVertexArray(m_vao);
        if (m_useEbo) {
            //static float angle = -45;
            //angle += 0.5;

            //glm::mat4 trans;
            //trans = glm::rotate(m_unitMat, glm::radians(angle), glm::vec3(1.0, 1.0, 1.0));

            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(5.0f, 5.0f, -13.0f));
            unsigned int transformLoc = glGetUniformLocation(m_shader->ID, "model");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

            glm::mat4 view(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            transformLoc = glGetUniformLocation(m_shader->ID, "view");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(view));

            glm::mat4 projection(1.0f);
            projection = glm::perspective<float>(glm::radians(45.0f), 16 / 9, 0.1f, 100.0f);
            transformLoc = glGetUniformLocation(m_shader->ID, "projection");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

            int quadCnt = 6;
            for (int i = 0; i < quadCnt; i++) {
                glBindTexture(GL_TEXTURE_2D, m_tex[i]);
                glUniform1i(glGetUniformLocation(m_shader->ID, "texIdx"), i);
                glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
            }
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glBindVertexArray(0);
    }

private:
    glPoint m_pointData[24] = {

        { 0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f, 1.0f,  1, 0},   // 右下角
        { 0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f, 1.0f,  1, 1},   // 右上角
        {-0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f, 1.0f,  0, 0},   // 左下角
        {-0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f, 1.0f,  0, 1},   // 左上角

        { 0.5f, -0.5f,  -0.5f,    0.0f, 1.0f, 0.0f, 1.0f,  1, 0},   // 右下角
        { 0.5f,  0.5f,  -0.5f,    1.0f, 0.0f, 0.0f, 1.0f,  1, 1},   // 右上角
        {-0.5f, -0.5f,  -0.5f,    0.0f, 0.0f, 1.0f, 1.0f,  0, 0},   // 左下角
        {-0.5f,  0.5f,  -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,  0, 1},   // 左上角

        { 0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f, 1.0f,  1, 0},   // 右下角
        { 0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f, 1.0f,  1, 1},   // 右上角
        { 0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f, 1.0f,  0, 0},   // 左下角
        { 0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f, 1.0f,  0, 1},   // 左上角

        {-0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f, 1.0f,  1, 0},   // 右下角
        {-0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f, 1.0f,  1, 1},   // 右上角
        {-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f, 1.0f,  0, 0},   // 左下角
        {-0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f, 1.0f,  0, 1},   // 左上角

        { 0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f, 1.0f,  1, 0},   // 右下角
        { 0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f, 1.0f,  1, 1},   // 右上角
        {-0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f, 1.0f,  0, 0},   // 左下角
        {-0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f, 1.0f,  0, 1},   // 左上角

        { 0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f, 1.0f,  1, 0},   // 右下角
        { 0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f, 1.0f,  1, 1},   // 右上角
        {-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f, 1.0f,  0, 0},   // 左下角
        {-0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f, 1.0f,  0, 1},   // 左上角
    };

    uint8_t indices[24] = {
        1, 0, 2, 3,
    };

    uint32_t m_vbo;
    uint32_t m_vao;
    uint32_t m_ebo;
    uint32_t m_tex[6];
    const char* m_texPath[6] = {
        "D:\\lr\\code\\resource\\1.png",
        "D:\\lr\\code\\resource\\2.png",
        "D:\\lr\\code\\resource\\3.png",
        "D:\\lr\\code\\resource\\4.png",
        "D:\\lr\\code\\resource\\5.png",
        "D:\\lr\\code\\resource\\6.png"
    };
    uint32_t m_texUserMapmip;
    
    bool     m_useEbo = true;

    std::shared_ptr<Shader> m_shader;

    glm::mat4 m_unitMat;
};

int main(int argc, char** argv) {

    glWin mainWin;

    if (mainWin.createWindow(1280, 720) ) {
        mainWin.run();
    }

    return 0;
}


