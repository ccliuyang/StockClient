# StockClient

#### 介绍
自动新股申购程序，服务端采用TdxTradeServer

#### 软件架构
软件架构说明
1. 采用VS2015进行编译,静态编译
2. 首先下载vcpkg，采用cvpkg进行软件包依赖的配置
3. 使用vcpkg下载 cpr:x86-windows-static nlohmann-json:x86-windows-static

#### 安装教程

1. 配置TdxTradeServer为多账户模式，并且设置为不加密传输方式
2. 启动TdxTradeServer
3. 修改代码中登陆ip地址，账号等信息，然后重新编译
3. 启动本软件


#### 使用说明
自动申购新股

#### 参与贡献



