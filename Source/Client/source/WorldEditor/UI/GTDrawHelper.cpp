// GTDrawHelper.cpp: implementation of the GTDrawHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "GTColorDialog.h"
#include "GTDrawHelper.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GTDrawHelper::GTDrawHelper()
{
}

GTDrawHelper::~GTDrawHelper()
{
}

void GTDrawHelper::DrawPanel (HDC hdc, int left, int top, int right, int bottom, int flags)
{
	HPEN	highlight_pen, shadow_pen, old_pen;
	HBRUSH	old_brush;

	shadow_pen = CreatePen (PS_SOLID, 0, GetSysColor (COLOR_BTNSHADOW));
	highlight_pen = CreatePen (PS_SOLID, 0, GetSysColor (COLOR_BTNHILIGHT));

	// old pen is preserved here
	if (flags & sunken)
	{
		old_pen = (HPEN) SelectObject (hdc, shadow_pen);
	}
	else
	{
		old_pen = (HPEN) SelectObject (hdc, highlight_pen);
	}

	// draw left and top side
	MoveToEx (hdc, left, bottom, NULL);
	LineTo (hdc, left, top);
	LineTo (hdc, right + 1, top);

	if (flags & sunken)
	{
		SelectObject (hdc, highlight_pen);
	}
	else
	{
		SelectObject (hdc, shadow_pen);
	}

	// bottom and right side
	MoveToEx (hdc, left + 1, bottom, NULL);
	LineTo (hdc, right, bottom);
	LineTo (hdc, right, top);

	SelectObject (hdc, GetStockObject (BLACK_PEN));

	if (flags & blackbox)
	{
		old_brush = (HBRUSH) SelectObject (hdc, GetStockObject (NULL_BRUSH));
		Rectangle (hdc, left + 1, top + 1, right, bottom);
		SelectObject (hdc, old_brush);
	}

	// restore the old pen here
	SelectObject (hdc, old_pen);

	DeleteObject (shadow_pen);
	DeleteObject (highlight_pen);
}

void GTDrawHelper::DrawTriangle (HDC hdc, int x, int y, enum types type, int size, COLORREF line_color, COLORREF fill_color)
{
	POINT	pt[3];								// array of points used for polygon
	HPEN	stroke_pen, old_pen;
	HBRUSH	fill_brush, old_brush;

	size --;
	// fill the array with points according to triangle mode
	pt[0].x = x;
	pt[0].y = y;
	switch (type)
	{
		case left:
			pt[1].x = x + size;
			pt[1].y = y + size;
			pt[2].x = x + size;
			pt[2].y = y - size;
			break;

		case top:
			pt[1].x = x + size;
			pt[1].y = y + size;
			pt[2].x = x - size;
			pt[2].y = y + size;
			break;

		case right:
			pt[1].x = x - size;
			pt[1].y = y - size;
			pt[2].x = x - size;
			pt[2].y = y + size;
			break;

		case bottom:
			pt[1].x = x - size;
			pt[1].y = y - size;
			pt[2].x = x + size;
			pt[2].y = y - size;
			break;

		case left_top:
			pt[1].x = x + size;
			pt[1].y = y;
			pt[2].x = x;
			pt[2].y = y + size;
			break;

		case left_bottom:
			pt[1].x = x + size;
			pt[1].y = y;
			pt[2].x = x;
			pt[2].y = y - size;
			break;

		case right_top:
			pt[1].x = x - size;
			pt[1].y = y;
			pt[2].x = x;
			pt[2].y = y + size;
			break;

		case right_bottom:
			pt[1].x = x - size;
			pt[1].y = y;
			pt[2].x = x;
			pt[2].y = y - size;
			break;

		default:
			// unknown triangle orientation
			// debug_message("base::draw_triangle: unknown type %d", type);
			ASSERT (FALSE);
			return;
	}

	stroke_pen = CreatePen (PS_SOLID, 0, line_color);
	fill_brush = CreateSolidBrush (fill_color);

	old_pen = (HPEN) SelectObject (hdc, stroke_pen);
	old_brush = (HBRUSH) SelectObject (hdc, fill_brush);

	Polygon (hdc, pt, 3);

	SelectObject (hdc, old_pen);
	SelectObject (hdc, old_brush);

	DeleteObject (stroke_pen);
	DeleteObject (fill_brush);
}

