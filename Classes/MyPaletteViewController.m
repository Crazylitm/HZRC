//
//  MyPaletteViewController.m
//  MyPalette
//
//  Created by xiaozhu on 11-6-23.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "MyPaletteViewController.h"
#import "zinnia.h"

@implementation MyPaletteViewController
@synthesize Segment;
@synthesize labelLoanshift;
@synthesize labelColor;
@synthesize labelShowHZ;
-(IBAction)myPalttealllineclear
{
	[(Palette*)self.view myalllineclear];
}
//
-(IBAction)LineFinallyRemove
{
	[(Palette*)self.view myLineFinallyRemove];
    
}
-(IBAction)distance
{
    /*
    if (first == last) return 0.0;
    
    const float a = 1.0*last->x/320 - 1.0*first->x/320;
    const float b = 1.0*last->y/460 - 1.0*first->y/460;
    const float c = 1.0*last->y/460 * 1.0*first->x/320 - 1.0*last->x/320 * 1.0*first->y/460;
    
    float max = -1.0;
    for (const Node *n = first; n != last; ++n) {
        const float dist = std::fabs((a * 1.0*n->y/460) -(b * 1.0*n->x/320) + c);
        if (dist > max) {
            max = dist;
            *best = const_cast<Node *>(n);
        }
    }
    
    return max * max /(a * a + b * b);
*/
    /*
    float first_x = 141;
    float first_y = 127;
    float last_x  =207;
    float last_y  =231;
    float n_x[] ={141,134,124,113,99,91,82,75,72,70,68,68,68,68,73,76,79,85,89,96,103,114,126,134,150,159,174,186,192,198,202,205,206,207,207,207};//62;
    float n_y[] ={127,128,132,139,151,161,174,189,199,211,224,235,248,261,286,295,301,307,310,311,312,312,308,304,296,291,281,272,264,255,247,240,236,233,232,231};//151;
    int iCnt =37;
    const float a = 1.0*last_x/320 - 1.0*first_x/320;
    const float b = 1.0*last_y/460 - 1.0*first_y/460;
    const float c = 1.0*last_y/460 * 1.0*first_x/320 - 1.0*last_x/320 * 1.0*first_y/460;
    
    for(int i=0; i< iCnt;++i)
    {
        const float dist = (a * 1.0*n_y[i]/460) -(b * 1.0*n_x[i]/320) + c;
        float dic = dist*dist/(a*a +b*b);
        NSLog(@"---a=%.2f----b=%.2f-----c=%.2f---dist=%.2f----dic=%.2f-----x[%d]=%.0f",a,b,c,dist,dic,i,n_x[i]);
    }
*/



}
//汉字识别初始化
-(void)ZinniaInit
{
    //创建手写识别
    if (character == 0) {
        character = zinnia_character_new();
        zinnia_character_clear(character);
        zinnia_character_set_width(character, 320);
        zinnia_character_set_height(character, 460);
    }
    if (recognizer == 0) {
        recognizer = zinnia_recognizer_new();
    }
    
    //打开成功返回1
    if (!zinnia_recognizer_open(recognizer, "/usr/local/lib/zinnia/model/tomoe/handwriting-zh_CN.model")) {
        fprintf(stderr, "ERROR1: %s\n", zinnia_recognizer_strerror(recognizer));
        return;
    }
    handwriteCount = 0;//笔画数清零
}
//汉字识别button
-(IBAction)zinniabutton10
{
    char myOutString[5000];
    NSString *mystr=NULL;
    zinnia_result_t *result=NULL;
    labelShowHZ.text=@"开始识别........\n";
    labelShowHZ.textColor=[UIColor blackColor];
    // NSLog(@"%s",myStrokes);
    
    if(zinnia_character_strokes_size(character)>0)
    {
        if(!(result = zinnia_recognizer_classify2(recognizer, character)))
        {
            labelShowHZ.text=@"识别失败........\n";
            labelShowHZ.textColor=[UIColor redColor];
        }
    }
    if (result != 0) {
        int i=0;
        
        memset(myOutString,0,2000);
        for (i = 0; i < zinnia_result_size(result); ++i) {
            if(strlen(myOutString)<=0 || i==0)
            {
                printf( "%s\t%f\n",
                        zinnia_result_value(result, i),
                        zinnia_result_score(result, i));
            
                sprintf(myOutString, "%s\t%f\n",
                        zinnia_result_value(result, i),
                        zinnia_result_score(result, i));
            }
            else
            {
                char stemp[2000];
                printf( "%s\t%f\n",
                       zinnia_result_value(result, i),
                       zinnia_result_score(result, i));
                sprintf(stemp, "%s\t%f\n",
                        zinnia_result_value(result, i),
                        zinnia_result_score(result, i));
                strcat(myOutString, stemp);
            }
            
        }
       // NSStringEncoding enc=CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingHZ_GB_2312);
       //  NSStringEncoding enc=CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_2312_80);
        NSStringEncoding enc=CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
        NSData *data=[NSData dataWithBytes:myOutString length:strlen(myOutString)];
        
        mystr = [[NSString alloc] initWithData:data encoding:enc];//[NSString stringWithCString:(myOutString) encoding:enc ];
        NSLog(@"%@",mystr);
        printf("mystr=%s\n",mystr);
        labelShowHZ.text=mystr;
        zinnia_result_destroy(result);
    }
    
    zinnia_character_clear(character);
    handwriteCount = 0;//笔画数清零
    
    /*
     char *outText=NULL; 
     outText = new char[5000];
     memset(outText,0,5000);
     recog(outText);
     
     NSString *mystr = [NSString stringWithCString:(outText) encoding:(NSUTF8StringEncoding)];
     
     labelShowHZ.text=mystr;
     */
    //清空背景颜色  
    labelShowHZ.backgroundColor = [UIColor clearColor];  
    //设置字体颜色为白色  
    labelShowHZ.textColor = [UIColor blackColor];  
    //文字居中显示  
    labelShowHZ.textAlignment = UITextAlignmentCenter;  
    //自动折行设置  
    labelShowHZ.lineBreakMode = UILineBreakModeWordWrap;  
    labelShowHZ.numberOfLines = 0;  
    [(Palette*)self.view setNeedsDisplay];
    //delete outText;
}
-(IBAction)zinniabutton
{
   
    
    char myOutString[5000];
    NSString *mystr=NULL;
    zinnia_result_t *result=NULL;
   // labelShowHZ.text=@"开始识别........\n";
    labelShowHZ.textColor=[UIColor blackColor];
   // NSLog(@"%s",myStrokes);
    //[(Palette*)self.view  myalllineclear];
    //[self ZinniaInit];
    if(zinnia_character_strokes_size(character)>0)
    {
        if(!(result = zinnia_recognizer_classify2(recognizer, character)))
        {
         //   labelShowHZ.text=@"识别失败........\n";
            labelShowHZ.textColor=[UIColor redColor];
        }
    }
    
    if (result != 0) {
        int i=0;
        
        
        memset(myOutString,0,2000);
        for (i = 0; i < zinnia_result_size_pairs(result); ++i) {
            CGPoint tempPoint;
            int iStrokeNum=0;
            tempPoint.x = zinnia_result_getx(result, i);
            tempPoint.y = zinnia_result_gety(result, i);
            iStrokeNum = zinnia_result_getStorkeNum(result, i);
            [(Palette*)self.view IntroductionmyNotePairspoint:(tempPoint) andStrokeNum:(iStrokeNum)];
  /*
             NSLog(@"tempPoint-Count=%d,x=%.0f,y=%.0f,Num=%lu",zinnia_result_size_pairs(result), tempPoint.x , tempPoint.y,iStrokeNum);
            if(strlen(myOutString)<=0 || i==0)
                sprintf(myOutString, "%.0f\t%.0f\t%lu\n",
                        zinnia_result_getx(result, i),
                        zinnia_result_gety(result, i),iStrokeNum);
            else
            {
                char stemp[2000];
                sprintf(stemp, "%.0f\t%.0f\t%lu\n",
                        zinnia_result_getx(result, i),
                        zinnia_result_gety(result, i),iStrokeNum);
                strcat(myOutString, stemp);
            }
            */
        }
        [(Palette*)self.view setNeedsDisplay];//只有写了这句话 UIView中的drawRect才会被系统马上调用重绘
       // labelShowHZ.text=mystr;
        
        
        memset(myOutString,0,2000);
        for (i = 0; i < zinnia_result_size_hz(result); ++i) {
            if(strlen(myOutString)<=0 || i==0)
            {
                printf( "%s\n",
                       zinnia_result_hz(result, i));
                
                sprintf(myOutString, "%s\n",
                        zinnia_result_hz(result, i));
            }
            else
            {
                char stemp[2000];
                printf( "%s\n",
                       zinnia_result_hz(result, i));
                sprintf(stemp, "%s\n",
                        zinnia_result_hz(result, i));
                strcat(myOutString, stemp);
            }
            
        }
        // NSStringEncoding enc=CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingHZ_GB_2312);
        //  NSStringEncoding enc=CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_2312_80);
        NSStringEncoding enc=CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
        NSData *data=[NSData dataWithBytes:myOutString length:strlen(myOutString)];
        
        mystr = [[NSString alloc] initWithData:data encoding:enc];//[NSString stringWithCString:(myOutString) encoding:enc ];
        NSLog(@"%@",mystr);
        printf("mystr=%s\n",mystr);
        labelShowHZ.text=mystr;
        
        
         zinnia_result_destroy(result);
    }
    
    zinnia_character_clear(character);
    handwriteCount = 0;//笔画数清零
    
    /*
    char *outText=NULL; 
    outText = new char[5000];
    memset(outText,0,5000);
    recog(outText);
    
    NSString *mystr = [NSString stringWithCString:(outText) encoding:(NSUTF8StringEncoding)];
    
    labelShowHZ.text=mystr;
    */
    //清空背景颜色  
    labelShowHZ.backgroundColor = [UIColor clearColor];  
    //设置字体颜色为白色  
    labelShowHZ.textColor = [UIColor blackColor];  
    //文字居中显示  
    labelShowHZ.textAlignment = UITextAlignmentCenter;  
    //自动折行设置  
    labelShowHZ.lineBreakMode = UILineBreakModeWordWrap;  
    labelShowHZ.numberOfLines = 0;  
    
    //delete outText;
}
//颜色
-(IBAction)myAllColor
{
	[self segmentColor];
}
//宽度
-(IBAction)myAllWidth
{
	[self segmentWidth];
}
//橡皮
-(IBAction)myRubberSeraser
{
	
	Segment=10;
}

