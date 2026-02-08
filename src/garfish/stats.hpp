/**
* @file
* @author Spencer Banasik
* @relates EngineStats
*/
#ifndef GF_STATS_HPP
#define GF_STATS_HPP
namespace gf {
struct EngineStats {
    float frametime;
    int triangle_count;
    int drawcall_count;
    float scene_update_time;
    float mesh_draw_time;
};
}
#endif
