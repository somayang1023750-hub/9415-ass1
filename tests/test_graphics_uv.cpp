#include <catch2/catch_test_macros.hpp>

#include "graphics/Voxel.h"
#include "graphics/SkinUV.h"
#include <stb_image.h>
#include "core/AssetPath.h"

TEST_CASE("Grass faces select the coloured top, fringe and dirt atlas tiles", "[atlas]") {
  using namespace craft::graphics;
  int width, height, channels;
  auto* pixels = stbi_load(craft::core::assetPath("textures/texture.png").string().c_str(),
                          &width, &height, &channels, 4);
  REQUIRE(pixels != nullptr);
  const auto sample = [&](BlockType type, FaceDirection face, float heightFraction) {
    const auto uv = getFaceUVs(type, face);
    const float u = (uv[0].x + uv[1].x) * 0.5F;
    const float v = uv[2].y + heightFraction * (uv[0].y - uv[2].y);
    const auto i = (static_cast<int>(v * height) * width + static_cast<int>(u * width)) * 4;
    return glm::ivec3(pixels[i], pixels[i+1], pixels[i+2]);
  };
  const auto top = sample(BlockType::Grass, FaceDirection::Top, 0.5F);
  const auto fringe = sample(BlockType::Grass, FaceDirection::South, 0.03F);
  const auto sideDirt = sample(BlockType::Grass, FaceDirection::South, 0.9F);
  const auto bottom = getFaceUVs(BlockType::Grass, FaceDirection::Bottom);
  const auto dirt = getFaceUVs(BlockType::Dirt, FaceDirection::Bottom);
  stbi_image_free(pixels);
  REQUIRE(top.g > top.r);
  REQUIRE(fringe.g > fringe.r);
  REQUIRE(sideDirt.r > sideDirt.g);
  for (int i = 0; i < 4; ++i) REQUIRE(bottom[i] == dirt[i]);
  REQUIRE(getFaceBrightness(FaceDirection::Top) > getFaceBrightness(FaceDirection::South));
  REQUIRE(getFaceBrightness(FaceDirection::South) > getFaceBrightness(FaceDirection::Bottom));
}

TEST_CASE("Skin UVs use unflipped 64x64 image coordinates and upright back faces", "[skin]") {
  using namespace craft::graphics;
  const auto head = boxSkinUVs(8, 8, 8, 0, 0);
  REQUIRE(head[2][0] == glm::vec2(8.0F/64, 16.0F/64));
  REQUIRE(head[2][2] == glm::vec2(16.0F/64, 8.0F/64));
  REQUIRE(head[3][0] == glm::vec2(24.0F/64, 16.0F/64));
  REQUIRE(head[3][1] == glm::vec2(32.0F/64, 16.0F/64));
  // Modern left limbs live in the lower half of the skin, unlike right limbs.
  const auto leftArm = boxSkinUVs(4, 12, 4, 32, 48);
  const auto leftLeg = boxSkinUVs(4, 12, 4, 16, 48);
  REQUIRE(leftArm[2][0] == glm::vec2(36.0F/64, 1.0F));
  REQUIRE(leftLeg[2][2] == glm::vec2(24.0F/64, 52.0F/64));
  for (const auto& face : leftArm) {
    for (const auto& uv : face) {
      REQUIRE(uv.x >= 0.0F);
      REQUIRE(uv.x <= 1.0F);
      REQUIRE(uv.y >= 0.0F);
      REQUIRE(uv.y <= 1.0F);
    }
  }
}

TEST_CASE("Natural props select bark, end grain and cutout foliage from the bundled atlas", "[atlas][tree]") {
  using namespace craft::graphics;
  const auto bark=getFaceUVs(BlockType::Wood,FaceDirection::South);
  const auto end=getFaceUVs(BlockType::Wood,FaceDirection::Top);
  REQUIRE(bark == atlasTileUVs(4,14));
  REQUIRE(end == atlasTileUVs(4,13));
  REQUIRE(getFaceUVs(BlockType::Leaves,FaceDirection::Top) == atlasTileUVs(14,15));
  int width,height,channels;
  auto* pixels=stbi_load(craft::core::assetPath("textures/texture.png").string().c_str(),&width,&height,&channels,4);
  REQUIRE(pixels != nullptr);
  int leafCutouts=0, greenLeaves=0, flowerCutouts=0, redPetals=0;
  for(int y=0;y<16;++y) for(int x=0;x<16;++x) {
    const auto* leaf=pixels+((15*16+y)*width+14*16+x)*4;
    leafCutouts+=leaf[0]==255 && leaf[1]==0 && leaf[2]==255;
    greenLeaves+=leaf[1]>leaf[0] && leaf[1]>leaf[2];
    const auto* flower=pixels+((12*16+y)*width+2*16+x)*4;
    flowerCutouts+=flower[0]==255 && flower[1]==0 && flower[2]==255;
    redPetals+=flower[0]>flower[1]*2 && flower[2]<100;
  }
  stbi_image_free(pixels);
  REQUIRE(leafCutouts > 0);
  REQUIRE(greenLeaves > 100);
  REQUIRE(flowerCutouts > 100);
  REQUIRE(redPetals > 10);
}
