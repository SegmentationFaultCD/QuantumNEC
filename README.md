# QuantumNEC ![](https://komarev.com/ghpvc/?username=SegmentationFaultCD&color=green)
The operating system is made by a middle student.<br>
It has been developed since 2023-1-1.<br>
In 2022-7-16, I watched a video about developing a operating system, for learning some knowledge about operating system, I decided to make a operating system by myself.<br>
I wish my operating system could run as fast as the Quantum Matrix. For the reason, the OS is named after QuantumNEC.
[![MasterHead](images/background.jpeg)](https://github.com/SegmentationFaultCD)

# Main Language

[![Top Langs](https://github-readme-stats.vercel.app/api/top-langs/?username=SegmentationFaultCD&layout=donut-vertical)](https://github.com/SegmentationFaultCD/QuantumNEC/tree/limine)

# Install and using instructions

## Compile environment

### Linux(Taking Arch Linux as an example)

#### Compile tools
- GCC or Clang
#### Other tools
- Qemu
- Git
- Xmake
#### How to compile source code
 
First, Clone submodules by git. (There are many submodules, for instance, bootloader: [limine](https://github.com/limine-bootloader/limine))
```bash
git submodule update --init
```
Second，write a [limine.conf](./source/boot/limine.conf), enter __QuantumNEC__ directory and execute the order like this:<br>
```bash
xmake -P .
```
After all of files are compiled completely，you will get a __vm__ directory under QuantumNEC directory.<br>
- EFI
  - boot
    - BootX64.efi
    - limine.conf
- QuantumNEC
    - micro_kernel.elf
    - SYSTEM64
      - ...<br>

## Running on the Virtual machine
### qemu
 - You must install qemu before the step.
 - After installing qemu, you can execute the order to start.
```bash
xmake run # 运行
```
## Running on the Physical machine
 - Format your U disk or Hard disk by fat32.
 - copy all of files in the __vm__ directory to your U disk or Hard disk.
# Reference
- Modern Bootloader[limine](https://github.com/limine-bootloader/limine)
# Contribution
If you want to make a contribute to this project, please read[Contribution guidebook](scripts/CONTRIBUTING.md)