# UnrealFRP – UE4/5 多人广域网联机房间系统
## Languages
- [English](EN.md)
## 简介
UnrealFRP 是一个基于 `FRP（Fast Reverse Proxy）` 的 `Unreal Engine` 联机房间系统插件，包含 `插件` + `服务端` + `演示客户端`，用于在 `UE4/UE5` 中快速搭建 `广域网联机房间系统`

![img](img.png)
## 项目结构
- FRPClient（UE演示客户端）
- FRPServer（服务端）
- UnrealFRP（UE插件）
## 功能简介
- 创建/关闭房间  
- 连接至房间  
- 房间列表  
- 房间名称/属性/密码 
## 开发环境
- Win10 x64 22H2
- Unreal Engine 4.27.2
- Visual Studio 2022
## 如何使用演示客户端？
1. 使用 `VS2022` 或 `其他CMake工具` 打开 `FRPServer` 并进行编译
2. 运行编译好的 `FRPServer/windows/FRPServer.exe`
3. 打开UE演示客户端
4. 以 `Standalone Game` 运行
## 如何添加到自己的游戏？
1. 在你的项目目录下创建一个 `Plugins` 文件夹
2. 将 `UnrealFRP` 插件放入文件夹
3. 重新生成并编译项目
4. 详细使用方法见 [使用文档](DOC_CN.md)
## 技术支持
- QQ：3201548104
- EMail1：3201548104@qq.com
- EMail2：lhy1339@gmail.com