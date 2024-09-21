# Makefile
SRC:=.

ENTRYPOINT:=0x10000				# kernel开始的位置

CFLAGS:= -m64					# 64位程序
CFLAGS+= -fno-builtin 			# 不需要gcc内置函数
CFLAGS+= -fno-pic 				# 不需要位置无关代码
CFLAGS+= -fno-pie 				# 不需要位置无关可执行程序
CFLAGS+= -static				# 静态编译
CFLAGS+= -fno-stack-protector	# 不需要栈保护
CFLAGS+= -mcmodel=large			# 更大内存空间
CFLAGS:=$(strip ${CFLAGS})		# 合并

DEBUG:= -g						# 可调试

INCLUDE:= -I$(SRC)		# C++/C头文件位置

ifeq ($(OS),Windows_NT)
RM:=-rd /s /q
BUILD:= ../build
else
RM:=-rm -rf
BUILD:= ../build
endif

# 如果要变量请到上面修改

$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	nasm -f bin $< -o $@

$(BUILD)/kernel/%.o: $(SRC)/kernel/%.asm
	nasm -f elf64 $< -o $@

$(BUILD)/kernel/%.o: $(SRC)/kernel/%.c
	g++ $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel.bin: $(BUILD)/kernel/start.o
# 	后面开发
$(BUILD)/system.bin: $(BUILD)/kernel.bin
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary $< $@

$(BUILD)/system.map: $(BUILD)/kernel.bin
	nm $< | sort > $@

$(BUILD)/vhd/System.vhd: $(BUILD)/QuantumNEC.img
	qemu-img convert -pO vmdk $< $@

# boot.bin
.PHONY: boot.bin
boot.bin: $(BUILD)/boot/boot.bin
# loader.bin
.PHONY: loader.bin
loader.bin: $(BUILD)/boot/loader.bin
# start.o
.PHONY: start.o
start.o: $(BUILD)/kernel/start.o
# main.o
.PHONY: main.o
main.o: $(BUILD)/kernel/main.o
# kernel.bin
.PHONY: kernel.bin
kernel.bin: $(BUILD)/kernel.bin
# system.bin
.PHONY: system.bin
system.bin: $(BUILD)/system.bin
# system.map
.PHONY: system.map
system.map: $(BUILD)/system.map
# System.vhd
.PHONY: System.vhd
System.vhd: $(BUILD)/vhd/System.vhd
# 模式处理

NewFolder:
	mkdir "$(BUILD)"
	mkdir "$(BUILD)/vhd"
	mkdir "$(BUILD)/boot"
	mkdir "$(BUILD)/kernel"
	mkdir "$(BUILD)/media"
compile:\
	NewFolder \
	boot.bin \
	loader.bin \
	start.o \
	main.o \
	kernel.bin \
	system.bin \
	system.map								# 手写编译(写的时候累，编译时轻松)
CreateDisk: compile 						# 创建系统硬盘和光盘映像文件
	qemu-img create $(BUILD)/QuantumNEC.img 512M
	qemu-img create $(BUILD)/vhd/System.vhd 1G
writeData: CreateDisk
# 	dd boot.bin for QuantumNEC.img
	dd if=$(BUILD)/boot/boot.bin of=$(BUILD)/QuantumNEC.img bs=512 count=1 conv=notrunc
#	挂载并写入
	mount $(BUILD)/QuantumNEC.img $(BUILD)/media -o loop -t vfat
	cp $(BUILD)/boot/loader.bin $(BUILD)/media
	sync
	umount $(BUILD)/media
	
ConvertImgToVhd: \
	writeData \
	System.vhd									# 将外部介质转换成硬盘文件
qemu: ConvertImgToVhd 							# 启动Qemu
	qemu-system-x86_64 -hda $(BUILD)/vhd/System.vhd -fda $(BUILD)/QuantumNEC.img -boot a -m 128M
clean: qemu 								# 清理无用文件(爱用就用,懒得写，自己搞去，不搞去死)
	$(RM) $(BUILD)
	clear
run: clean 									# 启动点

# qemu-system-x86_64 -pflash bios.bin -hda fat:rw:hda-contents -net none

# 调试点
.PHONY: text0
text0:
	mkdir "$(BUILD)"
	mkdir "$(BUILD)/vhd"
	mkdir "$(BUILD)/boot"
	mkdir "$(BUILD)/kernel"
	mkdir "$(BUILD)/media"
	mkdir "$(BUILD)/run-ovmf"
	mkdir "$(BUILD)/run-ovmf/hda-contents"
	mkdir "$(BUILD)/esp"
.PHONY: text1
text1:
	qemu-img create $(BUILD)/QuantumNEC.img 1474560B
	qemu-img create $(BUILD)/vhd/System.vhd 1G

	nasm -f bin $(SRC)/boot/boot.asm -o $(BUILD)/boot/boot.bin
	nasm -f bin $(SRC)/boot/loader.asm -o $(BUILD)/boot/loader.bin

#	dd if=$(BUILD)/boot/boot.bin of=$(BUILD)/QuantumNEC.img bs=512 count=1 conv=notrunc
	cp $(BUILD)/boot/boot.bin $(BUILD)/esp -r
	cp $(BUILD)/boot/loader.bin $(BUILD)/esp -r
	
	g++ -E  $(SRC)/kernel/start.S > $(BUILD)/kernel/start.s
	as --64 -o $(BUILD)/kernel/start.o $(BUILD)/kernel/start.s
#	nasm -f elf64 $(SRC)/kernel/start.asm -o $(BUILD)/kernel/head.o
	g++ $(CFLAGS) $(DEBUG) $(INCLUDE) -S $(SRC)/kernel/SystemMain.cpp -o $(BUILD)/kernel/SystemMain.o
	as --64 -o $(BUILD)/kernel/SystemMain.ld $(BUILD)/kernel/SystemMain.o
	ld -b pe-x86-64 -o $(BUILD)/kernel/system.bin $(BUILD)/kernel/start.o $(BUILD)/kernel/SystemMain.ld -T $(SRC)/Kernel.lds
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" $(BUILD)/kernel/system.bin -O binary $(BUILD)/kernel/kernel.sys
	
	cp  $(BUILD)/kernel/kernel.sys $(BUILD)/esp/ -r

#	qemu-img convert -pO vmdk $(BUILD)/QuantumNEC.img $(BUILD)/vhd/System.vhd

.PHONY: text2
text2:
	$(RM) $(BUILD)
.PHONY: text3
QemuB:
	qemu-system-x86_64-hda fat:rw:$(BUILD)/esp/ -boot c -m 256M -device VGA, id=video0, vgamem_mb=256
BochsB:
	bochs -f boot/bochsrc.floppy

test0:
	g++ $(CFLAGS) ./kernel/SystemMain.cpp -S -o o.S
	as --64 -o a.out o.S
	ld -b pe-x86-64 a.out