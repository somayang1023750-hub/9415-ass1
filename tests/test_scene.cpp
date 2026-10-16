#include <catch2/catch_test_macros.hpp>
#include "graphics/VoxelWorld.h"
#include "graphics/Camera.h"
#include "graphics/HumanoidPose.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <limits>

using namespace craft::graphics;

TEST_CASE("Generated terrain is repeatable and counts only final occupied voxels", "[world]") {
    VoxelWorld world;
    std::size_t counted = 0;
    for (int x = 0; x < 32; ++x)
        for (int y = 0; y < 32; ++y)
            for (int z = 0; z < 32; ++z) counted += world.isSolid(x,y,z);
    REQUIRE(counted == world.solidBlockCount());
    REQUIRE(counted >= 100);
    REQUIRE(VoxelWorld::terrainHeight(0,0) != VoxelWorld::terrainHeight(16,16));
    REQUIRE(world.blockAt(16,11,19) == BlockType::Air); // Tower doorway.
    REQUIRE(world.blockAt(14,11,19) == BlockType::Brick);
    REQUIRE(world.blockAt(16,11,16) == BlockType::Air); // Hollow interior.
    REQUIRE(world.blockAt(16,22,16) == BlockType::Brick);
    REQUIRE(world.blockAt(16, VoxelWorld::terrainHeight(16,24), 24) == BlockType::Grass);
    REQUIRE_FALSE(world.isSolid(-1, 0, 0));
    world.generateTerrain();
    REQUIRE(world.solidBlockCount() == counted);
    VoxelWorld second;
    for (int x = 0; x < 32; ++x)
        for (int y = 0; y < 32; ++y)
            for (int z = 0; z < 32; ++z) REQUIRE(world.blockAt(x,y,z) == second.blockAt(x,y,z));
}

TEST_CASE("Voxel replacement and regeneration preserve accurate counts", "[world]") {
    VoxelWorld world(false);
    REQUIRE(world.solidBlockCount() == 0);
    world.setBlock(1,2,3,BlockType::Grass);
    world.setBlock(1,2,3,BlockType::Brick);
    REQUIRE(world.solidBlockCount() == 1);
    world.setBlock(1,2,3,BlockType::Air);
    REQUIRE(world.solidBlockCount() == 0);
    REQUIRE_THROWS_AS(world.setBlock(32,0,0,BlockType::Stone), std::out_of_range);
}

TEST_CASE("Fly camera cannot tunnel through a wall and slides along it", "[camera]") {
    VoxelWorld world(false);
    for (int y=0; y<8; ++y)
        for (int z=0; z<32; ++z) world.setBlock(3,y,z,BlockType::Stone);
    Camera camera({1.0F,3.0F,24.0F}, {0,1,0}, -45.0F, 0);
    for (int i=0; i<20; ++i) camera.processKeyboard(GLFW_KEY_W, GLFW_PRESS, 0.1F, &world);
    REQUIRE(camera.position().x < 2.81F);
    REQUIRE(camera.position().z < 10.0F); // Tangential motion continues.
    REQUIRE_FALSE(world.intersectsCamera(camera.position()));
    Camera fast({1,3,4}, {0,1,0}, 0,0);
    fast.processKeyboard(GLFW_KEY_W, GLFW_PRESS, 20.0F, &world);
    REQUIRE(fast.position().x < 2.81F);
    Camera free({1,3,4}, {0,1,0}, 0,0);
    free.processKeyboard(GLFW_KEY_W, GLFW_PRESS, 0.25F);
    REQUIRE(free.position().x > 3.0F);
}

TEST_CASE("Fly camera respects floor bounds, frame time, pitch and release", "[camera]") {
    VoxelWorld world(false);
    Camera camera({0,1,0});
    for (int i=0; i<20; ++i) camera.processKeyboard(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, .1F, &world);
    REQUIRE(camera.position().y >= 0.21F);
    const auto before = camera.position();
    camera.processKeyboard(GLFW_KEY_W, GLFW_RELEASE, 0.1F, &world);
    camera.processKeyboard(GLFW_KEY_W, GLFW_PRESS, -1.0F, &world);
    camera.processKeyboard(GLFW_KEY_W, GLFW_PRESS, std::numeric_limits<float>::quiet_NaN(), &world);
    REQUIRE(camera.position() == before);
    camera.processMouseMovement(0,10000);
    REQUIRE(camera.pitch() == 89.0F);
    camera.processMouseMovement(0,-20000);
    REQUIRE(camera.pitch() == -89.0F);
    Camera a, b;
    a.processKeyboard(GLFW_KEY_W, GLFW_PRESS, .1F);
    b.processKeyboard(GLFW_KEY_W, GLFW_PRESS, .05F);
    b.processKeyboard(GLFW_KEY_W, GLFW_PRESS, .05F);
    REQUIRE(glm::length(a.position()-b.position()) < 0.0001F);
}

TEST_CASE("Local head and shoulder poses leave siblings unchanged; root moves all parts", "[hierarchy]") {
    const glm::mat4 identity(1);
    const auto neutral = humanoidTransforms(identity);
    const auto posed = humanoidTransforms(identity, {60,-70});
    REQUIRE(posed.torso == neutral.torso);
    REQUIRE(posed.leftArm == neutral.leftArm);
    REQUIRE(posed.leftLeg == neutral.leftLeg);
    REQUIRE(posed.rightLeg == neutral.rightLeg);
    REQUIRE(posed.head != neutral.head);
    REQUIRE(posed.rightArm != neutral.rightArm);
    const auto root = glm::translate(identity, glm::vec3(3,4,5));
    const auto moved = humanoidTransforms(root);
    REQUIRE(moved.head == root * neutral.head);
    REQUIRE(moved.rightArm == root * neutral.rightArm);
    REQUIRE(moved.leftArm == root * neutral.leftArm);
    REQUIRE(moved.leftLeg == root * neutral.leftLeg);
    REQUIRE(moved.rightLeg == root * neutral.rightLeg);
    REQUIRE(moved.torso == root * neutral.torso);
}
