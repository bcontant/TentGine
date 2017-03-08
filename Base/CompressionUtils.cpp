#include "precompiled.h"

#include "Buffer.h"

//--------------------------------------------------------------------------------
bool RLE_Decode(Buffer* in_SrcBuffer, Buffer* out_DstBuffer, unsigned int in_uiSizeToDecode, unsigned char in_ucElementSize)
{
	unsigned int bytesRead = 0;
	static unsigned char value[4];
	unsigned char runCount = 0;

	//Loop for the whole image
	while (bytesRead < in_uiSizeToDecode)
	{
		//MSB == 0 : Raw data   MSB == 1 : RLE data
		in_SrcBuffer->Read(&runCount, 1);
		if (runCount & 0x80)
		{
			runCount -= 127;
			in_SrcBuffer->Read(value, in_ucElementSize);
			for (unsigned int i = 0; i < runCount; i++)
			{
				out_DstBuffer->Write(value, in_ucElementSize);
			}
		}
		else
		{
			++runCount;
			in_SrcBuffer->Read(out_DstBuffer, runCount * in_ucElementSize);
		}
		bytesRead += runCount * in_ucElementSize;
	}
	Assert(bytesRead == in_uiSizeToDecode);
	return (bytesRead == in_uiSizeToDecode);
}

//--------------------------------------------------------------------------------
bool RLE_Encode(Buffer* in_SrcBuffer, Buffer* out_DstBuffer, unsigned char in_ucElementSize)
{
	unsigned int writtenBytes = 0;
	unsigned char consecutiveRunSize = 1;
	unsigned char rawRunSize = 0;
	unsigned int currentRunIndex = 0;

	while (writtenBytes < in_SrcBuffer->Size())
	{
		while (consecutiveRunSize < 128 && rawRunSize < 128 && writtenBytes + currentRunIndex < in_SrcBuffer->Size())
		{
			if (writtenBytes + currentRunIndex + in_ucElementSize < in_SrcBuffer->Size() &&
				memcmp(in_SrcBuffer->PeekAt(writtenBytes + currentRunIndex), in_SrcBuffer->PeekAt(writtenBytes + currentRunIndex + in_ucElementSize), in_ucElementSize) == 0 )
			{
				consecutiveRunSize++;
			}
			else
			{
				//if (consecutiveRunSize >= 3)
				if (consecutiveRunSize >= 2)
				{
					break;
				}

				rawRunSize++;
				if (consecutiveRunSize > 1)
					rawRunSize++;

				consecutiveRunSize = 1;
			}
			currentRunIndex += in_ucElementSize;
		}

		//Write the rawRun if any
		if (rawRunSize > 0)
		{
			Assert(rawRunSize > 0);
			unsigned char runSize = rawRunSize - 1;
			out_DstBuffer->Write(&runSize, 1);
			out_DstBuffer->Write(in_SrcBuffer, rawRunSize * in_ucElementSize);
			writtenBytes += rawRunSize * in_ucElementSize;
		}

		//Write the consecutiveRun if any
		if (consecutiveRunSize >= 2)
		{
			unsigned char runSize = consecutiveRunSize - 1;
			runSize |= 0x80;
			out_DstBuffer->Write(&runSize, 1);
			out_DstBuffer->Write(in_SrcBuffer, in_ucElementSize);
			in_SrcBuffer->Advance((consecutiveRunSize - 1) * in_ucElementSize);
			writtenBytes += consecutiveRunSize * in_ucElementSize;
		}

		rawRunSize = 0;
		consecutiveRunSize = 1;
		currentRunIndex = 0;
	}

	return true;
}
