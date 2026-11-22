#ifndef GARFISH_COMPONENTSTORE_HPP
#define GARFISH_COMPONENTSTORE_HPP

#include <vector>

namespace gf_ecs {
template <class T>
class ComponentStore {
  public:
    std::vector<T> data;
    // TODO: figure out details
};
} // namespace gf_ecs
#endif