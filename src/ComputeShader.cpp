#include "ComputeShader.h"

ComputeShader::ComputeShader(const std::string& shaderPath)
    : m_shaderPath(shaderPath)
{
    std::string computeShader = ParseShader(shaderPath);
    m_RendererID = CreateShader(computeShader);
}

void ComputeShader::Dispatch(unsigned int x, unsigned int y, unsigned int z) const {
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

unsigned int ComputeShader::CreateShader(const std::string& computeShader) {
    unsigned int cs = CompileShader(GL_COMPUTE_SHADER, computeShader);

    std::vector<unsigned int> shaders = {cs};
    return CreateShaderProgram(shaders);
}