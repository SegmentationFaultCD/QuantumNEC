#pragma once
#include <limine.h>
namespace Kernel {
class Loader {
private:
    Loader( void ) {}

public:
    static auto initialize( limine_module_response *modules ) -> Loader *;

private:
} inline *module_loader;

}     // namespace Kernel