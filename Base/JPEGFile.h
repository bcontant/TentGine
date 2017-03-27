#pragma once

class Path;
class BitmapData;

bool SaveJPEG(const Path& in_file, BitmapData* in_pData, s32 in_Quality = 100);
BitmapData* LoadJPEG(const Path& in_file);
