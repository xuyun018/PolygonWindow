#include <Windows.h>

#include <math.h>

#include <stdint.h>
#include <stdio.h>

const uint32_t rgb_colors_[83] = {
	// 蓝
	0x00e6f7ff,
	0x00bae7ff,
	0x0091d5ff,
	0x0069c0ff,
	0x0040a9ff,
	0x001890ff,
	0x00096dd9,
	0x000050b3,
	0x00003a8c,
	0x00002766,
	// 紫色
	0x00f9f0ff,
	0x00efdbff,
	0x00d3adf7,
	0x00b37feb,
	0x009254de,
	0x00722ed1,
	0x00531dab,
	0x00391085,
	0x0022075e,
	0x00120338,
	// 红色
	0x00fff1f0,
	0x00ffccc7,
	0x00ffa39e,
	0x00ff7875,
	0x00ff4d4f,
	0x00f5222d,
	0x00cf1322,
	0x00a8071a,
	0x00820014,
	0x005c0011,
	// 橙色
	0x00fff7e6,
	0x00ffe7ba,
	0x00ffd591,
	0x00ffc069,
	0x00ffa940,
	0x00fa8c16,
	0x00d46b08,
	0x00ad4e00,
	0x00873800,
	0x00612500,
	// 黄金色
	0x00fffbe6,
	0x00fff1b8,
	0x00ffe58f,
	0x00ffd666,
	0x00ffc53d,
	0x00faad14,
	0x00d48806,
	0x00ad6800,
	0x00874d00,
	0x00613400,
	// 绿色
	0x00f6ffed,
	0x00d9f7be,
	0x00b7eb8f,
	0x0095de64,
	0x0073d13d,
	0x0052c41a,
	0x00389e0d,
	0x00237804,
	0x00135200,
	0x00092b00,
	// 蓝绿色, 青色
	0x00e6fffb,
	0x00b5f5ec,
	0x0087e8de,
	0x005cdbd3,
	0x0036cfc9,
	0x0013c2c2,
	0x0008979c,
	0x00006d75,
	0x0000474f,
	0x00002329,
	// 灰色
	0x00ffffff,
	0x00fafafa,
	0x00f5f5f5,
	0x00f0f0f0,
	0x00d9d9d9,
	0x00bfbfbf,
	0x008c8c8c,
	0x00595959,
	0x00434343,
	0x00262626,
	0x001f1f1f,
	0x00141414,
	0x00000000
};

const float LABXRF = 0.433953f;         // RGB转XYZ的校正后系数(浮点类型）
const float LABXGF = 0.376219f;
const float LABXBF = 0.189828f;
const float LABYRF = 0.212671f;
const float LABYGF = 0.715160f;
const float LABYBF = 0.072169f;
const float LABZRF = 0.017758f;
const float LABZGF = 0.109477f;
const float LABZBF = 0.872765f;

const float LABRXF = 3.0799327f;        // XYZ转RGB的校正后系数(浮点类型）
const float LABRYF = -1.53715f;
const float LABRZF = -0.542782f;
const float LABGXF = -0.921235f;
const float LABGYF = 1.875991f;
const float LABGZF = 0.04524426f;
const float LABBXF = 0.0528909755f;
const float LABBYF = -0.204043f;
const float LABBZF = 1.15115158f;

const int LAB_Shift = 10;
const int LAB_HalfShiftValue = 1 << (LAB_Shift - 1);

const int LABXRI = (int)(LABXRF * (1 << LAB_Shift) + 0.5);      // RGB转XYZ的校正后系数
const int LABXGI = (int)(LABXGF * (1 << LAB_Shift) + 0.5);
const int LABXBI = (1 << LAB_Shift) - LABXRI - LABXGI;          //保证三个系数之和为(1 << LAB_Shift)
const int LABYRI = (int)(LABYRF * (1 << LAB_Shift) + 0.5);
const int LABYGI = (int)(LABYGF * (1 << LAB_Shift) + 0.5);
const int LABYBI = (1 << LAB_Shift) - LABYRI - LABYGI;
const int LABZRI = (int)(LABZRF * (1 << LAB_Shift) + 0.5);
const int LABZGI = (int)(LABZGF * (1 << LAB_Shift) + 0.5);
const int LABZBI = (1 << LAB_Shift) - LABZRI - LABZGI;

