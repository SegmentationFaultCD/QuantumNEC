set_project("QuantumNEC")

add_rules("mode.debug", "mode.release")

-- limine的和自己的头文件路径
add_includedirs("./include", "source/boot/limine")
-- 架构自选
set_arch("x86-64")
-- 不优化
set_optimize("none")

set_languages("c23", "c++26") 

target("sys")
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
            "-fno-lto",
            "-fPIC", {force = true}
    )
    set_kind("static")
    add_files("source/lib/*.cpp")
    before_build(function (target) 
        print("开始编译静态库<libsys>")
        end)
    after_build(function (target) 
        run_dir = target:rundir()
        os.cp(""..run_dir.."/libsys.a", "./library/")
        print("静态库<libsys>编译完成，在"..run_dir)
    end)
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
            "-fno-lto",
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
            "-fno-lto",
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
target("micro_kernel")
    add_deps("sys", "c", "cxx")

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
            "-ffunction-sections",-- 优化
            "-fdata-sections", -- 优化
            "-D APIC",
            "-fPIE",        
            "-fno-lto",
            "-Wno-reorder", {force = true} -- 构造函数的初始化顺序不固定
    )
 
    add_linkdirs("library")
    add_links("sys", "cxx", "c")

    add_files(
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
        os.cp("source/boot/OVMF.fd", "vm/")
    end)
    on_build(function (target)
        local object_dir = target:objectdir()
        local run_dir = target:rundir()
        local ldfiles = ""
        for key,val in pairs(target:objectfiles()) do 
            ldfiles = ldfiles..val.." "
        end
        local ldflags = "-L./library"
        local libs = "-lsys -lcxx -lc"
        local lds = "source/kernel/System.lds"
        os.exec("ld "..ldflags.." -o "..run_dir.."/micro_kernel.elf "..ldfiles.." "..libs.." -T "..lds)
        os.cp(run_dir.."/micro_kernel.elf", "vm/QuantumNEC/")
    end)
    after_build(function (target)
        run_dir = target:rundir()
        print("编译内核完成")
    end)
target("qemu")
    set_kind("phony")
    add_deps("micro_kernel")
    set_default(true)
    on_build(function (target)
        local qemu_flags = "-cpu qemu64,x2apic \
                      -m 8G \
                      -smp 4,cores=4,threads=1,sockets=1 \
                      -device nec-usb-xhci,id=xhci \
                      -no-shutdown \
                      -chardev stdio,mux=on,id=com1  \
                      -serial chardev:com1 \
                      -device qxl-vga,vgamem_mb=128 \
                      -device ich9-intel-hda \
                      -device virtio-serial-pci \
                      -nic user,model=virtio-net-pci \
                      -device virtio-mouse-pci \
                      -device virtio-keyboard-pci \
                      -name QuantumNEC \
                      -boot order=dc \
                      -net none \
                      -rtc base=localtime"
        os.exec("qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=vm/OVMF.fd -drive file=fat:rw:vm,index=0,format=vvfat "..qemu_flags)
    end)

