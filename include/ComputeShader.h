#pragma once
#include "Shader.h"


class ComputeShader : public Shader {
private:
    std::string m_shaderPath;

public:
    ComputeShader(const std::string& shaderPath);
    ~ComputeShader() override = default;

    void Dispatch(unsigned int x, unsigned int y, unsigned int z) const;

private:
    unsigned int CreateShader(const std::string& computeShader);
};
