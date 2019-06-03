

#import <Foundation/Foundation.h>

#import <AudioToolbox/AudioQueue.h>
#import <AudioToolbox/AudioServices.h>
#include <AudioToolbox/AudioToolbox.h>

#include <iostream>

using namespace std;

#define kBufferDurationSeconds .02
#define kNumberRecordBuffers	3


typedef void (*AUDIO_RECODER_CB)(void *p, char* data, int len);

@interface AudioRecorder : NSObject {

	AudioQueueRef				mQueue;
	AudioQueueBufferRef			mBuffers[kNumberRecordBuffers];
	AudioStreamBasicDescription mRecordFormat;
    
    void            *m_pUser;
    AUDIO_RECODER_CB m_cb;
}


-(void) StartRecord:(AUDIO_RECODER_CB)cb userData:(void*)puser;
-(void) StopRecord;



static void RecordCallback(
						   void *inUserData, 
						   AudioQueueRef inAQ,
						   AudioQueueBufferRef buffer,
						   const AudioTimeStamp *inStartTime,
						   UInt32 inNumPackets,
						   const AudioStreamPacketDescription *inPacketDesc
						   );

- (void) audioQueueOutputWithQueue:(AudioQueueRef)audioQueue

                       queueBuffer:(AudioQueueBufferRef)audioQueueBuffer ;
	
	
@end


