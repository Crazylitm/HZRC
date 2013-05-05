HZRC v0.1
====

简单背景说明：
===
1、写这个代码是为了4－6岁这个阶段的儿童，读书时很多汉字不认识，可以通过手写识别的方式，来认识汉字和汉字的书写过程。

2、一个临时写的IOS版本的试验类型的汉字手写识别软件，虽然引入了zinnia代码，但是实际汉字识别过程是重新写的。
代码可以在XCode4.3.2上编译通过。使用了Object c和C++混合编译模式。这个代码因为最初的交互是由一个画图程序MyPalette基础上改的，
所以项目名字就叫MyPalette了。刚开始研究zinnia代码时觉得挺兴奋，但是很快发现它的汉字识别模型太简陋，识别复杂的汉字准确率高
一些，但是识别简单的汉字反而很不准确。后来一个朋友给推荐了一篇《基于笔画的联机手写汉字识别系统研究与实现》的论文，根据该论文
里面撰写的原理，重新编写了识别的模块。RecognizerHZ.h 和 RecognizerHZ.cpp。由于缺少论文中笔画识别的字库，只在网上找了一个
五笔划的汉字字库，根据该五笔划的汉字字库和引擎进行了对接。汉字识别程序 v0.1版本完成。

不足和后续需要改进：
===
1、整个代码框架当时写得很临时，需要重新构建和规整过。
    包括：1.1 代码使用了类似MyPalette得手机交互操作的部分，以及把zinnia中
              没有使用的代码剔除掉，只留下汉字骨架识别的部分。
          1.2 包括对于char filename[]="//Users//macbookpro//D///d//soft///hand//mytestHZRC//HZRC//ziku//zikuGB.txt";写
          类我计算机上的绝对路径，应该采用手机的本地路径加载模式。
          1.3  etc.....
2、汉字识别的问题：
    1.1 对汉字的书写的笔顺要求高，笔顺错误后无法有效识别。
    1.2 对于汉字的连笔和草书未支持识别。
    1.3 整体说就是一个简陋的识别模块。
    
3、计划后续加入的功能： 
    1、除了解决汉字识别问题外，还要加入汉字到汉字语义解释的部分。
    2、汉字语义解释和汉字的发音关联。
    3、加入汉字的田字格熟悉提示功能等。
    