const int LABRXI = (int)(LABRXF * (1 << LAB_Shift) + 0.5);        // XYZ转RGB的校正后系数
const int LABRYI = (int)(LABRYF * (1 << LAB_Shift) + 0.5);
const int LABRZI = (1 << LAB_Shift) - LABRXI - LABRYI;            //保证三个系数之和为(1 << LAB_Shift)
const int LABGXI = (int)(LABGXF * (1 << LAB_Shift) + 0.5);
const int LABGYI = (int)(LABGYF * (1 << LAB_Shift) + 0.5);
const int LABGZI = (1 << LAB_Shift) - LABGXI - LABGYI;
const int LABBXI = (int)(LABBXF * (1 << LAB_Shift) + 0.5);
const int LABBYI = (int)(LABBYF * (1 << LAB_Shift) + 0.5);
const int LABBZI = (1 << LAB_Shift) - LABBXI - LABBYI;

const int LAB_ScaleLC = (int)(16 * 2.55 * (1 << LAB_Shift) + 0.5);
const int LAB_ScaleLT = (int)(116 * 2.55 + 0.5);

const int LAB_Threshold = (int)(6 * 6 * 6.0f / (29 * 29 * 29) * 255 * 4 + 0.5);

const float LAB_Div116 = 1.0f / 116 * (100.0F / 255);
const float LAB_Div500 = 1.0f / 500;
const float LAB_Div200 = 1.0f / 200;
const float LAB_Add16 = 16.0f / 116;
const float LAB_ThresoldF = 6.0f / 29;
const float LAB_MulT = 3 * (6 * 6.0f) / (29 * 29);
const float LAB_Sub4Div29 = 4.0f / 29;

//static int* LabTab = (int*)Marshal.AllocHGlobal(1024 * 4);
//static int* TabX = (int*)Marshal.AllocHGlobal(256 * 256 * 4);
//static int* TabY = (int*)Marshal.AllocHGlobal(256 * 4);
//static int* TabZ = (int*)Marshal.AllocHGlobal(256 * 256 * 4);

void lab_gen_table(int *table_lab, int *table_x, int *table_y, int *table_z)
{
	int i, j, index = 0;
	float x, y, z, t;

	for (i = 0; i < 1024; i++)
	{
		if (i > LAB_Threshold)
		{
			table_lab[i] = (int)(pow((float)i / 1020, 1.0F / 3) * (1 << LAB_Shift) + 0.5);
		}
		else
		{
			table_lab[i] = (int)((29 * 29.0 * i / (6 * 6 * 3 * 1020) + 4.0 / 29) * (1 << LAB_Shift) + 0.5);
		}
	}

	for (i = 0; i < 256; i++)
	{
		t = i * LAB_Div116 + LAB_Add16;
		if (t > LAB_ThresoldF)
		{
			y = t * t * t;
		}
		else
		{
			y = LAB_MulT * (t - LAB_Sub4Div29);
		}
		table_y[i] = (int)(y * 255 + 0.5);
		for (j = 0; j < 256; j++)
		{
			x = t + LAB_Div500 * (j - 128);
			if (x > LAB_ThresoldF)
			{
				x = x * x * x;
			}
			else
			{
				x = LAB_MulT * (x - LAB_Sub4Div29);
			}
			table_x[index] = (int)(x * 255 + 0.5);

			z = t - LAB_Div200 * (j - 128);
			if (z > LAB_ThresoldF)
			{
				z = z * z * z;
			}
			else
			{
				z = LAB_MulT * (z - LAB_Sub4Div29);
			}
			table_z[index] = (int)(z * 255 + 0.5);

			index++;
		}
	}
}

