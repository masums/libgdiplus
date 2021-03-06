#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(USE_WINDOWS_GDIPLUS)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")
#else
#include <GdiPlusFlat.h>
#endif

#if defined(USE_WINDOWS_GDIPLUS)
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include "testhelpers.h"

static const char *file = "temp_asset.bmp";
static WCHAR wFile[] = {'t', 'e', 'm', 'p', '_', 'a', 's', 's', 'e', 't', '.', 'b', 'm', 'p', 0};
GpImage *image;
INT bmpFlags = ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly;

#define createFile(buffer, expectedStatus) \
{ \
	GpStatus status; \
	FILE *f = fopen (file, "wb+"); \
	assert (f); \
	fwrite ((void *) buffer, sizeof (BYTE), sizeof (buffer), f); \
	fclose (f); \
 \
	status = GdipLoadImageFromFile (wFile, &image); \
	assertEqualInt (status, expectedStatus); \
}

#define createFileSuccessDispose(buffer, expectedFormat, width, height, expectedFlags, dispose) \
{ \
	createFile (buffer, Ok); \
	verifyBitmap (image, bmpRawFormat, expectedFormat, width, height, expectedFlags, 0, TRUE); \
	if (dispose) \
		GdipDisposeImage (image); \
}

#define createFileSuccess(buffer, expectedFormat, width, height, expectedFlags) \
	createFileSuccessDispose(buffer, expectedFormat, width, height, expectedFlags, TRUE)

static void test_validImage1bppOS2Header ()
{
	BYTE image1x1[] = {
		'B', 'M', 36, 0, 0, 0, 0, 0, 0, 0, 0x20, 0, 0, 0,
		12, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0,
		255, 0, 0, 0, 255, 0,
		B8(10000000), B8(00000000), B8(00000000), 0
	};
	BYTE image24x4[] = {
		'B', 'M', 48, 0, 0, 0, 0, 0, 0, 0, 0x20, 0, 0, 0,
		12, 0, 0, 0, 24, 0, 4, 0, 1, 0, 1, 0,
		255, 0, 0, 0, 255, 0,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(00100111), B8(00000000), B8(00000000), 0,
		B8(11111111), B8(11111111), B8(11111111), 0
	};

	createFileSuccessDispose (image1x1, PixelFormat1bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image24x4, PixelFormat1bppIndexed, 24, 4, bmpFlags, FALSE);
	ARGB image24x4Pixels[] = {
		0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00,
		0xFF0000FF, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF0000FF, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
		0xFF00FF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
		0xFF00FF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
	};
	verifyPixels (image, image24x4Pixels);
	GdipDisposeImage (image);
}

static void test_validImage1bppBitmapInfoHeader ()
{
	BYTE image1x1[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255,
		B8(10000000), 0, 0, 0
	};
	BYTE image24x4CustomSizedPalette[] = {
		'B', 'M', 82, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 24, 0, 0, 0, 4, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 255, 255, 255, 255,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(00100111), B8(00000000), B8(00000000), 0,
		B8(11111111), B8(11111111), B8(11111111), 0
	};
	BYTE image1x1NegativeHeight[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 255, 255, 255, 255, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255,
		B8(10000000), 0, 0, 0
	};
	BYTE image24x4NegativeHeight[] = {
		'B', 'M', 82, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 24, 0, 0, 0, 252, 255, 255, 255, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 255, 255, 255, 255,
		B8(11111111), B8(11111111), B8(11111111), 0,
		B8(00100111), B8(00000000), B8(00000000), 0,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(10000000), B8(00000000), B8(00000000), 0
	};

	createFileSuccessDispose (image1x1, PixelFormat1bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image24x4CustomSizedPalette, PixelFormat1bppIndexed, 24, 4, bmpFlags, FALSE);
	ARGB image24x4Pixels[] = {
		0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00,
		0xFF0000FF, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF0000FF, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
		0xFF00FF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
		0xFF00FF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
	};
	verifyPixels (image, image24x4Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1NegativeHeight, PixelFormat1bppIndexed, 1, 1, bmpFlags, FALSE);
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image24x4NegativeHeight, PixelFormat1bppIndexed, 24, 4, bmpFlags, FALSE);
	verifyPixels (image, image24x4Pixels);
	GdipDisposeImage (image);
}

static void test_validImage1bppBitmapV3Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x52, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x42, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x38, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat1bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage1bppBitmapV4Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x86, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x82, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x6C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3HEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV4CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV4Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV4GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV4GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV4GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat1bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage1bppBitmapV5Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x96, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x92, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x7C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3HEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV5CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV5Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV5GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV5GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV5GammaBlue */     0x00, 0x00, 0x00, 0x00,
		/* bV5Intent */        0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileData */   0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileSize */   0x00, 0x00, 0x00, 0x00,
		/* bV5Reserved */      0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat1bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage4bppOS2Header ()
{
	BYTE image1x1[] = {
		'B', 'M', 122, 0, 0, 0, 0, 0, 0, 0, 0x59, 0, 0, 0,
		12, 0, 0, 0, 1, 0, 1, 0, 1, 0, 4, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		B8(00010010), 0, 0, 0
	};
	BYTE image6x4[] = {
		'B', 'M', 94, 0, 0, 0, 0, 0, 0, 0, 0x29, 0, 0, 0,
		12, 0, 0, 0, 6, 0, 4, 0, 1, 0, 4, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		B8(00010011), B8(11111111), B8(00110001), 0,
		B8(00010011), B8(10101010), B8(00110001), 0,
		B8(00010011), B8(00110011), B8(00110001), 0,
		B8(00010001), B8(00010001), B8(00010001), 0
	};

	createFileSuccessDispose (image1x1, PixelFormat4bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFF00FF};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4, PixelFormat4bppIndexed, 6, 4, bmpFlags, FALSE);
	ARGB image6x4Pixels[] = {
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
		0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF,
		0xFFFF00FF, 0xFFFF00FF, 0xFF0000FF, 0xFF0000FF, 0xFFFF00FF, 0xFFFF00FF
	};
	verifyPixels (image, image6x4Pixels);
	GdipDisposeImage (image);
}

static void test_validImage4bppBitmapInfoHeader ()
{
	BYTE image1x1[] = {
		'B', 'M', 122, 0, 0, 0, 0, 0, 0, 0, 0x75, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		B8(00010010), 0, 0, 0
	};
	BYTE image6x4CustomSizedPalette[] = {
		'B', 'M', 122, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		B8(00010011), B8(11111111), B8(00110001), 0,
		B8(00010011), B8(10101010), B8(00110001), 0,
		B8(00010011), B8(00110011), B8(00110001), 0,
		B8(00010001), B8(00010001), B8(00010001), 0
	};

	BYTE image1x1NegativeHeight[] = {
		'B', 'M', 122, 0, 0, 0, 0, 0, 0, 0, 0x75, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 255, 255, 255, 255, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		B8(00010010), 0, 0, 0
	};
	BYTE image6x4NegativeHeight[] = {
		'B', 'M', 122, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 252, 255, 255, 255, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		B8(00010001), B8(00010001), B8(00010001), 0,
		B8(00010011), B8(00110011), B8(00110001), 0,
		B8(00010011), B8(10101010), B8(00110001), 0,
		B8(00010011), B8(11111111), B8(00110001), 0
	};

	createFileSuccessDispose (image1x1, PixelFormat4bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4CustomSizedPalette, PixelFormat4bppIndexed, 6, 4, bmpFlags, FALSE);
	// FIXME: these are incorrectly decoded by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image6x4Pixels[] = {
		0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00,
		0xFF00FF00, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF00FF00,
		0xFF00FF00, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF00FF00,
		0xFF00FF00, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF00FF00
	};
	verifyPixels (image, image6x4Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1NegativeHeight, PixelFormat4bppIndexed, 1, 1, bmpFlags, FALSE);
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4NegativeHeight, PixelFormat4bppIndexed, 6, 4, bmpFlags, FALSE);
	// FIXME: this is broken.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyPixels (image, image6x4Pixels);
