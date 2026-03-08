#ifndef GARFISH_WORLD_HPP
#define GARFISH_WORLD_HPP

#include <garfish/componentstore.hpp>
#include <garfish/framepipeline.hpp>
namespace gf_ecs {
class World {
  public:
    void execute(gf_fp::FRAME_PHASE phase) { frame_pipeline.execute(phase); } // Goated reference

    gf_fp::FramePipeline frame_pipeline;

  private:
};
} // namespace gf_ecs
#endif