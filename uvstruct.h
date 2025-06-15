#ifndef UVSTRUCT_H
#define UVSTRUCT_H

//===========================================================================
//
//    FILENAME: uvstruct.h [D:\PROJECTS\PROPHECY\SRC\PLATFORM\WIN95\UV2\]
//   COPYRIGHT: (C) 1997 BY Origin
//
// DESCRIPTION: structures used for UV2
//      AUTHOR: Martin Griffiths (original author) [mod by Hideki Ikeda]
//        DATE: 07/22/97 21:39:25
//
//     HISTORY: 07/22/97 [HAI] - File created
//            :
//
//===========================================================================

// Options
#define X_OFFSET            (1)
#define Y_OFFSET            (2)
#define X_CENTRE            (4)
#define Y_CENTRE            (8)
#define Y_DOUBLE            (16)
#define Y_SKIP              (32)
#define X_DOUBLE            (64)
#define X_SKIP              (128)

typedef unsigned int		u_int;
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned short		Pixel16;
typedef unsigned char		u_int8;
typedef float				real;
#define false				FALSE
#define true				TRUE

enum VIDEO_TYPE
{
	TGV_KEY_FRAME = 1,
	TGV_MOTION_FRAME,
	UV_KEY_FRAME,
	UV2_KEY_FRAME
};

struct PIXELFORMAT_DESC
{
	u_int Rbits,Gbits,Bbits;
	u_int Rshifts,Gshifts,Bshifts;
	u_int RGBmask;
	u_int BitsPerPixel;
};

struct uv_options
{
	u_int        option_flags;
	int          x_offset;
	int          y_offset;
};

struct uv_instance
{
	int             gamma;
	u_int           height,width;
	u_char         *curr_buffer;
	u_char         *last_buffer;
	int             quality_index;
	int             no_adjusted_yuv;
	u_int           no_motion;

	uv_options      options;

	PIXELFORMAT_DESC pixformat;

	long            dc_dct_pred[3];
};

//===========================================================================

#endif