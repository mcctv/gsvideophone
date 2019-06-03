//
//  ViewController.h
//  GSHomeVideoPhone
//
//  Created by mcctv on 14-5-13.
//  Copyright (c) 2014年 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "GSHomeManager.h"

#import "AudioPlayer.h"
#import "AudioRecorder.h"
#import "VideoCapture.h"


@interface ViewController : UIViewController
{
    IBOutlet UIView      *m_viewPreview;
    IBOutlet UIImageView *m_imageView;
    IBOutlet UIButton    *m_btnSend;
    IBOutlet UIButton    *m_btnRequest;
    IBOutlet UITextField *m_textSend;
    IBOutlet UITextField *m_textRequest;
    IBOutlet UILabel     *m_labelStatusSend;
    IBOutlet UILabel     *m_labelStatusRequest;
    
    GSHomeManager  *m_mgr;
    AudioPlayer    *m_audioPlayer;
    AudioRecorder  *m_audioRecoder;
    VideoCapture   *m_videoCapture;
  
    
    FILE           *m_fPCM;
    

}
@property(retain, nonatomic) UIView      *m_viewPreview;
@property(retain, nonatomic) UIImageView *m_imageView;
@property(retain, nonatomic) UIButton    *m_btnSend;
@property(retain, nonatomic) UIButton    *m_btnRequest;
@property(retain, nonatomic) UITextField *m_textSend;
@property(retain, nonatomic) UITextField *m_textRequest;
@property(retain, nonatomic) UILabel     *m_labelStatusSend;
@property(retain, nonatomic) UILabel     *m_labelStatusRequest;


-(IBAction)stopBtnPressed:(id)sender;
-(IBAction)requestBtnPressed:(id)sender;
-(IBAction)sendBtnPressed:(id)sender;

-(void)viewTapped:(UITapGestureRecognizer*)tapGr;
-(void)viewWillDisappear:(BOOL)animated;
-(IBAction)moveWindow;
-(IBAction)hidekeybord:(id)sender;

-(void)RGBtoIMage:(char*)buf imgwide:(NSInteger)mwide imghigh:(NSInteger)mhigh;
-(void)PlayAudio:(char*)data length:(int)len;
-(void)EncodeAudio:(char*)data length:(int)len;
//call back
void audio_recoder_cb(void *pUser, char *data, int len);
void video_decoder_cb(void *pUser, int w, int h, char *data);
void audio_decoder_cb(void *puser,  char* data, int len);


@end
