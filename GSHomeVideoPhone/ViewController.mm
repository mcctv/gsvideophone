//
//  ViewController.m
//  GSHomeVideoPhone
//
//  Created by mcctv on 14-5-13.
//  Copyright (c) 2014年 ___FULLUSERNAME___. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

@synthesize m_imageView;
@synthesize m_btnSend;
@synthesize m_btnRequest;
@synthesize m_textSend;
@synthesize m_textRequest;
@synthesize m_labelStatusSend;
@synthesize m_labelStatusRequest;
@synthesize m_viewPreview;


void video_decoder_cb(void *puser, int w, int h, char *data);
void audio_decoder_cb(void *puser,  char* data, int len);
void audio_recoder_cb(void *pUser, char *data, int len);

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    UITapGestureRecognizer *tapGr = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewTapped:)];
    tapGr.cancelsTouchesInView = NO;
    [self.view addGestureRecognizer:tapGr];
    
   
    
    
    m_mgr = new GSHomeManager;
    [m_textRequest setText:@"250"];
    [m_textSend setText:@"250"];
    [m_labelStatusRequest setText:@"上传关闭"];
    [m_labelStatusSend setText:@"请求关闭"];
}
- (void) viewWillDisappear:(BOOL)animated
{
    if(m_mgr != NULL)
    {
        m_mgr->CloseVideoDecoder();
    }
    
    [m_labelStatusRequest setText:@"上传关闭"];
    [m_labelStatusSend setText:@"请求关闭"];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(IBAction)sendBtnPressed:(id)sender
{
    
    ////////////////test
    /*
    NSArray *doc = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES) ;
    NSString *strDocument = [doc objectAtIndex:0];
    NSString *strFile = [strDocument stringByAppendingPathComponent:@"recorder.pcm"];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if ( ![fileManager fileExistsAtPath:strFile])
    {
        [fileManager createFileAtPath:strFile contents:nil attributes:nil];
    }

    
    m_fPCM = fopen([strFile UTF8String], "r");
    */
    ///////////////end test
    int iRet = 0;
    
    int  num = [[m_textRequest text] intValue];
    if(m_mgr != NULL)
    {
        m_mgr->CloseAudioEncoder();
        iRet = m_mgr->OpenAudioEncoder(num, "192.168.1.199", 2000);
        if(iRet ==0)
        {
            [m_labelStatusSend setText:@"请求打开成功"];
        }
        
        if(m_audioRecoder != NULL)
        {
            [m_audioRecoder StopRecord];
        }
        m_audioRecoder = [[AudioRecorder alloc] init];
        [m_audioRecoder StartRecord:&audio_recoder_cb userData:(__bridge void*)self];
    }
 
//    if (m_videoCapture)
////    {
 //       [m_videoCapture stopVideoCapture];
 //       m_videoCapture = NULL;
//    }
    m_videoCapture = [[VideoCapture alloc] init];
  //  [m_videoCapture startVideoCapture: m_viewPreview];

    
}
-(IBAction)requestBtnPressed:(id)sender
{
    int iRet = 0;
    
    int  num = [[m_textRequest text] intValue];
    
    if(m_mgr != NULL)
    {
        m_mgr->CloseVideoDecoder();
        iRet = m_mgr->OpenVideoDecoder((__bridge void*)self, &video_decoder_cb, 1000000+num, "192.168.1.199", 2002);
        if(iRet ==0)
        {
            [m_labelStatusRequest setText:@"请求打开成功"];
        }
    }
    
    if(m_audioPlayer != NULL)
    {
        [m_audioPlayer stop];
    }
    m_audioPlayer = [[AudioPlayer alloc]init];
    [m_audioPlayer start];
    
    
    if(m_mgr != NULL)
    {
        m_mgr->CloseAudioDecoder();
        iRet = m_mgr->OpenAudioDecoder((__bridge void*)self, &audio_decoder_cb, num, "192.168.1.199", 2002);
        if(iRet ==0)
        {
            [m_labelStatusRequest setText:@"请求打开成功"];
        }
    }

}

-(IBAction)stopBtnPressed:(id)sender
{
    /*
    if (m_videoCapture)
    {
        [m_videoCapture stopVideoCapture];
        m_videoCapture = NULL;
    }
    */
    if(m_audioRecoder != NULL)
    {
        [m_audioRecoder StopRecord];
        m_audioRecoder = NULL;
    }
    
    if(m_mgr != NULL)
    {
        m_mgr->CloseVideoDecoder();
        m_mgr->CloseAudioDecoder();
        m_mgr->CloseAudioEncoder();
    }
    if(m_audioPlayer != NULL)
    {
        [m_audioPlayer stop];
        m_audioPlayer = NULL;
    }
    
    
    [m_labelStatusRequest setText:@"请求关闭"];
    [m_labelStatusSend setText:@"上传关闭"];
    
}

-(void)RGBtoIMage:(char*)buf imgwide:(NSInteger)mwide imghigh:(NSInteger)mhigh
{
	
	Byte *buffer=(Byte*)buf;
    
	CGRect sourceRect =CGRectMake(0, 0,mwide, mhigh);
	
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL,
															  buffer,
															  sourceRect.size.width*sourceRect.size.height*3,
															  NULL);
	CGImageRef iref = CGImageCreate(sourceRect.size.width,
									sourceRect.size.height,
									8,
									24,
									sourceRect.size.width*3,
									CGColorSpaceCreateDeviceRGB(),
									kCGBitmapByteOrderDefault,
									provider,
									NULL,
									NO,
									kCGRenderingIntentDefault);
	UIImage *myImage = [[UIImage  alloc]initWithCGImage:iref];
	[m_imageView performSelectorOnMainThread:@selector(setImage:) withObject:myImage waitUntilDone:FALSE];
	[m_imageView performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:FALSE];
    
	CGImageRelease(iref);
	CGDataProviderRelease(provider);
}

