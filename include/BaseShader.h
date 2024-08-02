#pragma once
#include "Shader.h"


class BaseShader : public Shader {
private:
    std::string m_vertPath;
    std::string m_fragPath;
    std::string m_geoPath;

public:
    BaseShader(const std::string& vertPath, const std::string& fragPath);
    BaseShader(const std::string& vertPath, const std::string& geoPath, const std::string& fragPath);
    ~BaseShader() override = default;

private:
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader);
};
