#include "ShaderValueType.h"
#include <glm/glm.hpp>

namespace xge {

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<float>() {
        return ShaderValueType::Float;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<glm::vec2>() {
        return ShaderValueType::Vec2;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<glm::vec3>() {
        return ShaderValueType::Vec3;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<glm::vec4>() {
        return ShaderValueType::Vec4;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<int>() {
        return ShaderValueType::Int;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<glm::ivec3>() {
        return ShaderValueType::IVec3;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<glm::ivec4>() {
        return ShaderValueType::IVec4;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<glm::mat2>() {
        return ShaderValueType::Matrix2;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<glm::mat3>() {
        return ShaderValueType::Matrix3;
    }

    template <>
    ShaderValueType ShaderValueTypeFromBaseType<glm::mat4>() {
        return ShaderValueType::Matrix4;
    }

}