//
//  RecognizerHZ.h
//  MyPalette
//
//  Created by  on 12-10-24.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef MyPalette_RecognizerHZ_h
#define MyPalette_RecognizerHZ_h
#include "zinnia.h"
#include <map>
#include "common.h"

#define BYTE unsigned long int
namespace zinnia {
//定义五笔笔画确定一个汉字的部首
#define F_H 100   //横
#define F_S 200   //竖
#define F_P 300   //撇
#define F_N 400   //捺
#define F_Z 500   //折
//定义slice字段的走向
#define R  1   //向右
#define RD 2   //向右下
#define D  3   //向下
#define LD 4   //向左下

#define L  5   //向左
#define LU 6   //向左上
#define U  7   //向上
#define RU 8   //向右上
    //横
#define T_H  9
    //竖
#define T_S  10
    //撇
#define T_P  11
#define T_Sp  12
    //捺
#define T_N  13
#define T_Hn  14
#define T_Nh  15
    //提
#define T_T  16
    //横折
#define T_Hs 17
    //横撇,横勾
#define T_Hp 18
    //横折钩
#define T_Hsg 19
    //竖勾
#define T_Sg 20
    //竖弯勾
#define T_Sht 21
    //撇折
#define T_Ph 22
    //竖提
#define T_St 23
    //竖折,竖弯
#define T_Sh 24
    //竖折折钩
#define T_Shsg 25
    //斜钩
#define T_Xt 26
#define T_Snt 27
    //撇点
#define  T_Pn 28
#define T_Sn 29
    //横折提
#define T_Hst 30
    //弯钩
#define T_Wg 31
    //横折弯钩，横折斜钩
#define T_Hnt 32
#define T_Hsht 33
#define T_Hpht 34
    //横折折折
#define T_Hshs 35
#define T_Hphs 36
    //横折折折钩,横折折折弯钩
#define T_Hphsg 37
#define T_Hshsg 38
#define T_Hpsg 39
#define T_Hpng 40
#define T_Hshpg 41
#define T_Hphpg 42
    //横折折撇
#define T_Hshp 43
#define T_Hphp 44
    //竖折撇，竖折折
#define T_Shs 45
#define T_Shp 46
    //横折折,横折弯
#define T_Hsh 47
    //左点
#define T_ZD 48
    //右点
#define T_YD 49
//状态图多出来的部首
#define T_Ns 50
#define T_W  51
#define T_Hph 52
#define T_Hps 53
#define T_Hpn 54
#define T_Snh 55
#define T_Shw 56
//定义stroke部首
/*
//横
#define T_H  R
//竖
#define T_S  D
//撇
#define T_P  LD
#define T_Sp  LD
//捺
#define T_N  RD
#define T_Hn  RD
#define T_Nh  RD
//提
#define T_T  RU
//横折
#define T_Hs R,D
//横撇,横勾
#define T_Hp R,LD
//横折钩
#define T_Hsg R,D,LU
//竖勾
#define T_Sg D,LU
//竖弯勾
#define T_Sht D,R,U
//撇折
#define T_Ph LD,R
//竖提
#define T_St D,RU
//竖折,竖弯
#define T_Sh D,R
//竖折折钩
#define T_Shsg D,R,D,LU
//斜钩
#define T_Xt D,RD,LU
#define T_Snt D,RD,LU
//撇点
#define  T_Pn LD,RD
#define T_Sn LD,RD
//横折提
#define T_Hst R,D,RU
//弯钩
#define T_Wg D,LD,LU
//横折弯钩，横折斜钩
#define T_Hnt R,D,R,LU
#define T_Hsht R,D,R,LU
#define T_Hpht R,D,R,LU
//横折折折
#define T_Hshs R,D,R,D
#define T_Hphs R,D,R,D
//横折折折钩,横折折折弯钩
#define T_Hphsg R,D,R,D,LU
#define T_Hshsg R,D,R,D,LU
#define T_Hpsg R,D,R,D,LU
#define T_Hpng R,D,R,D,LU
#define T_Hshpg R,D,R,D,LU
#define T_Hphpg R,D,R,D,LU
//横折折撇
#define T_Hshp R,D,R,LD
#define T_Hphp R,D,R,LD
//竖折撇，竖折折
#define T_Shs D,R,LD
#define T_Shp D,R,LD
//横折折,横折弯
#define T_Hsh R,D,R
//左点
#define T_ZD LD
//右点
#define T_YD RD
*/
    struct StrokeNode {
        float x;
        float y;
        int iTimeNum;
        StrokeNode():iTimeNum(0){}
    };
    struct StrokeSliceItem{
        int Weight; //笔段权重
        BYTE Direct; //笔段的方向码 
        int iTimeNum;//存储当前笔端属于第几笔
        StrokeSliceItem* Nextltem; //指向下一个笔段节点 
        StrokeSliceItem* Anotherltem; //指向当前笔段的其他方向码
        //std::vector<StrokeSliceItem> vecAnotherltem;
    };
    struct autostat{
        BYTE one;
        BYTE two;
        BYTE three;
        BYTE outFiveStroke;//对应的五笔笔画的编码
        char outStrokeName[50];//对应的汉字部首的名字
    };
    /*
    struct ResultStroke{
        BYTE outthree;//对应的笔画编码
        BYTE outFiveStroke;//对应的五笔笔画的编码
        char outStrokeName[50];//对应的汉字名字
    };*/
    //存储汉字字库的类
    class CFiveHZZK
    {
    private:
        std::multimap<std::string,std::string> HZZK;//使用multimap不用map原因是汉字库中存在一个编码对应多个不同汉字的情况
        //汉字五笔部首编码
        //汉字名字
        char            *text;
        size_t       length;
        std::string  fileName;
        int    fd;
        int    flag;
        whatlog what_;
    private:
        char&       operator[](size_t n)       { return *(text + n); }
        const char& operator[](size_t n) const { return *(text + n); }
        char*       begin()           { return text; }
        char*       end()           { return text + size(); }
        size_t size()               { return length/sizeof(char); }
        const char *file_name()     { return fileName.c_str(); }
        size_t file_size()          { return length; }
        bool empty()                { return(length == 0); }
        const char *what()          { return what_.str(); }
        bool open(const char *filename, const char *mode = "r") ;
        void close();
    public:
        CFiveHZZK(){InitLoadHZZKtoMemory();};
        ~CFiveHZZK(){HZZK.clear();this->close();};
        int InitLoadHZZKtoMemory();//将汉字编码库上载到程序内存中
        int SeachHZZK(std::string  &inStroke,std::vector<std::string> &OutHZ);//搜索单个部首数组所匹配的汉字。找不到返回 0,找到返回找到的汉字数量，并且在OutHZ的数组中传递出来。1、第一个是编码，第2个是汉字名字
        
    };
    class CStrokeSlice{
    private:
        StrokeSliceItem *pSliceHead;//笔端的链表头节点
        StrokeSliceItem *pSliceTail;
        //std::vector<StrokeSliceItem> vecNextltem;
        int iSliceCnt;
        //std::vector<BYTE> *pslice;//存储字段识别结果 
        std::vector<autostat> automachine;//状态机
        CFiveHZZK hzzk;//汉字字库
    private:
        StrokeSliceItem* CreateStrokeSliceItem(int w,BYTE d,int iTimeNum);//创建1个链节点。该算法输入方向码的权重帛l方向码,返同一个方向码结点
        void AddSliceItem(StrokeSliceItem* p,int ifFlogAnotherItem=0);//ifFlogAnotherItem ＝0 下一个节点,=1AnotherItem节点
       // void DelSSItem(StrokeSliceItem *p);//删除1个链节点。/该算法输入一个方向码结点
       // void AddSSItemInSSList(int x0,int y0,int x1,int y1,BYTE* Direct,float * Reliability,StrokeSliceItem * pTailSSList);//在链表晕添加节点。//该算法输入一个笔画段的起点,终点坐标,可能方向码集合,可信度,链笔末尾指针
       // void DelSSList(StrokeSliceItem * pHeadSSList);//删除除链表。//该算法输入链笔头指针
        int TravelSSList(std::vector<std::vector<StrokeSliceItem> > &Slice);//遍历树返回slice结果数组（存放所有组合的字段）
        BYTE AutoMachine(BYTE inOneState,BYTE inTwoState,autostat &Stroke);//输入状态，状态机给一个输出状态。
        int AutoMachineofStroke(std::vector<std::vector<autostat> > &Stroke);//通过自动状态机将字段pSlice自动识别为可能的部首stroke结果
    public:
        CStrokeSlice(){pSliceHead = NULL;pSliceTail =NULL;iSliceCnt=0;InitAutoMachine();};
       // void TravelSSList(StrokeSliceItem *pHeadSSList,int PoperWeight);//遍历链表得到当前笔画轨迹对应的方向码序列。/该算法输入链笔头指针.合适权重
        ~CStrokeSlice(){};
        //1、先调用该函数，输入显著点数据的结果集合
         int SetRecognizeSliceRLDU(Result *result);//将节点转换成笔端方向码  return 值此次识别的总字段数量，既链表中的节点数量。
        //int TravelSlice(std::vector<BYTE> &pslice);//pslice返回遍历后的结果数组
        //2、再调用该函数，得到识别的汉字结果集合
        int GetRecognizeHZResult(std::vector<std::string> &result);//得到汉字的结果集合
        
        int InitAutoMachine();//状态机初始化
         
    };

    
}

#endif
