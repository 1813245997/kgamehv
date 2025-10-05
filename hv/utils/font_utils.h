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

class Font {
private:
	ULONG g_MaxSize = 0;
	CFontInfo g_FontList[MAX_CHAR_COUNT];
	USHORT g_FontIdxList[MAX_CHAR_VALUE];
	float g_ScaleFactor = 1.0f;  
	
public:
	// Font initialization
	BOOL InitFont(const BYTE* file_base, ULONG file_size);

	// Character access
	CFontInfo* GetChar(USHORT ch);

	// Scale management
	void SetScale(float scale);
	float GetScale() const;

	// Scaled dimensions
	int GetScaledSize() const;
	int GetScaledCharWidth(USHORT ch) const;
	int GetScaledCharHeight(USHORT ch) const;
};
