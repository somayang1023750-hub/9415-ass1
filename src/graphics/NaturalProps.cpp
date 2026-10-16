#include "graphics/NaturalProps.h"
#include "graphics/VoxelWorld.h"

namespace craft::graphics {
namespace {
constexpr int kTrees[2][2]={{9,22},{26,10}};
constexpr int kPlants[12][3]={{8,22,1},{10,25,3},{22,24,2},{25,21,0},
    {7,14,2},{24,7,1},{28,17,0},{3,20,2},{20,28,3},{12,5,0},{9,10,2},{28,25,1}};
}
NaturalProps::NaturalProps() {
    // Resolve terrain anchors once, outside the measured render loop.
    for (int i=0;i<2;++i) {
        const int x=kTrees[i][0], z=kTrees[i][1];
        treeOrigins_[i]={x+.5F,VoxelWorld::terrainHeight(x,z)+1,z+.5F};
    }
    for (int i=0;i<12;++i) {
        const int x=kPlants[i][0], z=kPlants[i][1];
        plantOrigins_[i]={x+.5F,VoxelWorld::terrainHeight(x,z)+1,z+.5F};
    }
}
void NaturalProps::draw(const Shader& shader, const std::array<TreeControls,2>& trees) const {
    for (int i=0;i<2;++i) {
        auto root=glm::translate(glm::mat4(1),treeOrigins_[i]+trees[i].offset);
        root=glm::rotate(root,glm::radians(trees[i].yaw),glm::vec3(0,1,0));
        const auto m=treeTransforms(root,trees[i].leftBranch,trees[i].rightBranch);
        for (const auto& model : {m.trunk,m.leftBranch,m.rightBranch}) {
            shader.setMat4("uModel",model); wood_.draw();
        }
        const bool culling=glIsEnabled(GL_CULL_FACE)==GL_TRUE;
        glDisable(GL_CULL_FACE); // See both sides through the cutout leaf texels.
        for (const auto& model : {m.topCrown,m.leftCrown,m.rightCrown}) {
            shader.setMat4("uModel",model); leaves_.draw();
        }
        if (culling) glEnable(GL_CULL_FACE);
    }
    // Fixed decorative layout, independent of character/tree controls.
    const AtlasMesh* meshes[]={&grass_,&red_,&yellow_,&blue_};
    for (int i=0;i<12;++i) {
        shader.setMat4("uModel",glm::translate(glm::mat4(1),plantOrigins_[i]));
        meshes[kPlants[i][2]]->draw();
    }
}
} // namespace craft::graphics
