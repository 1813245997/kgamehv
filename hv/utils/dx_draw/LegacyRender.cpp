#include "../global_defs.h"
#include "LegacyRender.h"


bool g_pagehit[0x10000] = { };
bool g_pagevaild[0x10000] = { };

float roundf(float a) {
	int ia = (int)a;
	float xs = a - (float)ia;
	if (xs >= .5f) {
		return a + 1.f;
	}
	return a;
}

float floorf(float a) {
	int ia = (int)a;
	float xs = a - (float)ia;
	if (xs > 0.f) {
		return a + 1.f;
	}
	return a;
}

__forceinline FColor::FColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
{
	RGBA[0] = B;
	RGBA[1] = G;
	RGBA[2] = R;
	RGBA[3] = A;
}

__forceinline FColor::FColor(uint32_t val)
{
	*(uint32_t*)&RGBA = val;
}

__forceinline uint32_t FColor::Get() const
{
	return *(uint32_t*)&RGBA;
}


int  ByteRender::OddNumber(int val) {
	return (val % 2) ? val : val + 1;
}


int ByteRender::EvenNumber(int val) {
	return (val % 2) ? val + 1 : val;
}

FColor ByteRender::GetPixel(int x, int y) {
	return FColor(Arr[(y * w2) + x]);
}


FColor  ByteRender::ColorBlend(FColor prev, FColor cur)
{
	//pixels rgba to float
	int ca = cur.RGBA[3];
	int pa = prev.RGBA[3];
	float added[] = {
	static_cast<float>(cur.RGBA[0]),
	static_cast<float>(cur.RGBA[1]),
	static_cast<float>(cur.RGBA[2]),
	static_cast<float>(ca) / 255.f
	};

	float base[] = {
		static_cast<float>(prev.RGBA[0]),
		static_cast<float>(prev.RGBA[1]),
		static_cast<float>(prev.RGBA[2]),
		static_cast<float>(pa) / 255.f
	};

	//mix colors
	float mix[4];
	mix[3] = 1 - (1 - added[3]) * (1 - base[3]); // alpha
	mix[0] = roundf((added[0] * added[3] / mix[3]) + (base[0] * base[3] * (1 - added[3]) / mix[3])); // red
	mix[1] = roundf((added[1] * added[3] / mix[3]) + (base[1] * base[3] * (1 - added[3]) / mix[3])); // green
	mix[2] = roundf((added[2] * added[3] / mix[3]) + (base[2] * base[3] * (1 - added[3]) / mix[3])); // blue

	//set new pixel color
	return FColor(mix[0], mix[1], mix[2], (mix[3] * 255.f));
}


