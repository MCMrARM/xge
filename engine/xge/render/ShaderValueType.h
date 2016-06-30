#pragma once

namespace xge {

    enum class ShaderValueType {
        Float, Vec2, Vec3, Vec4, Int, IVec2, IVec3, IVec4, UInt, UIVec2, UIVec3, UIVec4,
        Matrix2x2, Matrix3x3, Matrix4x4
    };
    static int GetShaderValueTypeComponentCount(ShaderValueType type) {
        switch(type) {
            case ShaderValueType::Float:
            case ShaderValueType::Int:
            case ShaderValueType::UInt:
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
            case ShaderValueType::Matrix2x2:
                return 2*2;
            case ShaderValueType::Matrix3x3:
                return 3*3;
            case ShaderValueType::Matrix4x4:
                return 4*4;
        }
        return 0;
    }

}