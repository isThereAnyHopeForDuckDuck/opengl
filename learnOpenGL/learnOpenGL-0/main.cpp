#include <glad/glad.h>

#include <memory>

#include "openglWindow.h"
#include "shader.h"

struct glPoint {
    float x, y, z;
    float r, g, b, a;
};

class glWin : public openglWindow {
private:
    void compileShader(uint32_t& shaderId, unsigned int shaderType, const char** pSourceCode) {
        shaderId = glCreateShader(shaderType);
        glShaderSource(shaderId, 1, pSourceCode, NULL);
        glCompileShader(shaderId);
    }

    bool compileResult(int shaderId) {
        int  success;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            return false;
        }
        return true;
    }

    bool link(uint32_t& programId, uint32_t vertexShader, uint32_t fragShader) {
        programId = glCreateProgram();
        glAttachShader(programId, vertexShader);
        glAttachShader(programId, fragShader);
        glLinkProgram(programId);

        int  success;
        char infoLog[512];
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::LINK_FAILED\n" << infoLog << std::endl;
            return false;
        }

        return true;
    }

public:

    bool	onInit() override {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        glViewport(0, 0, 800, 600);

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

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return true;
    }

    void	onUninit() override {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

    void doRender() override {
        static float greenValue = 0;
        greenValue += 0.003;

        if (greenValue > 1) {
            greenValue = 0;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_shader->setFloat("greenValue", greenValue);
        m_shader->use();

        glBindVertexArray(m_vao);
        if (m_useEbo) {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glBindVertexArray(0);
    }

private:
    glPoint m_pointData[4] = {
        { 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},   // 右上角
        { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},   // 右下角
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},   // 左下角
        {-0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},   // 左上角
    };

    uint8_t indices[6] = {
        // 注意索引从0开始! 
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    uint32_t m_vbo;
    uint32_t m_vao;
    uint32_t m_ebo;
    bool     m_useEbo = true;

    std::shared_ptr<Shader> m_shader;
};

int main(int argc, char** argv) {

    glWin mainWin;

    if (mainWin.createWindow(800, 600) ) {
        mainWin.run();
    }

    return 0;
}