//方法名
//方法作用
//方法
//===========================================================
-(void)segmentColor
{
	NSArray* segmentArray=[[NSArray alloc] initWithObjects:
						   [NSString stringWithString:@"黑"],
						   [NSString stringWithString:@"红"],
						   [NSString stringWithString:@"蓝"],
						   [NSString stringWithString:@"绿"],
						   [NSString stringWithString:@"黄"],
						   [NSString stringWithString:@"橙"],
						   [NSString stringWithString:@"灰"],
						   [NSString stringWithString:@"紫"],
						   [NSString stringWithString:@"棕"],
						   [NSString stringWithString:@"粉红"],nil];
	ColorButton=[[UISegmentedControl alloc] initWithItems:segmentArray];
	CGRect ButtonRect=CGRectMake(0, 0, 320, 40);
	[ColorButton setFrame:ButtonRect];
	[ColorButton setMomentary:YES];
	[ColorButton addTarget:self action:@selector(segmentColorButton:) 
		  forControlEvents:UIControlEventValueChanged];
	
	[ColorButton setSegmentedControlStyle:UISegmentedControlStyleBar];
	
	[ColorButton setTintColor:[UIColor darkGrayColor]];
	
	[self.view addSubview:ColorButton];
}
-(void)segmentColorButton:(id)sender
{
	Segment = [sender selectedSegmentIndex] ;
	switch (Segment)
	{
		case 0:
			labelColor.text=@"颜色:黑色";
			labelColor.textColor=[UIColor blackColor];
			break;
		case 1:
			labelColor.text=@"颜色:红色";
			labelColor.textColor=[UIColor redColor];
			break;
		case 2:
			labelColor.text=@"颜色:蓝色";
			labelColor.textColor=[UIColor blueColor];
			break;
		case 3:
			labelColor.text=@"颜色:绿色";
			labelColor.textColor=[UIColor greenColor];
			break;
		case 4:
			labelColor.text=@"颜色:黄色";
			labelColor.textColor=[UIColor yellowColor];
			break;
		case 5:
			labelColor.text=@"颜色:橙色";
			labelColor.textColor=[UIColor orangeColor];
			break;
		case 6:
			labelColor.text=@"颜色:灰色";
			labelColor.textColor=[UIColor grayColor];
			break;
		case 7:
			labelColor.text=@"颜色:紫色";
			labelColor.textColor=[UIColor purpleColor];
			break;
		case 8:
			labelColor.text=@"颜色:棕色";
			labelColor.textColor=[UIColor brownColor];
			break;
		case 9:
			labelColor.text=@"颜色:粉红色";
			labelColor.textColor=[UIColor magentaColor];
			break;
		default:
			break;
	}
}
//====================================================================
-(void)segmentWidth
{
	NSArray* segmentArray=[[NSArray alloc] initWithObjects:
						   [NSString stringWithString:@"1.0"],
						   [NSString stringWithString:@"2.0"],
						   [NSString stringWithString:@"3.0"],
						   [NSString stringWithString:@"4.0"],
						   [NSString stringWithString:@"5.0"],
						   [NSString stringWithString:@"6.0"],
						   [NSString stringWithString:@"7.0"],
						   [NSString stringWithString:@"8.0"],
						   [NSString stringWithString:@"9.0"],
						   [NSString stringWithString:@"12.0"],nil];
	WidthButton=[[UISegmentedControl alloc] initWithItems:segmentArray];
	CGRect ButtonRect=CGRectMake(0, 0, 320, 40);
	[WidthButton setFrame:ButtonRect];
	[WidthButton setMomentary:YES];
	[WidthButton addTarget:self action:@selector(segmentWidthButton:) 
		  forControlEvents:UIControlEventValueChanged];
	
	[WidthButton setSegmentedControlStyle:UISegmentedControlStyleBar];
	[WidthButton setTintColor:[UIColor darkGrayColor]];
	
	[self.view addSubview:WidthButton];
}
-(void)segmentWidthButton:(id)sender
{
	SegmentWidth =[sender selectedSegmentIndex];
	switch (SegmentWidth)
	{
		case 0:
			labelLoanshift.text=@"字号:1.0";
		//	labelColor.textColor=[UIColor blackColor];
			break;
		case 1:
			labelLoanshift.text=@"字号:2.0";
		//	labelColor.textColor=[UIColor redColor];
			break;
		case 2:
			labelLoanshift.text=@"字号:3.0";
		//	labelColor.textColor=[UIColor blueColor];
			break;
		case 3:
			labelLoanshift.text=@"字号:4.0";
		//	labelColor.textColor=[UIColor greenColor];
			break;
		case 4:
			labelLoanshift.text=@"字号:5.0";
		///	labelColor.textColor=[UIColor yellowColor];
			break;
		case 5:
			labelLoanshift.text=@"字号:6.0";
		//	labelColor.textColor=[UIColor orangeColor];
			break;
		case 6:
			labelLoanshift.text=@"字号:7.0";
		//	labelColor.textColor=[UIColor grayColor];
			break;
		case 7:
			labelLoanshift.text=@"字号:8.0";
		//	labelColor.textColor=[UIColor purpleColor];
			break;
		case 8:
			labelLoanshift.text=@"字号:9.0";
		//	labelColor.textColor=[UIColor brownColor];
			break;
		case 9:
			labelLoanshift.text=@"字号:12.0";
		//	labelColor.textColor=[UIColor magentaColor];
			break;
		default:
			break;
	}

	
}
//******************************************************************************************
-(IBAction)captureScreen
{
	//保存瞬间把view上的所有按钮的Alpha值改为０
	[[self.view subviews] makeObjectsPerformSelector:@selector (setAlpha:)];
	
	UIGraphicsBeginImageContext(self.view.bounds.size);
	
	[self.view.layer renderInContext:UIGraphicsGetCurrentContext()];
	
	UIImage* image=UIGraphicsGetImageFromCurrentImageContext();
	
	UIGraphicsEndImageContext();
	
	UIImageWriteToSavedPhotosAlbum(image, self, nil, nil);
	
	[[self.view subviews] makeObjectsPerformSelector:@selector (setAlpha:)];
	//遍历view全部按钮在把他们改为１
	for (UIView* temp in [self.view subviews]) 
	{
		[temp setAlpha:1.0];
	}
	
	
}

