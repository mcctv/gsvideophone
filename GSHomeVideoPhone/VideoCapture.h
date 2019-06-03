//
//  VideoCapture.h
//  GSHomeVideoPhone
//
//  Created by mcctv on 14-5-20.
//  Copyright (c) 2014年 mcctv. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include <iostream>

using namespace std;

@interface VideoCapture : NSObject
//<AVCaptureVideoDataOutputSampleBufferDelegate>
{
    
    AVCaptureSession *m_session;
    AVCaptureDevice  *m_device;
    AVCaptureVideoDataOutput *m_output;
    UIView           *m_preview;
   
    
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection;

-(int)startVideoCapture:(UIView*)preview;
-(int)stopVideoCapture;

@end



