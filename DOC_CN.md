# 使用文档
## 蓝图节点介绍
- **OpenSession**  
参数：无  
简介：开启会话  
注意事项：在打开关卡时需要加上 `listen`，否则不会生效
---
- **CloseSession**  
参数：无  
简介：关闭会话  
注意事项：无
---
- **SetSessionPassword**  
参数：`string` Password  
简介：设置会话的密码  
注意事项：如果留空则代表无密码，任何人都可以连接
---
- **SetSessionProperty**  
参数：`string` Property  
简介：设置会话的属性  
注意事项：多用于房间名，不建议在 **OpenSession** 后更改
---
- **SetSessionPropertyAppend**  
参数：`string` PropertyAppend  
简介：设置会话的附加属性  
注意事项：多用于关卡、人数等属性，支持在 **OpenSession** 后动态修改
---
- **BindOnAddrUpdate**  
参数：`委托` Delegate  
简介：绑定事件到“当请求到Addr时”  
注意事项：无
---
- **UnbindOnAddrUpdate**  
参数：无  
简介：解绑事件“当请求到Addr时”  
注意事项：注意不要空绑定
---
- **BindOnSessionListUpdate**  
参数：`委托` Delegate  
简介：绑定事件到“当获取到会话列表时”  
注意事项：无
---
- **UnbindOnSessionListUpdate**  
参数：无  
简介：解绑事件“当获取到会话列表时”  
注意事项：注意不要空绑定
---
- **FlushFRPSessionList**  
参数：无  
简介：刷新会话列表  
注意事项：配合 **BindOnSessionListUpdate** 使用
---
- **AskForAddress**  
参数：`string` ID | `string` Password  
简介：根据会话ID和密码请求Addr  
注意事项：密码可以留空
---
- **SetAppAddress**  
参数：`int` Port  
简介：设置App对应端口  
注意事项：与服务器保持一致
---
- **SetMsgAddress**  
参数：`int` Port  
简介：设置Msg对应端口  
注意事项：与服务器保持一致
---
- **SetBaseIP**  
参数：`string` IP  
简介：设置服务器IP  
注意事项：与服务器保持一致
---
- **ConnectSession**  
参数：`string` Addr  
简介：在请求Addr成功后连接到会话  
注意事项：配合 **BindOnAddrUpdate** 使用
---