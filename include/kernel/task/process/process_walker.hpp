#pragma once

#include <kernel/task/process/process_creater.hpp>
#include <kernel/task/process/process_destoryer.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    struct ProcessWalker : ProcessCreater, ProcessDestoryer {};
}