#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include "ShaderValue.h"
#include "ShaderValueType.h"

namespace xge {

    class Texture;

    enum class ShaderType {
        Vertex, Fragment,
        // gles 3.1+
        Compute,
        // gles 3.2+
        TessControl, TessEvaluation, Geometry
    };

    struct ShaderAttribute {
        int attributeId;
        ShaderValueType type;
    };
    struct ShaderUniform {
        int uniformId;
        ShaderValueType type;

        /**
         * Before setting this uniform, first make sure the owner program is bound.
         */
        void set(ShaderValue *value, int count = 1);
        void set(Texture *value);
    };

    class Shader {

    protected:
        const ShaderType type;
        unsigned int id = 0;

    public:
        Shader(ShaderType type);
        Shader(const Shader &) = delete;
        Shader(Shader &&s) : type(s.type), id(s.id) {
            s.id = 0;
        }
        ~Shader();

        /**
         * This returns the OpenGL shader id.
         */
        inline unsigned int getId() const {
            return id;
        }

        /**
         * Those functions will set the Shader's source (replacing the previous one) to the specified one.
         * If sourceLen is -1, the string is assumed to be null terminated.
         */
        inline void setSource(const char *source, int sourceLen = -1) {
            const char *sources[] = { source };
            int sourceLens[1] = { sourceLen };
            setSource(1, sources, sourceLen == -1 ? nullptr : sourceLens);
        }
        inline void setSource(std::string source) { setSource(source.c_str(), (int) source.length()); }
        inline void setSource(std::vector<char> source) { setSource(source.data(), (int) source.size()); }

        /**
         * Those functions allow setting a source composed of multiple source fragments (for example to easily load
         * both a shared common shader code and some specialized code). This is the preferred way to join multiple
         * shader code segments.
         */
        void setSource(int sourceCount, const char **sources, int *sourceLengths = nullptr);
        void setSource(std::vector<std::string> sources);
        void setSource(std::vector<std::vector<char>> sources);

        /**
         * This function will compile the shader and if the compilation fails the function will throw
         * a ShaderCompileError.
         */
        void compile();
        std::string getInfoLog();

        inline ShaderType getShaderType() const {
            return type;
        }

        static Shader createFromAssets(ShaderType type, std::string fileName);

    };

    class ShaderProgram {

    private:
        unsigned int id;
        std::map<std::string, int> attributes;
        std::set<int> vertexAttribIds;
        std::map<std::string, int> uniforms;

    public:
        ShaderProgram();
        ~ShaderProgram();

        void attach(Shader const &shader);
        void link();

        std::string getInfoLog();

        void use();

        ShaderAttribute getAttribute(std::string name, ShaderValueType type);
        ShaderUniform getUniform(std::string name, ShaderValueType type);

        static ShaderProgram* current;
        static std::vector<int> enabledVertexArrays;

    };

}