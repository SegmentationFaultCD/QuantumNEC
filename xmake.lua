set_project("QuantumNEC")

add_rules("mode.debug", "mode.release")

-- limine的和自己的头文件路径
add_includedirs("./include", "source/boot/limine")
-- 架构自选
set_arch("x86-64")
-- 不优化
set_optimize("none")

set_languages("c23", "c++26") 
target("c")
    add_cxxflags(
            "-fno-builtin", -- 不要内建函数
            "-mcmodel=large", -- 大内存模式
            "-ffreestanding", -- 生成不依赖于任何操作系统或运行环境的代码
            "-fno-stack-protector", -- 不要栈保护
            "-nostdlib", -- 不要标准库
            "-nostartfiles", -- 不要默认启动文件
            "-fno-strict-aliasing", -- 关闭严格的别名规则优化
            "-fno-common", -- 共享全局变量
            "-fno-rtti", -- 不要运行时类型信息鉴别
            "-fno-exceptions", -- 不需要异常
            "-mno-red-zone", -- 禁用红色区域
            "-fno-stack-check", -- 不要栈检查
            "-Wall", 
            "-Wextra",
            "-Werror", 
            "-fPIC", {force = true}
    )
    set_kind("static")
    add_files("source/libc/*.cpp")
    before_build(function (target) 
        print("开始编译静态库<libc>")
        end)
    after_build(function (target) 
        run_dir = target:rundir()
        os.cp(""..run_dir.."/libc.a", "./library/")
        print("静态库<libc>编译完成，在"..run_dir)
    end)
target("cxx")
    add_deps("c")
    add_cxxflags(
            "-fno-builtin", -- 不要内建函数
            "-mcmodel=large", -- 大内存模式
            "-ffreestanding", -- 生成不依赖于任何操作系统或运行环境的代码
            "-fno-stack-protector", -- 不要栈保护
            "-nostdlib", -- 不要标准库
            "-nostartfiles", -- 不要默认启动文件
            "-fno-strict-aliasing", -- 关闭严格的别名规则优化
            "-fno-common", -- 共享全局变量
            "-fno-rtti", -- 不要运行时类型信息鉴别
            "-fno-exceptions", -- 不需要异常
            "-mno-red-zone", -- 禁用红色区域
            "-fno-stack-check", -- 不要栈检查
            "-Wall", 
            "-Wextra",
            "-Werror",
            "-fPIC", {force = true}
    )
    set_kind("static")
    
    add_files("source/libcxx/*.cpp")
 
    before_build(function (target) 
        print("开始编译静态库<libcxx>")
        end)
    after_build(function (target) 
        run_dir = target:rundir()
        os.cp(""..run_dir.."/libcxx.a", "./library/")
        print("静态库<libcxx>编译完成，在"..run_dir)
    end)
target("servicer.elf")
    add_deps("c", "cxx")
    add_cxxflags(
            "-fno-builtin", -- 不要内建函数
            "-mcmodel=large", -- 大内存模式
            "-ffreestanding", -- 生成不依赖于任何操作系统或运行环境的代码
            "-fno-stack-protector", -- 不要栈保护
            "-nostdlib", -- 不要标准库
            "-nostartfiles", -- 不要默认启动文件
            "-fno-strict-aliasing", -- 关闭严格的别名规则优化
            "-fno-common", -- 共享全局变量
            "-fno-rtti", -- 不要运行时类型信息鉴别
            "-fno-exceptions", -- 不需要异常
            "-mno-red-zone", -- 禁用红色区域
            "-fno-stack-check", -- 不要栈检查
            "-Wall", 
            "-Wextra",
            "-Werror",
            "-static",
            "-fPIE", {force = true}
    )
    set_kind("binary") 
    add_files("source/modules/service/servicer.cpp")
    add_linkdirs("library")
    add_links("c", "cxx")
    add_ldflags("-ffreestanding")
    before_build(function (target) 
        print("开始编译模块文件servicer.elf")
        end)
    on_link(function (target)
        local object_dir = target:objectdir()
        local run_dir = target:rundir()
        local ldfiles = ""
        for key,val in pairs(target:objectfiles()) do 
            ldfiles = ldfiles..val.." "
        end
        local ldflags = "-L./library -e __libc_start_main"
        local libs = "-lcxx -lc"
        os.exec("ld "..ldflags.." -o "..run_dir.."/servicer.elf "..ldfiles.." "..libs)
        os.cp(run_dir.."/servicer.elf", "vm/QuantumNEC/SYSTEM64/")
    end)
    after_build(function (target) 
        run_dir = target:rundir()
        print("模块文件servicer.elf编译完成，在"..run_dir)
    end)
