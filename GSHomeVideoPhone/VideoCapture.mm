//
//  VideoCapture1.h
//  GSHomeVideoPhone
//
//  Created by mcctv on 14-5-20.
//  Copyright (c) 2014年 mcctv. All rights reserved.
//

#import "VideoCapture.h"


@implementation VideoCapture

-(int)startVideoCapture:(UIView*)preview
{
    m_preview = preview;
    NSError *error = nil;
    
    // Create the session
    m_session = [[AVCaptureSession alloc] init];
    
    // Configure the session to produce lower resolution video frames, if your
    // processing algorithm can cope. We'll specify medium quality for the
    // chosen device.
    m_session.sessionPreset = AVCaptureSessionPreset640x480;
    
    // Find a suitable AVCaptureDevice
    m_device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    
    // Create a device input with the device and add it to the session.
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:m_device error:&error];
    if (!input) {
        // Handling the error appropriately.
    }
    [m_session addInput:input];
    
    // Create a VideoDataOutput and add it to the session
    m_output = [[AVCaptureVideoDataOutput alloc] init] ;
    [m_session addOutput:m_output];
    
    // Configure your output.
    dispatch_queue_t queue = dispatch_queue_create("myQueue", NULL);
    [m_output setSampleBufferDelegate:self queue:queue];
    // dispatch_release(queue);
    
    // Specify the pixel format
    m_output.videoSettings =
    [NSDictionary dictionaryWithObject:
     [NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8Planar]
                                forKey:(id)kCVPixelBufferPixelFormatTypeKey];
    
    
    // If you wish to cap the frame rate to a known value, such as 15 fps, set
    // minFrameDuration.
    m_output.minFrameDuration = CMTimeMake(1, 15);
    
    //prewview
    
    AVCaptureVideoPreviewLayer* previewLayer = [AVCaptureVideoPreviewLayer layerWithSession: m_session];
    previewLayer.frame = m_preview.bounds;
    previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    [m_preview.layer addSublayer: previewLayer];
    
    
    // Start the session running to start the flow of data
    [m_session startRunning];
    return 0;
}
-(int)stopVideoCapture
{
    if(m_session)
    {
        [m_session stopRunning];
        // [m_session release];
        m_session = nil;
    }
    if(m_device)
    {
        //  [m_device release];
        m_device = nil;
    }
    
    
    if(m_preview)
    {
        for (UIView *view in m_preview.subviews)
        {
            [view removeFromSuperview];
        }
    }
    
    return 0;
}


- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    
}

@end
