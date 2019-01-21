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

问题解决
---
我以前误解了数据速率单位。事实上，网络流量单位通常使用`比特率`来描述，它使用每秒流过的比特数（“bit / s”或“bps”）来量化。
> 1Kbit/s = 1000bit/s
> 1Mbit/s = 1000Kbit/s
> 1Gbit/s = 1000Mbit/s

有时候，每秒字流过的字节数（“B / s”）也可用于量化`比特率`。
> 1B/s = 8bit/s

遗留问题
---
我不知道是不是因为刷新过于频繁，导致有时界面会出现闪烁，而且时间过长后会导致Explorer内存爆炸。

如果有人能帮助我解决这个问题将会十分感谢！ :grin:



参考
---
- [DeskBand实现之——对象介绍 - CSDN][1]
- [GetIfTable function - MSDN - Microsoft][2]
- [LockKeys - slivermeteor][3]


[1]: https://blog.csdn.net/leehong2005/article/details/8608783 "DeskBand实现之——对象介绍"
[2]: https://msdn.microsoft.com/en-us/aa365943?f=255&MSPPError=-2147217396 "GetIfTable function"
[3]: https://github.com/slivermeteor/LockKeys.git "LockKeys"