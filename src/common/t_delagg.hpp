/**
* @file
* @author Spencer Banasik
* @relates DeletionAggregate
*/

#ifndef COM_DELAGG_HPP
#define COM_DELAGG_HPP
namespace com {
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
}
#endif