int GTDrawHelper::HSV2RGB (double hue, double sat, double value, double* red, double* green, double* blue)
{
	double	frac, coef1, coef2, coef3;
	double	intp;
	// hsv values valid?
	if (sat < 0.0 || sat > 1.0 || value < 0.0 || value > 1.0)
	{
		return (-1);
	}
	if (hue < 0.0 || hue > 360.0)
	{
		return (-1);
	}

	// gray?
	if (sat == 0.0)
	{
		*red = *green = *blue = value;
	}
	else
	{
		// hue(chromatic) 360 == hue 0
		if (hue == 360.0)
		{
			hue = 0;
		}
		hue = hue / 60; 						// hue in [0, 6)
		frac = modf (hue, &intp);				// split hue to integer and fraction
		coef1 = value * (1 - sat);
		coef2 = value * (1 - sat * frac);
		coef3 = value * (1 - sat * (1 - frac));
		switch ((int) intp)
		{
			case 0:
				*red = value;
				*green = coef3;
				*blue = coef1;
				break;
			case 1:
				*red = coef2;
				*green = value;
				*blue = coef1;
				break;
			case 2:
				*red = coef1;
				*green = value;
				*blue = coef3;
				break;
			case 3:
				*red = coef1;
				*green = coef2;
				*blue = value;
				break;
			case 4:
				*red = coef3;
				*green = coef1;
				*blue = value;
				break;
			case 5:
				*red = value;
				*green = coef1;
				*blue = coef2;
				break;
		}
	}
	return (0);
}

COLORREF GTDrawHelper::HSV2RGB (double hue, double sat, double value)
{
	double	red, green, blue;

	// ceil(a - 0.5) = round(a) = floor(a + 0.5)
	if (HSV2RGB (hue, sat, value, &red, &green, &blue) == 0)
		return (RGB (
					ceil (red * 255 - 0.5),
					ceil (green * 255 - 0.5),
					ceil (blue * 255 - 0.5)));
	return (RGB (0, 0, 0));
}

int GTDrawHelper::RGB2HSV (double red, double green, double blue, double* hue, double* sat, double* value)
{
	double max, min, delta;

	max = MAX (red, MAX (green, blue));
	min = MIN (red, MIN (green, blue));

	// check the rgb values to see if valid
	if (min < 0.0 || max > 1.0)
	{
		return (-1);    // out of range
	}

	*value = max;								// calculate the value v

	if (max > 0.0)
	{
		*sat = (max - min) / max;
	}
	else
		// red = green = blue = 0
	{
		*sat = 0.0;
	}

	if (*sat == 0.0)
	{
		*hue = 0.0;
	}
	else
	{
		delta = max - min;
		if (red == max)
			// between yellow and magenta
		{
			*hue = (green - blue) / delta;
		}
		else if (green  == max)
			// between cyan and yellow
		{
			*hue = 2 + (blue - red) / delta;
		}
		else
			// between magenta and cyan
		{
			*hue = 4 + (red - green) / delta;
		}

		// hue to degrees
		*hue = *hue * 60;
		if (*hue < 0)
			// make hue > 0
		{
			*hue = *hue + 360;
		}
	}

	ASSERT (*hue >= 0.0 && *hue <= 360.0);
	ASSERT (*sat >= 0.0 && *sat <= 1.0);
	ASSERT (*value >= 0.0 && *value <= 1.0);

	return (0);
}

void GTDrawHelper::GetRGB (DWORD *buffer, int samples, COLORREF start, COLORREF end)
{
	int	red, green, blue;
	int	red_adv, green_adv, blue_adv;

	red = scaled_red (start);
	green = scaled_green (start);
	blue = scaled_blue (start);
	red_adv = (scaled_red (end) - red) / (samples - 1);
	green_adv = (scaled_green (end) - green) / (samples - 1);
	blue_adv = (scaled_blue (end) - blue) / (samples - 1);

	while (samples--)
	{
		// set current pixel(in DIB bitmap format is BGR, not RGB!)
		*buffer++ = RGB (
						(BYTE) (blue >> int_extend),
						(BYTE) (green >> int_extend),
						(BYTE) (red >> int_extend));
		// advance color values to the next pixel
		red += red_adv;
		green += green_adv;
		blue += blue_adv;
	}
}


