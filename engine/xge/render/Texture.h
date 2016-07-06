#pragma once

namespace xge {

    class Image;

    enum class TextureInternalFormat {
        ALPHA, RGB, RGBA, LUMINANCE, LUMINANCE_ALPHA
    };

    enum class TextureFormat {
        ALPHA, RGB, RGB565, RGBA, RGBA4444, RGBA5551
    };

    class Texture {

    private:
        bool hasId = false;
        unsigned int id = 0;
        int bindId = -1;

        void createTexture();

    public:
        Texture() { }
        Texture(Texture const &) = delete;
        Texture(Texture &&t) : hasId(t.hasId), id(t.id) {
            t.hasId = false;
            t.id = 0;
        }
        /**
         * This constructor will create a new texture and upload the specified Image's data to GPU.
         */
        Texture(Image const &image);
        ~Texture();

        inline unsigned int getId() {
            if (!hasId)
                createTexture();
            return id;
        }
        /**
         * This gets the texture index to which the texture is bound to. If it's not bound, it'll return -1.
         */
        inline int getBindId() {
            return bindId;
        }

        void upload(TextureInternalFormat internalFormat, TextureFormat format, char *data, int w, int h, int level = 0);
        void upload(TextureFormat format, char *data, int w, int h, int level = 0);

        /**
         * This binds the texture to the specified OpenGL bind id. You probably will not have to use it as long as you
         * use Meshes and use setUniform in there.
         */
        void bind(int texId);
        /**
         * This flags the texture as unbound.
         */
        void unbind();

    };

}


