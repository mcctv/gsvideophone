

#import "AudioRecorder.h"
#include <sys/socket.h>



static void RecordCallback(
						   void *inUserData, 
						   AudioQueueRef inAQ,
						   AudioQueueBufferRef buffer,
						   const AudioTimeStamp *inStartTime,
						   UInt32 inNumPackets,
						   const AudioStreamPacketDescription *inPacketDesc
						   )
{
	AudioRecorder * record = (__bridge AudioRecorder*)inUserData;
	[record audioQueueOutputWithQueue:inAQ queueBuffer:buffer];
}

@implementation AudioRecorder

- (void) audioQueueOutputWithQueue:(AudioQueueRef)audioQueue

                       queueBuffer:(AudioQueueBufferRef)audioQueueBuffer 
{


//	memcpy (pbuf+23,audioQueueBuffer->mAudioData,160);


    m_cb(m_pUser, (char*)(audioQueueBuffer->mAudioData), audioQueueBuffer->mAudioDataByteSize);
	AudioQueueEnqueueBuffer(audioQueue, audioQueueBuffer, 0, NULL);
	
	
}
	
	
int ComputeRecordBufferSize(const AudioStreamBasicDescription *format, float seconds ,AudioQueueRef mQueue)
{
	int packets, frames, bytes = 0;

		frames = (int)ceil(seconds * format->mSampleRate);
		
		if (format->mBytesPerFrame > 0)
			bytes = frames * format->mBytesPerFrame;
		else {
			UInt32 maxPacketSize;
			if (format->mBytesPerPacket > 0)
				maxPacketSize = format->mBytesPerPacket;	// constant packet size
			else {
				UInt32 propertySize = sizeof(maxPacketSize);
				AudioQueueGetProperty(mQueue, kAudioQueueProperty_MaximumOutputPacketSize, &maxPacketSize,
													&propertySize);
			}
			if (format->mFramesPerPacket > 0)
				packets = frames / format->mFramesPerPacket;
			else
				packets = frames;	// worst-case scenario: 1 frame in a packet
			if (packets == 0)		// sanity check
				packets = 1;
			bytes = packets * maxPacketSize;
		}
	
	return bytes;
}

-(void) StartRecord:(AUDIO_RECODER_CB)cb userData:(void*)puser;
{
    m_pUser = puser;
    m_cb  = cb;

	int i, bufferByteSize;
	UInt32 size;
	
	mRecordFormat.mFormatID = kAudioFormatLinearPCM ;
    mRecordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    
	mRecordFormat.mSampleRate = 44100;
	mRecordFormat.mChannelsPerFrame = 1;
	mRecordFormat.mFramesPerPacket = 1;
	mRecordFormat.mBitsPerChannel = 16;
	mRecordFormat.mBytesPerFrame = mRecordFormat.mChannelsPerFrame * sizeof(SInt16);
	
	mRecordFormat.mBytesPerPacket = mRecordFormat.mBytesPerFrame = (mRecordFormat.mBitsPerChannel / 8) * mRecordFormat.mChannelsPerFrame;
		
	AudioQueueNewInput(
					   &mRecordFormat,
					   RecordCallback,
					   (__bridge void*)self /* userData */,
					   CFRunLoopGetCurrent() /* run loop */, 
					   kCFRunLoopCommonModes /* run loop mode */,
					   0 /* flags */,
					   &mQueue
					   );
	
	size = sizeof(mRecordFormat);
	AudioQueueGetProperty(mQueue, kAudioQueueProperty_StreamDescription,	
						  &mRecordFormat, &size);
	
	
	
	bufferByteSize = ComputeRecordBufferSize(&mRecordFormat, kBufferDurationSeconds ,mQueue);	// enough bytes for half a second
	for (i = 0; i < kNumberRecordBuffers; ++i) {
		AudioQueueAllocateBuffer(mQueue, bufferByteSize, &mBuffers[i]);
		AudioQueueEnqueueBuffer(mQueue, mBuffers[i], 0, NULL);
	}	
	AudioQueueStart(mQueue, NULL);	
}

-(void) StopRecord
{
	//AudioQueueStop(mQueue, FALSE);
	AudioQueueDispose(mQueue, TRUE);
	
}
@end