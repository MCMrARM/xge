#pragma once

namespace xge {

    union ShaderValue {
        float f;
        int i;
        unsigned int ui;
    };

    static inline ShaderValue AsShaderValue(float f) {
        ShaderValue val;
        val.f = f;
        return val;
    }
    static inline ShaderValue AsShaderValue(int i) {
        ShaderValue val;
        val.i = i;
        return val;
    }
    static inline ShaderValue AsShaderValue(unsigned int i) {
        ShaderValue val;
        val.ui = i;
        return val;
    }

}