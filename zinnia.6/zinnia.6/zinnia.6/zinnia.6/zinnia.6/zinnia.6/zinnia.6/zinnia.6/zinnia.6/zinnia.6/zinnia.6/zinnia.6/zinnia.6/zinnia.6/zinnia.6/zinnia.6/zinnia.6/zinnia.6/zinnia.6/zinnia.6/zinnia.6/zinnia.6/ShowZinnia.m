//
//  ShowZinnia.m
//  zinnia.5
//
//  Created by  on 12-10-8.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "ShowZinnia.h"
#import "zinnia.h"

@implementation ShowZinnia

- (IBAction)begintButton:(id)sender
{
    char *outText=NULL; 
   outText = new char[1000];
    memset(outText,0,1000);
    recog(outText);

   // NSString *mystr = [NSString stringWithCString:(outText) encoding:(NSUTF8StringEncoding)];
  
    delete outText;
}    
@end
