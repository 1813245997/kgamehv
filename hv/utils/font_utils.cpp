#include "global_defs.h"
#include "font_utils.h"

// Font class implementation

BOOL Font::InitFont(const BYTE* file_base, ULONG file_size)
{
	UNREFERENCED_PARAMETER(file_size);
	RtlZeroMemory(g_FontList, sizeof(g_FontList));
	RtlZeroMemory(g_FontIdxList, sizeof(g_FontIdxList));


	FileHeader* header = (FileHeader*)file_base;
	//if (header->tag != 'JH') {
	//	return FALSE;
	//}
	ULONG bitmaps_size = 0;
	CharFileInfo* info = (CharFileInfo*)(file_base + sizeof(FileHeader));
	g_MaxSize = info->size;
	for (ULONG i = 0; i < header->Count; i++) {
		ULONG size = info->bitmap_sz * 3 + info->stroke_sz * 3;
		bitmaps_size += size;
		info = (CharFileInfo*)(((PUCHAR)info) + sizeof(CharFileInfo) + size);
	}
	info = (CharFileInfo*)(file_base + sizeof(FileHeader));
	int fidx = 1;
	for (ULONG i = 0; i < header->Count; i++) {
		ULONG size = info->bitmap_sz * 3 + info->stroke_sz * 3;
		CFontInfo Glyph;
		memset(&Glyph, 0, sizeof(CFontInfo));
		if ((int)info->Char >= MAX_CHAR_VALUE)
			goto loop_cont;
		Glyph.Char = info->Char;
		Glyph.Size = info->size;
		Glyph.Height = info->bitmap_height;
		Glyph.Width = info->bitmap_width;
		Glyph.Left = info->bitmap_left;
		Glyph.Top = info->bitmap_top;
		Glyph.bitmap = (BYTE*)(info + 1);
		Glyph.bitmap_sz = info->bitmap_sz;
		Glyph.stroke_sz = info->stroke_sz;

		g_FontIdxList[Glyph.Char] = static_cast<USHORT> (fidx);
		g_FontList[fidx] = Glyph;
		fidx++;
		if (fidx >= MAX_CHAR_COUNT)
			break;
	loop_cont:
		info = (CharFileInfo*)(((PUCHAR)info) + sizeof(CharFileInfo) + size);
	}
	return TRUE;
}

CFontInfo* Font::GetChar(USHORT ch)
{
    if (ch >= MAX_CHAR_VALUE)
        return nullptr;
    
    USHORT idx = g_FontIdxList[ch];
    
    if (idx != 0)
        return &g_FontList[idx];
    
    return nullptr;
}

void Font::SetScale(float scale)
{
    if (scale > 0.1f && scale <= 5.0f) {
        g_ScaleFactor = scale;
    } else if (scale <= 0.1f) {
        g_ScaleFactor = 0.1f;   // Minimum scale limit
    } else if (scale > 5.0f) {
        g_ScaleFactor = 5.0f;    // Maximum scale limit
    }
}

float Font::GetScale() const
{
    return g_ScaleFactor;
}

int Font::GetScaledSize() const
{
    return static_cast<int>(g_MaxSize * g_ScaleFactor);
}

int Font::GetScaledCharWidth(USHORT ch) const
{
    CFontInfo* chfont = const_cast<Font*>(this)->GetChar(ch);
    if (chfont) {
        return static_cast<int>((chfont->Width + chfont->Left) * g_ScaleFactor);
    }
    return 0;
}

int Font::GetScaledCharHeight(USHORT ch) const
{
    CFontInfo* chfont = const_cast<Font*>(this)->GetChar(ch);
    if (chfont) {
        return static_cast<int>(chfont->Height * g_ScaleFactor);
    }
    return 0;
}