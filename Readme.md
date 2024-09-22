# 简介
 一个自制的操作系统，最现代，最快速，最安全，最轻量级（The most modern, The Fastest, The safest and the most miniature(MFSS)）<br>
 它由一个**初一学生**从2023-1-1日开始开发
# 由来
 这个OS，怎么说呢，是我看了别人视频，突发奇想开发的<br>
 为此，我把它命名为**QuantumNEC**---__量子死亡脉冲__<br>
 "**Quantum**"的意思是"**量子**"，意味着这个操作系统的运行速度和量子矩阵传播速度一样快(虽然几乎不可能),<br>
 "**NEC**"是"**死亡脉冲**"，我也不知道怎么取得🤣,<br>
 弄这个东西还是为了学习底层知识，__*炫技（划掉）*__<br>
# 安装与使用说明
 首先，你需要自己安装了GNU系列编译工具链（GCC & G++, AR, NM, OBJCOPY等）<br>
 其次，你需要安装QEMU，Git以及Xmake(arch linux直接pacman即可，windows用户请自查)<br>
 想要编译OS，你需要安装**Limine**<br>
 如果您想要安装**limine**，请按照如下方法:
```bash
git submodule update --init
```
 并且，编写**limine.conf**，或者就用我写的
 然后，进入QuantumNEC文件夹，编译静态库
```bash
xmake -F source/Libc/xmake.lua        # 编译C库
xmake -F source/Libcxx/xmake.lua      # 编译C++库
xmake -F source/Lib/xmake.lua         # 编译系统库
```
 等待编译完成后，开始编译链接内核
```bash
xmake build_kernel # 编译内核文件
```
 然后，开始编译链接模块驱动
```bash
xmake -F source/Modules/xmake.lua # 编译模块驱动文件
```
 等待所有文件编译链接完成后，在主文件夹下得到vm文件，目录如下
- EFI
  - boot
    - BootX64.efi 此为limine引导文件
    - limine.conf  此为limine引导配置文件
- QuantumNEC
    - micro_kernel.elf 此为内核文件
    - SYSTEM64
      - Unicode.bin
      - switch.elf
      - servicer.elf
      - to_process.elf
      - ...所有模块文件
 注：有时可能会编译错误，那是作者代码没写好😋
## 虚拟平台启动
### qemu（你想要别的虚拟机也行，自己弄）
 - 安装Qemu(已经安装的可以跳过)
 - 在主文件夹下运行如下命令以启动
```bash
xmake run # 运行
```
## 物理平台启动
 
 - 将vm的内容全部复制到u盘或者硬盘中，在此之前记得格式化为fat32格式

# 依赖

- [limine](https://github.com/limine-bootloader/limine/tree/19b41531b6130801475f1421d998fc8de69a8096)
    使用现代化引导limine进行来装载内核，不需要自己编写boot loader

# 贡献
如果您想为此项目做出贡献，请阅读[贡献指南](https://github.com/用户名/仓库名/blob/master/CONTRIBUTING.md)。