-(void)viewTapped:(UITapGestureRecognizer*)tapGr
{
    CGRect rc = [self.view frame];
    rc.origin.x = 0;
    
    
    [UIView setAnimationWillStartSelector: nil];
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration: 0.3];
    [self.view setFrame:rc];
    
    [UIView setAnimationDidStopSelector: nil];
    [UIView setAnimationDelegate: self];
    [UIView commitAnimations];

    
    [m_textSend    resignFirstResponder];
    [m_textRequest resignFirstResponder];
    
    
}



-(IBAction)moveWindow
{
    CGRect rc = [self.view frame];
    rc.origin.x = -100;
    
    
	[UIView setAnimationWillStartSelector: nil];
	[UIView beginAnimations:nil context:NULL];
	[UIView setAnimationDuration: 0.3];
	[self.view setFrame:rc];
    
	[UIView setAnimationDidStopSelector: nil];
	[UIView setAnimationDelegate: self];
	[UIView commitAnimations];
}

-(IBAction)hidekeybord:(id)sender
{
	UITextField *textView=(UITextField *)sender;
    CGRect rc = [self.view frame];
    rc.origin.x = 0;

    
    [UIView setAnimationWillStartSelector: nil];
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration: 0.3];
    [self.view setFrame:rc];
    
    [UIView setAnimationDidStopSelector: nil];
    [UIView setAnimationDelegate: self];
    [UIView commitAnimations];
    	
	[textView resignFirstResponder];
}

-(void)PlayAudio:(char*)data length:(int)len
{
    [m_audioPlayer play:data length:len];
}

-(void)EncodeAudio:(char*)data length:(int)len
{
    if(m_mgr != NULL)
    {
        /*
        if (m_fPCM != NULL)
        {
            fread(data, 1, len, m_fPCM);
        }
         */
        m_mgr->AduioEncoder(data, len);
    }
}

void video_decoder_cb(void *pUser, int w, int h, char *data)
{
     ViewController *sel = (__bridge ViewController*)pUser;
  //   NSLog(@"video_decoder_cb \n");
    [sel RGBtoIMage:data imgwide:w imghigh:h];
}

void audio_decoder_cb(void *puser,  char* data, int len)
{
    ViewController *sel = (__bridge ViewController*)puser;
    [sel PlayAudio:data length:len];
  //  NSLog(@"audio_decoder_cb \n");
}
void audio_recoder_cb(void *pUser, char *data, int len)
{
    ViewController *sel = (__bridge ViewController*)pUser;
    [sel EncodeAudio:data length:len];
    
}


@end
