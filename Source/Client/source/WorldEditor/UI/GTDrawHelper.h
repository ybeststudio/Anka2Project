// GTDrawHelper.h: interface for the GTDrawHelper class.
//
//////////////////////////////////////////////////////////////////////
/*
	Modified 7/15/2002 by David Swigger (dwswigger@yahoo.com)

    these functions were not static before...allowed me to do
	GTDrawHelper:: without instantiating a class...

	static int	HSV2RGB (double hue,double sat,double value,double *red,double *green,double *blue);
	static int	RGB2HSV(double red,double green,double blue,double *hue,double *sat,double *value);
	static COLORREF HSV2RGB(double hue,double sat,double value);
*/
#if !defined(AFX_GTDRAWHELPER_H__F818AFD1_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
#define AFX_GTDRAWHELPER_H__F818AFD1_4DE0_11D6_A56D_525400EA266C__INCLUDED_

#pragma once

#include "math.h"

#if	!defined (MIN)
	#define	MIN	min
#endif

#if	!defined (MAX)
	#define	MAX	max
#endif

#define WM_FO_SELECTBULLETTYPEOK				WM_USER+270 // Select arrow ok.

#define	HSV_LOOP_STEPS(w)	((w) - 1)

#define	HSV_0()	RGB ((BYTE) (coef1 >> int_extend),(BYTE) (coef3 >> int_extend),(BYTE) (val >> int_extend))

#define	HSV_HUE_ADV_0() coef1 += coef1_adv,coef3 += coef3_adv

#define	HSV_1()	RGB ((BYTE) (coef1 >> int_extend),(BYTE) (val >> int_extend),(BYTE) (coef2 >> int_extend))

#define	HSV_HUE_ADV_1()	coef1 += coef1_adv,	coef2 += coef2_adv


#define	HSV_2()	RGB ((BYTE) (coef3 >> int_extend),(BYTE) (val >> int_extend),(BYTE) (coef1 >> int_extend))

#define	HSV_HUE_ADV_2()		HSV_HUE_ADV_0()

#define	HSV_3()	RGB ((BYTE) (val >> int_extend),(BYTE) (coef2 >> int_extend),(BYTE) (coef1 >> int_extend))

#define	HSV_HUE_ADV_3()		HSV_HUE_ADV_1()

#define	HSV_4()	RGB ((BYTE) (val >> int_extend),(BYTE) (coef1 >> int_extend),(BYTE) (coef3 >> int_extend))

#define	HSV_HUE_ADV_4()		HSV_HUE_ADV_0()

#define	HSV_5()	 RGB (	(BYTE) (coef2 >> int_extend),(BYTE) (coef1 >> int_extend),(BYTE) (val >> int_extend))

#define	HSV_HUE_ADV_5()		HSV_HUE_ADV_1()

// initialize for HSV colorspace in SAT mode, for HUE between 0 and 1 (0 and 60 deg)
#define	HSV_SAT_INIT_0()	coef3 = coef1,	coef3_adv = (int) ((val - coef3) / HSV_LOOP_STEPS (j))

// advance for HSV colorspace in SAT mode, for HUE between 0 and 1 (0 and 60 deg)
#define	HSV_SAT_ADV_0()	    coef3 += coef3_adv

#define	HSV_SAT_INIT_1()	coef2 = val, coef2_adv = (int) ((val * (1.0 - sat) - coef2) / HSV_LOOP_STEPS (j))

#define	HSV_SAT_ADV_1()	    coef2 += coef2_adv

#define	HSV_SAT_INIT_2()	HSV_SAT_INIT_0()
#define	HSV_SAT_ADV_2()		HSV_SAT_ADV_0()

#define	HSV_SAT_INIT_3()	HSV_SAT_INIT_1()
#define	HSV_SAT_ADV_3()		HSV_SAT_ADV_1()

#define	HSV_SAT_INIT_4()	HSV_SAT_INIT_0()
#define	HSV_SAT_ADV_4()		HSV_SAT_ADV_0()

#define	HSV_SAT_INIT_5()	HSV_SAT_INIT_1()
#define	HSV_SAT_ADV_5()		HSV_SAT_ADV_1()

// for HSV colorspace, VAL mode is calculate in a same manner as SAT mode
//	so all macroses simply maps over SAT mode macroses
#define	HSV_VAL_INIT_0()	HSV_SAT_INIT_0()
#define	HSV_VAL_ADV_0()		HSV_SAT_ADV_0()

