#pragma once

class Path;
class BitmapData;

bool SavePNG(const Path& in_file, BitmapData* in_pData);
BitmapData* LoadPNG(const Path& in_file);
