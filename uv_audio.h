//===========================================================================
//
// uv_audio.h
//
//===========================================================================

#ifndef UV_AUDIO_H
#define UV_AUDIO_H

//========================================================== include files ==

struct uv_instance;

//========================================================== include files ==

class UV2Audio
{
public:
	static void decodemxa(int numframes, int *psample1,int *psample2, const unsigned char *psrc, void *ppdst);
	static void decodesxa(int numframes, int *plsample1, int *plsample2, int *prsample1, int *prsample2, const unsigned char *psrc, void *ppdst);
};

//===========================================================================

#endif