void rgb2lab(const int *table_lab, const uint8_t *rgb, uint8_t *lab)
{
	int _x, _y, _z, l, a, b;
	uint8_t _r, _g, _b;

	_b = rgb[0];
	_g = rgb[1];
	_r = rgb[2];

	_x = (_b * LABXBI + _g * LABXGI + _r * LABXRI + LAB_HalfShiftValue) >> (LAB_Shift - 2);
	_y = (_b * LABYBI + _g * LABYGI + _r * LABYRI + LAB_HalfShiftValue) >> (LAB_Shift - 2);
	_z = (_b * LABZBI + _g * LABZGI + _r * LABZRI + LAB_HalfShiftValue) >> (LAB_Shift - 2);
	_x = table_lab[_x];
	_y = table_lab[_y];
	_z = table_lab[_z];
	l = ((LAB_ScaleLT * _y - LAB_ScaleLC + LAB_HalfShiftValue) >> LAB_Shift);
	a = ((500 * (_x - _y) + LAB_HalfShiftValue) >> LAB_Shift) + 128;
	b = ((200 * (_y - _z) + LAB_HalfShiftValue) >> LAB_Shift) + 128;
	lab[0] = l;
	lab[1] = a;
	lab[2] = b;
}
void lab2rgb(const int *table_x, const int *table_y, const int *table_z, const uint8_t *lab, uint8_t *rgb)
{
	int _x, _y, _z, l, a, b;
	// 应该不会太大
	int16_t _r, _g, _b;

	l = lab[0];
	a = lab[1];
	b = lab[2];

	_x = table_x[(l << 8) + a];      // *256编译后会自动优化为移位的
	_y = table_y[l];
	_z = table_z[(l << 8) + b];
	_b = (_x * LABBXI + _y * LABBYI + _z * LABBZI + LAB_HalfShiftValue) >> LAB_Shift;
	_g = (_x * LABGXI + _y * LABGYI + _z * LABGZI + LAB_HalfShiftValue) >> LAB_Shift;
	_r = (_x * LABRXI + _y * LABRYI + _z * LABRZI + LAB_HalfShiftValue) >> LAB_Shift;
	
	if (_r > 255)
	{
		_r = 255;
	}
	else if (_r < 0)
	{
		_r = 0;
	}
	if (_g > 255)
	{
		_g = 255;
	}
	else if (_g < 0)
	{
		_g = 0;
	}
	if (_b > 255)
	{
		_b = 255;
	}
	else if (_b < 0)
	{
		_b = 0;
	}

	rgb[0] = _b;
	rgb[1] = _g;
	rgb[2] = _r;
}

void rgb2lab(const uint8_t *rgb, uint8_t *lab)
{
	//int _x, _y, _z, l, a, b;
	float _x, _y, _z;
	int l, a, b;
	float _r, _g, _b;

	// 取值 0 到 1
	_b = rgb[0] / 255.0;
	_g = rgb[1] / 255.0;
	_r = rgb[2] / 255.0;

	// gamma 2.2
	if (_r > 0.04045)
	{
		_r = pow((_r + 0.055) / 1.055, 2.4);
	}
	else
	{
		_r = _r / 12.92;
	}
	if (_g > 0.04045)
	{
		_g = pow((_g + 0.055) / 1.055, 2.4);
	}
	else
	{
		_g = _g / 12.92;
	}
	if (_b > 0.04045)
	{
		_b = pow((_b + 0.055) / 1.055, 2.4);
	}
	else
	{
		_b = _b / 12.92;
	}
	// sRGB
	_x = _r * 0.436052025 + _g * 0.385081593 + _b * 0.143087414;
	_y = _r * 0.222491598 + _g * 0.71688606 + _b * 0.060621486;
	_z = _r * 0.013929122 + _g * 0.097097002 + _b * 0.71418547;
	// XYZ range: 0~100
	_x = _x * 100.0;
	_y = _y * 100.0;
	_z = _z * 100.0;

	// Reference White Point
	_x = _x / 96.4221;
	_y = _y / 100.0;
	_z = _z / 82.5211;
	// Lab
	if (_x > 0.008856)
	{
		_x = pow((double)_x, 1 / 3.0);
	}
	else
	{
		_x = 7.787 * _x + 16 / 116.0;
	}
	if (_y > 0.008856)
	{
		_y = pow((double)_y, 1 / 3.0);
	}
	else
	{
		_y = 7.787 * _y + 16 / 116.0;
	}
	if (_z > 0.008856)
	{
		_z = pow((double)_z, 1 / 3.0);
	}
	else {
		_z = 7.787 * _z + 16 / 116.0;
	}

	l = 116.0 * _y - 16.0;
	a = 500.0 * (_x - _y);
	b = 200.0 * (_y - _z);

	lab[0] = l;
	lab[1] = a;
	lab[2] = b;
};

