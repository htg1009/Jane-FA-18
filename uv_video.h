//===========================================================================
//
//    FILENAME: video.h [D:\projects\prophecy\src\platform\win95\uv2\]
//   COPYRIGHT: (C) 1997 BY Origin
//
// DESCRIPTION: 
//      AUTHOR: Martin Griffiths (original author)
//        DATE: 07/11/97 17:40:45
//
//     HISTORY: 07/11/97 [HAI] - File created
//            : 
//
//===========================================================================

#ifndef UV_VIDEO_H
#define UV_VIDEO_H

//========================================================== include files ==

struct uv_instance;
struct video_buffer;

//========================================================== include files ==

class UV2Video
{
public:
	static void initVideoTables(uv_instance *uv);
	static void decodeVideoBuffer(uv_instance *uv, unsigned char *src);
};

//===========================================================================

#endif

