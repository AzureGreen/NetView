NetView
===

语言
---
- [中文](./README_zh.md)
- [English](./README.md)

前言
---
临近毕业，迫于毕设课题要求忙于研究机器学习知识，好久没有写`c++`代码了。最近手痒想写点不一样的代码了，碰巧一直想做个小东西玩玩，于是在毕设间隙抽空尝试做了这个玩意。

介绍
---
这是一个查看实时网速的桌面任务栏插件，使用的是自定义`DeskBand`的COM对象技术，结合微软提供的API--`GetIfTable`获取网卡接口信息与数据，计算完成网速监测。效果图如下所示：

![](./Image/1.png)


使用方法
---
在release栏中下载发行版本，双击运行其中的register.bat，完成插件注册。右键任务栏出现菜单，点击工具栏出现二级菜单，选择刚注册的`NetView`即可。

![](./Image/2.png)

测试平台
---
Windows10 x64

如果其他平台有问题，麻烦在`issue`中告知，谢谢~

参考
---
- [DeskBand实现之——对象介绍 - CSDN][1]
- [GetIfTable function - MSDN - Microsoft][2]
- [LockKeys - slivermeteor][3]


[1]: https://blog.csdn.net/leehong2005/article/details/8608783 "DeskBand实现之——对象介绍"
[2]: https://msdn.microsoft.com/en-us/aa365943?f=255&MSPPError=-2147217396 "GetIfTable function"
[3]: https://github.com/slivermeteor/LockKeys.git "LockKeys"