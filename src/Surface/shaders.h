
namespace sgl::shaders {
    const char* line_batch_shader_src = 
    "#version 460 core\n"
    "layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n"
    "layout(rgba8ui, binding = 0) uniform uimage2D imgOutput;\n"
    "struct Line\n"
    "{\n"
    "   uvec2 start;\n"
    "   uvec2 end;\n"
    "};\n"
    "layout(std430, binding = 1) buffer LineData\n"
    "{\n"
    "   uvec4 color;\n"
    "   Line[] lines;\n"
    "};\n"
    "void main() {\n"
    "   if(gl_GlobalInvocationID.y >= lines.length()) return;\n"
    "   uint coord = gl_GlobalInvocationID.x;\n"
    "   Line line = lines[gl_GlobalInvocationID.y];\n"
    "   float pos = float(coord) / 32.f / 16.f;\n"
    "   ivec2 texelCoord = ivec2(vec2(line.start) + (vec2(line.end)-vec2(line.start))*pos);\n"
    
    "   imageStore(imgOutput, texelCoord, color);\n"
    "}";

}