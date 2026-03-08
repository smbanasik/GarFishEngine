#ifndef GARFISH_FRAMEPIPELINE_HPP
#define GARFISH_FRAMEPIPELINE_HPP

#include <algorithm>
#include <array>
#include <memory>
#include <stdint.h>
#include <vector>

#include <garfish/isystem.hpp>

namespace gf_framepipe {

enum FRAME_PHASE : uint8_t {
    INPUT,   // React to player input
    UPDATE,  // Main logic layer
    PHYSICS, // For physics
    RENDER   // During render?
};

constexpr uint8_t PHASE_COUNT = 4;

struct Hook {
    std::shared_ptr<gf_sys::ISystem> system;
    FRAME_PHASE phase;
    uint8_t priority; // Higher = priority
};

struct HookHandle {
    const std::weak_ptr<gf_sys::ISystem> system;
    const FRAME_PHASE phase;
};

/**
 * @brief Acts as hook system for engine.
 * @author Spencer Banasik
 */
class FramePipeline {
  public:
    HookHandle pending_insert(std::shared_ptr<gf_sys::ISystem> system, FRAME_PHASE phase, uint8_t priority) {
        pending_adds.emplace_back(Hook{system, phase, priority});
        return HookHandle{system, phase};
    }

    void pending_remove(HookHandle handle) { pending_removes.push_back(handle); }

    void execute(FRAME_PHASE phase) {
        for (auto it = systems[phase].begin(); it != systems[phase].end(); it++) {
            it->system->action();
        }
    }

    void handle_pending() {
        if (!pending_removes.empty()) {
            for (auto pend_it = pending_removes.begin(); pend_it != pending_removes.end(); pend_it++) {
                auto sys = pend_it->system.lock();
                auto& phase = systems[pend_it->phase];
                for (auto it = phase.begin(); it != phase.end();) {
                    if (it->system.get() == sys.get()) {
                        it = phase.erase(it);
                    } else {
                        it++;
                    }
                }
            }
            pending_removes.clear();
        }
        if (!pending_adds.empty()) {
            for (auto pend_it = pending_adds.begin(); pend_it != pending_adds.end(); pend_it++) {
                systems[pend_it->phase].push_back(*pend_it);
            }
            pending_adds.clear();
            for (auto& phase : systems) {
                std::sort(phase.begin(), phase.end(),
                          [](const Hook& a, const Hook& b) { return a.priority > b.priority; });
            }
        }
    }

  private:
    std::array<std::vector<Hook>, PHASE_COUNT> systems;
    std::vector<Hook> pending_adds;
    std::vector<HookHandle> pending_removes;
};

} // namespace gf_framepipe
#endif