void ByteRender::SetPixel(int x, int y, FColor color, int br  )
{
	if ((x >= w2) || (y >= h2))
		return;
	if (x < 0 || y < 0)
		return;
	//need fix color
	//
	//
	//
	//
	//
	//
	//
	//
	//
	//

	//apply pixels
	PVOID ppix = &Arr[(y * w2) + x];

	int pageidx = ((ULONG_PTR)ppix - (ULONG_PTR)Arr) / 0x1000;
	if (pageidx < 0x10000) {
		if (g_pagehit[pageidx] == 0) {
			int edgeidx = (((h2 * w2) + w2 - 1) * 4) / 0x1000;
			if (pageidx > edgeidx)
				return;
			PVOID prevpix = 0;
			PVOID nextpix = 0;
			if (pageidx == 0) {
				prevpix = ppix;
				nextpix = (PVOID)((ULONG_PTR)ppix + 0x1000);
			}
			else if (pageidx == edgeidx) {
				prevpix = (PVOID)((ULONG_PTR)ppix - 0x1000);
				nextpix = ppix;
			}
			else {
				prevpix = (PVOID)((ULONG_PTR)ppix - 0x1000);
				nextpix = (PVOID)((ULONG_PTR)ppix + 0x1000);
			}
			g_pagevaild[pageidx] = (utils::memory::get_physical_address(reinterpret_cast<unsigned long long> (ppix)) == 0 ||
				utils::memory::get_physical_address(reinterpret_cast<unsigned long long>(prevpix)) == 0 ||
				utils::memory::get_physical_address(reinterpret_cast<unsigned long long>(nextpix)) == 0) ? false : true;
			g_pagehit[pageidx] = 1;
		}


		//if (MmiGetPhysicalAddress(ppix)) {
		if (g_pagevaild[pageidx]) {
			if (br)
			{
				//apply brightness
				//color.RGBA[3] = 255 - br;
				auto prevPx = GetPixel(x, y);

				//need color blend
				if (prevPx.RGBA[3])
					color = ColorBlend(prevPx, color);
			}
			Arr[(y * w2) + x] = color.Get();
		}

		//}
	}


}


  void  ByteRender::plotLineWidth(int x0, int y0, int x1, int y1, FColor clr, float th)
{                              /* plot an anti-aliased line of width th pixel */
	auto dx = abs(x1 - x0), sx = x0 < x1 ? (float)1 : (float)-1;
	auto dy = abs(y1 - y0), sy = y0 < y1 ? (float)1 : (float)-1;
	float err, e2 = sqrt(dx * dx + dy * dy);                            /* length */

	//if (th <= 1 || e2 == 0) return plotLineAA(x0, y0, x1, y1, clr);    /* assert */
	dx *= 255 / e2; dy *= 255 / e2; th = 255 * (th - 1);               /* scale values */

	if (dx < dy) {                                               /* steep line */
		x1 = (int)roundf((e2 + th / 2) / dy);                          /* start offset */
		err = x1 * dy - th / 2;                  /* shift error value to offset width */
		for (x0 -= x1 * sx; ; y0 += sy) {
			SetPixel(x1 = x0, y0, clr, err);                  /* aliasing pre-pixel */
			for (e2 = dy - err - th; e2 + dy < 255; e2 += dy)
				SetPixel(x1 += sx, y0, clr);                      /* pixel on the line */
			SetPixel(x1 + sx, y0, clr, e2);                    /* aliasing post-pixel */
			if (y0 == y1) break;
			err += dx;                                                 /* y-step */
			if (err > 255) { err -= dy; x0 += sx; }                    /* x-step */
		}
	}
	else {                                                      /* flat line */
		y1 = (int)roundf((e2 + th / 2) / dx);                          /* start offset */
		err = y1 * dx - th / 2;                  /* shift error value to offset width */
		for (y0 -= y1 * sy; ; x0 += sx) {
			SetPixel(x0, y1 = y0, clr, err);                  /* aliasing pre-pixel */
			for (e2 = dx - err - th; e2 + dx < 255; e2 += dx)
				SetPixel(x0, y1 += sy, clr);                      /* pixel on the line */
			SetPixel(x0, y1 + sy, clr, e2);                    /* aliasing post-pixel */
			if (x0 == x1) break;
			err += dy;                                                 /* x-step */
			if (err > 255) { err -= dx; y0 += sy; }                    /* y-step */
		}
	}
}


