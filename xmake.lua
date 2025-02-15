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
    add_toolchains("gcc")
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
    add_files("source/libc/*.cpp", "source/libc/*.S")
    before_build(function (target) 
        print("开始编译静态库<libc>")
        end)
    after_build(function (target) 
        run_dir = target:rundir()
        os.cp(""..run_dir.."/libc.a", "./library/")
        print("静态库<libc>编译完成，在"..run_dir)
    end)
target("cxx")
    add_toolchains("gcc")
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

target("os-terminal") 
    set_kind("phony")
    before_build(function (target) 
        print("开始编译静态库<libos-terminal>")
        print("注意：此静态库为外部提供")
        end)
    on_build(function (target)
        os.cd("source/libos-terminal")
        os.exec("rustup default nightly")
        os.exec("rm -rf libos_terminal-x86_64.a")
        os.exec("rustup target add x86_64-unknown-none")
        os.exec("cargo install cbindgen")
        os.exec("rustup component add rust-src")
        os.exec("cargo build --release --target x86_64-unknown-none")
        os.exec("mv target/x86_64-unknown-none/release/libos_terminal.a libos_terminal-x86_64.a")
        os.exec("cbindgen --only-target-dependencies --output os_terminal.h")
        os.cd("../..")
        end)
    after_build(function (target)
        local inch_path = "include/libos-terminal"
        local souh_path = "source/libos-terminal"
        local lib_path = "library"
        os.cp("source/libos-terminal/os_terminal.h", inch_path.."/")
        os.cp("source/libos-terminal/libos_terminal-x86_64.a", lib_path.."/")
        print("静态库<libos-terminal>编译完成")
    end) 

target("servicer.elf")
    add_toolchains("gcc")
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
        local ldflags = "-L./library -T ./source/libc/libclinker.lds"
        local libs = "-lcxx -lc"
        os.exec("ld "..ldflags.." -o "..run_dir.."/servicer.elf "..ldfiles.." "..libs)
        os.cp(run_dir.."/servicer.elf", "vm/QuantumNEC/SYSTEM64/")
    end)
    after_build(function (target) 
        run_dir = target:rundir()
        print("模块文件servicer.elf编译完成，在"..run_dir)
    end)

target("micro_kernel")
    add_toolchains("gcc")
    add_deps("c", "cxx", "servicer.elf", "os-terminal") 
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
            -- "-mno-mmx",
            -- "-mno-sse",
            -- "-mno-sse2",
            -- "-mno-80387",
            "-Wextra",
            "-Werror",
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
    add_links("cxx", "c")

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
        local libs = "-lcxx -lc -los_terminal-x86_64"
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
    set_default(true)
    on_build(function (target)
        local qemu_flags =  "-cpu qemu64,+x2apic \
                             -drive if=pflash,format=raw,readonly=on,file=scripts/bios/x86_64efi.bios \
                             -drive file=fat:rw:vm,index=0,format=vvfat \
                             -m 4G \
                             -smp 4,cores=4,threads=1,sockets=1 \
                             -device nec-usb-xhci,id=xhci \
                             -no-shutdown \
                             -device qxl-vga,vgamem_mb=128 \
                             -device ich9-intel-hda \
                             -device virtio-serial-pci \
                             -nic user,model=virtio-net-pci \
                             -device virtio-mouse-pci \
                             -device virtio-keyboard-pci \
                             -name QuantumNEC \
                             -boot order=dc \
                             -net none \
                             -rtc base=localtime -nographic"
                      -- -nographic"-- -d in_asm"   -serial chardev:com1 -chardev stdio,mux=on,id=com1 \ --   
        os.exec("qemu-system-x86_64 "..qemu_flags)
    end)

