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
namespace zinnia {
    struct StrokeSliceItem{
        int Weight; //笔段权重
        BYTE Direct; //笔段的方向码 
        StrokeSliceltem* Nextltem; //指向下一个笔段节点 
        StrokeSliceltem* Anotherltem; //指向当前笔段的其他方向码
    );
 
    class CStrokeSlice{
    private:
    
    public:
        StrokeSliceItem* CreateStrokeSliceItem(int w,BYTE d);//创建1个链节点。该算法输入方向码的权重帛l方向码,返同一个方向码结点
        void DelSSItem(StrokeSlice*tem *p);//删除1个链节点。/该算法输入一个方向码结点
        void AddSSItemInSSList(int x0,int y0,int x1,int y1,
                              BYTE* Direct,float * Reliability,
                              StrokeSliceItem * pTailSSList);//在链表晕添加节点。//该算法输入一个笔画段的起点,终点坐标,可能方向码集合,可信度,链笔末尾指针
        void DelSSList(StrokeSliceItem * pHeadSSList);//删除除链表。//该算法输入链笔头指针
        void TravelSSList(StrokeSliceItem *pHeadSSList,int PoperWeight);//遍历链表得到当前笔画轨迹对应的方向码序列。/该算法输入链笔头指针.合适权重
        
        
    };
    class CStroke{
        
    };
    class 
}

#endif
