/**
* @file
* @author Spencer Banasik
* @relates DeletionStack
*/

#ifndef COM_DELSTACK_HPP
#define COM_DELSTACK_HPP

#include <deque>
#include <functional>

namespace com {
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
    void push_function(std::function<void()>&& function) {
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
}
#endif
