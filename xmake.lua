set_project("QuantumNEC")

add_rules("mode.debug")

-- limine的和自己的头文件路径
add_includedirs("./include", "source/kernel/boot/limine", "source/lib/libfmt/include/", "source/lib/libos-terminal")
-- 架构自选
set_arch("x86-64")
-- 不优化
set_optimize("none")

set_languages("c23", "c++26") 

target("filesystem")
    add_toolchains("clang") 
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
            "-static",
            "-fPIE", {force = true} -- 构造函数的初始化顺序不固定
    )   
    
    add_ldflags("-fuse-ld=lld","-static","-nostdlib", {force = true}, "-target x86_64-freestanding", "-e main") 
    add_files(
        "source/module/filesystem/*.cpp",
        "source/module/filesystem/*.S"
    )
    after_build(function (target)
        run_dir = target:rundir()
        os.cp(run_dir.."/filesystem", "vm/OS/bin/")
    end)

target("micro_kernel.elf")
    add_toolchains("clang") 
    set_kind("binary")
    add_cxxflags(
            "-fno-builtin", -- 不要内建函数
            "-mcmodel=kernel",
            "-fno-stack-protector", -- 不要栈保护
            "-nostdlib", -- 不要标准库
            "-nostartfiles", -- 不要默认启动文件
            "-fno-strict-aliasing", -- 关闭严格的别名规则优化
            "-fno-rtti", -- 不要运行时类型信息鉴别
            "-fno-exceptions", -- 不需要异常
            "-mno-red-zone", -- 禁用红色区域
            "-fno-stack-check", -- 不要栈检查
            "-Wall", 
            "-Wextra", 
            "-static",
            "-fPIC",
             "-g3",
            "-Wpointer-arith",
            "-Wno-missing-field-initializers",
            "-Wwrite-strings",
            "-fno-threadsafe-statics", 
            "-Wno-reorder", {force = true} -- 构造函数的初始化顺序不固定
    )   
    add_linkdirs("source/lib/libos-terminal")
    add_links("os_terminal")
    add_ldflags("-fuse-ld=lld","-static","-nostdlib", {force = true}, "-target x86_64-freestanding", "-T scripts/linker/x86_64linker.lds") 
    add_files(
        "source/kernel/*/*.cpp",
        "source/kernel/*/*.S",
        "source/kernel/*/*/*.cpp", 
        "source/lib/*.cpp"
    )

    before_build(function (target) 
        os.mkdir("vm")
        os.mkdir("vm/EFI")
        os.mkdir("vm/EFI/boot")
        os.mkdir("vm/OS")
        os.mkdir("vm/OS/bin")
        os.cp("source/kernel/boot/limine.conf", "./vm/EFI/boot/")
        os.cp("source/kernel/boot/limine/BOOTX64.EFI", "vm/EFI/boot/")
        os.cp("images/wallpaper.jpg", "vm/EFI/")
    end)
    after_build(function (target)
        run_dir = target:rundir()
        os.cp(run_dir.."/micro_kernel.elf", "vm/OS/")
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
                             -rtc base=localtime -no-reboot -D qemu.log -d int" --  -no-reboot -D qemu.log -d in_asm  -S -s
        os.exec("qemu-system-x86_64 "..qemu_flags)
    end)

