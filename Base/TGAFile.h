#pragma once

class BitmapData;

void SaveTGA(const Path& in_file, BitmapData* in_pData);
BitmapData* LoadTGA(const Path& in_file);