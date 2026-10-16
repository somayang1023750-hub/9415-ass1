#include <catch2/catch_test_macros.hpp>
#include "graphics/VoxelWorld.h"
#include "graphics/Camera.h"
#include "graphics/HumanoidPose.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <limits>
#include <cmath>
#include "app/SceneSettings.h"

using namespace craft::graphics;

TEST_CASE("Generated terrain is repeatable and counts only final occupied voxels", "[world]") {
    VoxelWorld world;
    std::size_t counted = 0;
    for (int x = 0; x < 32; ++x)
        for (int y = 0; y < 32; ++y)
            for (int z = 0; z < 32; ++z) counted += world.isSolid(x,y,z);
    REQUIRE(counted == world.solidBlockCount());
    REQUIRE(counted >= 100);
    const int floor = VoxelWorld::towerFloorHeight();
    REQUIRE(world.blockAt(16,floor+2,19) == BlockType::Air); // Tower doorway.
    REQUIRE(world.blockAt(14,floor+2,19) == BlockType::Brick);
    REQUIRE(world.blockAt(16,floor+2,16) == BlockType::Air); // Hollow interior.
    REQUIRE(world.blockAt(16,VoxelWorld::towerRoofHeight(),16) == BlockType::Brick);
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

TEST_CASE("Fly camera respects lower flight bounds, frame time, pitch and release", "[camera]") {
    VoxelWorld world(false);
    Camera camera({0,1,0});
    for (int i=0; i<20; ++i) camera.processKeyboard(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, .1F, &world);
    REQUIRE(camera.position().y >= -16.0F);
    REQUIRE(camera.position().y < -15.9F);
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

TEST_CASE("Seeded soil and rock form lower patches while the top stays stone-free", "[world]") {
    VoxelWorld world;
    int bottomStone = 0, bottomDirt = 0, deepStone = 0, deepDirt = 0;
    for (int x=0; x<32; ++x) for (int z=0; z<32; ++z) {
        const auto bottom = world.blockAt(x,0,z);
        bottomStone += bottom == BlockType::Stone;
        bottomDirt += bottom == BlockType::Dirt;
        // The tower deliberately overwrites its footprint, so examine terrain outside it.
        if (x>=13 && x<=19 && z>=13 && z<=19) continue;
        const int h = VoxelWorld::terrainHeight(x,z);
        REQUIRE(world.blockAt(x,h,z) == BlockType::Grass);
        REQUIRE(world.blockAt(x,h-1,z) == BlockType::Dirt);
        REQUIRE(world.blockAt(x,h-2,z) == BlockType::Dirt);
        for (int y=0; y<=h-4; ++y) {
            const auto block = world.blockAt(x,y,z);
            deepStone += block == BlockType::Stone;
            deepDirt += block == BlockType::Dirt;
        }
    }
    REQUIRE(bottomDirt > 5);
    REQUIRE(bottomStone > bottomDirt*3);
    REQUIRE(deepDirt > 5);
    REQUIRE(deepStone > deepDirt*3);
}

TEST_CASE("Collision-enabled camera can fly below the island but cannot enter its underside", "[camera]") {
    VoxelWorld world;
    Camera camera({40,5,16}, {0,1,0}, -90,0);
    for (int i=0; i<14; ++i) camera.processKeyboard(GLFW_KEY_LEFT_SHIFT,GLFW_PRESS,.1F,&world);
    REQUIRE(camera.position().y < -10.0F);
    for (int i=0; i<16; ++i) camera.processKeyboard(GLFW_KEY_A,GLFW_PRESS,.1F,&world);
    REQUIRE(camera.position().x > 19.0F);
    REQUIRE(camera.position().x < 22.0F);
    for (int i=0; i<30; ++i) camera.processKeyboard(GLFW_KEY_SPACE,GLFW_PRESS,.1F,&world);
    REQUIRE(camera.position().y < -0.19F);
    REQUIRE(camera.position().y > -0.41F);
    REQUIRE_FALSE(world.intersectsCamera(camera.position()));
}

TEST_CASE("Walking swings opposite limbs in phase and legs pivot at fixed hips", "[hierarchy][walk]") {
    const auto pose = walkingPose({},90.0F,35.0F);
    REQUIRE(std::abs(pose.rightArmPitch-35.0F) < .0001F);
    REQUIRE(pose.leftArmPitch == -pose.rightArmPitch);
    REQUIRE(pose.rightLegPitch == -pose.rightArmPitch);
    REQUIRE(pose.leftLegPitch == pose.rightArmPitch);
    const auto reverse = walkingPose({},270.0F,35.0F);
    REQUIRE(std::abs(reverse.rightArmPitch+pose.rightArmPitch) < .0001F);
    const glm::mat4 identity(1);
    const auto m = humanoidTransforms(identity,pose);
    const auto neutral = humanoidTransforms(identity);
    REQUIRE(m.torso == neutral.torso);
    REQUIRE(m.head == neutral.head);
    constexpr float s = 1.0F/16.0F;
    const glm::vec4 hipInLeg(0,6*s,0,1);
    REQUIRE(glm::length(m.rightLeg*hipInLeg-glm::vec4(-2*s,12*s,0,1)) < .0001F);
    REQUIRE(glm::length(m.leftLeg*hipInLeg-glm::vec4(2*s,12*s,0,1)) < .0001F);
    const glm::vec4 shoulderInArm(0,6*s,0,1);
    REQUIRE(glm::length(m.rightArm*shoulderInArm-glm::vec4(-6*s,24*s,0,1)) < .0001F);
    REQUIRE(glm::length(m.leftArm*shoulderInArm-glm::vec4(6*s,24*s,0,1)) < .0001F);
    const auto base = walkingPose({10,20,-15,5,-8},90,0);
    REQUIRE(base.headYaw == 10);
    REQUIRE(base.rightArmPitch == 20);
    REQUIRE(base.leftArmPitch == -15);
    REQUIRE(base.rightLegPitch == 5);
    REQUIRE(base.leftLegPitch == -8);
}

TEST_CASE("Walk animation can pause, scrub, reset and remain independent per character", "[walk]") {
    craft::app::SceneSettings settings;
    auto& a = settings.characters[0];
    auto& b = settings.characters[1];
    const float phase = a.walkPhase;
    a.advanceWalk(.1F);
    REQUIRE(a.walkPhase == phase); // Neutral by default, including in benchmarks.
    a.walking = a.animateWalk = true;
    a.advanceWalk(.1F);
    REQUIRE(a.walkPhase != phase);
    REQUIRE(b.walkPhase == phase);
    a.animateWalk = false;
    const auto paused = a.walkPhase;
    a.advanceWalk(.1F);
    REQUIRE(a.walkPhase == paused);
    a.walkPhase = 270;
    REQUIRE(a.effectivePose().rightArmPitch < 0);
    a = {};
    REQUIRE_FALSE(a.walking);
    REQUIRE(a.effectivePose().rightLegPitch == 0);
    REQUIRE(a.effectivePose().leftArmPitch == 0);
}

TEST_CASE("Perlin height noise is varied, coherent, seeded and repeatable", "[world][noise]") {
    int minimum = 32, maximum = 0, changedSeed = 0;
    bool levels[32]{};
    int heights[32][32]{};
    for (int x=0; x<32; ++x) for (int z=0; z<32; ++z) {
        const int h = VoxelWorld::terrainHeight(x,z);
        heights[x][z] = h;
        REQUIRE(h >= 4);
        REQUIRE(h <= 15);
        REQUIRE(h == VoxelWorld::terrainHeight(x,z,VoxelWorld::kTerrainSeed));
        minimum = std::min(minimum,h); maximum = std::max(maximum,h); levels[h] = true;
        changedSeed += h != VoxelWorld::terrainHeight(x,z,VoxelWorld::kTerrainSeed+1);
        if (x<31) REQUIRE(std::abs(h-VoxelWorld::terrainHeight(x+1,z)) <= 4);
        if (z<31) REQUIRE(std::abs(h-VoxelWorld::terrainHeight(x,z+1)) <= 4);
    }
    int distinct = 0;
    for (bool level : levels) distinct += level;
    REQUIRE(maximum-minimum >= 4);
    REQUIRE(distinct >= 5);
    REQUIRE(changedSeed > 100);
    // A reverse traversal cannot consume or change a hidden RNG sequence.
    for (int x=31; x>=0; --x) for (int z=31; z>=0; --z)
        REQUIRE(heights[x][z] == VoxelWorld::terrainHeight(x,z));
}

TEST_CASE("Tower foundation and character surfaces adapt to Perlin hills", "[world][noise]") {
    VoxelWorld world;
    const int floor = VoxelWorld::towerFloorHeight(), roof = VoxelWorld::towerRoofHeight();
    REQUIRE(roof+3 < VoxelWorld::kHeight);
    for (int x=13; x<=19; ++x) for (int z=13; z<=19; ++z) {
        REQUIRE(floor >= VoxelWorld::terrainHeight(x,z));
        REQUIRE(world.blockAt(x,floor,z) == BlockType::Brick);
        REQUIRE(world.blockAt(x,roof,z) == BlockType::Brick);
        for (int y=0; y<floor; ++y) REQUIRE(world.isSolid(x,y,z));
    }
    const int ground = VoxelWorld::terrainHeight(16,24)+1;
    REQUIRE_FALSE(world.intersectsCamera({16.0F,static_cast<float>(roof)+1.25F,16.0F}));
    REQUIRE_FALSE(world.intersectsCamera({16.0F,static_cast<float>(ground)+1.25F,24.0F}));
}

TEST_CASE("Tree branches carry their own crowns without moving trunk or siblings", "[hierarchy][tree]") {
    const glm::mat4 identity(1);
    const auto neutral=treeTransforms(identity);
    const auto bent=treeTransforms(identity,60,30);
    REQUIRE(bent.trunk == neutral.trunk);
    REQUIRE(bent.topCrown == neutral.topCrown);
    REQUIRE(bent.rightBranch == neutral.rightBranch);
    REQUIRE(bent.rightCrown == neutral.rightCrown);
    REQUIRE(bent.leftBranch != neutral.leftBranch);
    REQUIRE(bent.leftCrown != neutral.leftCrown);
    const auto root=glm::rotate(glm::translate(identity,glm::vec3(4,5,6)),glm::radians(40.0F),glm::vec3(0,1,0));
    const auto moved=treeTransforms(root);
    for (int column=0; column<4; ++column)
        REQUIRE(glm::length(moved.trunk[column]-(root*neutral.trunk)[column]) < 0.00001F);
    for (int column=0; column<4; ++column)
        REQUIRE(glm::length(moved.topCrown[column]-(root*neutral.topCrown)[column]) < 0.00001F);
    for (int column=0; column<4; ++column)
        REQUIRE(glm::length(moved.leftBranch[column]-(root*neutral.leftBranch)[column]) < 0.00001F);
    for (int column=0; column<4; ++column)
        REQUIRE(glm::length(moved.rightBranch[column]-(root*neutral.rightBranch)[column]) < 0.00001F);
    for (int column=0; column<4; ++column)
        REQUIRE(glm::length(moved.leftCrown[column]-(root*neutral.leftCrown)[column]) < 0.00001F);
    for (int column=0; column<4; ++column)
        REQUIRE(glm::length(moved.rightCrown[column]-(root*neutral.rightCrown)[column]) < 0.00001F);
    craft::app::SceneSettings settings;
    settings.trees[0].leftBranch=60;
    REQUIRE(settings.trees[1].leftBranch == 30);
    settings={};
    REQUIRE(settings.trees[0].leftBranch == 30);
}
