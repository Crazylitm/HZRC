//
//  RecognizerHZ.cpp
//  MyPalette
//
//  Created by  on 12-10-24.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "RecognizerHZ.h"
#include "zinnia.h"
#include <stack>
#include <map>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"
#include "mmap.h"
/*
#include <cwchar>
#include <clocale>
#include <stdlib.h>
#include <cstdio>
 */
#ifndef O_BINARY
#define O_BINARY 0
#endif
//#include "feature.h"
//根据显著点结果形成字段的识别结果存储在类CStrokeSlice中
int  zinnia::CStrokeSlice::SetRecognizeSliceRLDU(Result *result)
{
    int isize =  result->size_pairs();
    for (int i = 0; i < isize; i++) {
        //遍历显著点结果集合，形成LRDU的字段识别结果
        StrokeNode beginnote;
        StrokeNode endnote;
        double Wi1=0,Wi2=0;
        BYTE direct =0;//判断当前节点的字段走向
        BYTE direct2=0;//如果存在两种可能性的第2个可能的字段走向
        if(isize - i>1)
        {
            beginnote.x = result->getx(i);//85
            beginnote.y = result->gety(i);//109
            beginnote.iTimeNum = result->getStorkeNum(i);
            
            endnote.x = result->getx(i+1);//236
            endnote.y = result->gety(i+1);//92
            endnote.iTimeNum = result->getStorkeNum(i+1);
            if(endnote.iTimeNum != beginnote.iTimeNum)continue;//如果不是同一笔的则不计算。
            direct =0;
            //纯粹根据坐标大小判断direct的方向，这部分判断完毕后再通过角度进行修正
            
            if(endnote.x > beginnote.x)
            { //R
                direct = R;
            }
            
            else if(endnote.x < beginnote.x)
            { //L
                direct = L;
            }
            
            if(endnote.y > beginnote.y)
            { //L
                if(direct == L)
                      direct = LD;
                else if(direct == R)
                      direct = RD;
                else
                    direct = D;
            }
            else if(endnote.y < beginnote.y)
            { //R
                if(direct == L)
                    direct = LU;
                else if(direct == R)
                    direct = RU;
                else
                    direct = U;
            }
            //根据夹角的度数判断是否需要调整direct的值
            double dtag = std::atan2(std::fabs(endnote.y - beginnote.y), std::fabs(endnote.x - beginnote.x));
            double degree = std::fabs( dtag * 180/M_PI);//夹角转换成度数
            if(degree <=5 )
            {//0-5度之间认为属于正常走向
                switch (direct) {
                    case LU:
                        direct = L;
                        break;
                    case RU:
                        direct = R;
                        break;
                    case LD:
                        direct = L;
                        break;
                    case RD:
                        direct = R;
                        break;
                    default:
                        break;
                }
            }
            else if(degree>85 && degree <=90)
            {//在这个度数范围内 85-90 认为是正上或者正下
                switch (direct) {
                    case LU:
                        direct = U;
                        break;
                    case RU:
                        direct = U;
                        break;
                    case LD:
                        direct = D;
                        break;
                    case RD:
                        direct = D;
                        break;
                    default:
                        break;
                }
            }
            else if(degree>=70 && degree <=85)
            {//在这个度数范围内70-85：70－85 不明确是上或者斜
                switch (direct) {//direct存储第一种可能，direct2存储第2种可能
                    case LU:
                        direct = U;
                        direct2 = LU;
         
                        break;
                    case RU:
                        direct = U;
                        direct2 = RU;
                        break;
                    case LD:
                        direct = D;
                        direct2 = LD;
                        break;
                    case RD:
                        direct = D;
                        direct2 = RD;
                        break;
                    default:
                        break;
                        
                }
                //求两种方向情况下的权重
                int Li = std::sqrt(endnote.x  * beginnote.x + endnote.y * beginnote.y);
                int Ti1 = 1 - 1/dtag;
                int Ti2 = 1/dtag;
                Wi1 = Li*Ti1;
                Wi2 = Li*Ti2;
            }
            else if(degree >=5 && degree<=20)
            {//在这个度数方位5-20：不明确是横或者斜
                switch (direct) {//direct存储第一种可能，direct2存储第2种可能
                    case LU:
                        direct = L;
                        direct2 = LU;
                        break;
                    case RU:
                        direct = R;
                        direct2 = RU;
                        break;
                    case LD:
                        direct = L;
                        direct2 = LD;
                        break;
                    case RD:
                        direct = R;
                        direct2 = RD;
                        break;
                    default:
                        break;
                        
                }
                //求两种方向情况下的权重
                int Li = std::sqrt(endnote.x  * beginnote.x + endnote.y * beginnote.y);
                int Ti1 = 1 - dtag;
                int Ti2 = dtag;
                Wi1 = Li*Ti1;
                Wi2 = Li*Ti2;
            }
            else { //if(degree>20 && degree <70)在这个度数范围内：明确就是 斜向：左上或者左下，右上或者右下
                //NULL;
            }
        
            //当目前为止：上面把direct求出，并且如果存在direct2的情况下也分别求出了各自的权重。
            //下面的代码就是把这些结果存储在链表结构中，保存起来给外部使用。
            
             StrokeSliceItem *p  = CreateStrokeSliceItem(Wi1,direct,beginnote.iTimeNum);
             AddSliceItem(p);
            iSliceCnt++;
            printf("--My[%d],Wi1[%.2f],Direct[%d]Num[%d][(bx=%.0f,by=%.0f)(ex=%.0f,ey=%.0f)]\n",iSliceCnt,Wi1,direct,beginnote.iTimeNum,beginnote.x,beginnote.y,endnote.x,endnote.y);
            if(direct2 !=0)
            {
                StrokeSliceItem *p  = CreateStrokeSliceItem(Wi2,direct2,beginnote.iTimeNum);
                AddSliceItem(p,1);
                iSliceCnt++;
                printf("--Bro[%d],Wi1[%.2f],Direct[%d]Num[%d][(bx=%.0f,by=%.0f)(ex=%.0f,ey=%.0f)]\n",iSliceCnt,Wi1,direct,beginnote.iTimeNum,beginnote.x,beginnote.y,endnote.x,endnote.y);}
        
            
        }//if
        

    }//for
    return iSliceCnt;
};
void zinnia::CStrokeSlice::AddSliceItem(StrokeSliceItem* p,int ifFlogAnotherItem)
{
    if(p==NULL)return;
    if(ifFlogAnotherItem == 0)
    {//挂pNext
        if (pSliceTail==NULL) {
            pSliceTail = p;
            pSliceHead = p;
        }
        else {
            pSliceTail->Nextltem = p;
            if(pSliceTail->Anotherltem !=NULL){
                StrokeSliceItem *q =  pSliceTail->Anotherltem;
                
                while(q != NULL)
                {
                    q->Nextltem =p;
                    q = q->Anotherltem;
                }
            }
            pSliceTail = p;
        }//if-else
        
    }else
    {//挂pAnother
        if (pSliceTail==NULL) {
            pSliceTail = p;
            pSliceHead = p;
        }
        else {
             StrokeSliceItem *q =  pSliceTail->Anotherltem;
            if(q==NULL)pSliceTail->Anotherltem = p;
            else {
            
                    while(q!=NULL)
                    {
                        if(q->Anotherltem== NULL)
                        {
                            q->Anotherltem = p;
                            break;
                        }
                        else
                            q = q->Anotherltem;
                        
                    }
            }//if else
            
        }
    }//else

}
zinnia::StrokeSliceItem* zinnia::CStrokeSlice::CreateStrokeSliceItem(int w,BYTE d,int iTimeNum)
{
    StrokeSliceItem *pItem = new StrokeSliceItem; 
    pItem->Nextltem = NULL;
    pItem->Anotherltem = NULL;
    pItem->Weight = w;
    pItem->Direct = d;
    pItem->iTimeNum = iTimeNum;
    return pItem;
}
//遍历链表形成可能的字段组合
int zinnia::CStrokeSlice::TravelSSList(std::vector<std::vector<StrokeSliceItem> > &Slice)
{
    StrokeSliceItem *p = pSliceHead,*q=NULL;
    if(p == NULL ) return 0;
    if(iSliceCnt<=0)return 0;

    int i=0,j=0;
    int iSliceNum=1;
   
    std::stack<StrokeSliceItem*> stackslices;
    
    //求该数据结构的共需要多少种笔段的组合
    while(p)
    {
        j++;
        q = p->Anotherltem;
        while(q)
        {
            j++;
            q = q->Anotherltem;
        }
        iSliceNum = iSliceNum * j;
        j=0;
        p = p->Nextltem;    
    }
    
    //std::vector<std::vector<BYTE> > slice(iSliceNum);
    Slice.resize(iSliceNum);
    p = pSliceHead;
    i=0;j=0;
    //int oldnum=-1;
   // printf("stackslices.push=");
    while(p)
    {
        stackslices.push(p);
        //printf("%d,",p->Direct);
        int oldnum=-1;
        if(p->Nextltem == NULL)
        {
           // printf("\n");
            //找到第一个序列结果集合
            std::stack<StrokeSliceItem*>  tmp1,tmp2;
            tmp1 = stackslices;
            int icnt=0;
            while(tmp1.empty() == FALSE)
            {
                tmp2.push(tmp1.top());
                tmp1.pop();
                icnt++;
            }
            Slice[i].resize(icnt);
            j=0;
            printf("Slice=(");
            
            while (tmp2.empty() == FALSE) {
                q = tmp2.top();
                
                Slice[i][j].Direct  = q->Direct;//?
                Slice[i][j++].iTimeNum  = q->iTimeNum;
                
                if(oldnum == -1)
                {
                    oldnum = q->iTimeNum;
                    printf("%d[%d],",q->Direct,q->iTimeNum);
                }
                else if(oldnum == q->iTimeNum){
                    printf("%d[%d],",q->Direct,q->iTimeNum);
                }else {
                        printf(")(%d[%d],",q->Direct,q->iTimeNum);
                        oldnum = q->iTimeNum;
                }
                tmp2.pop();
                
            }
            printf(")\n");
            //------------------------
            while(stackslices.empty()==FALSE)
            {
                
                    p = stackslices.top();
                    p = p->Anotherltem;
                
                    stackslices.pop();  
                if(p!=NULL)
                {
                    i++;
                    break;
                }//if
            }//while
            
        }
        else {
            p = p->Nextltem;
        }//if
            
    }//while
    return 1;
}
//初始化状态机
int zinnia::CStrokeSlice::InitAutoMachine()
{
    //T_Hphpg R,D,R,D,LU
    automachine.resize(150);
    //一级状态初始化//10个
    automachine[0].one=0;automachine[0].two=R;automachine[0].three=T_H;                  automachine[0].outFiveStroke=F_H;strcpy(automachine[0].outStrokeName,"横");         
    automachine[1].one=T_H;automachine[1].two=R;automachine[1].three=T_H;                 automachine[1].outFiveStroke=F_H;strcpy(automachine[1].outStrokeName,"横");         
    automachine[2].one=0;automachine[2].two=RU;automachine[2].three=T_T;                  automachine[2].outFiveStroke=F_H;strcpy(automachine[2].outStrokeName,"提");         
    automachine[3].one=T_T;automachine[3].two=RU;automachine[3].three=T_T;                automachine[3].outFiveStroke=F_H;strcpy(automachine[3].outStrokeName,"提");         
    automachine[4].one=0;automachine[4].two=RD;automachine[4].three=T_N;                  automachine[4].outFiveStroke=F_N;strcpy(automachine[4].outStrokeName,"捺");         
    automachine[5].one=T_N;automachine[5].two=RD;automachine[5].three=T_N;                automachine[5].outFiveStroke=F_N;strcpy(automachine[5].outStrokeName,"捺");         
    automachine[6].one=0;automachine[6].two=LD;automachine[6].three=T_P;                  automachine[6].outFiveStroke=F_P;strcpy(automachine[6].outStrokeName,"撇");         
    automachine[7].one=T_P;automachine[7].two=LD;automachine[7].three=T_P;                automachine[7].outFiveStroke=F_P;strcpy(automachine[7].outStrokeName,"撇");         
    automachine[8].one=0;automachine[8].two=D;automachine[8].three=T_S;                   automachine[8].outFiveStroke=F_S;strcpy(automachine[8].outStrokeName,"竖");         
    automachine[9].one=T_S;automachine[9].two=D;automachine[9].three=T_S;                 automachine[9].outFiveStroke=F_S;strcpy(automachine[9].outStrokeName,"竖");         
    //二级状态初始化 T_H状态                                                                                                                                                  
    automachine[10].one=T_H;automachine[10].two=RD;automachine[10].three=T_Hn;            automachine[10].outFiveStroke=F_N;strcpy(automachine[10].outStrokeName,"捺");       
    automachine[11].one=T_Hn;automachine[11].two=RD;automachine[11].three=T_Hn;           automachine[11].outFiveStroke=F_N;strcpy(automachine[11].outStrokeName,"捺");       
    automachine[12].one=T_H;automachine[12].two=LD;automachine[12].three=T_Hp;            automachine[12].outFiveStroke=F_Z;strcpy(automachine[12].outStrokeName,"横钩、横撇");       
    automachine[13].one=T_Hp;automachine[13].two=LD;automachine[13].three=T_Hp;           automachine[13].outFiveStroke=F_Z;strcpy(automachine[13].outStrokeName,"横钩、横撇");       
    automachine[14].one=T_H;automachine[14].two=D;automachine[14].three=T_Hs;             automachine[14].outFiveStroke=F_Z;strcpy(automachine[14].outStrokeName,"横折");       
    automachine[15].one=T_Hs;automachine[15].two=D;automachine[15].three=T_Hs;            automachine[15].outFiveStroke=F_Z;strcpy(automachine[15].outStrokeName,"横折");       
    //二级状态初始化 T_T状态                                                                                                                                                  
    automachine[16].one=T_T;automachine[16].two=D;automachine[16].three=T_Hs;             automachine[16].outFiveStroke=F_Z;strcpy(automachine[16].outStrokeName,"横折");       
    //二级状态初始化 T_N状态                                                                                                                                                  
    automachine[17].one=T_N;automachine[17].two=R;automachine[17].three=T_Nh;             automachine[17].outFiveStroke=F_N;strcpy(automachine[17].outStrokeName,"捺");       
    automachine[18].one=T_Nh;automachine[18].two=R;automachine[18].three=T_Nh;            automachine[18].outFiveStroke=F_N;strcpy(automachine[18].outStrokeName,"捺");       
    automachine[19].one=T_N;automachine[19].two=D;automachine[19].three=T_Ns;             automachine[19].outFiveStroke=F_Z;strcpy(automachine[19].outStrokeName,"捺竖（自己起名）");       
    automachine[20].one=T_Ns;automachine[20].two=D;automachine[20].three=T_Ns;           automachine[20].outFiveStroke=F_Z;strcpy(automachine[20].outStrokeName,"捺竖（自己起名）");       
    automachine[21].one=T_N;automachine[21].two=LD;automachine[21].three=T_W;            automachine[21].outFiveStroke=F_Z;strcpy(automachine[21].outStrokeName,"捺弯（自己起名）");       
    automachine[22].one=T_N;automachine[22].two=U;automachine[22].three=T_Xt;            automachine[22].outFiveStroke=F_Z;strcpy(automachine[22].outStrokeName,"斜钩");       
    automachine[23].one=T_N;automachine[23].two=RU;automachine[23].three=T_Xt;           automachine[23].outFiveStroke=F_Z;strcpy(automachine[23].outStrokeName,"斜钩");       
    //二级状态初始化 T_P状态                                                                                                                                                  
    automachine[24].one=T_P;automachine[24].two=R;automachine[24].three=T_Ph;             automachine[24].outFiveStroke=F_Z;strcpy(automachine[24].outStrokeName,"撇折");       
    automachine[25].one=T_Ph;automachine[25].two=R;automachine[25].three=T_Ph;            automachine[25].outFiveStroke=F_Z;strcpy(automachine[25].outStrokeName,"撇折");       
    automachine[26].one=T_P;automachine[26].two=RD;automachine[26].three=T_Pn;            automachine[26].outFiveStroke=F_Z;strcpy(automachine[26].outStrokeName,"撇点");       
    automachine[27].one=T_Pn;automachine[27].two=RD;automachine[27].three=T_Pn;           automachine[27].outFiveStroke=F_Z;strcpy(automachine[27].outStrokeName,"撇点");       
    //二级状态初始化 T_S状态                                                                                                                                                  
    automachine[28].one=T_S;automachine[28].two=R;automachine[28].three=T_Sh;             automachine[28].outFiveStroke=F_Z;strcpy(automachine[28].outStrokeName,"竖折、竖弯");       
    automachine[29].one=T_Sh;automachine[29].two=R;automachine[29].three=T_Sh;            automachine[29].outFiveStroke=F_Z;strcpy(automachine[29].outStrokeName,"竖折、竖弯");       
    automachine[30].one=T_S;automachine[30].two=RD;automachine[30].three=T_Sn;            automachine[30].outFiveStroke=F_Z;strcpy(automachine[30].outStrokeName,"撇点");       
    automachine[31].one=T_Sn;automachine[31].two=RD;automachine[31].three=T_Sn;           automachine[31].outFiveStroke=F_Z;strcpy(automachine[31].outStrokeName,"撇点）");       
    automachine[32].one=T_S;automachine[32].two=LD;automachine[32].three=T_Sp;            automachine[32].outFiveStroke=F_P;strcpy(automachine[32].outStrokeName,"撇");       
    automachine[33].one=T_Sp;automachine[33].two=LD;automachine[33].three=T_Sp;           automachine[33].outFiveStroke=F_P;strcpy(automachine[33].outStrokeName,"撇");       
    automachine[34].one=T_S;automachine[34].two=RU;automachine[34].three=T_St;            automachine[34].outFiveStroke=F_Z;strcpy(automachine[34].outStrokeName,"竖提");       
    automachine[35].one=T_St;automachine[35].two=RU;automachine[35].three=T_St;           automachine[35].outFiveStroke=F_Z;strcpy(automachine[35].outStrokeName,"竖提"); //可能与图不符合      
    automachine[36].one=T_S;automachine[36].two=LU;automachine[36].three=T_Sg;            automachine[36].outFiveStroke=F_S;strcpy(automachine[36].outStrokeName,"竖勾");       
    //三级状态初始化 T_Hn                                                                                                                                                     
    automachine[37].one=T_Hn;automachine[37].two=U;automachine[37].three=T_Hnt;           automachine[37].outFiveStroke=F_Z;strcpy(automachine[37].outStrokeName,"横折弯钩，横斜钩");       
    automachine[38].one=T_Hn;automachine[38].two=RU;automachine[38].three=T_Hnt;          automachine[38].outFiveStroke=F_Z;strcpy(automachine[38].outStrokeName,"横折弯钩，横斜钩");       
    automachine[39].one=T_Hn;automachine[39].two=LU;automachine[39].three=T_Wg;           automachine[39].outFiveStroke=F_Z;strcpy(automachine[39].outStrokeName,"弯钩");       
    automachine[40].one=T_Hn;automachine[40].two=D;automachine[40].three=T_W;             automachine[40].outFiveStroke=F_Z;strcpy(automachine[40].outStrokeName,"弯（自己起名字）");       
    automachine[41].one=T_Hn;automachine[41].two=LD;automachine[41].three=T_Wg;           automachine[41].outFiveStroke=F_Z;strcpy(automachine[41].outStrokeName,"弯钩");       
    //三级状态初始化 T_Hp                                                                                                                                                     
    automachine[42].one=T_Hp;automachine[42].two=R;automachine[42].three=T_Hph;           automachine[42].outFiveStroke=F_Z;strcpy(automachine[42].outStrokeName,"横钩横（自己起名字）");       
    automachine[43].one=T_Hph;automachine[43].two=R;automachine[43].three=T_Hph;          automachine[43].outFiveStroke=F_Z;strcpy(automachine[43].outStrokeName,"横钩横（自己起名字）");       
    automachine[44].one=T_Hp;automachine[44].two=D;automachine[44].three=T_Hps;           automachine[44].outFiveStroke=F_Z;strcpy(automachine[44].outStrokeName,"横钩竖（自己起名字）");       
    automachine[45].one=T_Hps;automachine[45].two=D;automachine[45].three=T_Hps;          automachine[45].outFiveStroke=F_Z;strcpy(automachine[45].outStrokeName,"横钩竖（自己起名字）");       
    automachine[46].one=T_Hps;automachine[46].two=LD;automachine[46].three=T_Hps;         automachine[46].outFiveStroke=F_Z;strcpy(automachine[46].outStrokeName,"横钩竖（自己起名字）");       
    automachine[47].one=T_Hp;automachine[47].two=RD;automachine[47].three=T_Hpn;          automachine[47].outFiveStroke=F_Z;strcpy(automachine[47].outStrokeName,"横钩捺（自己起名字）");       
    automachine[48].one=T_Hpn;automachine[48].two=D;automachine[48].three=T_Hpn;          automachine[48].outFiveStroke=F_Z;strcpy(automachine[48].outStrokeName,"横钩捺（自己起名字）");       
    automachine[49].one=T_Hp;automachine[49].two=LD;automachine[49].three=T_Hpn;          automachine[49].outFiveStroke=F_Z;strcpy(automachine[49].outStrokeName,"横钩捺（自己起名字）");       
    automachine[50].one=T_Hpn;automachine[50].two=RD;automachine[50].three=T_Hpn;         automachine[50].outFiveStroke=F_Z;strcpy(automachine[50].outStrokeName,"横钩捺（自己起名字）");       
    automachine[51].one=T_Hp;automachine[51].two=LU;automachine[51].three=T_Hsg;          automachine[51].outFiveStroke=F_Z;strcpy(automachine[51].outStrokeName,"横折钩");       
    //三级状态初始化 T_Hs                                                                                                                                                     
    automachine[52].one=T_Hs;automachine[52].two=LU;automachine[52].three=T_Hsg;          automachine[52].outFiveStroke=F_Z;strcpy(automachine[52].outStrokeName,"横折钩");       
    automachine[53].one=T_Hs;automachine[53].two=R;automachine[53].three=T_Hsh;           automachine[53].outFiveStroke=F_Z;strcpy(automachine[53].outStrokeName,"横折折、横折弯");       
    automachine[54].one=T_Hsh;automachine[54].two=R;automachine[54].three=T_Hsh;          automachine[54].outFiveStroke=F_Z;strcpy(automachine[54].outStrokeName,"横折折、横折弯");       
    automachine[55].one=T_Hs;automachine[55].two=RU;automachine[55].three=T_Hst;          automachine[55].outFiveStroke=F_Z;strcpy(automachine[55].outStrokeName,"横折提");       
    automachine[56].one=T_Hst;automachine[56].two=RU;automachine[56].three=T_Hst;         automachine[56].outFiveStroke=F_Z;strcpy(automachine[56].outStrokeName,"横折提");       
    //三级状态初始化 T_Nh                                                                                                                                                     
    automachine[57].one=T_Nh;automachine[57].two=RU;automachine[57].three=T_Xt;           automachine[57].outFiveStroke=F_Z;strcpy(automachine[57].outStrokeName,"斜钩");       
    automachine[58].one=T_Nh;automachine[58].two=U;automachine[58].three=T_Xt;            automachine[58].outFiveStroke=F_Z;strcpy(automachine[58].outStrokeName,"斜钩");       
    automachine[59].one=T_Nh;automachine[59].two=LU;automachine[59].three=T_Xt;           automachine[59].outFiveStroke=F_Z;strcpy(automachine[59].outStrokeName,"斜钩");       
    //三级状态初始化 T_Ns                                                                                                                                                     
    automachine[60].one=T_Ns;automachine[60].two=LU;automachine[60].three=T_Hnt;          automachine[60].outFiveStroke=F_Z;strcpy(automachine[60].outStrokeName,"横折弯钩、横斜钩");       
    automachine[61].one=T_Ns;automachine[61].two=LD;automachine[61].three=T_W;            automachine[61].outFiveStroke=F_Z;strcpy(automachine[61].outStrokeName,"横竖弯（自己定义）");       
    //三级状态初始化 T_W                                                                                                                                                      
    automachine[62].one=T_W;automachine[62].two=LU;automachine[62].three=T_Wg;            automachine[62].outFiveStroke=F_Z;strcpy(automachine[62].outStrokeName,"弯钩");       
    //三级状态初始化 T_Ph                                                                                                                                                     
    automachine[63].one=T_Ph;automachine[63].two=LD;automachine[63].three=T_Shp;          automachine[63].outFiveStroke=F_Z;strcpy(automachine[63].outStrokeName,"横折折撇");       
    automachine[64].one=T_Shp;automachine[64].two=LD;automachine[64].three=T_Shp;         automachine[64].outFiveStroke=F_Z;strcpy(automachine[64].outStrokeName,"横折折撇");       
    //三级状态初始化 T_Sh                                                                                                                                                     
    automachine[65].one=T_Sh;automachine[65].two=D;automachine[65].three=T_Shs;           automachine[65].outFiveStroke=F_Z;strcpy(automachine[65].outStrokeName,"竖折折、竖折撇");       
    automachine[66].one=T_Shs;automachine[66].two=D;automachine[66].three=T_Shs;          automachine[66].outFiveStroke=F_Z;strcpy(automachine[66].outStrokeName,"竖折折、竖折撇");       
    automachine[67].one=T_Sh;automachine[67].two=LD;automachine[67].three=T_Shp;          automachine[67].outFiveStroke=F_Z;strcpy(automachine[67].outStrokeName,"竖折折、竖折撇");       
    automachine[68].one=T_Shp;automachine[68].two=LD;automachine[68].three=T_Shp;         automachine[68].outFiveStroke=F_Z;strcpy(automachine[68].outStrokeName,"竖折折、竖折撇");       
    automachine[69].one=T_Sh;automachine[69].two=U;automachine[69].three=T_Sht;           automachine[69].outFiveStroke=F_Z;strcpy(automachine[69].outStrokeName,"竖弯钩");       
    automachine[70].one=T_Sh;automachine[70].two=RU;automachine[70].three=T_Sht;          automachine[70].outFiveStroke=F_Z;strcpy(automachine[70].outStrokeName,"竖弯钩");       
    automachine[71].one=T_Sh;automachine[71].two=LU;automachine[71].three=T_Sht;          automachine[71].outFiveStroke=F_Z;strcpy(automachine[71].outStrokeName,"竖弯钩");       
    //三级状态初始化 T_Sn                                                                                                                                                     
    automachine[72].one=T_Sn;automachine[72].two=R;automachine[72].three=T_Snh;           automachine[72].outFiveStroke=F_Z;strcpy(automachine[72].outStrokeName,"撇点横（自己定义）");       
    automachine[73].one=T_Snh;automachine[73].two=R;automachine[73].three=T_Snh;          automachine[73].outFiveStroke=F_Z;strcpy(automachine[73].outStrokeName,"撇点横（自己定义）");       
    automachine[74].one=T_Sn;automachine[74].two=U;automachine[74].three=T_Snt;           automachine[74].outFiveStroke=F_Z;strcpy(automachine[74].outStrokeName,"斜钩");       
    automachine[75].one=T_Sn;automachine[75].two=RU;automachine[75].three=T_Snt;          automachine[75].outFiveStroke=F_Z;strcpy(automachine[75].outStrokeName,"斜钩");       
    automachine[76].one=T_Sn;automachine[76].two=LU;automachine[76].three=T_Snt;          automachine[76].outFiveStroke=F_Z;strcpy(automachine[76].outStrokeName,"斜钩");       
    //三级状态初始化 T_Sp                                                                                                                                                     
    automachine[77].one=T_Sp;automachine[77].two=U;automachine[77].three=T_Wg;            automachine[77].outFiveStroke=F_Z;strcpy(automachine[77].outStrokeName,"弯钩");       
    automachine[78].one=T_Sp;automachine[78].two=LU;automachine[78].three=T_Wg;           automachine[78].outFiveStroke=F_Z;strcpy(automachine[78].outStrokeName,"弯钩");       
    //四级别状态初始化 THph                                                                                                                                                   
    automachine[79].one=T_Hph;automachine[79].two=D;automachine[79].three=T_Hphs;         automachine[79].outFiveStroke=F_Z;strcpy(automachine[79].outStrokeName,"横折折折");       
    automachine[80].one=T_Hphs;automachine[80].two=D;automachine[80].three=T_Hphs;        automachine[80].outFiveStroke=F_Z;strcpy(automachine[80].outStrokeName,"横折折折");       
    automachine[81].one=T_Hph;automachine[81].two=LD;automachine[81].three=T_Hphp;        automachine[81].outFiveStroke=F_Z;strcpy(automachine[81].outStrokeName,"横折折折");       
    automachine[82].one=T_Hphp;automachine[82].two=LD;automachine[82].three=T_Hphp;       automachine[82].outFiveStroke=F_Z;strcpy(automachine[82].outStrokeName,"横折折折");       
    automachine[83].one=T_Hph;automachine[83].two=U;automachine[83].three=T_Hpht;         automachine[83].outFiveStroke=F_Z;strcpy(automachine[83].outStrokeName,"横折弯钩、横斜钩");       
    automachine[84].one=T_Hph;automachine[84].two=RU;automachine[84].three=T_Hpht;        automachine[84].outFiveStroke=F_Z;strcpy(automachine[84].outStrokeName,"横折弯钩、横斜钩");       
    //四级状态初始化 T_Hps                                                                                                                                                    
    automachine[85].one=T_Hps;automachine[85].two=LU;automachine[85].three=T_Hpsg;        automachine[85].outFiveStroke=F_Z;strcpy(automachine[85].outStrokeName,"横折折折钩、横撇弯钩");       
    //四级状态初始化 T_Hpn                                                                                                                                                    
    automachine[86].one=T_Hpn;automachine[86].two=D;automachine[86].three=T_Hpng;         automachine[86].outFiveStroke=F_Z;strcpy(automachine[86].outStrokeName,"横折折折钩、横撇弯钩");       
    //四级状态初始化 T_Hsh                                                                                                                                                    
    automachine[87].one=T_Hsh;automachine[87].two=D;automachine[87].three=T_Hshs;         automachine[87].outFiveStroke=F_Z;strcpy(automachine[87].outStrokeName,"横折折折");       
    automachine[88].one=T_Hshs;automachine[88].two=D;automachine[88].three=T_Hshs;        automachine[88].outFiveStroke=F_Z;strcpy(automachine[88].outStrokeName,"横折折折");       
    automachine[89].one=T_Hsh;automachine[89].two=LD;automachine[89].three=T_Hshp;        automachine[89].outFiveStroke=F_Z;strcpy(automachine[89].outStrokeName,"竖折折、竖折撇");       
    automachine[90].one=T_Hshp;automachine[90].two=LD;automachine[90].three=T_Hshp;       automachine[90].outFiveStroke=F_Z;strcpy(automachine[90].outStrokeName,"竖折折、竖折撇");       
    automachine[91].one=T_Hsh;automachine[91].two=U;automachine[91].three=T_Hsht;         automachine[91].outFiveStroke=F_Z;strcpy(automachine[91].outStrokeName,"横折弯钩、横斜钩");       
    automachine[92].one=T_Hsh;automachine[92].two=RD;automachine[92].three=T_Hsht;        automachine[92].outFiveStroke=F_Z;strcpy(automachine[92].outStrokeName,"横折弯钩、横斜钩");       
    //四级状态初始化 T_Shs                                                                                                                                                    
    automachine[93].one=T_Shs;automachine[93].two=LD;automachine[93].three=T_Shw;         automachine[93].outFiveStroke=F_Z;strcpy(automachine[93].outStrokeName,"竖弯弯（自己起名字）");       
    automachine[94].one=T_Shs;automachine[94].two=LU;automachine[94].three=T_Shsg;        automachine[94].outFiveStroke=F_Z;strcpy(automachine[94].outStrokeName,"竖折折钩");       
    //四级状态初始化 T_Snh                                                                                                                                                    
    automachine[95].one=T_Snh;automachine[95].two=U;automachine[95].three=T_Xt;           automachine[95].outFiveStroke=F_Z;strcpy(automachine[95].outStrokeName,"斜钩");       
    automachine[96].one=T_Snh;automachine[96].two=RU;automachine[96].three=T_Xt;          automachine[96].outFiveStroke=F_Z;strcpy(automachine[96].outStrokeName,"斜钩");       
    automachine[97].one=T_Snh;automachine[97].two=LU;automachine[97].three=T_Xt;          automachine[97].outFiveStroke=F_Z;strcpy(automachine[97].outStrokeName,"斜钩");       
    
    //五级状态初始化 T_Hphs                                                                                                                                                   
    automachine[98].one=T_Hphs;automachine[98].two=LU;automachine[98].three=T_Hphsg;      automachine[98].outFiveStroke=F_Z;strcpy(automachine[98].outStrokeName,"横折折折钩、横撇弯钩");       
    //五级状态初始化 T_Hphp                                                                                                                                                   
    automachine[99].one=T_Hphp;automachine[99].two=U;automachine[99].three=T_Hphpg;       automachine[99].outFiveStroke=F_Z;strcpy(automachine[99].outStrokeName,"横折折折钩、横撇弯钩");       
    automachine[100].one=T_Hphp;automachine[100].two=LU;automachine[100].three=T_Hphpg;   automachine[100].outFiveStroke=F_Z;strcpy(automachine[100].outStrokeName,"横折折折钩、横撇弯钩");     
    //五级状态初始化 T_Hshs                                                                                                                                                   
    automachine[101].one=T_Hshs;automachine[101].two=LU;automachine[101].three=T_Hshsg;   automachine[101].outFiveStroke=F_Z;strcpy(automachine[101].outStrokeName,"横折折折钩、横撇弯钩");     
    //五级状态初始化 T_Hshp                                                                                                                                                   
    automachine[102].one=T_Hshp;automachine[102].two=LU;automachine[102].three=T_Hshpg;   automachine[102].outFiveStroke=F_Z;strcpy(automachine[102].outStrokeName,"横折折折钩、横撇弯钩");     
    //五级状态初始化 T_Shw                                                                                                                                                    
    automachine[103].one=T_Shw;automachine[103].two=LU;automachine[103].three=T_Shsg;     automachine[103].outFiveStroke=F_Z;strcpy(automachine[103].outStrokeName,"横折折折钩、横撇弯钩");  
    //补充状态图中没有右点和左点的笔画
    automachine[104].one=0;automachine[104].two=T_YD;automachine[104].three=T_YD;     automachine[104].outFiveStroke=F_N;strcpy(automachine[104].outStrokeName,"右点"); 
    automachine[105].one=0;automachine[105].two=T_ZD;automachine[105].three=T_ZD;     automachine[105].outFiveStroke=F_N;strcpy(automachine[105].outStrokeName,"左点"); 
    return 1;
}
//BYTE inOneState,BYTE inTwoState 为输入的状态，返回值为状态机求的的输出状态 和当前的Stroke
BYTE zinnia::CStrokeSlice::AutoMachine(BYTE inOneState,BYTE inTwoState,autostat &Stroke)
{
    if(automachine.size()<=0)return 0;
    BYTE rec=0;
    for(int i=0;i<automachine.size();i++)
    {
        if(automachine[i].one == inOneState && 
           automachine[i].two == inTwoState)
        {
            Stroke.one = automachine[i].one;
            Stroke.two = automachine[i].two;
            Stroke.three = automachine[i].three;
            Stroke.outFiveStroke = automachine[i].outFiveStroke;
            strcpy(Stroke.outStrokeName, automachine[i].outStrokeName);
            rec = Stroke.three;
            break;
        }
    }
    return rec;
}
//根据字段与部首之间的关系建立的自动状态机来求出Stroke的部首结果集合 返回 0 失败，正确返回识别的部首数量
int zinnia::CStrokeSlice::AutoMachineofStroke(std::vector<std::vector<autostat> > &Stroke)
{
    std::vector<std::vector<StrokeSliceItem> > Slice;
    TravelSSList(Slice);
    int icnt=0;
    icnt = Slice.size();
    if(icnt <= 0)return 0;
    BYTE CurState=0;
    BYTE OldState=0;
    int OldiTimeNum=0;
    autostat stroke,oldstroke;
    Stroke.resize(icnt);
    int intStrokeCnt=0;
    int rec=0;
    //打印所有字段信息
    
    
    for(int i=0;i<icnt;i++)
    {
        printf("slice[%d]=(",i);
        int oldnum=-1;
        for(int j=0;j<(Slice[i].size());j++)
            {
                if(oldnum == -1)
                {
                    oldnum = Slice[i][j].iTimeNum;
                    printf("%d,",Slice[i][j].Direct);
                }
                else if(oldnum == Slice[i][j].iTimeNum){
                    
                    printf("%d,",Slice[i][j].Direct);
                }
                else {
                    printf(")(%d,",Slice[i][j].Direct);
                    oldnum = Slice[i][j].iTimeNum;
                }
                
            }
        printf(")\n");
    }
    //将连笔的字段拆解为多笔，并在Slice中插入新的笔顺
    
    
    //将单笔单笔书写的字段，与其他字段组合成同一笔，识别儿童不会单独一笔一笔描字的图像。
    
    //纠正笔顺错误的字段
    
    //根据Slice的笔顺查找字库的汉字
    for(int i=0;i<icnt;i++)
    {
        intStrokeCnt=0;
        for(int j=0;j<(Slice[i]).size();j++)
        {
            if(j==0)
            {//第一个次和每次找到一个部首之后再寻找下一个部首时调用
                OldState =0;
                CurState = AutoMachine(0,Slice[i][j].Direct,stroke);
                if(CurState == 0)
                {//没有匹配到,则笔画就是上一次的找到的
                    printf("AutoMachineofStroke,CurState==0\n");
                    rec =0;
                    break;
                }
                else {
                    OldState = CurState;
                    OldiTimeNum = Slice[i][j].iTimeNum;
                    
                    oldstroke.one = stroke.one;
                    oldstroke.two = stroke.two;
                    oldstroke.three = stroke.three;
                    oldstroke.outFiveStroke = stroke.outFiveStroke;
                    strcpy(oldstroke.outStrokeName , stroke.outStrokeName);
                }
            }
            else//之后使用第一次的结果查找
            {
                if(OldiTimeNum == Slice[i][j].iTimeNum)//在同一笔中寻找笔画，如果不是同一笔则认为当前笔画识别结束。当第一次进入该笔画时也认为是可以查找的。
                    CurState = AutoMachine(OldState,Slice[i][j].Direct,stroke);
                else 
                {//如果当前的笔画和上一笔不是同一笔则，新的笔画开始搜索
                    //保存上一笔的结果。
                    Stroke[i].resize((Stroke[i]).size()+1);
                
                    Stroke[i][intStrokeCnt].one = stroke.one;
                    Stroke[i][intStrokeCnt].two = stroke.two;
                    Stroke[i][intStrokeCnt].three = stroke.three;
                    Stroke[i][intStrokeCnt].outFiveStroke = stroke.outFiveStroke;
                    strcpy(Stroke[i][intStrokeCnt++].outStrokeName , stroke.outStrokeName);
                    
                    OldState=0;//寻找下一个部首
                    OldiTimeNum = 0;
                    rec = 1;
                    //搜索新的一笔
                    CurState = AutoMachine(0,Slice[i][j].Direct,stroke);
                }
                if(CurState == 0)
                {//没有匹配到,则OldState就是找到的最后笔画
                    Stroke[i].resize((Stroke[i]).size()+1);
                    
                    Stroke[i][intStrokeCnt].one = stroke.one;
                    Stroke[i][intStrokeCnt].two = stroke.two;
                    Stroke[i][intStrokeCnt].three = stroke.three;
                    Stroke[i][intStrokeCnt].outFiveStroke = stroke.outFiveStroke;
                    strcpy(Stroke[i][intStrokeCnt++].outStrokeName , stroke.outStrokeName);
                    
                    OldState=0;//寻找下一个部首
                    OldiTimeNum = 0;
                    rec = 1;
                }
                else {
                    OldState = CurState;
                    OldiTimeNum = Slice[i][j].iTimeNum;
                    
                    oldstroke.one = stroke.one;
                    oldstroke.two = stroke.two;
                    oldstroke.three = stroke.three;
                    oldstroke.outFiveStroke = stroke.outFiveStroke;
                    strcpy(oldstroke.outStrokeName , stroke.outStrokeName);
                    //保存最后一笔
                    if ((j+1)>=(Slice[i]).size()) {
                        Stroke[i].resize((Stroke[i]).size()+1);
                        
                        Stroke[i][intStrokeCnt].one = stroke.one;
                        Stroke[i][intStrokeCnt].two = stroke.two;
                        Stroke[i][intStrokeCnt].three = stroke.three;
                        Stroke[i][intStrokeCnt].outFiveStroke = stroke.outFiveStroke;
                        strcpy(Stroke[i][intStrokeCnt++].outStrokeName , stroke.outStrokeName);
                        
                        OldState=0;//寻找下一个部首
                        OldiTimeNum = 0;
                        rec = 1;
                    }
                }//if
            }//if
        }//for
    }//for
    return rec;
}
void zinnia::CFiveHZZK::close()
{
    if (fd >= 0) {
        close__(fd);
        fd = -1;
    }
    
    if (text) {
        if (flag == O_RDWR) {
            int fd2;
            if ((fd2 = open__(fileName.c_str(), O_RDWR)) >= 0) {
                write(fd2, text, length);
                close__(fd2);
            }
        }
        delete [] text;

    }
    
    text = 0;
}
 bool zinnia::CFiveHZZK::open(const char *filename, const char *mode ) 
{
    this->close();
    struct stat st;
    fileName = std::string(filename);
    
    if      (std::strcmp(mode, "r") == 0)
        flag = O_RDONLY;
    else if (std::strcmp(mode, "r+") == 0)
        flag = O_RDWR;
    else
        CHECK_CLOSE_FALSE(false) << "unknown open mode: " << filename;
    
    CHECK_CLOSE_FALSE((fd = open__(filename, flag | O_BINARY)) >= 0)
    << "open failed: " << filename;
    
    CHECK_CLOSE_FALSE(fstat(fd, &st) >= 0)
    << "failed to get file size: " << filename;
    
    length = st.st_size;
    
    text = new char[length];
    CHECK_CLOSE_FALSE(read(fd, text, length) >= 0)
    << "read() failed: " << filename;
    
    close__(fd);
    fd = -1;
    
    return true;
}
//将汉字字库上载到对象的map数据结构中，方便于查找 成功返回 上载的数量，失败：返回 0;
int zinnia::CFiveHZZK::InitLoadHZZKtoMemory()
{
    int iRec=0;
    //将字库文件打开并放在char对象text中。
    char filename[]="//Users//macbookpro//D///d//soft///hand//mytestHZRC//HZRC//ziku//zikuGB.txt";
    CHECK_FALSE(open(filename))
    << "no such file or directory: " << filename;
    //将字库处理到map对象里面。
  //  printf("text=%s\n",text);
    /*
    for(int i=0;i<length;i++)
    {
        if((i+1)<length && text[i]=='\xFF' && text[i+1]=='\xFE'){//跳过字库文件头
            i=2;
        };
        if((i+1)<length){//是汉字
            char hz[3];
            char hzcode[100];
            hz[0]=text[i];
            hz[1]=text[i+1];
            hz[2]=0;
            int j=i+2;//跳过汉字
            if(j<length)
            {//将汉字的编码放入hzcode数组中。
                memset(hzcode,0,100);
                while(j<length && text[j]!='\r' )j++;//找到此行的结尾
                int k=0,n=0;                
                for(k=i+2; k<j; k++)
                {//跳过数字后面的0x00
                    if(text[k] !='\0')
                        hzcode[n++] = text[k];  
                }
                std::string test=hz;
                //test.
              //  printf("HZZK=[%s,%s]\n",hzcode,hz);
                HZZK.insert(std::make_pair(std::string(hzcode),std::string(hz)));//加入到mulmap对象中
                iRec++;
            }
            i = j+3;//跳过行结尾0x0D 0X00 0X0A 0X00            
        }
     
    }
     */
    for(int i=0;i<length;i++)
    { 
        char hz[3];
        char hzcode[100];
        
        hz[0]=text[i];
        hz[1]=text[i+1];
        hz[2]=0;
        int j=i+2;//跳过汉字
        if(j<length)
        {//将汉字的编码放入hzcode数组中。
            memset(hzcode,0,100);
            while(j<length && text[j]!='\r' )j++;//找到此行的结尾
            int k=0,n=0;                
            for(k=i+2; k<j; k++)
            {//跳过数字后面的0x00
                if(text[k] !='\0')
                    hzcode[n++] = text[k];  
            }
           // std::string test=hz;
           // wprintf("%s",hz);
            //char hzhex[20];
            //sprintf(hzhex,"［%X %X]",hz[0]&0xff,hz[1]&0xff);
            //NSString *mystr = [NSString stringWithCString:(hz) encoding:(NSUTF8StringEncoding) ];
            HZZK.insert(std::make_pair(std::string(hzcode),hz));//加入到mulmap对象中
            iRec++;
        
        }
        i = j+1;
         
    }
    return iRec;
}
//从字库中查找汉字编码所对应的汉字，并已数组的方h司返回。
int zinnia::CFiveHZZK::SeachHZZK(std::string  &inStroke,std::vector<std::string> &OutHZ)
{
    std::multimap<std::string, std::string>::iterator  it2;// = HZZK.lower_bound(inStroke);
    //int i=0;
    int num = HZZK.count(inStroke);
    it2 = HZZK.find(inStroke);
    if(num>0)
    printf("fint num=%d,it2=[%s,%s]\n",num,it2->first.c_str(), it2->second.c_str());
  /*  
    while (it2 != HZZK.upper_bound(inStroke))
    {
        
        printf("fint it2=[%s,%s]\n",it2->first.c_str(), it2->second.c_str());
        OutHZ.push_back(it2->second);
        i++;
        it2++;
    }
    */
    for(int i=1;i<=num;i++)
    {
        printf("fint i=%d,it2=[%s,%s]\n",i,it2->first.c_str(), it2->second.c_str());
        OutHZ.push_back(it2->second);
        
    }

            
    return num;
}
//从Stroke栈中寻找对应的每一个汉字
int zinnia::CStrokeSlice::GetRecognizeHZResult(std::vector<std::string> &result){
    
    std::vector<std::vector<autostat> > Stroke;
    int rec=0;
    AutoMachineofStroke(Stroke);//得到汉字部首识别结果
    //从汉字编码库中查找编码相同的汉字
    int istroke_size = Stroke.size();
    if(istroke_size<=0)return 0;
     
    for(int i=0;i<istroke_size;i++)
    {
        int isize = (Stroke[i]).size();
        if(isize <=0)continue;
        std::string inhz;
        for(int j=0;j<isize;j++)
        {
            switch (Stroke[i][j].outFiveStroke) {
                case F_H:
                    inhz +='1';
                    break;
                case F_S:
                    inhz +='2';
                    break;
                case F_P:
                    inhz +='3';
                    break;
                case F_N:
                    inhz +='4';
                    break;
                case F_Z:
                    inhz +='5';
                    break;
                default:
                    printf("error--no find\n");
                    break;
            }
        }//for
        //在mulmap中查找匹配的汉字
        std::vector<std::string> outhz;
        rec = hzzk.SeachHZZK(inhz, outhz);
        
        printf("result.push_back=［%s］",inhz.c_str());
        if(rec !=0)
        {
            for(int n=0;n<rec;n++)
            {
                //result.resize(n+1);
                result.push_back(outhz[n]);
                printf("[%s],",outhz[n].c_str());
            }
            
        }//if
        printf("\n");
        //
    }//for
    rec = result.size();
    return rec;
}