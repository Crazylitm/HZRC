//
//  MyPaletteViewController.h
//  MyPalette
//
//  Created by xiaozhu on 11-6-23.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Palette.h"
#import <QuartzCore/QuartzCore.h>
#import "zinnia.h"

@interface MyPaletteViewController : UIViewController 
<UINavigationControllerDelegate,UIImagePickerControllerDelegate>
{
	UILabel* labelColor;
	UILabel* labelLoanshift;
    UILabel* labelShowHZ;//------为汉字识别输出做准备
	CGPoint MyBeganpoint;
	CGPoint MyMovepoint;
	int Segment;
	int SegmentWidth;
	//----------------
	UIImageView* pickImage;
	
	UISegmentedControl* WidthButton;
	UISegmentedControl* ColorButton;
    //汉字识别对象
    zinnia_recognizer_t *recognizer;
    zinnia_character_t  *character;
    int handwriteCount;
    char myStrokes[5000];
    
}
@property int Segment;
@property (nonatomic,retain)IBOutlet UILabel* labelColor;
@property (nonatomic,retain)IBOutlet UILabel* labelLoanshift;
@property (nonatomic,retain)IBOutlet UILabel* labelShowHZ;//------为汉字识别输出做准备

-(void)Makestrokes:(int) x whithY:(int) y;

-(IBAction)myAllColor;
-(IBAction)myAllWidth;
-(IBAction)myPalttealllineclear;
-(IBAction)LineFinallyRemove;
-(IBAction)myRubberSeraser;

-(void)segmentColor;
-(void)segmentWidth;
-(void)ZinniaInit;
-(IBAction)distance;
//=====================
-(IBAction)callCame;
-(IBAction)captureScreen;
//========================为汉字开始运算的button
-(IBAction)zinniabutton;
@end

