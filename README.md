# GobangC

一个Windows平台上的基于C的控制台五子棋游戏，实现本地多人游戏、本地人机对战和基于TCP协议的联网多人游戏。

## 项目结构

- main.c 主程序
- gobang.h 五子棋主要逻辑(含输赢判定等)
- ui.h 用户界面头文件
- ai.h 人机对战
- internet.h 联网对战
- message.h 语言以及内部交互规范

## 已知Bug

- 联机对战时，悔棋请求可能会发送两次
- 对方的下棋无效时，自己也能看见
- 对战时按住鼠标左键并拖动可以跳过对方回合连续下棋

## 如何使用

- 启动程序后，在标题栏上右键，点击属性，将`选项`选项卡中的`快速编辑模式`选项的勾去掉
- 完成上述设置后，输入`0`并回车进行本地人机游戏，输入`1`并回车进行本地多人游戏，输入`2`并回车进行联网多人游戏
- 在进入联网多人模式之后，选择`1`为客户端模式，进入后要求输入服务端的IP地址并连接到服务端，选择`2`为服务器模式，进入后监听`0.0.0.0:5050`，等待客户端连接。需要特别说明的是，当您与对手不处于同个局域网内，您需要拥有公网IP地址或者进行端口映射才能正常进行游戏。
- 进入游戏界面之后，用鼠标进行操作即可。
- 在游戏的界面中，设有`[Undo]`悔棋按钮和`[Surender]`投降按钮，在本地多人游戏模式下悔棋和投降按钮均直接生效，多人模式下必须在您的回合内才能点击两个按钮，且悔棋需要经过对方同意。
- 多人模式和人机模式下黑方白方随机分配

## 编译

```powershell
gcc main.c -o gobang.exe -lws2_32 -fexec-charset=gbk -finput-charset=gbk
```

