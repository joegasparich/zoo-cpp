#pragma once

#include <iostream>
#include <unordered_map>

#include "glm/glm.hpp"

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void bind() const;
    void unbind() const;

    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);
    void setUniform2f(const std::string& name, glm::vec2& value);
    void setUniform3f(const std::string& name, glm::vec3& value);
    void setUniform4f(const std::string& name, glm::vec4& value);

    void setUniformMat3f(const std::string& name, const glm::mat3& matrix);
    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
    std::string parseShader(std::string filePath);
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int getUniformLocation(const std::string& name) const;

    unsigned int m_rendererID;
    mutable std::unordered_map<std::string, int> m_uniformLocationCache;
};