#define	HSV_VAL_INIT_1()	HSV_SAT_INIT_1()
#define	HSV_VAL_ADV_1()		HSV_SAT_ADV_1()

#define	HSV_VAL_INIT_2()	HSV_SAT_INIT_2()
#define	HSV_VAL_ADV_2()		HSV_SAT_ADV_2()

#define	HSV_VAL_INIT_3()	HSV_SAT_INIT_3()
#define	HSV_VAL_ADV_3()		HSV_SAT_ADV_3()

#define	HSV_VAL_INIT_4()	HSV_SAT_INIT_4()
#define	HSV_VAL_ADV_4()		HSV_SAT_ADV_4()

const int	int_extend = 20;

enum modes_visual
{
	modes_rgb_red = 0,
	modes_rgb_green = 1,
	modes_rgb_blue = 2,
	modes_hsv_hue = 3,
	modes_hsv_sat = 4,
	modes_hsv_value = 5,
	modes_max,
	modes_mask = 0x7F,
	modes_reverse = 0x80
};

// coefficients to scale color components with to calculate real component value
enum scale_values
{
	scale_rgb = 1,
	scale_rgb_red = scale_rgb,
	scale_rgb_green = scale_rgb,
	scale_rgb_blue = scale_rgb,
	scale_hsv_hue = 10,
	scale_hsv_sat = 1000,
	scale_hsv_value = 1000
};

enum max_values
{
	max_rgb = 255,
	max_rgb_red = max_rgb,
	max_rgb_green = max_rgb,
	max_rgb_blue = max_rgb,
	max_hsv_hue = 3599,
	max_hsv_sat = 1000,
	max_hsv_value = 1000
};

class GTDrawHelper
{
	public:
		GTDrawHelper();
		virtual ~GTDrawHelper();

		// draw_panel flags bit mask
		enum flags
		{
			raised = 1,
			sunken = 2,
			blackbox = 4
		};

		enum types
		{
			left = 1,
			top = 2,
			right = 3,
			bottom = 4,
			left_top = 5,
			left_bottom = 6,
			right_top = 7,
			right_bottom = 8
		};

		// Draw panel border.
		virtual void DrawPanel (HDC hdc, int left, int top, int right, int bottom, int flags);

		// Draw panel border.
		virtual void DrawPanel (HDC hdc, LPRECT rc, int flags)
		{
			DrawPanel (hdc, rc->left, rc->top, rc->right, rc->bottom, flags);
		};

		// Draw triangle.
		virtual void DrawTriangle (HDC hdc, int x, int y, enum types type, int size, COLORREF line_color, COLORREF fill_color);

		// Convert from hsv to rgb value.
		static int	HSV2RGB (double hue, double sat, double value, double* red, double* green, double* blue);

		// Convert from rgb to hsv value.
		static int	RGB2HSV (double red, double green, double blue, double* hue, double* sat, double* value);

		// Convert from hsv to rgb value.
		static COLORREF HSV2RGB (double hue, double sat, double value);

		// Get rgb value.
		void GetRGB (DWORD *buffer, int samples, COLORREF start, COLORREF end);

		// hsv from hue.
		void HSV_HUE (DWORD *buffer, int samples, double sat, double val);

		// hsv from sat.
		void HSV_SAT (DWORD *buffer, int samples, double hue, double val);

		// hsv value.
		void HSV_VAL (DWORD *buffer, int samples, double hue, double sat);

		// Set
		void set (unsigned long* buffer, unsigned long value, size_t count);

		//
		void set (unsigned long** buffer, unsigned long value, size_t count);

		//
		void copy (unsigned long* target, const unsigned long* source, size_t count);

		//
		void copy (unsigned long** target, const unsigned long* source, size_t count);

		//
		void copy_reverse (unsigned long* target, const unsigned long* source, size_t count);

		//
		void reverse (unsigned long* buffer, size_t count);

};

inline int scaled_red (COLORREF c)
{
	return (GetRValue (c) << int_extend);
}

inline int scaled_green (COLORREF c)
{
	return (GetGValue (c) << int_extend);
}

inline int scaled_blue (COLORREF c)
{
	return (GetBValue (c) << int_extend);
}

template <class T, class T1> void in_range (T& x, T1 start, T1 end)
{
	if (x < static_cast <T> (start))
	{
		x = static_cast <T> (start);
	}
	if (x > static_cast <T> (end))
	{
		x = static_cast <T> (end);
	}
}

#endif // !defined(AFX_GTDRAWHELPER_H__F818AFD1_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