#endif
	GdipDisposeImage (image);
}

static void test_validImage4bppBitmapV3Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x52, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x42, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x38, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x04, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x02, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x10, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat4bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage4bppBitmapV4Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x86, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x82, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x6C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x04, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x02, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3HEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV4CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV4Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV4GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV4GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV4GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x10, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat4bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage4bppBitmapV5Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x96, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x92, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x7C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x04, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x02, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3HEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV5CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV5Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV5GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV5GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV5GammaBlue */     0x00, 0x00, 0x00, 0x00,
		/* bV5Intent */        0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileData */   0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileSize */   0x00, 0x00, 0x00, 0x00,
		/* bV5Reserved */      0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x10, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat4bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage4bppRle4Compression ()
{
	BYTE fullNoTerminator[] = {
		'B', 'M', 82, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, B8(00000001), 0, 0,
		6, B8(00000001), 0, 0,
		6, B8(00000001), 0, 0,
		6, B8(00000001), 0, 0
	};
	BYTE fullTerminator[] = {
		'B', 'M', 84, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, B8(00000001), 0, 0,
		6, B8(00000001), 0, 0,
		6, B8(00000001), 0, 0,
		6, B8(00000001), 0, 0,
		0, 1
	};
	BYTE split[] = {
		'B', 'M', 88, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		4, B8(00010001), 0, 0,
		3, B8(00000000), 3, B8(00010001), 0, 0,
		5, B8(00110011), 0, 0,
		1, B8(00000000), 2, B8(00110011), 0, 0,
		0, 1
	};
	BYTE overflowNoTerminator[] = {
		'B', 'M', 68, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		255, 0
	};
	BYTE overflowTerminator[] = {
		'B', 'M', 70, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		255, 0,
		0, 1
	};

	BYTE jump[] = {
		'B', 'M', 70, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		255, 0,
		0, 1
	};
	BYTE jumpMissingDx[] = {
		'B', 'M', 68, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2
	};
	BYTE jumpMissingDy[] = {
		'B', 'M', 69, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2, 0
	};
	BYTE jumpInvalidDx[] = {
		'B', 'M', 74, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2, 255, 2,
		3, 0,
		0, 1
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE jumpInvalidDy[] = {
		'B', 'M', 74, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2, 2, 255,
		3, 0,
		0, 1
	};
#endif

	BYTE absoluteModeOddPixels[] = {
		'B', 'M', 72, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 3, B8(00010011), B8(00000001),
		0, 1
	};
	BYTE absoluteModeEvenPixels[] = {
		'B', 'M', 72, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 4, B8(00010011), B8(00000001),
		0, 1
	};

	BYTE absoluteModeNoData[] = {
		'B', 'M', 68, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 3
	};
	BYTE absoluteModeShortData[] = {
		'B', 'M', 69, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 3, 1
	};
	BYTE absoluteModeNoPadding[] = {
		'B', 'M', 71, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 5, 1, 2, 1
	};

	BYTE fullNegativeHeight[] = {
		'B', 'M', 84, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 252, 255, 255, 255, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, 1, 0, 0,
		6, 0, 0, 0,
		6, 2, 0, 0,
		6, 0, 0, 0,
		0, 1
	};
	BYTE incompleteAfterLineBreak[] = {
		'B', 'M', 70, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, 1, 0, 0
	};
	BYTE noImageData[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255
	};
	BYTE singleByteData[] = {
		'B', 'M', 67, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		3
	};
	BYTE singleEscapeCode[] = {
		'B', 'M', 67, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0
	};
	BYTE earlyTerminator[] = {
		'B', 'M', 74, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, 1, 0, 0,
		2, 0, 0, 1
	};
	BYTE earlyTerminatorWithTrailingBytes[] = {
		'B', 'M', 88, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, 1, 0, 0,
		2, 0, 0, 1,
		6, 2, 255, 255
	};
	BYTE tooManyNewLines[] = {
		'B', 'M', 78, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 4, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 0,
		0, 0,
		0, 0,
		0, 0,
		0, 0,
		1, 0
	};

	// FIXME: GDI+ converts BI_RLE4 bitmaps to 32bpp.
#if defined(USE_WINDOWS_GDIPLUS)
	PixelFormat expectedRle4Format = PixelFormat32bppRGB;
#else
	PixelFormat expectedRle4Format = PixelFormat4bppIndexed;
#endif
	createFileSuccessDispose (fullNoTerminator, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	ARGB fullNoTerminatorPixels[] = {
		0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
		0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
		0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
		0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00
	};
	verifyPixels (image, fullNoTerminatorPixels);
	GdipDisposeImage(image);

	createFileSuccessDispose (fullTerminator, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	ARGB fullTerminatorPixels[] = {
		0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
		0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
		0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
		0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00
	};
	verifyPixels (image, fullTerminatorPixels);
	GdipDisposeImage(image);

	createFileSuccessDispose (split, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB splitPixels[] = {
		0xFF0000FF, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00,
		0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, splitPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (overflowNoTerminator, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
    ARGB overflowNoTerminatorPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
    };
	verifyPixels (image, overflowNoTerminatorPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (overflowTerminator, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB overflowTerminatorPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
	};
	verifyPixels (image, overflowTerminatorPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (jump, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB jumpPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
	};
	verifyPixels (image, jumpPixels)
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (jumpMissingDx, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB jumpMissingDxPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpMissingDxPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (jumpMissingDy, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB jumpMissingDyPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpMissingDyPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (jumpInvalidDx, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB jumpInvalidDxPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpInvalidDxPixels);
#endif
	GdipDisposeImage(image);

	// FIXME: this causes a heap buffer overflow in libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	createFileSuccessDispose (jumpInvalidDy, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	ARGB jumpInvalidDyPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpInvalidDyPixels);
	GdipDisposeImage(image);
#endif

	createFileSuccessDispose (absoluteModeOddPixels, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeOddPixelsPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF00FF00, 0xFF000000, 0xFF0000FF, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeOddPixelsPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (absoluteModeEvenPixels, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeEvenPixelsPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF00FF00, 0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeEvenPixelsPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (absoluteModeNoData, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeNoDataPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeNoDataPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (absoluteModeShortData, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeShortDataPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeShortDataPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (absoluteModeNoPadding, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeNoPaddingPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFFFF0000, 0xFF0000FF, 0xFF000000
	};
	verifyPixels (image, absoluteModeNoPaddingPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (fullNegativeHeight, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	ARGB fullNegativeHeightPixels[] = {
			0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
			0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
			0xFF0000FF, 0xFFFF0000, 0xFF0000FF, 0xFFFF0000, 0xFF0000FF, 0xFFFF0000,
			0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
	};
	verifyPixels (image, fullNegativeHeightPixels);
	GdipDisposeImage(image);

	createFileSuccessDispose (incompleteAfterLineBreak, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB incompleteAfterLineBreakPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00
	};
	verifyPixels (image, incompleteAfterLineBreakPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (noImageData, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);

	createFileSuccessDispose (singleByteData, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB singleByteDataPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, singleByteDataPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (singleEscapeCode, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB singleEscapeCodePixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, singleEscapeCodePixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (earlyTerminator, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB earlyTerminatorPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF0000FF, 0xFF0000FF, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00
	};
	verifyPixels (image, earlyTerminatorPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (earlyTerminatorWithTrailingBytes, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB earlyTerminatorWithTrailingBytesPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF0000FF, 0xFF0000FF, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00
	};
	verifyPixels (image, earlyTerminatorWithTrailingBytesPixels);
#endif
	GdipDisposeImage(image);

	createFileSuccessDispose (tooManyNewLines, expectedRle4Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle4 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB tooManyNewLinesPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, tooManyNewLinesPixels);
#endif
	GdipDisposeImage(image);
}

static void test_validImage8bppOS2Header ()
{
	BYTE image1x1[] = {
		'B', 'M', 30, 4, 0, 0, 0, 0, 0, 0, 25, 4, 0, 0,
		12, 0, 0, 0, 1, 0, 1, 0, 1, 0, 8, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		1, 0, 0, 0
	};
	BYTE image6x4[] = {
		'B', 'M', 48, 4, 0, 0, 0, 0, 0, 0, 25, 4, 0, 0,
		12, 0, 0, 0, 6, 0, 4, 0, 1, 0, 8, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		1, 0, 2, 0, 2, 1, 0, 0,
		1, 0, 2, 0, 2, 1, 0, 0,
		1, 0, 2, 0, 2, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 0, 0
	};

	createFileSuccessDispose (image1x1, PixelFormat8bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFF0000};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4, PixelFormat8bppIndexed, 6, 4, bmpFlags, FALSE);
	// FIXME: these are incorrectly decoded by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image6x4Pixels[] = {
        0xFF0000FF, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF,
        0xFF0000FF, 0xFFFF00FF, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
        0xFF0000FF, 0xFFFF00FF, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00,
        0xFFFF0000, 0xFFFF00FF, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00
	};
	verifyPixels (image, image6x4Pixels);
#endif
	GdipDisposeImage (image);
}

static void test_validImage8bppBitmapInfoHeader ()
{
	BYTE image1x1[] = {
		'B', 'M', 58, 4, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		1, 0, 0, 0
	};
	BYTE image6x4CustomSizedPalette[] = {
		'B', 'M', 98, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255,
		1, 0, 2, 0, 2, 1, 0, 0,
		1, 0, 2, 0, 2, 1, 0, 0,
		1, 0, 2, 0, 2, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 0, 0
	};
	BYTE image1x1NegativeHeight[] = {
		'B', 'M', 58, 4, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 255, 255, 255, 255, 1, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 8, 8, 8, 8, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
		1, 0, 0, 0
	};
	BYTE image6x4NegativeHeight[] = {
		'B', 'M', 98, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 252, 255, 255, 255, 1, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255,
		1, 1, 1, 1, 1, 1, 0, 0,
		1, 0, 2, 0, 2, 1, 0, 0,
		1, 0, 2, 0, 2, 1, 0, 0,
		1, 0, 2, 0, 2, 1, 0, 0
	};

	createFileSuccessDispose (image1x1, PixelFormat8bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4CustomSizedPalette, PixelFormat8bppIndexed, 6, 4, bmpFlags, FALSE);
	ARGB image6x4CustomSizedPalettePixels[] = {
        0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00,
        0xFF00FF00, 0xFF0000FF, 0xFF000000, 0xFF0000FF, 0xFF000000, 0xFF00FF00,
        0xFF00FF00, 0xFF0000FF, 0xFF000000, 0xFF0000FF, 0xFF000000, 0xFF00FF00,
        0xFF00FF00, 0xFF0000FF, 0xFF000000, 0xFF0000FF, 0xFF000000, 0xFF00FF00
	};
	verifyPixels (image, image6x4CustomSizedPalettePixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1NegativeHeight, PixelFormat8bppIndexed, 1, 1, bmpFlags, FALSE);
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4NegativeHeight, PixelFormat8bppIndexed, 6, 4, bmpFlags, FALSE);
	verifyPixels (image, image6x4CustomSizedPalettePixels);
	GdipDisposeImage (image);
}

static void test_validImage8bppBitmapV3Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x52, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x42, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x38, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x08, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x02, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x01, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat8bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage8bppBitmapV4Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x86, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x82, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x6C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x08, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x02, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3HEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV4CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV4Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV4GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV4GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV4GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x01, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat8bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage8bppBitmapV5Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x96, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x92, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x7C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x08, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x02, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3HEADER -- //
		/* Red Mask */         0xFF, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0xFF, 0x00,
		/* Blue Mask */        0x00, 0xFF, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV5CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV5Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV5GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV5GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV5GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV5HEADER --//
		/* bV5Intent */        0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileData */   0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileSize */   0x00, 0x00, 0x00, 0x00,
		/* bV5Reserved */      0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x01, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat8bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFF00FF00};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage8bppRle8Compression ()
{
	BYTE fullNoTerminator[] = {
		'B', 'M', 82, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255,
		6, 1, 0, 0,
		6, 0, 0, 0,
		6, 2, 0, 0,
		6, 0, 0, 0
	};
	BYTE fullTerminator[] = {
		'B', 'M', 84, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255,
		6, 1, 0, 0,
		6, 0, 0, 0,
		6, 2, 0, 0,
		6, 0, 0, 0,
		0, 1
	};
	BYTE split[] = {
		'B', 'M', 86, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		4, 1, 0, 0,
		3, 0, 3, 1, 0, 0,
		5, 2, 0, 0,
		1, 0, 2, 2,
		0, 1
	};
	BYTE overflowNoTerminator[] = {
		'B', 'M', 68, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		255, 0
	};
	BYTE overflowTerminator[] = {
		'B', 'M', 69, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		255, 0,
		0, 1
	};

	BYTE jump[] = {
		'B', 'M', 74, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2, 1, 2,
		3, 0,
		0, 1
	};
	BYTE jumpMissingDx[] = {
		'B', 'M', 68, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2
	};
	BYTE jumpMissingDy[] = {
		'B', 'M', 69, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2, 1
	};
	BYTE jumpInvalidDx[] = {
		'B', 'M', 74, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2, 255, 2,
		3, 0,
		0, 1
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE jumpInvalidDy[] = {
		'B', 'M', 74, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 2, 1, 255,
		3, 0,
		0, 1
	};
#endif

	BYTE absoluteModeOddPixels[] = {
		'B', 'M', 73, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 3, 0, 1, 2, 0,
		0, 1
	};
	BYTE absoluteModeEvenPixels[] = {
		'B', 'M', 74, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 4, 0, 1, 2, 0,
		0, 1
	};
	BYTE absoluteModeNoData[] = {
		'B', 'M', 68, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 3
	};
	BYTE absoluteModeShortData[] = {
		'B', 'M', 69, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 3, 1
	};
	BYTE absoluteModeNoPadding[] = {
		'B', 'M', 71, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 3, 1, 2, 1
	};

	BYTE fullNegativeHeight[] = {
		'B', 'M', 84, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 252, 255, 255, 255, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, 1, 0, 0,
		6, 0, 0, 0,
		6, 2, 0, 0,
		6, 0, 0, 0,
		0, 1
	};
	BYTE incompleteAfterLineBreak[] = {
		'B', 'M', 70, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, 1, 0, 0
	};
	BYTE noImageData[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255
	};
	BYTE singleByteData[] = {
		'B', 'M', 67, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		3
	};
	BYTE singleEscapeCode[] = {
		'B', 'M', 67, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0
	};
	BYTE earlyTerminator[] = {
		'B', 'M', 74, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, 1, 0, 0,
		2, 0, 0, 1
	};
	BYTE earlyTerminatorWithTrailingBytes[] = {
		'B', 'M', 88, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		6, 1, 0, 0,
		2, 0, 0, 1,
		6, 2, 255, 255
	};
	BYTE tooManyNewLines[] = {
		'B', 'M', 78, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 8, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255,
		0, 0,
		0, 0,
		0, 0,
		0, 0,
		0, 0,
		1, 0
	};

	// FIXME: GDI+ converts BI_RLE8 bitmaps to 32bpp.
#if defined(USE_WINDOWS_GDIPLUS)
	PixelFormat expectedRle8Format = PixelFormat32bppRGB;
#else
	PixelFormat expectedRle8Format = PixelFormat8bppIndexed;
#endif
	createFileSuccessDispose (fullNoTerminator, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	ARGB fullNoTerminatorPixels[] = {
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
		0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00
	};
	verifyPixels (image, fullNoTerminatorPixels);	GdipDisposeImage (image);

	createFileSuccessDispose (fullTerminator, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	ARGB fullTerminatorPixels[] = {
        0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
        0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00
	};
	verifyPixels (image, fullTerminatorPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (split, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB splitPixels[] = {
		0xFF0000FF, 0xFFFF0000, 0xFFFF0000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFF000000,
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00,
		0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, splitPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (overflowNoTerminator, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB overflowNoTerminatorPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
	};
	verifyPixels (image, overflowNoTerminatorPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (overflowTerminator, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB overflowTerminatorPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
	};
	verifyPixels (image, overflowTerminatorPixels);	GdipDisposeImage (image);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (jump, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB jumpPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (jumpMissingDx, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB jumpMissingDxPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpMissingDxPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (jumpMissingDy, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB jumpMissingDyPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpMissingDyPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (jumpInvalidDx, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB jumpInvalidDxPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpInvalidDxPixels);
#endif
	GdipDisposeImage (image);

	// FIXME: this causes a heap buffer overflow in libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	createFileSuccessDispose (jumpInvalidDy, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	ARGB jumpInvalidDyPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, jumpInvalidDyPixels);
	GdipDisposeImage (image);
#endif

	createFileSuccessDispose (absoluteModeOddPixels, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeOddPixelsPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeOddPixelsPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (absoluteModeEvenPixels, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeEvenPixelsPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF0000FF, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeEvenPixelsPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (absoluteModeNoData, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeNoDataPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeNoDataPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (absoluteModeShortData, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeShortDataPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeShortDataPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (absoluteModeNoPadding, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB absoluteModeNoPaddingPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF00FF00, 0xFFFF0000, 0xFF00FF00, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, absoluteModeNoPaddingPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (fullNegativeHeight, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	ARGB fullNegativeHeightPixels[] = {
        0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00,
        0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF,
        0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000,
        0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF
	};
	verifyPixels (image, fullNegativeHeightPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (incompleteAfterLineBreak, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB incompleteAfterLineBreakPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00
	};
	verifyPixels (image, incompleteAfterLineBreakPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (noImageData, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);

	createFileSuccessDispose (singleByteData, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB singleByteDataPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, singleByteDataPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (singleEscapeCode, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB singleEscapeCodePixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, singleEscapeCodePixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (earlyTerminator, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB earlyTerminatorPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF0000FF, 0xFF0000FF, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00
	};
	verifyPixels (image, earlyTerminatorPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (earlyTerminatorWithTrailingBytes, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB earlyTerminatorWithTrailingBytesPixels[] = {
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF0000FF, 0xFF0000FF, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00
	};
	verifyPixels (image, earlyTerminatorWithTrailingBytesPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (tooManyNewLines, expectedRle8Format, 6, 4, bmpFlags, FALSE);
	// FIXME: rle8 encoding edgecases are not properly handled by libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB tooManyNewLinesPixels[] = {
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
			0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	verifyPixels (image, tooManyNewLinesPixels);
#endif
	GdipDisposeImage (image);
}

static void test_validImage16bppOS2Header ()
{
	BYTE image1x1[] = {
		'B', 'M', 42, 4, 0, 0, 0, 0, 0, 0, 0x26, 0, 0, 0,
		12, 0, 0, 0, 1, 0, 1, 0, 1, 0, 16, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0,
		B8(10101101), B8(01101010), 0, 0
	};
	BYTE image6x4[] = {
		'B', 'M', 86, 4, 0, 0, 0, 0, 0, 0, 0x26, 0, 0, 0,
		12, 0, 0, 0, 6, 0, 4, 0, 1, 0, 16, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0,
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000),
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000)
	};

	createFileSuccessDispose (image1x1, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
    ARGB image1x1Pixels[] = {0xFFD6AD6B};
    verifyPixels (image, image1x1Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
    ARGB image6x4Pixels[] = {
		0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
		0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6,
        0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
        0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6
    };
    verifyPixels (image, image6x4Pixels);
#endif
	GdipDisposeImage (image);
}

static void test_validImage16bppBitmapInfoHeader ()
{
	BYTE image1x1RedShift10[] = {
		'B', 'M', 70, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0,
		B8(10101101), B8(01101010), 0, 0
	};
	BYTE image6x4RedShift10[] = {
		'B', 'M', 114, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0,
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000),
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000)
	};
	BYTE image1x1RedShift11[] = {
		'B', 'M', 70, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 248, 0, 0, 224, 7, 0, 0, 0x1F, 0, 0, 0,
		B8(10101101), B8(01010101), 0, 0
	};
	BYTE image6x4RedShift11[] = {
		'B', 'M', 114, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 248, 0, 0, 224, 7, 0, 0, 0x1F, 0, 0, 0,
		B8(11111111), B8(11000000), B8(11111111), B8(11111111), B8(00000111), B8(11111111), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00011111),
		B8(11111111), B8(11111111), B8(11111111), B8(11100000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00011111), B8(00000000), B8(00000000),
		B8(11111111), B8(11000000), B8(11111111), B8(11111111), B8(00000111), B8(11111111), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00011111),
		B8(11111111), B8(11111111), B8(11111111), B8(11100000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00011111), B8(00000000), B8(00000000),
	};
	BYTE image1x1RedShiftUnknown[] = {
		'B', 'M', 70, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 248, 0, 0, 224, 7, 0, 0, 0x1F, 0, 0, 0,
		B8(10101101), B8(01101010), 0, 0
	};
	BYTE image6x4RedShiftUnknown[] = {
		'B', 'M', 114, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000),
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000)
	};
	BYTE image1x1NegativeHeight[] = {
		'B', 'M', 70, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 255, 255, 255, 255, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0,
		B8(10101101), B8(01101010), 0, 0
	};
	BYTE image6x4NegativeHeight[] = {
		'B', 'M', 114, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 252, 255, 255, 255, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0,
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000),
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000)
	};

#if defined(USE_WINDOWS_GDIPLUS)
	BYTE rle4Compression16bpp[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 255, 255, 0
	};
	BYTE rle8Compression16bpp[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 255, 255, 0
	};
#endif

	createFileSuccessDispose (image1x1RedShift10, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1RedShift10Pixels[] = {0xFFD6AD6B};
	verifyPixels (image, image1x1RedShift10Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4RedShift10, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image6x4RedShift10Pixels[] = {
        0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
        0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6,
        0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
        0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6
    };
    verifyPixels (image, image6x4RedShift10Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1RedShift11, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1RedShift11Pixels[] = {0xFFAD6B6B};
    verifyPixels (image, image1x1RedShift11Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4RedShift11, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image6x4RedShift11Pixels[] = {
        0xFFFFFFFF, 0xFFC639FF, 0xFF0039C6, 0xFFFF8439, 0xFF39FFC6, 0xFF000000,
        0xFF8439FF, 0xFFFFFFFF, 0xFFFFC639, 0xFF0039C6, 0xFF000000, 0xFF39FFC6,
        0xFFFFFFFF, 0xFFC639FF, 0xFF0039C6, 0xFFFF8439, 0xFF39FFC6, 0xFF000000,
        0xFF8439FF, 0xFFFFFFFF, 0xFFFFC639, 0xFF0039C6, 0xFF000000, 0xFF39FFC6
    };
    verifyPixels (image, image6x4RedShift11Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1RedShiftUnknown, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1RedShiftUnknownPixels[] = {0xFFD6AD6B};
	verifyPixels (image, image1x1RedShiftUnknownPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4RedShiftUnknown, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image6x4RedShiftUnknownPixels[] = {
        0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
        0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6,
        0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
        0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6
	};
	verifyPixels (image, image6x4RedShiftUnknownPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1NegativeHeight, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1NegativeHeightPixels[] = {0xFFD6AD6B};
	verifyPixels (image, image1x1NegativeHeightPixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4NegativeHeight, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image6x4NegativeHeightPixels[] = {
		0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6,
		0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
		0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6,
		0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000
	};
	verifyPixels (image, image6x4NegativeHeightPixels);
#endif
	GdipDisposeImage (image);

	// FIXME: this returns OutOfMemory libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	createFileSuccessDispose (rle4Compression16bpp, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);

	createFileSuccessDispose (rle8Compression16bpp, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);
#endif
}

static void test_validImage16bppBitmapV3Header ()
{
	BYTE image1x1RGB565[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x4A, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x46, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x38, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x10, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0x00, 0xF8, 0x00, 0x00,
		/* Green Mask */       0xE0, 0x07, 0x00, 0x00,
		/* Blue Mask */        0x1F, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xD7, 0xFE, 0x00, 0x00
	};
	BYTE image1x1RGB555[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x4A, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x46, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x38, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x10, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0x7C, 0x00, 0x00, 0x00,
		/* Green Mask */       0xE0, 0x03, 0x00, 0x00,
		/* Blue Mask */        0x1F, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xD7, 0xFE, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1RGB565, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: match GDI+ behaviour.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1RGB565Pixels[] = {0xFFFFB5BD};
	verifyPixels (image, image1x1RGB565Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1RGB555, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: match GDI+ behaviour.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1RGB555Pixels[] = {0xFFFFB5BD};
	verifyPixels (image, image1x1RGB555Pixels);
#endif
	GdipDisposeImage (image);
}

static void test_validImage16bppBitmapV4Header ()
{
	BYTE image1x1[] = {
		'B', 'M', 126, 0, 0, 0, 0, 0, 0, 0, 0x79, 0, 0, 0,
		108, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		B8(10101101), B8(01101010), 0, 0
	};
	BYTE image6x4[] = {
		'B', 'M', 126, 0, 0, 0, 0, 0, 0, 0, 0x79, 0, 0, 0,
		108, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000),
		B8(11111111), B8(11000000), B8(11111111), B8(11111110), B8(00000111), B8(11111110), B8(11111000), B8(00000000), B8(00000000), B8(00000000), B8(11111000), B8(00111110),
		B8(11111111), B8(11111110), B8(11111111), B8(11000000), B8(11111000), B8(00000000), B8(00000111), B8(11111110), B8(11111000), B8(00111110), B8(00000000), B8(00000000)
	};

	createFileSuccessDispose (image1x1, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1Pixels[] = {0xFFD6AD6B};
	verifyPixels (image, image1x1Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image6x4Pixels[] = {
        0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
        0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6,
        0xFFFFBDFF, 0xFF8439FF, 0xFF0039C6, 0xFFFF8439, 0xFF7BBDC6, 0xFF000000,
        0xFF8439FF, 0xFFFFBDFF, 0xFFFF8439, 0xFF0039C6, 0xFF000000, 0xFF7BBDC6
	};
	verifyPixels (image, image6x4Pixels);
#endif
	GdipDisposeImage (image);
}

static void test_validImage16bppBitmapV5Header ()
{
	BYTE image1x1RGB565[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x8E, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x8A, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x7C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x10, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3HEADER -- //
		/* Red Mask */         0x00, 0xF8, 0x00, 0x00,
		/* Green Mask */       0xE0, 0x07, 0x00, 0x00,
		/* Blue Mask */        0x1F, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV5CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV5Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV5GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV5GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV5GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV5HEADER --//
		/* bV5Intent */        0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileData */   0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileSize */   0x00, 0x00, 0x00, 0x00,
		/* bV5Reserved */      0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xD7, 0xFE, 0x00, 0x00
	};
	BYTE image1x1RGB555[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x8E, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x8A, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x7C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x10, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3HEADER -- //
		/* Red Mask */         0x7C, 0x00, 0x00, 0x00,
		/* Green Mask */       0xE0, 0x03, 0x00, 0x00,
		/* Blue Mask */        0x1F, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV5CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV5Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV5GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV5GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV5GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV5HEADER --//
		/* bV5Intent */        0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileData */   0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileSize */   0x00, 0x00, 0x00, 0x00,
		/* bV5Reserved */      0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xD7, 0xFE, 0x00, 0x00
	};

	createFileSuccessDispose (image1x1RGB565, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1RGB565Pixels[] = {0xFFFFB5BD};
	verifyPixels (image, image1x1RGB565Pixels);
#endif
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1RGB555, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: libgdiplus decodes this differently.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1RGB555Pixels[] = {0xFFFFB5BD};
	verifyPixels (image, image1x1RGB555Pixels);
#endif
	GdipDisposeImage (image);
}

static void test_validImage24bppOS2Header ()
{
	BYTE image1x1[] = {
		'B', 'M', 29, 0, 0, 0, 0, 0, 0, 0, 0x1A, 0, 0, 0,
		12, 0, 0, 0, 1, 0, 1, 0, 1, 0, 24, 0,
		0, 0, 255, 0
	};
	BYTE image6x4[] = {
		'B', 'M', 106, 0, 0, 0, 0, 0, 0, 0, 0x1A, 0, 0, 0,
		12, 0, 0, 0, 6, 0, 4, 0, 1, 0, 24, 0,
		0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 255, 0, 0,
		0, 0, 255, 255, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 255, 0, 255, 0, 0, 0,
		0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 255, 0, 0,
		0, 0, 255, 255, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 255, 0, 255, 0, 0, 0
	};

	createFileSuccessDispose (image1x1, PixelFormat24bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFF0000};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4, PixelFormat24bppRGB, 6, 4, bmpFlags, FALSE);
	ARGB image6x4Pixels[] = {
        0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000, 0xFFFFFF00, 0xFFFFFFFF, 0xFF00FF00,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF,
        0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000, 0xFFFFFF00, 0xFFFFFFFF, 0xFF00FF00,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF
	};
	verifyPixels (image, image6x4Pixels);
	GdipDisposeImage (image);
}

static void test_validImage24bppBitmapInfoHeader ()
{
	BYTE image1x1[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 255, 0
	};
	BYTE image6x4[] = {
		'B', 'M', 126, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 255, 0, 0,
		0, 0, 255, 255, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 255, 0, 255, 0, 0, 0,
		0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 255, 0, 0,
		0, 0, 255, 255, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 255, 0, 255, 0, 0, 0
	};
	BYTE image1x1NegativeHeight[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 255, 255, 255, 255, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 255, 0
	};
	BYTE image6x4NegativeHeight[] = {
		'B', 'M', 126, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 252, 255, 255, 255, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 255, 255, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 255, 0, 255, 0, 0, 0,
		0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 255, 0, 0,
		0, 0, 255, 255, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 255, 0, 255, 0, 0, 0,
		0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 255, 0, 0
	};

	BYTE rle4Compression24bpp[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 24, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 255, 255, 0
	};
	BYTE rle8Compression24bpp[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 24, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 255, 255, 0
	};

	createFileSuccessDispose (image1x1, PixelFormat24bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFF0000};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4, PixelFormat24bppRGB, 6, 4, bmpFlags, FALSE);
	ARGB image6x4Pixels[] = {
        0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000, 0xFFFFFF00, 0xFFFFFFFF, 0xFF00FF00,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF,
        0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000, 0xFFFFFF00, 0xFFFFFFFF, 0xFF00FF00,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF
	};
	verifyPixels (image, image6x4Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image1x1NegativeHeight, PixelFormat24bppRGB, 1, 1, bmpFlags, FALSE);
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4NegativeHeight, PixelFormat24bppRGB, 6, 4, bmpFlags, FALSE);
	verifyPixels (image, image6x4Pixels);
	GdipDisposeImage (image);

	// FIXME: GDI+ converts RLE compressed 24bpp images to 32bppRGB.
#if defined(USE_WINDOWS_GDIPLUS)
	PixelFormat expectedRleFormat = PixelFormat32bppRGB;
#else
	PixelFormat expectedRleFormat = PixelFormat24bppRGB;
#endif
	createFileSuccessDispose (rle4Compression24bpp, expectedRleFormat, 1, 1, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);

	createFileSuccessDispose (rle8Compression24bpp, expectedRleFormat, 1, 1, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);
}

static void test_validImage24bppBitmapV3Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x4A, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x46, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x38, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x18, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0x00, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0x00, 0x00,
		/* Blue Mask */        0x00, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xB9, 0xDA, 0xFF, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat24bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFFDAB9};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage24bppBitmapV4Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x7E, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x7A, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x6C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x18, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0x00, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0x00, 0x00,
		/* Blue Mask */        0x00, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV4CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV4Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV4GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV4GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV4GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xB9, 0xDA, 0xFF, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat24bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFFDAB9};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage24bppBitmapV5Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x8A, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x8A, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x6C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x18, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0x00, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0x00, 0x00,
		/* Blue Mask */        0x00, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV4CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV4Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV4GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV4GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV4GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV5HEADER --//
		/* bV5Intent */        0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileData */   0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileSize */   0x00, 0x00, 0x00, 0x00,
		/* bV5Reserved */      0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xB9, 0xDA, 0xFF, 0x00
	};

	createFileSuccessDispose (image1x1, PixelFormat24bppRGB, 1, 1, bmpFlags, FALSE);
	// FIXME: match GDI+ behaviour.
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB image1x1Pixels[] = {0xFFFFDAB9};
	verifyPixels (image, image1x1Pixels);
#endif
	GdipDisposeImage (image);
}

static void test_validImage32bppOS2Header ()
{
	BYTE image1x1[] = {
		'B', 'M', 30, 0, 0, 0, 0, 0, 0, 0, 0x1A, 0, 0, 0,
		12, 0, 0, 0, 1, 0, 1, 0, 1, 0, 32, 0,
		0, 0, 255, 255
	};
	BYTE image6x4[] = {
		'B', 'M', 122, 0, 0, 0, 0, 0, 0, 0, 0x1A, 0, 0, 0,
		12, 0, 0, 0, 6, 0, 4, 0, 1, 0, 32, 0,
		0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 127,
		0, 0, 255, 0, 255, 255, 255, 255, 0, 0, 255, 255, 0, 255, 255, 255, 255, 255, 127, 255, 0, 255, 0, 255,
		0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 127,
		0, 0, 255, 0, 255, 255, 255, 255, 0, 0, 255, 255, 0, 255, 255, 255, 255, 255, 127, 255, 0, 255, 0, 255
	};

	createFileSuccessDispose (image1x1, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFF0000};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	ARGB image6x4Pixels[] = {
        0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000, 0xFFFFFF00, 0xFF7FFFFF, 0xFF00FF00,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF,
        0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000, 0xFFFFFF00, 0xFF7FFFFF, 0xFF00FF00,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF
	};
	verifyPixels (image, image6x4Pixels);
	GdipDisposeImage (image);
}

static void test_validImage32bppBitmapInfoHeader ()
{
	BYTE image1x1[] = {
		'B', 'M', 59, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 255, 255
	};
	BYTE image6x4[] = {
		'B', 'M', 150, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 127,
		0, 0, 255, 0, 255, 255, 255, 255, 0, 0, 255, 255, 0, 255, 255, 255, 255, 255, 127, 255, 0, 255, 0, 255,
		0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 127,
		0, 0, 255, 0, 255, 255, 255, 255, 0, 0, 255, 255, 0, 255, 255, 255, 255, 255, 127, 255, 0, 255, 0, 255
	};
	BYTE image6x4NegativeHeight[] = {
		'B', 'M', 150, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 6, 0, 0, 0, 252, 255, 255, 255, 1, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 255, 0, 255, 255, 255, 255, 0, 0, 255, 255, 0, 255, 255, 255, 255, 255, 127, 255, 0, 255, 0, 255,
		0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 127,
		0, 0, 255, 0, 255, 255, 255, 255, 0, 0, 255, 255, 0, 255, 255, 255, 255, 255, 127, 255, 0, 255, 0, 255,
		0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 127
	};

	BYTE planesNotZero[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 4, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 255, 255
	};
	BYTE rle4Compression32bpp[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, BI_RLE4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 255, 255, 0
	};
	BYTE rle8Compression32bpp[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, BI_RLE8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 255, 255, 0
	};

	createFileSuccessDispose (image1x1, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFF0000};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	ARGB image6x4Pixels[] = {
        0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000, 0xFFFFFF00, 0xFF7FFFFF, 0xFF00FF00,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF,
        0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000, 0xFFFFFF00, 0xFF7FFFFF, 0xFF00FF00,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF
	};
	verifyPixels (image, image6x4Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (image6x4NegativeHeight, PixelFormat32bppRGB, 6, 4, bmpFlags, FALSE);
	verifyPixels (image, image6x4Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (planesNotZero, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (rle4Compression32bpp, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);

	createFileSuccessDispose (rle8Compression32bpp, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);
}

static void test_validImage32bppBitmapV3Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x4A, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x46, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x38, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x20, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0x00, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0x00, 0x00,
		/* Blue Mask */        0x00, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xB9, 0xDA, 0xFF, 0X7F
	};

	createFileSuccessDispose (image1x1, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFFDAB9};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage32bppBitmapV4Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x7E, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x7A, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x6C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x20, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0x00, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0x00, 0x00,
		/* Blue Mask */        0x00, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV4CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV4Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV4GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV4GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV4GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xB9, 0xDA, 0xFF, 0X7F
	};

	createFileSuccessDispose (image1x1, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFFDAB9};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage32bppBitmapV5Header ()
{
	BYTE image1x1[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x8A, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x8A, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x7C, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x20, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3INFOHEADER -- //
		/* Red Mask */         0x00, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0x00, 0x00,
		/* Blue Mask */        0x00, 0x00, 0x00, 0x00,
		/* Alpha Mask */       0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4HEADER --//
		/* bV4CSType */        0x00, 0x00, 0x00, 0x00,
		/* bV4Endpoints */     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* bV4GammaRed */      0x00, 0x00, 0x00, 0x00,
		/* bV4GammaGreen */    0x00, 0x00, 0x00, 0x00,
		/* bV4GammaBlue */     0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV5HEADER --//
		/* bV5Intent */        0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileData */   0x00, 0x00, 0x00, 0x00,
		/* bV5ProfileSize */   0x00, 0x00, 0x00, 0x00,
		/* bV5Reserved */      0x00, 0x00, 0x00, 0x00,
		// -- Image Data --/
		0xB9, 0xDA, 0xFF, 0X7F
	};

	createFileSuccessDispose (image1x1, PixelFormat32bppRGB, 1, 1, bmpFlags, FALSE);
	ARGB image1x1Pixels[] = {0xFFFFDAB9};
	verifyPixels (image, image1x1Pixels);
	GdipDisposeImage (image);
}

static void test_validImage32bppBitfields ()
{
#if defined(USE_WINDOWS_GDIPLUS)
	// Order: B, G, R
	BYTE bitmapInfoHeaderBitfieldsDefault[] = {
		'B', 'M', 98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0x00, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		0xFF, 0x00, 0x00, 0x00,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	// Order: R, B, G
	BYTE bitmapInfoHeaderBitfieldsCustom[] = {
		'B', 'M', 98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	// Order: R, B, G
	BYTE bitmapV3HeaderBitfieldsZeroAlphaMask[] = {
		'B', 'M', 98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00,
		56, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		// -- BITMAPV3 -- //
		0x00, 0x00, 0x00, 0x00,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	// Order: R, B, G
	BYTE bitmapV3HeaderBitfieldsNonZeroAlphaMask[] = {
		'B', 'M', 98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00,
		56, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		// -- BITMAPV3 -- //
		0x00, 0x00, 0x00, 0xFF,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	// Order: R, B, G
	BYTE bitmapV4HeaderBitfields[] = {
		'B', 'M', 154, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x00, 0x00, 0x00,
		108, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		// -- BITMAPV3 -- //
		0x00, 0x00, 0x00, 0xFF,
		// -- BITMAPV4 -- //
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	// Order: R, B, G
	BYTE bitmapV5HeaderBitfields[] = {
		'B', 'M', 154, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8A, 0x00, 0x00, 0x00,
		124, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		// -- BITMAPV3 -- //
		0x00, 0x00, 0x00, 0xFF,
		// -- BITMAPV4 -- //
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BitmapV5 -- //
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	BYTE bitmapInfoHeaderAllMasksZero[] = {
		'B', 'M', 98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	BYTE bitmapV3HeaderAllMasksZero[] = {
		'B', 'M', 98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00,
		56, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3 -- //
		0x00, 0x00, 0x00, 0x00,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	BYTE bitmapV4HeaderAllMasksZero[] = {
		'B', 'M', 154, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x00, 0x00, 0x00,
		108, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3 -- //
		0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4 -- //
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	BYTE bitmapV5HeaderAllMasksZero[] = {
		'B', 'M', 154, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8A, 0x00, 0x00, 0x00,
		124, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BITMAPV2 -- //
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV3 -- //
		0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV4 -- //
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// -- BitmapV5 -- //
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// ---- Bitmap Data ---- //
		0xFF, 0x00, 0x00, 0x7F, 0x00, 0xFF, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};
#endif

	// FIXME: libgdiplus can't decode these images.
#if defined(USE_WINDOWS_GDIPLUS)
	createFileSuccessDispose (bitmapInfoHeaderBitfieldsDefault, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	ARGB bitmapInfoHeaderBitfieldsDefaultPixels[] = {
		0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFFFF,
		0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFFFF
	};
	verifyPixels (image, bitmapInfoHeaderBitfieldsDefaultPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (bitmapInfoHeaderBitfieldsCustom, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	ARGB customBitfieldsPixels[] = {
        0xFFFF0000, 0xFF0000FF, 0xFF00FF00, 0xFFFFFFFF,
        0xFFFF0000, 0xFF0000FF, 0xFF00FF00, 0xFFFFFFFF
	};
	verifyPixels (image, customBitfieldsPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (bitmapV3HeaderBitfieldsNonZeroAlphaMask, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	verifyPixels (image, customBitfieldsPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (bitmapV4HeaderBitfields, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	verifyPixels (image, customBitfieldsPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (bitmapV5HeaderBitfields, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	verifyPixels (image, customBitfieldsPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (bitmapInfoHeaderAllMasksZero, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);

	createFileSuccessDispose (bitmapV3HeaderAllMasksZero, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);

	createFileSuccessDispose (bitmapV4HeaderAllMasksZero, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);

	createFileSuccessDispose (bitmapV5HeaderAllMasksZero, PixelFormat32bppRGB, 4, 2, bmpFlags, FALSE);
	// Don't verify the pixels, as the pixel data changes every time the program is run
	// due to a bug in GDI+.
	GdipDisposeImage (image);
#endif
}

static void test_valid ()
{
	BYTE nonZeroHorizontal[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x42, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x3E, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x28, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x01, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};
	BYTE nonZeroVertical[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x42, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x3E, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x28, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x01, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};
	BYTE nonZeroHorizontalNonZeroVertical[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x42, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x3E, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x28, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x01, 0x00, 0x00, 0x00,
		/* Vertical */         0x01, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};

	createFileSuccessDispose (nonZeroHorizontal, PixelFormat1bppIndexed, 1, 1, bmpFlags, FALSE);
	ARGB nonZeroHorizontalPixels[] = {0xFF00FF00};
	verifyPixels (image, nonZeroHorizontalPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (nonZeroVertical, PixelFormat1bppIndexed, 1, 1, bmpFlags, FALSE);
	verifyPixels (image, nonZeroHorizontalPixels);
	GdipDisposeImage (image);

	createFileSuccessDispose (nonZeroHorizontalNonZeroVertical, PixelFormat1bppIndexed, 1, 1, bmpFlags | ImageFlagsHasRealDPI, FALSE);
	verifyPixels (image, nonZeroHorizontalPixels);
	GdipDisposeImage (image);
}

static void test_invalidFileHeader ()
{
	BYTE shortSignature[] = {'B'};
	BYTE noImageSize[]    = {'B', 'M'};
	BYTE shortImageSize[] = {'B', 'M', 5, 0, 0};
	BYTE noReserved1[]    = {'B', 'M', 6, 0, 0, 0};
	BYTE shortReserved1[] = {'B', 'M', 7, 0, 0, 0, 0};
	BYTE noReserved2[]    = {'B', 'M', 8, 0, 0, 0, 0, 0};
	BYTE shortReserved2[] = {'B', 'M', 9, 0, 0, 0, 0, 0, 0};
	BYTE noOffset[]       = {'B', 'M', 10, 0, 0, 0, 0, 0, 0, 0};
	BYTE shortOffset[]    = {'B', 'M', 13, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0};

	createFile (shortSignature, OutOfMemory);
	createFile (noImageSize, OutOfMemory);
	createFile (shortImageSize, OutOfMemory);
	createFile (noReserved1, OutOfMemory);
	createFile (shortReserved1, OutOfMemory);
	createFile (noReserved2, OutOfMemory);
	createFile (shortReserved2, OutOfMemory);
	createFile (noOffset, OutOfMemory);
	createFile (shortOffset, OutOfMemory);
}

static void test_invalidHeader ()
{
	BYTE noHeader[]               = {'B', 'M', 14, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0};
	BYTE zeroHeaderSize[]         = {'B', 'M', 18, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE newFormatNoWidth[]       = {'B', 'M', 18, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0};
	BYTE newFormatNoHeight[]      = {'B', 'M', 22, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0};
	BYTE os2FormatNoWidthHeight[] =	{'B', 'M', 18, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 12, 0, 0, 0};
	BYTE noColorPlanes[]          = {'B', 'M', 26, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
	BYTE noBitsPerPixel[]         = {'B', 'M', 28, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
	BYTE noCompression[]          = {'B', 'M', 30, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0};
	BYTE noSize[]                 = {'B', 'M', 34, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
	BYTE noHorizontalResolution[] = {'B', 'M', 38, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0};
	BYTE noVerticalResolution[]   = {'B', 'M', 42, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0};
	BYTE noColors[]               = {'B', 'M', 46, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE noImportantColors[]      = {'B', 'M', 50, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE image2bpp[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 255, 255,
		B8(00000000), 0, 0, 0
	};

	BYTE image16bppNoRedMask[] = {
		'B', 'M', 54, 0, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	BYTE image16bppShortRedMask[] = {
		'B', 'M', 57, 0, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0
	};
	BYTE image16bppNoGreenMask[] = {
		'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0
	};
	BYTE image16bppShortGreenMask[] = {
		'B', 'M', 61, 0, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0
	};
	BYTE image16bppNoBlueMask[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0
	};
	BYTE image16bppShortBlueMask[] = {
		'B', 'M', 65, 0, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 3, 0, 0
	};
	BYTE image16bppNoData[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 53, 4, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0x1F, 0, 0, 0
	};

	BYTE bitfields32bppNoRedMask[] = {
		'B', 'M', 54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};
	BYTE bitfields32bppShortRedMask[] = {
		'B', 'M', 57, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0
	};
	BYTE bitfields32bppNoBlueMask[] = {
		'B', 'M', 58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0
	};
	BYTE bitfields32bppShortBlueMask[] = {
		'B', 'M', 61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0
	};
	BYTE bitfields32bppNoGreenMask[] = {
		'B', 'M', 62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0
	};
	BYTE bitfields32bppShortGreenMask[] = {
		'B', 'M', 65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0, 3, 0, 0
	};
	BYTE bitfields32bppNoImageData[] = {
		'B', 'M', 66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
		40, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 1, 0, 32, 0, BI_BITFIELDS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 124, 0, 0, 224, 3, 0, 0, 0, 3, 0, 0, 0
	};

	createFile (noHeader, OutOfMemory);
	createFile (zeroHeaderSize, OutOfMemory);
	createFile (newFormatNoWidth, OutOfMemory);
	createFile (newFormatNoHeight, OutOfMemory);
	createFile (os2FormatNoWidthHeight, OutOfMemory);
	createFile (noColorPlanes, OutOfMemory);
	createFile (noBitsPerPixel, OutOfMemory);
	createFile (noCompression, OutOfMemory);
	createFile (noSize, OutOfMemory);
	createFile (noHorizontalResolution, OutOfMemory);
	createFile (noVerticalResolution, OutOfMemory);
	createFile (noColors, OutOfMemory);
	createFile (noImportantColors, OutOfMemory);
	createFile (image2bpp, OutOfMemory);

	createFile (image16bppNoRedMask, OutOfMemory);
	createFile (image16bppShortRedMask, OutOfMemory);
	createFile (image16bppNoGreenMask, OutOfMemory);
	createFile (image16bppShortGreenMask, OutOfMemory);
	createFile (image16bppNoBlueMask, OutOfMemory);
	createFile (image16bppShortBlueMask, OutOfMemory);
	createFile (image16bppNoData, OutOfMemory);

	createFile (bitfields32bppNoRedMask, OutOfMemory);
	createFile (bitfields32bppShortRedMask, OutOfMemory);
	createFile (bitfields32bppNoBlueMask, OutOfMemory);
	createFile (bitfields32bppShortBlueMask, OutOfMemory);
	createFile (bitfields32bppNoGreenMask, OutOfMemory);
	createFile (bitfields32bppShortGreenMask, OutOfMemory);
	createFile (bitfields32bppNoImageData, OutOfMemory);
}

static void test_invalidHeaderSize ()
{
	BYTE zeroHeaderSize[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x42, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x3E, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x00, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};
	BYTE largeHeaderSize[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x42, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x3E, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0xFF, 0xFF, 0xFF, 0xFF,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};
	BYTE os22xHeader[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x5A, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x56, 0x00, 0x00, 0x00,
		// -- Os22xBitmapHeader
		/* Header Size */      0x40, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		/* Units */            0x00, 0x00,
		/* Reserved */         0x00, 0x00,
		/* Recording */        0x00, 0x00,
		/* Rendering */        0x00, 0x00,
		/* Size1 */            0x00, 0x00, 0x00, 0x00,
		/* Size2 */            0x00, 0x00, 0x00, 0x00,
		/* ColorEncoding */    0x00, 0x00, 0x00, 0x00,
		/* Identifier */       0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};
	BYTE v2Header[] = {
		// -- BITMAPCOREHEADER -- //
		/* Signature */ 0x42, 0x4D,
		/* File Size */ 0x4E, 0x00, 0x00, 0x00,
		/* Reserved */  0x00, 0x00, 0x00, 0x00,
		/* Offset */    0x4A, 0x00, 0x00, 0x00,
		// -- BITMAPINFOHEADER
		/* Header Size */      0x34, 0x00, 0x00, 0x00,
		/* Width */            0x01, 0x00, 0x00, 0x00,
		/* Height */           0x01, 0x00, 0x00, 0x00,
		/* Planes */           0x01, 0x00,
		/* Bit Count */        0x01, 0x00,
		/* Compression */      0x00, 0x00, 0x00, 0x00,
		/* Image Size */       0x00, 0x00, 0x00, 0x00,
		/* Horizontal */       0x00, 0x00, 0x00, 0x00,
		/* Vertical */         0x00, 0x00, 0x00, 0x00,
		/* Colors Used */      0x00, 0x00, 0x00, 0x00,
		/* Important Colors */ 0x00, 0x00, 0x00, 0x00,
		// -- BITMAPV2INFOHEADER -- //
		/* Red Mask */         0x00, 0x00, 0x00, 0x00,
		/* Green Mask */       0x00, 0x00, 0x00, 0x00,
		/* Blue Mask */        0x00, 0x00, 0x00, 0x00,
		// -- Color Palette -- //
		0xFF, 0x00, 0x00, 0x7F,
		0x00, 0xFF, 0x00, 0xFF,
		// -- Image Data --/
		0x80, 0x00, 0x00, 0x00
	};

	createFile (zeroHeaderSize, OutOfMemory);
	createFile (largeHeaderSize, OutOfMemory);
	createFile (os22xHeader, OutOfMemory);
	createFile (v2Header, OutOfMemory);
}

static void test_invalidImageData ()
{
	BYTE noColorEntries[]         = {'B', 'M', 54, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE notEnoughColorEntries[]  = {'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE noImageData[]            = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0};
	BYTE noImageDataBigSize[]     = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0};
	BYTE hasImageData4bpp[]       = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasImageData8bpp[]       = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE invalidBitsPerPixel[]    = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE bitfieldsNon16Bpp[]      = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE bitfields16Bpp[]         = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE notPaddedTo4Bytes[]      = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		B8(10000000)
	};
	BYTE missingNonFinalFinalLine[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		0x00, 0x00, 0x00, 0x00
	};
	BYTE missingFinalLine[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	createFile (noColorEntries, OutOfMemory);
	createFile (notEnoughColorEntries, OutOfMemory);
	createFile (noImageData, OutOfMemory);
	createFile (noImageDataBigSize, OutOfMemory);
	createFile (hasImageData4bpp, OutOfMemory);
	createFile (hasImageData8bpp, OutOfMemory);
	createFile (invalidBitsPerPixel, OutOfMemory);
	createFile (bitfieldsNon16Bpp, OutOfMemory);
	createFile (bitfields16Bpp, OutOfMemory);
	createFile (notPaddedTo4Bytes, OutOfMemory);
	createFile (missingNonFinalFinalLine, OutOfMemory);
	createFile (missingFinalLine, OutOfMemory);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_validImage1bppOS2Header ();
	test_validImage1bppBitmapInfoHeader ();
	test_validImage1bppBitmapV3Header ();
	test_validImage1bppBitmapV4Header ();
	test_validImage1bppBitmapV5Header ();
	test_validImage4bppOS2Header ();
	test_validImage4bppBitmapInfoHeader ();
	test_validImage4bppBitmapV3Header ();
	test_validImage4bppBitmapV4Header ();
	test_validImage4bppBitmapV5Header ();
	test_validImage4bppRle4Compression ();
	test_validImage8bppOS2Header ();
	test_validImage8bppBitmapInfoHeader ();
	test_validImage8bppBitmapV3Header ();
	test_validImage8bppBitmapV4Header ();
	test_validImage8bppBitmapV5Header ();
	test_validImage8bppRle8Compression ();
	test_validImage16bppOS2Header ();
	test_validImage16bppBitmapInfoHeader ();
	test_validImage16bppBitmapV3Header ();
	test_validImage16bppBitmapV4Header ();
	test_validImage16bppBitmapV5Header ();
	test_validImage24bppOS2Header ();
	test_validImage24bppBitmapInfoHeader ();
	test_validImage24bppBitmapV3Header ();
	test_validImage24bppBitmapV4Header ();
	test_validImage24bppBitmapV5Header ();
	test_validImage32bppOS2Header ();
	test_validImage32bppBitmapInfoHeader ();
	test_validImage32bppBitmapV3Header ();
	test_validImage32bppBitmapV4Header ();
	test_validImage32bppBitmapV5Header ();
	test_validImage32bppBitfields ();
	test_invalidFileHeader ();
	test_invalidHeader ();
	test_invalidHeaderSize ();
	test_invalidImageData ();
	test_valid ();

	deleteFile (file);

	SHUTDOWN;
	return 0;
}
