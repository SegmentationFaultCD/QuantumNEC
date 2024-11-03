# 简介
 一个自制的操作系统，最现代，最快速，最安全，最轻量级（The most modern, the fastest, the safest and the most miniature(MFSS)）<br>
 它由一个**初一学生**从2023-1-1日开始开发
# 由来
 这个OS，怎么说呢，是我看了别人视频，突发奇想开发的<br>
 为此，我把它命名为**QuantumNEC**---__量子死亡脉冲__<br>
 "**Quantum**"的意思是"**量子**"，意味着这个操作系统的运行速度和量子矩阵传播速度一样快(虽然几乎不可能),<br>
 "**NEC**"是"**死亡脉冲**"，我也不知道怎么取得🤣,<br>
 弄这个东西还是为了学习底层知识，__*炫技（划掉）*__<br>
# 安装与使用说明

## 编译环境

### Linux(以Arch Linux为例)

#### 编译工具

- GNU系列编译工具链（GCC & G++, AR, NM, OBJCOPY等），其中GCC要最新<br>
   - Q:为什么要最新
   - A:因为作者用了很多C++新特性
#### 其他依赖工具
- Qemu
- Git
- Xmake
#### 编译过程
 想要编译OS，你需要安装**Limine**依赖
```bash
git submodule update --init
```
 并且，编写**limine.conf**，或者就用我写的<br>
 然后，进入QuantumNEC文件夹，编译
```bash
    xmake
```
 等待所有文件编译链接完成后，在主文件夹下得到vm文件，目录如下
- EFI
  - boot
    - BootX64.efi 此为limine引导文件
    - limine.conf  此为limine引导配置文件
- QuantumNEC
    - micro_kernel.elf 此为内核文件
    - SYSTEM64
      - ...所有模块文件<br>
### Windows(以Windows 10为例)
 这里什么也没有啊（悲
### 注意事项
 有时可能会编译错误，那是作者代码没写好😋
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
- 使用现代化引导[limine](https://github.com/limine-bootloader/limine)进行来装载内核，不需要自己编写boot loader
# 贡献
如果您想为此项目做出贡献，请阅读[贡献指南](CONTRIBUTING.md)