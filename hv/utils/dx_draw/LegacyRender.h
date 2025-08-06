#pragma once
#include "math.h"
#include "font.h"
#include "pmcol.h"


extern  bool g_pagehit[0x10000] ;
extern  bool g_pagevaild[0x10000] ;
float roundf(float a);

float floorf(float a);

class FColor
{
public:
	uint8_t RGBA[4];
	__forceinline FColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255);
	__forceinline FColor(uint32_t val);

	__forceinline uint32_t Get() const;
	 
	 
};
class ByteRender
{
private:
	int w, h;
	bool align;
	uint32_t* Arr;

	int OddNumber(int val);

	int EvenNumber(int val);

	FColor GetPixel(int x, int y);

	FColor ColorBlend(FColor prev, FColor cur);
	
	void SetPixel(int x, int y, FColor color, int br = 0);
	 
	//line
	void plotLineWidth(int x0, int y0, int x1, int y1, FColor clr, float th);

	void plotEllipseRectWidth(int x0, int y0, int x1, int y1, float th, FColor clr);
 
public:
	template<typename T>
	__forceinline void Setup(int Width, int Height, T pixelsArr) {
		w = Width; h = Height;
		Arr = (uint32_t*)pixelsArr;
		align = true;
	}

	int DrawChar(class Font* f, const Vector2& Start, wchar_t ch, const FColor& Color);
	
	void String(class Font* f, const Vector2& Start, LPCWSTR str, const FColor& Color = FColor(255, 255, 255));

	void StringA(class Font* f, const Vector2& Start, LPCSTR str, const FColor& Color = FColor(255, 255, 255));
	void StringA(class Font* f, float x, float y, LPCSTR str, const FColor& Color);

	int StringWidth(class Font* f, const wchar_t* str);

	int CharHeight(class Font* f);

	void Line(const Vector2& Start, const Vector2& End, const FColor& Color, int Thickness = 1);

	void Rectangle(float x, float y, float w, float h, const FColor& Color, int thickness = 1);

	void FillRectangle(const Vector2& arg_Start, const Vector2& arg_Size, const FColor& Color);

	void Circle(const Vector2& Start, const FColor& Color, int Radius, int Thickness = 1);

	void DrawCircle(float center_x, float center_y, int radius, const FColor& color, int Thickness = 1);

	void FillCircle(const Vector2& Start, const FColor& Color, int Radius);

	 void LineY(int x, int y, int d, FColor col);

	 void LineX(int x, int y, int d, FColor col);

};