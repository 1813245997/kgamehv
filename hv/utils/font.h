#pragma once
#define MAX_CHAR_COUNT (256)
#define MAX_CHAR_VALUE (128)

struct CFontInfo {
	UINT Char;
	int Size;
	int Left;
	int Top;
	int Width;
	int Height;
	unsigned char* bitmap;
	int bitmap_sz;
	int stroke_sz;
};
class Font {
private:
	ULONG g_MaxSize = 0;
	CFontInfo g_FontList[MAX_CHAR_COUNT];
	USHORT g_FontIdxList[MAX_CHAR_VALUE];
	float g_ScaleFactor = 1.0f;  // 字体缩放因子
public:
	BOOL InitFont(const BYTE* file_base, ULONG file_size) {
		UNREFERENCED_PARAMETER(file_size);
		RtlZeroMemory(g_FontList, sizeof(g_FontList));
		RtlZeroMemory(g_FontIdxList, sizeof(g_FontIdxList));
		struct FileHeader {
			ULONG tag;
			ULONG Count;
		};
		struct CharFileInfo {
			wchar_t Char;
			int size;
			int bitmap_left;
			int bitmap_top;
			int bitmap_width;
			int bitmap_height;
			int bitmap_sz;
			int stroke_sz;
		};

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

	CFontInfo* GetChar(USHORT ch) {
		if (ch >= MAX_CHAR_VALUE)
			return 0;
		USHORT idx = g_FontIdxList[ch];

		if (idx != 0)
			return &g_FontList[idx];
		return 0;
	}

	// 设置字体缩放因子 (1.0f = 原始大小, 2.0f = 2倍大小, 0.5f = 一半大小)
	void SetScale(float scale) {
		if (scale > 0.1f && scale <= 5.0f) {  // 限制缩放范围，避免过小导致不清晰
			g_ScaleFactor = scale;
		} else if (scale <= 0.1f) {
			g_ScaleFactor = 0.1f;  // 最小缩放限制
		} else if (scale > 5.0f) {
			g_ScaleFactor = 5.0f;  // 最大缩放限制
		}
	}

	// 获取当前缩放因子
	float GetScale() const {
		return g_ScaleFactor;
	}

	// 获取缩放后的字体大小
	int GetScaledSize() const {
		return static_cast<int>(g_MaxSize * g_ScaleFactor);
	}

	// 获取缩放后的字符宽度
	int GetScaledCharWidth(USHORT ch) const {
		CFontInfo* chfont = const_cast<Font*>(this)->GetChar(ch);
		if (chfont) {
			return static_cast<int>((chfont->Width + chfont->Left) * g_ScaleFactor);
		}
		return 0;
	}

	// 获取缩放后的字符高度
	int GetScaledCharHeight(USHORT ch) const {
		CFontInfo* chfont = const_cast<Font*>(this)->GetChar(ch);
		if (chfont) {
			return static_cast<int>(chfont->Height * g_ScaleFactor);
		}
		return 0;
	}
};