void rgb2lab1(const uint8_t *rgb, uint8_t *lab)
{
	//int _x, _y, _z, l, a, b;
	float _x, _y, _z;
	float __r, __g, __b;
	int l, a, b;
	unsigned int _r, _g, _b;

	// 取值 0 到 1
	// 这里不除以 255, 取值 0 到 255
	_b = rgb[0];
	_g = rgb[1];
	_r = rgb[2];

	// gamma 2.2
	// _r > 0.04045
	// 0.04045 * 255 = 10.31475
	if (_r > 10)
	{
		// 0.055 * 255 = 14.025
		//_r = pow((_r + 0.055) / 1.055, 2.4);
		// 1.055 转换为 255 + 14 = 269
		__r = pow((double)(_r + 14) / 269, 2.4);
	}
	else
	{
		// 
		//_r = _r / 12.92;
		// 12.92 * 255 = 3294.6 约等于 3295
		__r = _r / 3294.6;
	}
	if (_g > 10)
	{
		__g = pow((double)(_g + 14) / 269, 2.4);
	}
	else
	{
		__g = _g / 3294.6;
	}
	if (_b > 10)
	{
		__b = pow((double)(_b + 14) / 269, 2.4);
	}
	else
	{
		__b = _b / 3294.6;
	}
	// sRGB
	_x = __r * 43.6052025 + __g * 38.5081593 + __b * 14.3087414;
	_y = __r * 22.2491598 + __g * 71.688606 + __b * 6.0621486;
	_z = __r * 1.3929122 + __g * 9.7097002 + __b * 71.418547;
	// XYZ range: 0~100

	// Reference White Point
	_x = _x / 96.4221;
	_y = _y / 100.0;
	_z = _z / 82.5211;
	// Lab
	if (_x > 0.008856)
	{
		_x = pow((double)_x, 1 / 3.0);
	}
	else
	{
		_x = 7.787 * _x + 16 / 116.0;
	}
	if (_y > 0.008856)
	{
		_y = pow((double)_y, 1 / 3.0);
	}
	else
	{
		_y = 7.787 * _y + 16 / 116.0;
	}
	if (_z > 0.008856)
	{
		_z = pow((double)_z, 1 / 3.0);
	}
	else {
		_z = 7.787 * _z + 16 / 116.0;
	}

	l = 116.0 * _y - 16.0;
	a = 500.0 * (_x - _y);
	b = 200.0 * (_y - _z);

	lab[0] = l;
	lab[1] = a;
	lab[2] = b;
};

unsigned int rgb_distance(const int *table_lab, const uint8_t *rgb1, const uint8_t *rgb2)
{
	int l1, a1, b1;
	int l2, a2, b2;
	unsigned int d;
	uint8_t lab1[4];
	uint8_t lab2[4];

	//
	rgb2lab(table_lab, rgb1, lab1);
	rgb2lab(table_lab, rgb2, lab2);

	l1 = lab1[0];
	a1 = lab1[1];
	b1 = lab1[2];
	l2 = lab2[0];
	a2 = lab2[1];
	b2 = lab2[2];
	d = (l2 - l1)*(l2 - l1) + (a2 - a1)*(a2 - a1)+ (b2 - b1)*(b2 - b1);
	return(d);
}
unsigned int rgb_distance(const uint8_t *rgb1, const uint8_t *rgb2)
{
	int l1, a1, b1;
	int l2, a2, b2;
	unsigned int d;
	uint8_t lab1[4];
	uint8_t lab2[4];

	//
	rgb2lab(rgb1, lab1);
	rgb2lab(rgb2, lab2);

	l1 = lab1[0];
	a1 = lab1[1];
	b1 = lab1[2];
	l2 = lab2[0];
	a2 = lab2[1];
	b2 = lab2[2];
	d = (l2 - l1)*(l2 - l1) + (a2 - a1)*(a2 - a1) + (b2 - b1)*(b2 - b1);
	return(d);
}
unsigned int rgb_distance1(const uint8_t *rgb1, const uint8_t *rgb2)
{
	int l1, a1, b1;
	int l2, a2, b2;
	unsigned int d;
	uint8_t lab1[4];
	uint8_t lab2[4];

	//
	rgb2lab1(rgb1, lab1);
	rgb2lab1(rgb2, lab2);

	l1 = lab1[0];
	a1 = lab1[1];
	b1 = lab1[2];
	l2 = lab2[0];
	a2 = lab2[1];
	b2 = lab2[2];
	d = (l2 - l1)*(l2 - l1) + (a2 - a1)*(a2 - a1) + (b2 - b1)*(b2 - b1);
	return(d);
}

