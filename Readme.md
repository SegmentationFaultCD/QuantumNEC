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
 其次，你需要安装QEMU与Xmake(arch linux直接pacman即可，windows用户请自查)<br>
 然后，首先进入QuantumNEC文件夹，执行以下命令
```bash
xmake -F source/Libc
xmake -F source/Libc
```
 



# 依赖

- [limine](https://github.com/limine-bootloader/limine/tree/19b41531b6130801475f1421d998fc8de69a8096)
    使用现代化引导limine进行来装载内核，不需要自己编写boot loader

# 贡献
如果您想为此项目做出贡献，请阅读[贡献指南](https://github.com/用户名/仓库名/blob/master/CONTRIBUTING.md)。