void  ByteRender::plotEllipseRectWidth(int x0, int y0, int x1, int y1, float th, FColor clr)
{               /* draw anti-aliased ellipse inside rectangle with thick line */
	float a = abs(x1 - x0);
	int b = abs(y1 - y0);
	float b1 = b & 1;  /* outer diameter */
	auto a2 = a - 2 * th, b2 = b - 2 * th;                            /* inner diameter */
	auto dx = 4 * (a - 1) * b * b, dy = 4 * (b1 - 1) * a * a;                /* error increment */
	float i = a + b2, err = b1 * a * a, dx2, dy2, e2, ed;
	/* thick line correction */
	//if (th < 1.5) return plotEllipseRectAA(x0, y0, x1, y1);
	if ((th - 1) * (2 * b - th) > a * a) b2 = sqrt(a * (b - a) * i * a2) / (a - th);
	if ((th - 1) * (2 * a - th) > b * b) { a2 = sqrt(b * (a - b) * i * b2) / (b - th); th = (a - a2) / 2; }
	//if (a == 0 || b == 0) return plotLineAA(x0, y0, x1, y1, clr);
	if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
	if (y0 > y1) y0 = y1;                                  /* .. exchange them */
	if (b2 <= 0) th = a;                                     /* filled ellipse */
	e2 = th - floorf(th); th = x0 + th - e2;
	dx2 = 4 * (a2 + 2 * e2 - 1) * b2 * b2; dy2 = 4 * (b1 - 1) * a2 * a2; e2 = dx2 * e2;
	y0 += (b + 1) >> 1; y1 = y0 - b1;                              /* starting pixel */
	a = 8 * a * a; b1 = 8 * b * b; a2 = 8 * a2 * a2; b2 = 8 * b2 * b2;

	do {
		for (;;) {
			if (err < 0 || x0 > x1) { i = x0; break; }
			i = min(dx, dy); ed = max(dx, dy);
			if (y0 == y1 + 1 && 2 * err > dx && a > b1) ed = a / 4;           /* x-tip */
			else ed += 2 * ed * i * i / (4 * ed * ed + i * i + 1) + 1;/* approx ed=sqrt(dx*dx+dy*dy) */
			i = 255 * err / ed;                             /* outside anti-aliasing */
			SetPixel(x0, y0, clr, i);
			SetPixel(x0, y1, clr, i);
			SetPixel(x1, y0, clr, i);
			SetPixel(x1, y1, clr, i);
			if (err + dy + a < dx) { i = x0 + 1; break; }
			x0++; x1--; err -= dx; dx -= b1;                /* x error increment */
		}

		for (; i < th && 2 * i <= x0 + x1; i++) {                /* fill line pixel */
			SetPixel(i, y0, clr);
			SetPixel(x0 + x1 - i, y0, clr);
			SetPixel(i, y1, clr);
			SetPixel(x0 + x1 - i, y1, clr);
		}

		while (e2 > 0 && x0 + x1 >= 2 * th) {               /* inside anti-aliasing */
			i = min(dx2, dy2); ed = max(dx2, dy2);
			if (y0 == y1 + 1 && 2 * e2 > dx2 && a2 > b2) ed = a2 / 4;         /* x-tip */
			else  ed += 2 * ed * i * i / (4 * ed * ed + i * i);                 /* approximation */
			i = 255 - 255 * e2 / ed;             /* get intensity value by pixel error */
			SetPixel(th, y0, clr, i);
			SetPixel(x0 + x1 - th, y0, clr, i);
			SetPixel(th, y1, clr, i);
			SetPixel(x0 + x1 - th, y1, clr, i);
			if (e2 + dy2 + a2 < dx2) break;
			th++; e2 -= dx2; dx2 -= b2;                     /* x error increment */
		}
		e2 += dy2 += a2;
		y0++; y1--; err += dy += a;                                   /* y step */
	} while (x0 < x1);

}


int  ByteRender::DrawChar(class Font* f, const Vector2& Start, wchar_t ch, const FColor& Color) {

	CFontInfo* chfont = f->GetChar(ch);
	if (chfont) {
	/*	int w = chfont->Width;
		int h = chfont->Height;*/
		int top = chfont->Top;
		int sz = (float)chfont->Size / 1.3f;
		unsigned char* pstroke = chfont->bitmap + chfont->bitmap_sz * 3;
		for (int i = 0; i < chfont->stroke_sz; i++) {
			unsigned char* pc = (unsigned char*)pstroke + i * 3;
			int x = pc[0];
			int y = pc[1];
			int a = pc[2];
			FColor col = PM_BLACK;
			col.RGBA[3] = static_cast<uint8_t>(a);
			SetPixel(Start.x + x - 1, Start.y + y - top + sz - 1, col, 1);
		}

		for (int i = 0; i < chfont->bitmap_sz; i++) {
			unsigned char* pc = (unsigned char*)chfont->bitmap + i * 3;
			int x = pc[0];
			int y = pc[1];
			int a = pc[2];
			FColor col = Color;
			col.RGBA[3] = static_cast<uint8_t>(a);
			SetPixel(Start.x + x, Start.y + y - top + sz, col, 1);
		}

		return chfont->Width + chfont->Left;
	}
	return 0;
}

void ByteRender::String(class Font* f, const Vector2& Start, LPCWSTR str, const FColor& Color  ) {

	size_t len = utils::string_utils::get_wide_string_length(str);
	int xoff = 0;
	for (size_t i = 0; i < len; i++) {
		if (str[i] == (WCHAR)L' ') {
			xoff += f->GetChar(L'9')->Width;
			continue;
		}
		CFontInfo* chfont = f->GetChar(str[i]);
		if (chfont) {
			if (i != 0) {
				xoff += chfont->Left;
			}

			Vector2 pos = Start;
			pos.x += xoff;
			DrawChar(f, pos, str[i], Color);
			xoff += chfont->Width + 1;
			//xoff += f->GetChar(L'M')->Width;
		}

	}

}

