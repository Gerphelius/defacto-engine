#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace DF::Math
{
    using vec2 = glm::vec2;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;

    using mat3 = glm::mat3;
    using mat4 = glm::mat4;

    inline float degToRad(float degrees)
    {
        return glm::radians(degrees);
    }

    inline mat4 translateMat4(const mat4& mat, const vec3& location)
    {
        return glm::translate(mat, location);
    }

    inline mat4 rotateMat4(const mat4& mat, float angle, const vec3& axis)
    {
        return glm::rotate(mat, angle, axis);
    }

    inline mat4 perspective(float fov, float aspectRatio, float near, float far)
    {
        return glm::perspective(fov, aspectRatio, near, far);
    }
}
