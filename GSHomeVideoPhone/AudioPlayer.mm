

#import "AudioPlayer.h"
#include <unistd.h>

static UInt32 gBufferSizeBytes = 2048;
int   gAudioFrame = 2048;


@implementation AudioPlayer

@synthesize queue;

// 回调（Callback）函数的实现

static void BufferCallback(void *inUserData, AudioQueueRef inAQ,
						   
						   AudioQueueBufferRef buffer) {
	
	AudioPlayer *play = (__bridge AudioPlayer*)inUserData;
	[play audioQueueOutputWithQueue:inAQ queueBuffer:buffer];
	
}

//初始化方法（为NSObject中定义的初始化方法）

- (id) init
{
    return self;
}

//缓存数据读取方法的实现

- (void) audioQueueOutputWithQueue:(AudioQueueRef)audioQueue  queueBuffer:(AudioQueueBufferRef)audioQueueBuffer
{
    
    BUFNODE *node = m_bufque->pop_node();
    if(node != NULL)
    {
        memcpy(audioQueueBuffer->mAudioData, node->pbuf, gAudioFrame);
        m_bufque->free_node(node);
    }
    else
    {
        memset(audioQueueBuffer->mAudioData, 0, gAudioFrame);
    }
    
	audioQueueBuffer->mAudioDataByteSize = gAudioFrame;
	AudioQueueEnqueueBuffer(audioQueue, audioQueueBuffer,0, NULL);
	 				
}

- (void) stop
{
	AudioQueueStop(queue, TRUE);
    if(m_bufque != NULL)
    {
        m_bufque->destroy_bufque();
        delete m_bufque;
        m_bufque = NULL;
    }
}
//音频播放方法的实现

-(void) start
{
    m_bufque = new cbufque;
    m_bufque->create_bufque(gAudioFrame,10);
    

    // 取得音频数据格式
	dataFormat.mFormatID = kAudioFormatLinearPCM;
	dataFormat.mFormatFlags =  kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
	dataFormat.mSampleRate = 44100;
	dataFormat.mFramesPerPacket = 1;
	dataFormat.mChannelsPerFrame = 1;
	dataFormat.mBitsPerChannel = 16;
	dataFormat.mBytesPerFrame = dataFormat.mChannelsPerFrame * sizeof(SInt16);
	dataFormat.mBytesPerPacket = dataFormat.mBytesPerFrame * dataFormat.mFramesPerPacket;
	
    // 创建播放用的音频队列
    AudioQueueNewOutput(&dataFormat, BufferCallback,(__bridge void*)self, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &queue);
	

    for (int i = 0; i < NUM_BUFFERS; i++)
    {
        AudioQueueAllocateBuffer(queue, gBufferSizeBytes, &buffers[i]);
		buffers[i]->mAudioDataByteSize = gAudioFrame;
        AudioQueueEnqueueBuffer(queue, buffers[i],0, NULL);
    }

    Float32 gain = 1.0;
    //设置音量
    AudioQueueSetParameter (queue,kAudioQueueParam_Volume,gain);
    //队列处理开始，此后系统会自动调用回调（Callback）函数
    AudioQueueStart(queue, nil);
	
}

- (void) play:(char*)data length:(int)len
{
    if(len != gAudioFrame || data == NULL)
    {
        return ;
    }
    if(m_bufque != NULL)
    {
        BUFNODE *node = m_bufque->alloc_node();
        if(node != NULL)
        {
            memcpy(node->pbuf, data, gAudioFrame);
            m_bufque->push_node(node);
        }
    }
}



@end