target("test02.elf")
    add_deps("c", "cxx")
    add_cxxflags(
            "-fno-builtin", -- 不要内建函数
            "-mcmodel=large", -- 大内存模式
            "-ffreestanding", -- 生成不依赖于任何操作系统或运行环境的代码
            "-fno-stack-protector", -- 不要栈保护
            "-nostdlib", -- 不要标准库
            "-nostartfiles", -- 不要默认启动文件
            "-fno-strict-aliasing", -- 关闭严格的别名规则优化
            "-fno-common", -- 共享全局变量
            "-fno-rtti", -- 不要运行时类型信息鉴别
            "-fno-exceptions", -- 不需要异常
            "-mno-red-zone", -- 禁用红色区域
            "-fno-stack-check", -- 不要栈检查
            "-Wall", 
            "-Wextra",
            "-Werror",
            "-static",
            "-fPIE", {force = true}
    )
    set_kind("binary") 
    add_files("source/modules/service/test02.cpp")
    add_linkdirs("library")
    add_links("c", "cxx")
    add_ldflags("-ffreestanding")
    before_build(function (target) 
        print("开始编译模块文件test02.elf")
        end)
    on_link(function (target)
        local object_dir = target:objectdir()
        local run_dir = target:rundir()
        local ldfiles = ""
        for key,val in pairs(target:objectfiles()) do 
            ldfiles = ldfiles..val.." "
        end
        local ldflags = "-L./library -e __libc_start_main"
        local libs = "-lcxx -lc"
        os.exec("ld "..ldflags.." -o "..run_dir.."/test02.elf "..ldfiles.." "..libs)
        os.cp(run_dir.."/test02.elf", "vm/QuantumNEC/SYSTEM64/")
    end)
    after_build(function (target) 
        run_dir = target:rundir()
        print("模块文件test02.elf编译完成，在"..run_dir)
    end)
  target("test03.elf")
    add_deps("c", "cxx")
    add_cxxflags(
            "-fno-builtin", -- 不要内建函数
            "-mcmodel=large", -- 大内存模式
            "-ffreestanding", -- 生成不依赖于任何操作系统或运行环境的代码
            "-fno-stack-protector", -- 不要栈保护
            "-nostdlib", -- 不要标准库
            "-nostartfiles", -- 不要默认启动文件
            "-fno-strict-aliasing", -- 关闭严格的别名规则优化
            "-fno-common", -- 共享全局变量
            "-fno-rtti", -- 不要运行时类型信息鉴别
            "-fno-exceptions", -- 不需要异常
            "-mno-red-zone", -- 禁用红色区域
            "-fno-stack-check", -- 不要栈检查
            "-Wall", 
            "-Wextra",
            "-Werror",
            "-static",
            "-fPIE", {force = true}
    )
    set_kind("binary") 
    add_files("source/modules/service/test03.cpp")
    add_linkdirs("library")
    add_links("c", "cxx")
    add_ldflags("-ffreestanding")
    before_build(function (target) 
        print("开始编译模块文件test03.elf")
        end)
    on_link(function (target)
        local object_dir = target:objectdir()
        local run_dir = target:rundir()
        local ldfiles = ""
        for key,val in pairs(target:objectfiles()) do 
            ldfiles = ldfiles..val.." "
        end
        local ldflags = "-L./library -e __libc_start_main"
        local libs = "-lcxx -lc"
        os.exec("ld "..ldflags.." -o "..run_dir.."/test03.elf "..ldfiles.." "..libs)
        os.cp(run_dir.."/test03.elf", "vm/QuantumNEC/SYSTEM64/")
    end)
    after_build(function (target) 
        run_dir = target:rundir()
        print("模块文件test03.elf编译完成，在"..run_dir)
    end)
 


