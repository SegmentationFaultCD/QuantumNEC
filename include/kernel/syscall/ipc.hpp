#pragma once
#include <lib/rbtree.hpp>

namespace QuantumNEC::Kernel {

class InterprocessCommunication {
public:
    class message {
    public:
        explicit message( void ) noexcept;

    private:
        //  Lib::RedBlackTree< message, uint64_t >::RedBlackTreeNode node;
    };

private:
    // inline static Lib::RedBlackTree< message, uint64_t > task_tree;
};
}     // namespace QuantumNEC::Kernel