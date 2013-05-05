//
//  Palette.m
//  MyPalette
//
//  Created by xiaozhu on 11-6-23.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Palette.h"


@implementation Palette
@synthesize x;
@synthesize y;
- (id)initWithFrame:(CGRect)frame {
    
	NSLog(@"initwithframe");
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code.
    }
    return self;
	
}
-(void)IntsegmentColor
{
	switch (Intsegmentcolor)
	{
		case 0:
			segmentColor=[[UIColor blackColor] CGColor];
			break;
		case 1:
			segmentColor=[[UIColor redColor] CGColor];
			break;
		case 2:
			segmentColor=[[UIColor blueColor] CGColor];

			break;
		case 3:
			segmentColor=[[UIColor greenColor] CGColor];
			break;
		case 4:
			segmentColor=[[UIColor yellowColor] CGColor];
			break;
		case 5:
			segmentColor=[[UIColor orangeColor] CGColor];
			break;
		case 6:
			segmentColor=[[UIColor grayColor] CGColor];
			break;
		case 7:
			segmentColor=[[UIColor purpleColor]CGColor];
			break;
		case 8:
			
			segmentColor=[[UIColor brownColor]CGColor];
			break;
		case 9:
			segmentColor=[[UIColor magentaColor]CGColor];
			break;
		case 10:
			segmentColor=[[UIColor whiteColor]CGColor];
			break;

		default:
			break;
	}
}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect 
{
	//NSLog(@"Thes is drawRect ");
	//获取上下文
	CGContextRef context=UIGraphicsGetCurrentContext();
	//设置笔冒
	CGContextSetLineCap(context, kCGLineCapRound);
	//设置画线的连接处　拐点圆滑
	CGContextSetLineJoin(context, kCGLineJoinRound);
	//第一次时候个myallline开辟空间
	static BOOL allline=NO;
	if (allline==NO)
	{
		myallline=[[NSMutableArray alloc] initWithCapacity:20];
		myallColor=[[NSMutableArray alloc] initWithCapacity:10];
		myallwidth=[[NSMutableArray alloc] initWithCapacity:10];
        myNotePairs=[[NSMutableArray alloc] initWithCapacity:50];
        myiStrokeNum =[[NSMutableArray alloc] initWithCapacity:50]; 
		allline=YES;
	}
	//画之前线
	if ([myallline count]>0)
	{
		for (int i=0; i<[myallline count]; i++)
		{
           // NSMutableArray *p = [myallline objectAtIndex:i];
         //   NSArray* tempArray2 = [NSArray arrayWithArray:p]; 
			NSArray* tempArray=[NSArray arrayWithArray:[myallline objectAtIndex:i]];
			//----------------------------------------------------------------
			if ([myallColor count]>0)
			{
				Intsegmentcolor=[[myallColor objectAtIndex:i] intValue];
				Intsegmentwidth=20.0;//[[myallwidth objectAtIndex:i]floatValue]+1;
			}
			//-----------------------------------------------------------------
			if ([tempArray count]>1)
			{
				CGContextBeginPath(context);
				CGPoint myStartPoint=[[tempArray objectAtIndex:0] CGPointValue];
				CGContextMoveToPoint(context, myStartPoint.x, myStartPoint.y);
				
				for (int j=0; j<[tempArray count]-1; j++)
				{
					CGPoint myEndPoint=[[tempArray objectAtIndex:j+1] CGPointValue];
					//--------------------------------------------------------
					CGContextAddLineToPoint(context, myEndPoint.x,myEndPoint.y);	
				}
				[self IntsegmentColor];
				CGContextSetStrokeColorWithColor(context, segmentColor);
				//-------------------------------------------------------
				CGContextSetLineWidth(context, Intsegmentwidth);
				CGContextStrokePath(context);
			}
		}
	}
	//画当前的线
	if ([myallpoint count]>1)
	{
		CGContextBeginPath(context);
		//-------------------------
		//起点
		//------------------------
		CGPoint myStartPoint=[[myallpoint objectAtIndex:0]   CGPointValue];
		CGContextMoveToPoint(context,    myStartPoint.x, myStartPoint.y);
		//把move的点全部加入　数组
		for (int i=0; i<[myallpoint count]-1; i++)
		{
			CGPoint myEndPoint=  [[myallpoint objectAtIndex:i+1] CGPointValue];
			CGContextAddLineToPoint(context, myEndPoint.x,   myEndPoint.y);
		}
		//在颜色和画笔大小数组里面取不相应的值
		Intsegmentcolor=[[myallColor lastObject] intValue];
		Intsegmentwidth= 20.0;//[[myallwidth lastObject]floatValue]+1;
		
		//-------------------------------------------
		//绘制画笔颜色
		[self IntsegmentColor];
		CGContextSetStrokeColorWithColor(context, segmentColor);
		CGContextSetFillColorWithColor (context,  segmentColor);
		//-------------------------------------------
		//绘制画笔宽度
		CGContextSetLineWidth(context, Intsegmentwidth);
		//把数组里面的点全部画出来
		CGContextStrokePath(context);
	}
    
    //画分析后字的显著点的圆
    int icnt = [myNotePairs count]; 
    if (icnt>0)
	{
        CGContextBeginPath(context);
        
        
        CGPoint myStartPoint=[[myNotePairs objectAtIndex:0]   CGPointValue];
        CGContextMoveToPoint(context,    myStartPoint.x, myStartPoint.y);
		CGContextAddArc(context, myStartPoint.x, myStartPoint.y, 6, 0, 2*M_PI, YES);
        for (int i=0; i<icnt-1; i++)
		{
			CGPoint myStartPoint=[[myNotePairs objectAtIndex:i+1]   CGPointValue];
            CGContextMoveToPoint(context,    myStartPoint.x, myStartPoint.y);
            CGContextAddArc(context, myStartPoint.x, myStartPoint.y, 6, 0, 2*M_PI, YES);
		}
        
        //绘制画笔颜色
		segmentColor=[[UIColor redColor] CGColor];
		CGContextSetStrokeColorWithColor(context, segmentColor);
		CGContextSetFillColorWithColor (context,  segmentColor);
		//-------------------------------------------
		//绘制画笔宽度
        Intsegmentwidth=1.0;
		CGContextSetLineWidth(context, Intsegmentwidth);
        
        CGContextStrokePath(context);
        //UIGraphicsPopContext();
    }
     
    
    //将显著点的圆心连接起来形成字
    icnt =0;
    icnt = [myNotePairs count]; 
    if (icnt>0)
	{   int OldiNum = 0;
        
        CGContextBeginPath(context);
        CGPoint myStartPoint=[[myNotePairs objectAtIndex:0] CGPointValue];
        CGContextMoveToPoint(context, myStartPoint.x, myStartPoint.y);
        OldiNum = [[myiStrokeNum objectAtIndex:0] intValue];
        for (int j=0; j<icnt-1; j++)
        {
            CGPoint myEndPoint=[[myNotePairs objectAtIndex:j+1] CGPointValue];
            //--------------------------------------------------------
            int iNum = [[myiStrokeNum objectAtIndex:j+1] intValue];
            if(OldiNum == iNum)
            {
               CGContextAddLineToPoint(context, myEndPoint.x,myEndPoint.y); 
            }
            else {
                OldiNum = iNum;
                CGContextMoveToPoint(context, myEndPoint.x, myEndPoint.y);
            }
            	
        }

        
        //绘制画笔颜色
		segmentColor=[[UIColor redColor] CGColor];
		CGContextSetStrokeColorWithColor(context, segmentColor);
		CGContextSetFillColorWithColor (context,  segmentColor);
		//-------------------------------------------
		//绘制画笔宽度
        Intsegmentwidth=1.0;
		CGContextSetLineWidth(context, Intsegmentwidth);
        
        CGContextStrokePath(context);
        //UIGraphicsPopContext();
    }
    
}
//===========================================================
//初始化
//===========================================================
-(void)Introductionpoint1
{
	//NSLog(@"in init allPoint");
	myallpoint=[[NSMutableArray alloc] initWithCapacity:50];
    myNotePairs=[[NSMutableArray alloc] initWithCapacity:50];
    myiStrokeNum =[[NSMutableArray alloc] initWithCapacity:50]; 
    
}
//===========================================================
//把画过的当前线放入　存放线的数组
//===========================================================
-(void)Introductionpoint2
{
	[myallline addObject:myallpoint];
}
//-(void)IntroductionmyNotePairspoint:(CGPoint)sender
-(void)IntroductionmyNotePairspoint:(CGPoint) sender andStrokeNum: (int) iStrokeNum;
{
	NSValue* pointvalue=[NSValue valueWithCGPoint:sender];
	[myNotePairs addObject:[pointvalue retain]];
	[pointvalue release];
    
    NSNumber* intNumber;
    intNumber = [NSNumber numberWithInt:iStrokeNum];


	[myiStrokeNum addObject:[intNumber retain]];
	[intNumber release];
    
    //[self setNeedsLayout];
    //UIGraphicsPopContext();
}
-(void)Introductionpoint3:(CGPoint)sender
{
	NSValue* pointvalue=[NSValue valueWithCGPoint:sender];
	[myallpoint addObject:[pointvalue retain]];
	[pointvalue release];
    //UIGraphicsPopContext();
    
}
//===========================================================
//接收颜色segement反过来的值
//===========================================================
-(void)Introductionpoint4:(int)sender
{
	//NSLog(@"Palette sender:%i", sender);
	NSNumber* Numbersender= [NSNumber numberWithInt:sender];
	[myallColor addObject:Numbersender];
}
//===========================================================
//接收线条宽度按钮反回来的值
//===========================================================
-(void)Introductionpoint5:(int)sender
{
	//NSLog(@"Palette sender:%i", sender);
	NSNumber* Numbersender= [NSNumber numberWithInt:sender];
	[myallwidth addObject:Numbersender];
}
//===========================================================
//清屏按钮
//===========================================================
-(void)myalllineclear
{
	if ([myallline count]>0)
	{
		[myallline removeAllObjects];
		[myallColor removeAllObjects];
		[myallwidth removeAllObjects];
		[myallpoint removeAllObjects];
        [myNotePairs removeAllObjects];
        [myiStrokeNum removeAllObjects];
		myallline=[[NSMutableArray alloc] initWithCapacity:10];
		myallColor=[[NSMutableArray alloc] initWithCapacity:10];
		myallwidth=[[NSMutableArray alloc] initWithCapacity:10];
		[self setNeedsDisplay];
	}
}
//===========================================================
//撤销
//===========================================================
-(void)myLineFinallyRemove
{
	if ([myallline count]>0)
	{
		[myallline  removeLastObject];
		[myallColor removeLastObject];
		[myallwidth removeLastObject];
		[myallpoint removeAllObjects];
	}
	[self setNeedsDisplay];	
}
//===========================================================
//橡皮擦　segmentColor=[[UIColor whiteColor]CGColor];
//===========================================================
//-(void)myrubberseraser
//{
//	segmentColor=[[UIColor whiteColor]CGColor];
//}
-(void)button
{
	NSLog(@"button");
	
	//[self setNeedsDisplay];
}
- (void)dealloc 
{
    [super dealloc];
}


@end
