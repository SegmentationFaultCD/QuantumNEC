#pragma once
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <lib/rbtree.hpp>
namespace QuantumNEC::Kernel {
template < typename PCB >
class InterprocessCommunicationMessages {
public:
    explicit InterprocessCommunicationMessages( void ) noexcept;
    ~InterprocessCommunicationMessages( void ) noexcept {
    }
    using result = uint64_t;

public:
    auto send( std::copy_constructible auto &&...datas ) -> result {
    }

private:
    // 作为user时使用
    Lib::RedBlackTree< InterprocessCommunicationMessages, uint64_t >::RedBlackTreeNode message_node;

    // 作为servicer时使用
    Lib::RedBlackTree< InterprocessCommunicationMessages, uint64_t > messages_tree;

    PCB *pcb;
};

}     // namespace QuantumNEC::Kernel