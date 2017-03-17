#include "precompiled.h"

#include "Buffer.h"

//--------------------------------------------------------------------------------
bool RLE_Decode(Buffer* in_SrcBuffer, Buffer* out_DstBuffer, u32 in_uiSizeToDecode, u8 in_ucElementSize)
{
	u32 bytesRead = 0;
	static u8 value[4];
	u8 runCount = 0;

	//Loop for the whole image
	while (bytesRead < in_uiSizeToDecode)
	{
		//MSB == 0 : Raw data   MSB == 1 : RLE data
		in_SrcBuffer->Read(&runCount, 1);
		if (runCount & 0x80)
		{
			runCount -= 127;
			in_SrcBuffer->Read(value, in_ucElementSize);
			for (u32 i = 0; i < runCount; i++)
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
bool RLE_Encode(Buffer* in_SrcBuffer, Buffer* out_DstBuffer, u8 in_ucElementSize)
{
	u32 writtenBytes = 0;
	u8 consecutiveRunSize = 1;
	u8 rawRunSize = 0;
	u32 currentRunIndex = 0;

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
			u8 runSize = rawRunSize - 1;
			out_DstBuffer->Write(&runSize, 1);
			out_DstBuffer->Write(in_SrcBuffer, rawRunSize * in_ucElementSize);
			writtenBytes += rawRunSize * in_ucElementSize;
		}

		//Write the consecutiveRun if any
		if (consecutiveRunSize >= 2)
		{
			u8 runSize = consecutiveRunSize - 1;
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
