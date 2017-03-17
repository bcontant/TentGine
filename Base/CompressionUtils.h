#pragma once

class Buffer;

//--------------------------------------------------------------------------------
bool RLE_Decode(Buffer* in_SrcBuffer, Buffer* out_DstBuffer, u32 in_uiSizeToDecode, u8 in_ucElementSize);
bool RLE_Encode(Buffer* in_SrcBuffer, Buffer* out_DstBuffer, u8 in_ucElementSize);


