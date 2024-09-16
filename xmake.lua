add_rules("mode.debug", "mode.release")

add_cxxflags(" -I source/Include/ \
               -c \
               -g \
               -m64 \
               -fno-builtin \
               -mcmodel=large \
               -ffreestanding \
               -fno-stack-protector \
               -static \
               -nostdlib \
               -nostartfiles \
               -fstrength-reduce \
               -falign-loops \
               -falign-jumps \
               -fno-strict-aliasing \
               -fno-common \
               -Wall \
               -Werror \
               -Wextra \
               -fno-rtti \
               -fno-exceptions \
               -D APIC \
               "
               )

set_optimize("none")
set_languages("c17", "c++23")
target("build")
    set_kind("binary")
    add_files( 
        "source/Kernel/*.cpp", "source/Kernel/*/*.cpp",
        "source/Lib/*/*.cpp", "source/Lib/*/*/*.cpp",
        "source/Utils/*.cpp"
    )
    architecture_dir = "source/Arch/x86_64"
    ld_architecture = "elf_x86_64"
    architecture = "x86_64"
    before_build(function (target) 
        print("创建虚拟机文件夹")
        os.mkdir("vm")
        os.mkdir("vm/EFI")
        os.mkdir("vm/EFI/Boot")
        os.mkdir("vm/QuantumNEC")
        os.mkdir("vm/QuantumNEC/SYSTEM64")
        print("复制所需要的文件")
        os.exec("cp source/Boot/Info/target.txt edk2/Conf/ -r")
        os.exec("cp source edk2/QuantumNECPkg -r")
        os.exec("cp source/Boot/Logo/SystemLogo.bmp vm/EFI/Boot/Logo.BMP -r")
        os.exec("cp source/Boot/Logo/SystemLogo(4-3).bmp vm/EFI/Boot/Narrow.BMP -r")
        os.exec("cp source/Boot/Info/SystemLog.log vm/QuantumNEC/SYSTEM64/SystemLogger.log -r")
        os.exec("cp resource/Unicode.dll vm/QuantumNEC/SYSTEM64/Unicode.dll -r")
        os.exec("cp source/Boot/Info/Config.ini vm/EFI/Boot/Config.ini -r")
        print("依靠EDK2制作UEFI引导文件")
        os.exec("bash source/build.sh")
        print("引导文件制作完成")
    end)
    add_files("source/Arch/x86_64/*/*.cpp","source/Arch/x86_64/*/*.S", "source/Arch/x86_64/*/*/*.cpp" )
    on_link(function(target)
        print("编译与链接内核文件")
        object_dir = target:objectdir()
        run_dir = target:rundir()
        object_string = ""
        for key,val in pairs(target:objectfiles()) do 
            object_string = object_string..val.." "
        end
        if target:is_arch("x86_64") then
            os.exec("cp edk2/Build/DEBUG_GCC5/X64/BootX64.efi vm/EFI/Boot/ -r")
        end
        os.mkdir(run_dir)
        os.exec("ld -m elf_x86_64 --allow-multiple-definition --no-warn-rwx-segments -z muldefs -flto -s "..object_string.." -o "..run_dir.."/microKernel.elf  -T source/Arch/x86_64/System.lds")
        os.exec("cp "..run_dir.."/microKernel.elf vm/QuantumNEC/ -r")
    end)
    before_build(function (target) 
        print("运行Qemu")
        os.exec(
            "qemu-system-x86_64 \
            -drive if=pflash,format=raw,readonly=on,file=source/Boot/Info/OVMF.fd \
            -m 8G \
            -smp 2,cores=2,threads=1,sockets=1 \
            -drive file=fat:rw:vm,index=0,format=vvfat \
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
            -rtc base=localtime \
            "
        )
    end)

target("clean")
    before_build(function (target) 
        print("开始清除不必要的文件")
        os.exec("rm -rf vm")
        os.exec("rm -rf build.sh")
        os.exec("rm -rf build")
        os.exec("rm -rf edk2/Build")
        os.exec("rm -rf edk2/QuantumNECPkg")
     end)
--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

