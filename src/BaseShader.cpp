#include "BaseShader.h"

BaseShader::BaseShader(const std::string& vertPath, const std::string& fragPath)
    : m_vertPath(vertPath), m_fragPath(fragPath)
{
    std::string vertexShader = ParseShader(vertPath);
    std::string fragmentShader = ParseShader(fragPath);
    m_RendererID = CreateShader(vertexShader, fragmentShader);
}

BaseShader::BaseShader(const std::string& vertPath, const std::string& geomPath, const std::string& fragPath)
    : m_vertPath(vertPath), m_geoPath(geomPath), m_fragPath(fragPath)
{
    std::string vertexShader = ParseShader(vertPath);
    std::string geometryShader = ParseShader(geomPath);
    std::string fragmentShader = ParseShader(fragPath);
    m_RendererID = CreateShader(vertexShader, geometryShader, fragmentShader);
}

unsigned int BaseShader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    std::vector<unsigned int> shaders = {vs, fs};
    return CreateShaderProgram(shaders);
}

unsigned int BaseShader::CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader) {
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    std::vector<unsigned int> shaders = {vs, gs, fs};
    return CreateShaderProgram(shaders);
}