void bgr2rgb(const uint32_t *bgrs, uint32_t *rgbs, unsigned int count)
{
	const uint32_t *p0 = bgrs;
	uint32_t *p1 = rgbs;
	unsigned int i;
	uint8_t _r, _g, _b;

	for (i = 0; i < count; i++)
	{
		_r = *p0 >> 16;
		_g = *p0 >> 8;
		_b = *p0;
		*p1 = (_b << 16) | (_g << 8) | _r;

		p0++;
		p1++;
	}
}

int wmain(int argc, WCHAR *argv[])
{
	uint32_t colors[83];
	unsigned int i;
	unsigned int j;

	Sleep(10000);

	int table_lab[1024];
	int *table_x;
	int table_y[256];
	int *table_z;

	table_x = new int[256 * 256];
	table_z = new int[256 * 256];
	lab_gen_table(table_lab, table_x, table_y, table_z);

	bgr2rgb(rgb_colors_, colors, 83);

	RECT rect;
	HBRUSH hbrush;
	HDC hdc = GetDC(NULL);

	for (i = 0; i < 83; i++)
	{
		uint8_t _r, _g, _b;

		_r = GetRValue(colors[i]);
		_g = GetGValue(colors[i]);
		_b = GetBValue(colors[i]);

		rect.left = 10;
		rect.top = 10 + i * 5;
		rect.right = rect.left + 90;
		rect.bottom = rect.top + 3;
		hbrush = CreateSolidBrush(RGB(_r, _g, _b));
		FillRect(hdc, &rect, hbrush);
		DeleteObject(hbrush);
	}

	uint8_t rgb1[4];
	uint8_t rgb2[4];
	unsigned int d1, d2;

	d1 = ~0;
	j = -1;

	rgb1[0] = 0;
	rgb1[1] = 0;
	rgb1[2] = 255;
	for (i = 0; i < 83; i++)
	{
		uint8_t _r, _g, _b;

		_r = GetRValue(colors[i]);
		_g = GetGValue(colors[i]);
		_b = GetBValue(colors[i]);

		rgb2[0] = _b;
		rgb2[1] = _g;
		rgb2[2] = _r;

		unsigned int d3;
		unsigned int d4;
		d3 = rgb_distance(table_lab, rgb1, rgb2);
		d4 = rgb_distance1(rgb1, rgb2);
		d2 = rgb_distance(rgb1, rgb2);
		printf("%d, 0x%08x, 0x%08x, d %d, %d, %d\r\n", i, colors[i], rgb_colors_[i], d2, d4, d3);
		if (d2 < d1)
		{
			d1 = d2;
			j = i;
		}
	}

	rect.left = 130;
	rect.top = 10 + j * 5;
	rect.right = rect.left + 90;
	rect.bottom = rect.top + 3;
	hbrush = CreateSolidBrush(RGB(rgb1[2], rgb1[1], rgb1[0]));
	FillRect(hdc, &rect, hbrush);
	DeleteObject(hbrush);

	ReleaseDC(NULL, hdc);

	delete[] table_x;
	delete[] table_z;

	printf("%d\r\n", j);
	printf("0x%08x\r\n", colors[j]);
	printf("0x%08x\r\n", rgb_colors_[j]);

	getchar();

	return(0);
}