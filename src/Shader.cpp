#include "Shader.h"
#include "Types.h"

Shader::Shader() : m_RendererID(0) {}

Shader::~Shader() {
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const {
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

unsigned int Shader::GetProgramID() const {
    return m_RendererID;
}

void Shader::SetUniform(const std::string& name, const glm::vec4& vector) {
    glUniform4f(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
}

void Shader::SetUniform(const std::string& name, const glm::vec3& vector) {
    glUniform3f(GetUniformLocation(name), vector.x, vector.y, vector.z);
}

void Shader::SetUniform(const std::string& name, const glm::vec2& vector) {
    glUniform2f(GetUniformLocation(name), vector.x, vector.y);
}

void Shader::SetUniform(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, double value) {
    glUniform1d(GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, unsigned int value) {
    glUniform1ui(GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, bool value) {
    glUniform1i(GetUniformLocation(name), value ? 1 : 0);
}

void Shader::SetUniform(const std::string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform(const std::string& name, const glm::mat4& matrix, bool transpose) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
}

std::string Shader::ParseShader(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
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
        std::cout << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : type == GL_FRAGMENT_SHADER ? "fragment" : "compute")
                  << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShaderProgram(const std::vector<unsigned int>& shaders) {
    unsigned int program = glCreateProgram();

    for (unsigned int shader : shaders) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    glValidateProgram(program);

    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        std::cout << "Failed to link shader program!" << std::endl;
        std::cout << message << std::endl;
    }

    for (unsigned int shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    return program;
}

unsigned int Shader::GetUniformLocation(const std::string& name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

    m_UniformLocationCache[name] = location;
    return location;
}