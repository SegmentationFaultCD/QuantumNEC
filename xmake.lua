add_rules("mode.debug", "mode.release")
add_cxxflags("-m64")
add_cxxflags("-fno-builtin")
add_cxxflags("-mcmodel=large")
add_cxxflags("-ffreestanding")
add_cxxflags("-fno-stack-protector")
add_cxxflags("-nostdlib")
add_cxxflags("-nostartfiles")
add_cxxflags("-fstrength-reduce")
add_cxxflags("-falign-loops")
add_cxxflags("-falign-jumps")
add_cxxflags("-fno-strict-aliasing")
add_cxxflags("-fno-common")
add_cxxflags("-fno-rtti")
add_cxxflags("-fno-exceptions")
add_cxxflags("-mno-red-zone")
add_cxxflags("-fno-stack-check")
add_cxxflags("-fno-lto")
add_cxxflags("-Wall")
add_cxxflags("-Wextra")
add_cxxflags ("-ffunction-sections")
add_cxxflags ("-fdata-sections")
-- add_cxxflags("-Werror")
add_cxxflags("-D APIC")
add_cxxflags("-fPIE")
add_cxxflags("-Wno-reorder")
add_includedirs("./include", "source/boot/limine")

set_optimize("none")
set_languages("c17", "c++23")

target("build")
    set_kind("binary")
    add_files( 
        "source/kernel/*.cpp",
        "source/kernel/*/*.cpp",
        "source/kernel/*/*/*.cpp",
        "source/kernel/*/*/*/*.cpp",
        "source/kernel/*/*/*/*/*.cpp",
        "source/kernel/*/*/*/*/*.S",
        "source/modules/*.cpp",
        "source/modules/loader/*.cpp"
    )
    
    before_build(function (target) 
        print("创建虚拟机文件夹")
        os.mkdir("vm")
        os.mkdir("vm/EFI")
        os.mkdir("vm/EFI/Boot")
        os.mkdir("vm/QuantumNEC")
        os.mkdir("vm/QuantumNEC/SYSTEM64")
        print("复制所需要的文件")
        os.exec("cp ./source/boot/limine.conf ./vm/EFI/Boot/ -r")
        os.exec("cp ./source/boot/limine/BOOTX64.EFI ./vm/EFI/Boot/ -r")
        os.exec("cp ./source/boot/OVMF.fd ./vm/ -r")
    end)
    on_link(function (target)
        run_dir = target:rundir()
        object_string = ""
        for key,val in pairs(target:objectfiles()) do 
            object_string = object_string..val.." "
        end
        print("编译链接内核文件")
        os.exec("ld -m elf_x86_64 --allow-multiple-definition --no-warn-rwx-segments -z muldefs -flto -s "..object_string.." "..run_dir.."/libcxx.a "..run_dir.."/libsys.a -o "..run_dir.."/micro_kernel.elf -T source/kernel/System.lds")
    end)
    after_build(function (target)
        run_dir = target:rundir()
        object_string = ""
        for key,val in pairs(target:objectfiles()) do 
            object_string = object_string..val.." "
        end
        os.exec("cp "..run_dir.."/micro_kernel.elf ./vm/QuantumNEC/ -r")
        os.exec("cp "..run_dir.."/servicer.elf ./vm/QuantumNEC/SYSTEM64/ -r")
        os.exec(
            "qemu-system-x86_64 \
            -drive if=pflash,format=raw,readonly=on,file=vm/OVMF.fd \
            -cpu qemu64,x2apic \
            -m 8G \
            -smp 4,cores=4,threads=1,sockets=1 \
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