void GTDrawHelper::HSV_HUE (DWORD *buffer, int samples, double sat, double val_fp)
{
	// value, but as integer in [0, 255 << int_extend]
	int		val;

	// loop counter
	int		j;

	// coefficients and advances
	int		coef1, coef2, coef3;
	int		coef2_adv, coef3_adv;

	// current position and advance to the next one
	double	pos, pos_adv;

	//
	// hue increments in [0, 360); indirectly
	//	intp changes - 0, 1, 2, 3, 4, 5; indirectly(separate loops)
	//	frac increments in [0, 1) six times; indirectly(coefficients)
	// sat - const, in [0, 1]
	// val - const, in [0,(255 << int_extend)]
	//
	// coef1 => val *(1 - sat)              => const, = val *(1 - sat)
	// coef2 => val *(1 - sat * frac)       => changes from val to val *(1 - sat)
	// coef3 => val *(1 - sat *(1 - frac)) => changes from val *(1 - sat) to val
	//

	// constants
	val = (int) (val_fp * 255) << int_extend;
	coef1 = (int) (val * (1 - sat));

	// prepare
	pos = 0;
	pos_adv = (double) samples / 6.0;

	// hue in [0, 60)
	pos += pos_adv;
	j = (int) pos;
	HSV_SAT_INIT_0();
	while (j--)
	{
		*buffer++ = HSV_0(), HSV_SAT_ADV_0();
	}

	pos += pos_adv;
	j = (int) pos - (int) (1 * pos_adv);
	HSV_SAT_INIT_1();
	while (j--)
	{
		*buffer++ = HSV_1(), HSV_SAT_ADV_1();
	}

	pos += pos_adv;
	j = (int) pos - (int) (2 * pos_adv);
	HSV_SAT_INIT_2();
	while (j--)
	{
		*buffer++ = HSV_2(), HSV_SAT_ADV_2();
	}

	pos += pos_adv;
	j = (int) pos - (int) (3 * pos_adv);
	HSV_SAT_INIT_3();
	while (j--)
	{
		*buffer++ = HSV_3(), HSV_SAT_ADV_3();
	}

	pos += pos_adv;
	j = (int) pos - (int) (4 * pos_adv);
	HSV_SAT_INIT_4();
	while (j--)
	{
		*buffer++ = HSV_4(), HSV_SAT_ADV_4();
	}

	pos += (pos_adv + 0.1);	// + 0.1 because of floating-point math's rounding errors
	j = (int) pos - (int) (5 * pos_adv);
	HSV_SAT_INIT_5();
	while (j--)
	{
		*buffer++ = HSV_5(), HSV_SAT_ADV_5();
	}
}


void GTDrawHelper::HSV_SAT (DWORD *buffer, int samples, double hue, double val_fp)
{
	// value, but as integer in [0, 255 << int_extend]
	int		val;

	// loop counter
	int		j;

	// coefficients and advances
	signed int		coef1, coef2, coef3;
	signed int		coef1_adv, coef2_adv, coef3_adv;

	double	intp, frac;

	//
	// hue - const, in [0, 360)
	//	intp - const in 0, 1, 2, 3, 4, 5
	//	frac - const in [0, 1)
	// sat - increments, in [0, 1]; indirectly(coefficients)
	// val - const, in [0,(255 << int_extend)]
	//
	// coef1 => val *(1 - sat)              => changes from val to 0
	// coef2 => val *(1 - sat * frac)       => changes from val to val *(1 - frac)
	// coef3 => val *(1 - sat *(1 - frac)) => changes from val to val * frac
	//

	// constants
	val = (int) (val_fp * 255) << int_extend;
	frac = modf (hue / 60.0, &intp);

	// prepare
	j = samples;

	coef1 = val;
	coef1_adv = -coef1 / samples;
	coef2 = val;
	coef2_adv = (int) ((1 - frac) * val - coef2) / samples;
	coef3 = val;
	coef3_adv = (int) (frac * val - coef3) / samples;

	switch ((int) intp)
	{
		case	0:
			while (j--)
			{
				*buffer++ = HSV_0(), HSV_HUE_ADV_0();
			}
			break;
		case	1:
			while (j--)
			{
				*buffer++ = HSV_1(), HSV_HUE_ADV_1();
			}
			break;
		case	2:
			while (j--)
			{
				*buffer++ = HSV_2(), HSV_HUE_ADV_2();
			}
			break;
		case	3:
			while (j--)
			{
				*buffer++ = HSV_3(), HSV_HUE_ADV_3();
			}
			break;
		case	4:
			while (j--)
			{
				*buffer++ = HSV_4(), HSV_HUE_ADV_4();
			}
			break;
		case	5:
			while (j--)
			{
				*buffer++ = HSV_5(), HSV_HUE_ADV_5();
			}
			break;
	}
}