target("micro_kernel")
    add_deps("c", "cxx", "test02.elf", "servicer.elf") 

    set_kind("binary")
    add_cxxflags(
            "-fno-builtin", -- 不要内建函数
            "-mcmodel=large", -- 大内存模式
            "-ffreestanding", -- 生成不依赖于任何操作系统或运行环境的代码
            "-fno-stack-protector", -- 不要栈保护
            "-nostdlib", -- 不要标准库
            "-nostartfiles", -- 不要默认启动文件
            "-fno-strict-aliasing", -- 关闭严格的别名规则优化
            "-fno-common", -- 共享全局变量
            "-fno-rtti", -- 不要运行时类型信息鉴别
            "-fno-exceptions", -- 不需要异常
            "-mno-red-zone", -- 禁用红色区域
            "-fno-stack-check", -- 不要栈检查
            "-Wall", 
            "-Wextra",
            -- "-Werror",
            "-D APIC",
            "-fPIE",
            "-Wpointer-arith",
            "-Wno-missing-field-initializers",
            "-Wwrite-strings",
            -- "-Wcast-align",
            -- "-Wmissing-prototypes",
            -- "-Wmissing-declarations",
            -- "-Wredundant-decls",
            -- "-Wnested-externs", 
            -- "-Wstrict-prototypes", 
            "-Wno-reorder", {force = true} -- 构造函数的初始化顺序不固定
    )
 
    add_linkdirs("library")
    add_links("sys", "cxx", "c")

    add_files(
        "source/boot/*.cpp",
        "source/kernel/**/*.cpp",
        "source/kernel/*.cpp",
        "source/kernel/**/*.S",
        "source/modules/loader/*.cpp",
        "source/modules/*.cpp"
    )
    
    before_build(function (target) 
        print("开始编译内核")
        os.mkdir("vm")
        os.mkdir("vm/EFI")
        os.mkdir("vm/EFI/Boot")
        os.mkdir("vm/QuantumNEC")
        os.mkdir("vm/QuantumNEC/SYSTEM64")
        os.cp("source/boot/limine.conf", "./vm/EFI/Boot/")
        os.cp("source/boot/limine/BOOTX64.EFI", "vm/EFI/Boot/")
        os.cp("images/wallpaper.jpg", "vm/EFI/")
    end)
    on_link(function (target)
        local object_dir = target:objectdir()
        local run_dir = target:rundir()
        local ldfiles = ""
        for key,val in pairs(target:objectfiles()) do 
            ldfiles = ldfiles..val.." "
        end
        local ldflags = "-L./library"
        local libs = "-lsys -lcxx -lc -los_terminal"
        local lds = "scripts/linker/x86_64linker.lds"
        os.exec("ld "..ldflags.." -o "..run_dir.."/micro_kernel.elf "..ldfiles.." "..libs.." -T "..lds)
        os.cp(run_dir.."/micro_kernel.elf", "vm/QuantumNEC/")
    end)
    after_build(function (target)
        run_dir = target:rundir()
        print("编译内核完成")
    end)
target("run")
    set_kind("phony")
    add_deps("micro_kernel")
    set_default(true)
    on_build(function (target)
        local qemu_flags =  "-enable-kvm \
                             -drive if=pflash,format=raw,readonly=on,file=scripts/bios/x86_64efi.bios \
                             -drive file=fat:rw:vm,index=0,format=vvfat \
                             -m 16G \
                             -smp 2,cores=2,threads=1,sockets=1 \
                             -device nec-usb-xhci,id=xhci \
                             -no-shutdown \
                             -device qxl-vga,vgamem_mb=128 \
                             -device ich9-intel-hda \
                             -device virtio-serial-pci \
                             -nic user,model=virtio-net-pci \
                             -device virtio-mouse-pci \
                             -device virtio-keyboard-pci \
                             -serial chardev:com1 -chardev stdio,mux=on,id=com1 \
                             -name QuantumNEC \
                             -boot order=dc \
                             -net none \
                             -rtc base=localtime"
                      -- -nographic"-- -d in_asm"    --   
        os.exec("qemu-system-x86_64 "..qemu_flags)
    end)

