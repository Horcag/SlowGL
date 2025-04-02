#include <stdint.h>
#include <atomic>
#include <unordered_map>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <glm/ext/vector_uint3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <gl/glew.h>

namespace sgl {
    class BaseSurface {
    private:
        std::atomic_uint32_t m_num_objects;

        struct line_t {
            glm::uvec2 start;
            glm::uvec2 end;
        };

        std::unordered_map<uint32_t, std::vector<line_t>> m_lines_batch;

    protected:
        sf::Texture m_texture;

        struct shaderData {
            GLuint program;
            GLuint shader;
            GLuint UBO[4];
        };

        static std::unordered_map<std::string, shaderData> shaderMap;

        GLuint m_depthTexture = 0;
    public:
        BaseSurface(uint32_t width, uint32_t height);

        BaseSurface(glm::uvec2 size) : BaseSurface(size.x, size.y) {
        };

        GLint getTextureId() const;

        const sf::Texture& getTexture() const { return m_texture; };

        void resize(uint32_t width, uint32_t height);

        void resize(glm::uvec2 size) { resize(size.x, size.y); };

        glm::uvec2 getSize() const;

        void clear(sf::Color color);

        //void setPixel(glm::uvec2 pos, sf::Color color);

        void drawTri(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, sf::Color color);
        void drawTris(const glm::mat4 transform, const std::vector<glm::vec3>& vertex, const std::vector<glm::uvec3>& indices, sf::Color color);
        void drawLine(glm::uvec2 start, glm::uvec2 end, sf::Color color);

        void flush();

        ~BaseSurface();

    private:
        void CompileShaders();

        void DestroyShaders();

        void FlushLines();
    };
}
