#pragma once

class Buffer;

//--------------------------------------------------------------------------------
bool RLE_Decode(Buffer* in_SrcBuffer, Buffer* out_DstBuffer, unsigned int in_uiSizeToDecode, unsigned char in_ucElementSize);
bool RLE_Encode(Buffer* in_SrcBuffer, Buffer* out_DstBuffer, unsigned char in_ucElementSize);


