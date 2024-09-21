add_rules("mode.release", "mode.debug")
add_cxxflags(" -m64 \
               -fno-builtin \
               -mcmodel=large \
               -ffreestanding \
               -fno-stack-protector \
               -nostdlib \
               -nostartfiles \
               -fstrength-reduce \
               -falign-loops \
               -falign-jumps \
               -fno-strict-aliasing \
               -fno-common \
               -fno-rtti \
               -fno-exceptions \
               -mno-red-zone \
               -fno-stack-check \
               -fno-lto \
               -Wall \
               -Wextra \
               -fPIC \
               ")
add_includedirs("../Include", "../Boot/limine/")
set_optimize("none")
set_languages("c17", "c++23")

target("s")
    set_kind("static")
    add_files("./*.cpp" )
    before_build(function (target) 
        print("开始编译库文件")
        end)
    after_build(function (target) 
        run_dir = target:rundir()
        print("静态库编译完成，在"..run_dir.."")
    end)