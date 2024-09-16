add_rules("mode.debug", "mode.release")
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
               -fPIE \
               ")
add_includedirs("../../Include")
set_optimize("none")
set_languages("c17", "c++23")

target("servicer.elf")
    set_kind("binary")
    add_files("servicer.cpp" )
    before_build(function (target) 
        print("开始编译模块文件servicer.elf")
    end)
    on_link(function (target)  
        object_dir = target:objectdir()
        run_dir = target:rundir()
        object_string = ""
        for key,val in pairs(target:objectfiles()) do 
            object_string = object_string..val.." "
        end 
        os.mkdir(run_dir)
        os.exec("ld -m elf_x86_64 --allow-multiple-definition --no-warn-rwx-segments -z muldefs -flto -s -z noexecstack\
                "..object_string.." "..run_dir.."/libcxx.a "..run_dir.."/libc.a -o "..run_dir.."/servicer.elf" )
    end)
    after_build(function (target) 
        run_dir = target:rundir()
        print("模块文件servicer.elf编译完成，在"..run_dir.."")
    end)

target("switch_to.elf")
    set_kind("binary")
    add_files("switch_to.S" )
    add_files("../../../build/linux/x86_64/release/libcxx.a")
    before_build(function (target) 
        print("开始编译模块文件")
    end)
    on_link(function (target)  
        object_dir = target:objectdir()
        run_dir = target:rundir()
        object_string = ""
        for key,val in pairs(target:objectfiles()) do 
            object_string = object_string..val.." "
        end 
        os.mkdir(run_dir)
        os.exec("ld -m elf_x86_64 --allow-multiple-definition --no-warn-rwx-segments -z muldefs -flto -s -z noexecstack "..object_string.." "..run_dir.."/libcxx.a -o "..run_dir.."/switch_to.elf" )
    end)
    after_build(function (target) 
        run_dir = target:rundir()
        print("模块文件switch_to.elf编译完成，在"..run_dir..", 开始清理为纯二进制文件...")
        os.exec("objcopy -I elf64-x86-64 -S -R \".eh_frame\" -R \".comment\" -O binary "..run_dir.."/switch_to.elf "..run_dir.."/switch_to.bin")
        
    end)
target("to_process.elf")
    set_kind("binary")
    add_files("switch_to.S" )
    add_files("../../../build/linux/x86_64/release/libcxx.a")
    before_build(function (target) 
        print("开始编译模块文件")
    end)
    on_link(function (target)  
        object_dir = target:objectdir()
        run_dir = target:rundir()
        object_string = ""
        for key,val in pairs(target:objectfiles()) do 
            object_string = object_string..val.." "
        end 
        os.mkdir(run_dir)
        os.exec("ld -m elf_x86_64 --allow-multiple-definition --no-warn-rwx-segments -z muldefs -flto -s -z noexecstack "..object_string.." "..run_dir.."/libcxx.a -o "..run_dir.."/to_process.elf" )
    end)
    after_build(function (target) 
        run_dir = target:rundir()
        print("模块文件to_process.elf编译完成，在"..run_dir..", 开始清理为纯二进制文件...")
        os.exec("objcopy -I elf64-x86-64 -S -R \".eh_frame\" -R \".comment\" -O binary "..run_dir.."/to_process.elf "..run_dir.."/to_process.bin")
        
    end)