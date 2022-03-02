#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>

#include "Shader.h"
#include "Renderer.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
 : m_rendererID{0} {
    auto vertexSource = parseShader(vertexPath);
    auto fragmentSource = parseShader(fragmentPath);

    m_rendererID = createShader(vertexSource, fragmentSource);
}

Shader::~Shader() {
    GL_CALL(glDeleteProgram(m_rendererID));
}

void Shader::bind() const {
    GL_CALL(glUseProgram(m_rendererID));
}

void Shader::unbind() const {
    GL_CALL(glUseProgram(0));
}

void Shader::setUniform1i(const std::string &name, int value) {
    GL_CALL(glUniform1i(getUniformLocation(name), value));
}

void Shader::setUniform1f(const std::string &name, float value) {
    GL_CALL(glUniform1f(getUniformLocation(name), value));
}

void Shader::setUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
    GL_CALL(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniformMat4f(const std::string &name, const glm::mat4 &matrix) {
    GL_CALL(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

unsigned int Shader::getUniformLocation(const std::string &name) {
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) {
        return m_uniformLocationCache[name];
    }

    GL_CALL(int location = glGetUniformLocation(m_rendererID, name.c_str()));
    if (location == -1) {
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    }
    m_uniformLocationCache[name] = location;

    return location;
}

std::string Shader::parseShader(std::string filePath) {
    std::ifstream stream(filePath);

    if (!stream.is_open()) {
        std::cout << "Could not open path " << filePath << std::endl;
    }

    std::string line;
    std::stringstream shaderSource;

    while (getline(stream, line)) {
        shaderSource << line << "\n";
    }

    return shaderSource.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