-(IBAction)callCame
{
	//指定图片来源
	UIImagePickerControllerSourceType sourceType=UIImagePickerControllerSourceTypeCamera;
	//判断如果摄像机不能用图片来源与图片库
	if (![UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
	{
		sourceType=UIImagePickerControllerSourceTypePhotoLibrary;
	}
	UIImagePickerController *picker=[[UIImagePickerController alloc] init];
	picker.delegate=self;
	//前后摄像机
	//picker.cameraDevice=UIImagePickerControllerCameraDeviceFront;
	picker.allowsEditing=YES;
	picker.sourceType=sourceType;
	[self presentModalViewController:picker animated:YES];
	[picker release];
}
-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
	//返回原来界面
	[picker dismissModalViewControllerAnimated:YES];
	//
	UIImage* image=[[info objectForKey:UIImagePickerControllerEditedImage] retain];
	//延时
	[self performSelector:@selector(saveImage:) withObject:image afterDelay:0.5];
	
}
//按取消键时
-(void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
	[picker dismissModalViewControllerAnimated:YES];
}
//保存图片
-(void)saveImage:(id)sender
{
	
	if (pickImage!=nil)
	{
		[pickImage removeFromSuperview];
		[pickImage initWithImage:sender];
		pickImage.frame=CGRectMake(40, 40, 200, 200);

		[self.view insertSubview:pickImage atIndex:2];
		//[self.view sendSubviewToBack:pickImage];//添加到最后一层
		//self.view.backgroundColor=[UIColor clearColor];
		//self.view.alpha=0;
		//[self.view addSubview:pickImage];
	
		
		
	}
	else 
	{
		pickImage=[[UIImageView alloc] initWithImage:sender];
		pickImage.frame=CGRectMake(40, 40, 200, 200);
		
		[self.view insertSubview:pickImage atIndex:2];
		//[self.view sendSubviewToBack:pickImage];//添加到最后一层
		//self.view.backgroundColor=[UIColor clearColor];
		//self.view.alpha=0;
		///[self.view addSubview:pickImage];
	}
}
//******************************************************************************************
/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad 
{
    [self ZinniaInit];
    [super viewDidLoad];
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning 
{
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc 
{
    [super dealloc];
}
#pragma mark -
//手指开始触屏开始
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	[[self.view subviews] makeObjectsPerformSelector:@selector (setAlpha:)];
	
	[ColorButton removeAllSegments];
	[WidthButton removeAllSegments];
	UITouch* touch=[touches anyObject];
	MyBeganpoint=[touch locationInView:self.view ];
	//NSLog(@"MyBeganpoint-Count=%d,x=%f,y=%f",handwriteCount,MyBeganpoint.x,MyBeganpoint.y);

	[(Palette*)self.view Introductionpoint4:Segment];
	[(Palette*)self.view Introductionpoint5:SegmentWidth];
	[(Palette*)self.view Introductionpoint1];
	[(Palette*)self.view Introductionpoint3:MyBeganpoint];

	
	//NSLog(@"======================================");
	//NSLog(@"MyPalette Segment=%i",Segment);
}
//输出strokes串
-(void)Makestrokes:(int) x whithY:(int) y
{
    static int icount=0;
    static int oldx=0,oldy=0;
    char stemp[2000];
    
    if(!(oldx== x && oldy == y))
    {
        if(icount == 0)
        {
            sprintf(myStrokes, "((%d %d)",x,y);
        }
        else if(icount == (handwriteCount+1) )
        {

            sprintf(stemp, "(%d %d)",x,y);
            strcat(myStrokes, stemp);
        }
        else if(icount != (handwriteCount+1))
        {
            sprintf(stemp, ")((%d %d)",x,y);
            strcat(myStrokes, stemp);
        }
    }
    oldx = x;
    oldy = y;
    icount = handwriteCount+1;
}
//手指移动时候发出
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    
	NSArray* MovePointArray=[touches allObjects];
	MyMovepoint=[[MovePointArray objectAtIndex:0] locationInView:self.view];
    int ix=(int)MyMovepoint.x;
    int iy=(int)MyMovepoint.y;
    zinnia_character_add(character,handwriteCount,ix,iy);//记录字开始的位置
   // NSLog(@"MyMovepoint-Count=%d,x=%d,y=%d",handwriteCount,ix,iy);
    //[self   Makestrokes : ix whithY: iy];
    //NSLog(@"%@",[[MovePointArray objectAtIndex:0] locationInView:self.view]);
	[(Palette*)self.view Introductionpoint3:MyMovepoint];
	[self.view setNeedsDisplay];
}
//当手指离开屏幕时候
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
   // zinnia_character_add(character,handwriteCount,ix,iy);//记录字开始的位置
    handwriteCount++;//笔画数增加
	//遍历view全部按钮在把他们改为１
	for (UIView* temp in [self.view subviews]) 
	{
		
		[temp setAlpha:1.0];
	}
	
	[(Palette*)self.view Introductionpoint2];
	[self.view setNeedsDisplay];
}
//电话呼入等事件取消时候发出
-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	//NSLog(@"touches Canelled");
}
@end
