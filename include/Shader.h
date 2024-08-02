#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "Types.h"

class Shader {
protected:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;

public:
    Shader();
    virtual ~Shader();

    void Bind() const;
    void Unbind() const;
    unsigned int GetProgramID() const;

    // Set Uniforms
    void SetUniform(const std::string& name, const glm::vec4& vector);
    void SetUniform(const std::string& name, const glm::vec3& vector);
    void SetUniform(const std::string& name, const glm::vec2& vector);
    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, double value);
    void SetUniform(const std::string& name, int value);
    void SetUniform(const std::string& name, unsigned int value);
    void SetUniform(const std::string& name, bool value);
    void SetUniform(const std::string& name, const glm::mat4& matrix);
    void SetUniform(const std::string& name, const glm::mat4& matrix, bool transpose);

protected:
    std::string ParseShader(const std::string& filePath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShaderProgram(const std::vector<unsigned int>& shaders);
    unsigned int GetUniformLocation(const std::string& name);
};