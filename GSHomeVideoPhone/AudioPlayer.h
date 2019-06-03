

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/AudioFile.h>
#include <iostream>
#include "cbufque.h"
using namespace std;

#define NUM_BUFFERS 3

@interface AudioPlayer : NSObject
{
    AudioStreamBasicDescription dataFormat;
    AudioQueueRef queue;
    AudioStreamPacketDescription *packetDescs;
    AudioQueueBufferRef buffers[NUM_BUFFERS];
	
    cbufque *m_bufque;
    

	
}

//定义队列为实例属性

@property AudioQueueRef queue;

//播放方法定义

- (void) start;
- (void) stop;
- (void) play:(char*)data length:(int)len;

//定义缓存数据读取方法

- (void) audioQueueOutputWithQueue:(AudioQueueRef)audioQueue

                       queueBuffer:(AudioQueueBufferRef)audioQueueBuffer;

//定义回调（Callback）函数

static void BufferCallback(void *inUserData, AudioQueueRef inAQ,
						   
						   AudioQueueBufferRef buffer);


@end
