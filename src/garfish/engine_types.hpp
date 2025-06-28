/**
* @file
* @brief Types used by the engine.
* @author Spencer Banasik
*/
#ifndef GF_ENGINE_TYPES_HPP
#define GF_ENGINE_TYPES_HPP

#include <vector>
#include <deque>
#include <functional>

#include <glm/vec4.hpp>

namespace gf {
/**
 * @class DeletionStack
 * @brief Pushes a set of deletor functions that are called.
 * @author Spencer Banasik
 * @todo Have deletors push_back and reverse iterate through the stack.
 * @details A deletion stack is more useful than a deletion queue, since
 * objects initialized first often needs to be deleted last.
 * 
 * @note Prefer having managers deleting objects via looping through an array as
 * opposed to storing callbacks like this.
 */
class DeletionStack {
public:

    /**
     * @brief Push a deletor function onto the stack.
     * @param [in] function The function to be pushed.
     */
    void push_function(std::function<void()> && function) {
        deletors.push_front(function);
    }

    /**
     * @brief Call every function on the stack
     */
    void flush() {
        for (auto it = deletors.begin(); it != deletors.end(); it++) {
            (*it)();
        }
        deletors.clear();
    }

private:
    std::deque<std::function<void()>> deletors;
};

template<class T>
class DeletionAggregate {
public:

    DeletionAggregate(std::function<void()> func, uint32_t elems = 0) : deletion(func) {
        aggregate.reserve(elems);
    };
    void push_elem(T elem) {
        aggregate.push_back(elem);
    }
    void flush() {
        deletion();
    }

private:
    std::vector<T> aggregate;
    std::function<void()> deletion;
};

struct EngineStats {
    float frametime;
    int triangle_count;
    int drawcall_count;
    float scene_update_time;
    float mesh_draw_time;
};
};

#endif

