#include "BaseSurface.h"
#include <stdio.h>
#include "shaders.h"
#include <glm/ext/vector_uint4.hpp>

sgl::BaseSurface::BaseSurface(uint32_t width, uint32_t height) : m_texture(sf::Vector2u(width, height))
{
    if(m_num_objects == 0){
        CompileShaders();
    }
    ++m_num_objects;
}

GLint sgl::BaseSurface::getTextureId() const
{
    sf::Texture::bind(&m_texture);
    GLint whichID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &whichID);
    sf::Texture::bind(nullptr);
    return whichID;
}

void sgl::BaseSurface::resize(uint32_t width, uint32_t height)
{
    m_texture.resize(sf::Vector2u(width, height));
}

glm::uvec2 sgl::BaseSurface::getSize() const
{
    sf::Vector2u size = m_texture.getSize();
    return glm::uvec2(size.x, size.y);
}

void sgl::BaseSurface::clear(sf::Color color)
{
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, getTextureId(), 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE);

    glClearColor((float)color.r/255., (float)color.g/255., (float)color.b/255., 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDeleteFramebuffers(1, &framebuffer);
}

void sgl::BaseSurface::drawLine(glm::uvec2 start, glm::uvec2 end, sf::Color color)
{
    if(m_lines_batch.find(color.toInteger()) == m_lines_batch.end()){
        m_lines_batch.emplace(color.toInteger(), std::vector<line_t>());
    }
    auto& lines = m_lines_batch[color.toInteger()];

    lines.push_back(line_t{start, end});
}

void sgl::BaseSurface::flush()
{
    FlushLines();
}

sgl::BaseSurface::~BaseSurface()
{
    --m_num_objects;
    if(m_num_objects == 0){
        DestroyShaders();
    }
}

void compileShader(const char* source, GLuint& compute, GLuint& proram){
    // compute shader
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &source, NULL);
    glCompileShader(compute);
    GLint status;
    glGetShaderiv(compute, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE){
        char errorLog[512]{};
        glGetShaderInfoLog(compute, 512, nullptr, &errorLog[0]);
        printf("%s", errorLog);
        std::abort();
    }

    // shader Program
    proram = glCreateProgram();
    glAttachShader(proram, compute);
    glLinkProgram(proram);

    glGetProgramiv(proram, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        char errorLog[512]{};
        glGetProgramInfoLog(proram, 512, nullptr, &errorLog[0]);
        printf("%s", errorLog);
        std::abort();  
    }
}

std::unordered_map<std::string, sgl::BaseSurface::shaderData> sgl::BaseSurface::shaderMap;

void sgl::BaseSurface::CompileShaders()
{
    shaderData lineBatchShader;
    compileShader(sgl::shaders::line_batch_shader_src, lineBatchShader.shader, lineBatchShader.program);
    glGenBuffers(1, &lineBatchShader.UBO);
    shaderMap.emplace("line_batch", lineBatchShader);

}

void sgl::BaseSurface::DestroyShaders()
{
    for(auto& shader: shaderMap){
        glDeleteShader(shader.second.shader);
        glDeleteProgram(shader.second.program);
        glDeleteBuffers(1, &shader.second.UBO);
    }
}

void sgl::BaseSurface::FlushLines()
{
    const auto& shader = shaderMap.at("line_batch");
    glUseProgram(shader.program);
    glBindImageTexture(0, getTextureId(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

    for(auto& lines : m_lines_batch){
        sf::Color color = sf::Color(lines.first);
        glm::uvec4 ucolor(color.r, color.g, color.b, 255);
        
        glNamedBufferStorage(shader.UBO, sizeof(ucolor) + sizeof(line_t) * lines.second.size(), NULL, GL_DYNAMIC_STORAGE_BIT);

        glNamedBufferSubData(shader.UBO, 0, sizeof(ucolor), &ucolor);
        glNamedBufferSubData(shader.UBO, sizeof(ucolor), sizeof(line_t) * lines.second.size(), lines.second.data());
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, shader.UBO);

        glDispatchCompute(16, lines.second.size()/32+1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    m_lines_batch.clear();
}