void ByteRender::StringA(class Font* f, const Vector2& Start, LPCSTR str, const FColor& Color  ) {
	if (!str) return;

	// 计算长度（最好 utf-8 实际转码后再判断）
	size_t len = strlen(str);
	if (len == 0) return;

	// UTF-8 到 Unicode，最大一个 utf-8 字符可能转成一个 WCHAR（保守估计）
	WCHAR* wstr = (WCHAR*)utils::internal_functions::pfn_ex_allocate_pool_with_tag (NonPagedPool, (len + 1) * sizeof(WCHAR), 'rtsW');   
	if (!wstr) return;

	RtlZeroMemory(wstr, (len + 1) * sizeof(WCHAR));

	utils::string_utils::utf8_to_unicode(str, wstr, len);   
	String(f, Start, wstr, Color);

	utils::internal_functions::pfn_ex_free_pool_with_tag (wstr, 'rtsW');
}

void ByteRender::StringA(class Font* f, float x, float y, LPCSTR str, const FColor& Color)
{
	Vector2 start = { x, y };
	StringA(f, start, str, Color);
}
int ByteRender::StringWidth(class Font* f, const wchar_t* str)
{
	size_t len = utils::string_utils::get_wide_string_length(str);
	int xoff = 0;
	for (size_t i = 0; i < len; i++) {
		if (str[i] == ' ') {
			xoff += f->GetChar(L'M')->Width;
			continue;
		}
		CFontInfo* chfont = f->GetChar(str[i]);
		if (chfont) {
			if (i != 0) {
				xoff += chfont->Left;
			}
			xoff += chfont->Width + 1;
		}

	}
	return xoff;
}

int ByteRender::CharHeight(class Font* f) {
	auto fp = f->GetChar(L'M');
	if (fp)
		return fp->Height;
	return 0;
}

  void ByteRender:: Line(const Vector2& Start, const Vector2& End, const FColor& Color, int Thickness  ) {

	plotLineWidth(Start.x, Start.y, End.x, End.y, Color, Thickness);
}



  void  ByteRender::Rectangle(float x, float y, float w, float h, const FColor& Color, int thickness  )
  {


	  int fix = thickness / 2;
	  thickness = OddNumber(thickness);

	  align = false;

	  plotLineWidth(x - fix, y, x + w + fix, y, Color, thickness);
	  plotLineWidth(x, y, x, y + h, Color, thickness);
	  plotLineWidth(x + w, y, x + w, y + h, Color, thickness);
	  plotLineWidth(x - fix, y + h, x + w + fix, y + h, Color, thickness);

	  align = true;
  }



  void ByteRender::FillRectangle(const Vector2& arg_Start, const Vector2& arg_Size, const FColor& Color) {
	  Rectangle(arg_Start.x, arg_Start.y, arg_Size.x, arg_Start.y, Color);
	  return;
  }
  //render circle
  void ByteRender::Circle(const Vector2& Start, const FColor& Color, int Radius, int Thickness  ) {
	  auto fix = (EvenNumber(Radius) + Thickness) / 2;
	  plotEllipseRectWidth(Start.x - fix, Start.y - fix, Start.x + fix, Start.y + fix, OddNumber(Thickness), Color);
  }

  void ByteRender::DrawCircle(float center_x, float center_y, int radius, const FColor& color, int Thickness ) {
	  // 将浮点数半径和厚度向上取整用于绘制边界框
	  float half_extent = (EvenNumber(radius) + Thickness) / 2;

	  float left = center_x - half_extent;
	  float top = center_y - half_extent;
	  float right = center_x + half_extent;
	  float bottom = center_y + half_extent;


	  plotEllipseRectWidth(left, top, right, bottom, Thickness, color);
  }

  void ByteRender::FillCircle(const Vector2& Start, const FColor& Color, int Radius) {
	  auto fix = EvenNumber(Radius) / 2;
	  plotEllipseRectWidth(Start.x - fix, Start.y - fix, Start.x + fix, Start.y + fix, (float)fix, Color);
  }

    void ByteRender::LineY(int x, int y, int d, FColor col) {
	  if (d < 0) {
		  d = -d;
		  y -= d;
	  }
	  for (int i = 0; i < d + 1; i++) {
		  SetPixel(x, y + i, col, 0);
	  }
  }
    void ByteRender::LineX(int x, int y, int d, FColor col) {
	  if (d < 0) {
		  d = -d;
		  x -= d;
	  }
	  for (int i = 0; i < d + 1; i++) {
		  SetPixel(x + i, y, col, 0);
	  }
  }