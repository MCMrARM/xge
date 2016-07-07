#pragma once

namespace xge {

    enum class ShaderValueType {
        Float, Vec2, Vec3, Vec4, Int, IVec2, IVec3, IVec4, UInt, UIVec2, UIVec3, UIVec4,
        Matrix2, Matrix3, Matrix4, Sampler2D
    };
    static int GetShaderValueTypeComponentCount(ShaderValueType type) {
        switch(type) {
            case ShaderValueType::Float:
            case ShaderValueType::Int:
            case ShaderValueType::UInt:
            case ShaderValueType::Sampler2D:
                return 1;
            case ShaderValueType::Vec2:
            case ShaderValueType::IVec2:
            case ShaderValueType::UIVec2:
                return 2;
            case ShaderValueType::Vec3:
            case ShaderValueType::IVec3:
            case ShaderValueType::UIVec3:
                return 3;
            case ShaderValueType::Vec4:
            case ShaderValueType::IVec4:
            case ShaderValueType::UIVec4:
                return 4;
            case ShaderValueType::Matrix2:
                return 2*2;
            case ShaderValueType::Matrix3:
                return 3*3;
            case ShaderValueType::Matrix4:
                return 4*4;
        }
        return 0;
    }

    template <typename T>
    ShaderValueType ShaderValueTypeFromBaseType();

}