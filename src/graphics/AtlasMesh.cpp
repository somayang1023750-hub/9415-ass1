#include "graphics/AtlasMesh.h"
#include <cstddef>

namespace craft::graphics {
namespace {
void quad(std::vector<VoxelVertex>& vertices, std::vector<unsigned int>& indices,
          const std::array<glm::vec3,4>& points, const std::array<glm::vec2,4>& uv,
          float brightness, bool doubleSided=false) {
    const auto start=static_cast<unsigned int>(vertices.size());
    for (int i=0;i<4;++i) vertices.push_back({points[i],uv[i],brightness,glm::vec3(1)});
    for (unsigned int i : {0U,1U,2U,2U,3U,0U}) indices.push_back(start+i);
    if (doubleSided)
        for (unsigned int i : {2U,1U,0U,0U,3U,2U}) indices.push_back(start+i);
}
}
AtlasMesh::AtlasMesh(BlockType type) {
    const std::array<std::array<glm::vec3,4>,6> points{{
        {{{-.5F,.5F,.5F},{.5F,.5F,.5F},{.5F,.5F,-.5F},{-.5F,.5F,-.5F}}},
        {{{-.5F,-.5F,-.5F},{.5F,-.5F,-.5F},{.5F,-.5F,.5F},{-.5F,-.5F,.5F}}},
        {{{.5F,-.5F,-.5F},{-.5F,-.5F,-.5F},{-.5F,.5F,-.5F},{.5F,.5F,-.5F}}},
        {{{-.5F,-.5F,.5F},{.5F,-.5F,.5F},{.5F,.5F,.5F},{-.5F,.5F,.5F}}},
        {{{-.5F,-.5F,-.5F},{-.5F,-.5F,.5F},{-.5F,.5F,.5F},{-.5F,.5F,-.5F}}},
        {{{.5F,-.5F,.5F},{.5F,-.5F,-.5F},{.5F,.5F,-.5F},{.5F,.5F,.5F}}}
    }};
    std::vector<VoxelVertex> vertices;
    std::vector<unsigned int> indices;
    for (int face=0;face<6;++face) {
        const auto direction=static_cast<FaceDirection>(face);
        quad(vertices,indices,points[face],getFaceUVs(type,direction),getFaceBrightness(direction));
    }
    upload(vertices,indices);
}
AtlasMesh::AtlasMesh(int tileX, int tileY) {
    std::vector<VoxelVertex> vertices;
    std::vector<unsigned int> indices;
    const auto uv=atlasTileUVs(tileX,tileY);
    quad(vertices,indices,{{{-.5F,0,0},{.5F,0,0},{.5F,1,0},{-.5F,1,0}}},uv,1,true);
    quad(vertices,indices,{{{0,0,.5F},{0,0,-.5F},{0,1,-.5F},{0,1,.5F}}},uv,1,true);
    upload(vertices,indices);
}
void AtlasMesh::upload(const std::vector<VoxelVertex>& vertices, const std::vector<unsigned int>& indices) {
    count_=static_cast<GLsizei>(indices.size());
    glGenVertexArrays(1,&vao_); glGenBuffers(1,&vbo_); glGenBuffers(1,&ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(VoxelVertex),vertices.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),indices.data(),GL_STATIC_DRAW);
    const int sizes[]={3,2,1,3};
    const std::size_t offsets[]={offsetof(VoxelVertex,position),offsetof(VoxelVertex,texCoord),
        offsetof(VoxelVertex,brightness),offsetof(VoxelVertex,colorModifier)};
    for (GLuint i=0;i<4;++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,sizes[i],GL_FLOAT,GL_FALSE,sizeof(VoxelVertex),reinterpret_cast<void*>(offsets[i]));
    }
    glBindVertexArray(0);
}
AtlasMesh::~AtlasMesh() {
    glDeleteBuffers(1,&ebo_); glDeleteBuffers(1,&vbo_); glDeleteVertexArrays(1,&vao_);
}
void AtlasMesh::draw() const {
    glBindVertexArray(vao_); glDrawElements(GL_TRIANGLES,count_,GL_UNSIGNED_INT,nullptr); glBindVertexArray(0);
}
} // namespace craft::graphics