void GTDrawHelper::HSV_VAL (DWORD *buffer, int samples, double hue, double sat)
{
	// loop counter
	int		j;

	// coefficients and advances
	signed int		coef1, coef2, coef3;
	signed int		coef1_adv, coef2_adv, coef3_adv;

	int		val, val_adv;
	int		val_max;

	double	intp, frac;

	//
	// hue - const, in [0, 360)
	//	intp - const in 0, 1, 2, 3, 4, 5
	//	frac - const in [0, 1)
	// sat - const, in [0, 1]
	// val - increments, in [0,(255 << int_extend)]; indirectly(coefficients)
	//
	// coef1 => val *(1 - sat)              => changes from 0 to val *(1 - sat)
	// coef2 => val *(1 - sat * frac)       => changes from 0 to val *(1 - sat * frac)
	// coef3 => val *(1 - sat *(1 - frac)) => changes from 0 to val *(1 - sat *(1 - frac))
	//

	// constants
	frac = modf (hue / 60.0, &intp);
	val_max = 255 << int_extend;

	// prepare
	j = samples;

	coef1 = 0;
	coef1_adv = (int) (val_max * (1 - sat)) / samples;
	coef2 = 0;
	coef2_adv = (int) (val_max * (1 - sat * frac)) / samples;
	coef3 = 0;
	coef3_adv = (int) (val_max * (1 - sat * (1 - frac))) / samples;
	val = 0;
	val_adv = val_max / samples;

	switch ((int) intp)
	{
		case	0:
			while (j--)
			{
				*buffer++ = HSV_0(), HSV_HUE_ADV_0(), val += val_adv;
			}
			break;
		case	1:
			while (j--)
			{
				*buffer++ = HSV_1(), HSV_HUE_ADV_1(), val += val_adv;
			}
			break;
		case	2:
			while (j--)
			{
				*buffer++ = HSV_2(), HSV_HUE_ADV_2(), val += val_adv;
			}
			break;
		case	3:
			while (j--)
			{
				*buffer++ = HSV_3(), HSV_HUE_ADV_3(), val += val_adv;
			}
			break;
		case	4:
			while (j--)
			{
				*buffer++ = HSV_4(), HSV_HUE_ADV_4(), val += val_adv;
			}
			break;
		case	5:
			while (j--)
			{
				*buffer++ = HSV_5(), HSV_HUE_ADV_5(), val += val_adv;
			}
			break;
	}
}

void GTDrawHelper::set (unsigned long* buffer, unsigned long value, size_t count)
{
	while (count--)
	{
		*buffer++ = value;
	}
}

void GTDrawHelper::set (unsigned long** buffer, unsigned long value, size_t count)
{
	unsigned long*	p = *buffer;

	while (count--)
	{
		*p++ = value;
	}
	*buffer = p;
}

void GTDrawHelper::copy (unsigned long* target, const unsigned long* source, size_t count)
{
	while (count--)
	{
		*target++ = *source++;
	}
}

void GTDrawHelper::copy (unsigned long** target, const unsigned long* source, size_t count)
{
	unsigned long*	p = *target;

	while (count--)
	{
		*p++ = *source++;
	}
	*target = p;
}

void GTDrawHelper::copy_reverse (unsigned long* target, const unsigned long* source, size_t count)
{
	source += (count - 1);
	while (count--)
	{
		*target++ = *source--;
	}
}

void GTDrawHelper::reverse (unsigned long* buffer, size_t count)
{
	unsigned long*	p = buffer + count - 1;

	while (buffer < p)
	{
		*buffer ^= *p;
		*p ^= *buffer;
		*buffer++ ^= *p--;
	}
}
