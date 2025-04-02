#include "BaseSurface.h"
#include <stdio.h>
#include "shaders.h"
#include <glm/ext/vector_uint4.hpp>
#include <glm/trigonometric.hpp>

sgl::BaseSurface::BaseSurface(uint32_t width, uint32_t height) : m_texture(sf::Vector2u(width, height))
{
    resize(width, height);
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
    if(!m_depthTexture){
        glGenTextures(1, &m_depthTexture);
    }
    
    m_texture.resize(sf::Vector2u(width, height));

    glBindTexture(GL_TEXTURE_2D, m_depthTexture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        (GL_R32F),
        static_cast<GLsizei>(m_texture.getSize().x),
        static_cast<GLsizei>(m_texture.getSize().y),
        0,
        GL_RED,
        GL_FLOAT,
        nullptr);
    
    glBindTexture(GL_TEXTURE_2D, NULL);

    printf("%d\n", glGetError());
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
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glClearColor((float)color.r/255., (float)color.g/255., (float)color.b/255., 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDeleteFramebuffers(1, &framebuffer);

    const float data = FLT_MAX;
    glClearTexImage(m_depthTexture, 0, GL_RED, GL_FLOAT, &data);
}

void sgl::BaseSurface::drawTri(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, sf::Color color)
{
    const auto& shader = shaderMap.at("tri_batch");
    glUseProgram(shader.program);
    glBindImageTexture(0, getTextureId(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

    glm::uvec4 ucolor(color.r, color.g, color.b, 255);
    
    glNamedBufferStorage(shader.UBO[0], sizeof(ucolor) + sizeof(glm::vec2)*3, NULL, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(shader.UBO[0], 0, sizeof(ucolor), &ucolor);
    glNamedBufferSubData(shader.UBO[0], sizeof(ucolor), sizeof(glm::vec2), &v0);
    glNamedBufferSubData(shader.UBO[0], sizeof(ucolor) + sizeof(glm::vec2), sizeof(glm::vec2), &v1);
    glNamedBufferSubData(shader.UBO[0], sizeof(ucolor) + sizeof(glm::vec2)*2, sizeof(glm::vec2), &v2);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, shader.UBO[0]);
    glDispatchCompute(m_texture.getSize().x / 32 + 1, m_texture.getSize().y / 32 + 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
}

void sgl::BaseSurface::drawTris(const glm::mat4 transform, const std::vector<glm::vec3> &vertex, const std::vector<glm::uvec3> &indices, sf::Color color)
{
    if(!indices.size()) return;

    glm::vec2 image_size = getSize();
    std::vector<glm::vec4> transformed(vertex.size(), glm::vec4());
    for(size_t i = 0; i < transformed.size(); i++){
        const glm::vec4 transf = transform * glm::vec4(vertex[i], 1.f);
        transformed[i] = (transf/transf.w) * glm::vec4(image_size/2.f,1.f,1.f) + glm::vec4(image_size/2.f,0.f,0.f);
    }

    const glm::vec3 camera = glm::normalize(glm::vec3(transform[0][2],transform[1][2],transform[2][2]));

    const auto& shader = shaderMap.at("tri_trd_batch");
    glUseProgram(shader.program);
    glBindImageTexture(0, getTextureId(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glBindImageTexture(1, m_depthTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glm::uvec4 ucolor(color.r, color.g, color.b, 255);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shader.UBO[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::uvec4)*indices.size(), NULL, GL_STREAM_COPY);
    for(size_t i = 0; i < indices.size(); i++){
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::uvec4)*i, sizeof(glm::uvec3), &indices.data()[i]);
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, shader.UBO[1]);


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shader.UBO[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (sizeof(glm::vec4)*2ull)*vertex.size(), NULL, GL_STREAM_COPY);
    for(size_t i = 0; i < vertex.size(); i++){
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*i*2, sizeof(glm::vec3), &vertex.data()[i]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(i*2+1), sizeof(glm::vec4), &transformed.data()[i]);
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, shader.UBO[2]);

    //const int index = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shader.UBO[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ucolor) + sizeof(transform) + sizeof(camera) + sizeof(int), NULL, GL_STREAM_COPY);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ucolor), &ucolor);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(ucolor), sizeof(transform), &transform);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(ucolor) + sizeof(transform), sizeof(camera), &camera);
    //glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(ucolor) + sizeof(transform) + sizeof(camera), sizeof(index), &index);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, shader.UBO[0]);

    const int block_size = 8;
    const int block_size_z = 8;
    const int block_size_zz = 1;

    int i = 0;
    while(i < indices.size()){
        glm::ivec2 max_diffs = glm::ivec2();

        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(ucolor) + sizeof(transform) + sizeof(camera), sizeof(i), &i);

        for(int j = 0; j < block_size_z * block_size_zz; j++, i++){
            if(i >= indices.size()) break;
            const glm::uvec3 index = indices[i];
            //if(transformed[index.x].w < 0.0001f && transformed[index.y].w < 0.0001f && transformed[index.z].w < 0.0001f) continue;

            const glm::vec3 v0 = vertex[index.x];
            const glm::vec3 v1 = vertex[index.y];
            const glm::vec3 v2 = vertex[index.z];

            glm::vec3 normal = glm::normalize(glm::cross(v1 - v2, v1 - v0));
            if(glm::dot(normal, camera) >= glm::cos(glm::radians(30.f))) continue;

            const glm::vec4 v0t = transformed[index.x];
            const glm::vec4 v1t = transformed[index.y];
            const glm::vec4 v2t = transformed[index.z];

            const glm::vec2 mins = glm::vec2(glm::min(glm::min(v0t, v1t), v2t));
            const glm::vec2 maxs = glm::vec2(glm::max(glm::max(v0t, v1t), v2t));

            const glm::ivec2 diffs = (glm::ivec2(maxs)/block_size)*block_size - (glm::ivec2(mins)/block_size)*block_size + glm::ivec2(1, 1);
            max_diffs = glm::max(max_diffs, diffs);
            //printf("%d %d\n", diffs.x, diffs.y);
        }

        glDispatchCompute(max_diffs.x, max_diffs.y, block_size_zz);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }
    //exit(0);
    //printf("%d\n", glGetError());
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
    if(m_depthTexture){
        glDeleteTextures(1, &m_depthTexture);
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
    glCreateBuffers(1, lineBatchShader.UBO);
    shaderMap.emplace("line_batch", lineBatchShader);

    shaderData triBatchShader;
    compileShader(sgl::shaders::tri_batch_shader_src, triBatchShader.shader, triBatchShader.program);
    glGenBuffers(1, triBatchShader.UBO);
    shaderMap.emplace("tri_batch", triBatchShader);

    shaderData triTransDepthShader;
    compileShader(sgl::shaders::tri_batch_depthtest_projection_shader_src, triTransDepthShader.shader, triTransDepthShader.program);
    glGenBuffers(3, triTransDepthShader.UBO);
    shaderMap.emplace("tri_trd_batch", triTransDepthShader);
}

void sgl::BaseSurface::DestroyShaders()
{
    for(auto& shader: shaderMap){
        glDeleteShader(shader.second.shader);
        glDeleteProgram(shader.second.program);
        glDeleteBuffers(1, shader.second.UBO);
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
        /*printf("%x ", glGetError());
        glNamedBufferStorage(shader.UBO[0], sizeof(ucolor) + sizeof(line_t) * lines.second.size(), NULL, GL_DYNAMIC_STORAGE_BIT);
        printf("%x ", glGetError());
        glNamedBufferSubData(shader.UBO[0], 0, sizeof(ucolor), &ucolor);
        printf("%x ", glGetError());
        glNamedBufferSubData(shader.UBO[0], sizeof(ucolor), sizeof(line_t) * lines.second.size(), lines.second.data());
        printf("%x ", glGetError());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, shader.UBO[0]);
        printf("%x ", glGetError());*/
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, shader.UBO[0]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ucolor) + sizeof(line_t) * lines.second.size(), NULL, GL_STREAM_COPY);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ucolor), &ucolor);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(ucolor), sizeof(line_t) * lines.second.size(), lines.second.data());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, shader.UBO[0]);
        glDispatchCompute(16, lines.second.size()/32+1, 1);
        
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    

    m_lines_batch.clear();
}
