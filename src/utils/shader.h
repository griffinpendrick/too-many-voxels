#pragma once

#include <sstream>
#include <fstream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "common.h"

struct Shader 
{
    Shader(const char* vertex, const char* fragment);

    void Use();
    void SetMat4(const char* name, const glm::mat4& mat) const;
    void SetVec3(const char* name, const glm::vec3& vec) const;
    void SetInt(const char* name, s32 value) const;

    u32 ID;
};
