#pragma once

class BitmapData;

void SaveTGA(const Path& in_file, BitmapData* in_pData, bool in_bCompress = false);
BitmapData* LoadTGA(const Path& in_file);

// Dump out the raw image data (in PBM format).
//Bug : in a PBM, 0 is white and 1 is black (wtf)
/*std::ofstream out_mono("F:\\out-mono.pbm", std::ios::binary);
out_mono << "P4 " << face->glyph->bitmap.width << " " << face->glyph->bitmap.rows << "\n";
unsigned char* pBuf = face->glyph->bitmap.buffer;
for(int k = 0; k < face->glyph->bitmap.rows; k++)
{
out_mono.write((const char *)pBuf, face->glyph->bitmap.width / 8 ? face->glyph->bitmap.width / 8 : 1);
pBuf += face->glyph->bitmap.pitch;
}*/