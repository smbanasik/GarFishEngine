#ifndef GARFISH_ISYSTEM_HPP
#define GARFISH_ISYSTEM_HPP
namespace gf_sys {
struct ISystem {
    virtual void action() = 0;
};
} // namespace gf_sys
#endif