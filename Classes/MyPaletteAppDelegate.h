//
//  MyPaletteAppDelegate.h
//  MyPalette
//
//  Created by xiaozhu on 11-6-23.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MyPaletteViewController;

@interface MyPaletteAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    MyPaletteViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet MyPaletteViewController *viewController;

@end

