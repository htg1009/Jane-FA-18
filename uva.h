#ifndef UVA_H
#define UVA_H

#ifdef __cplusplus
extern "C"
{
#endif

extern unsigned long uvTableY[128];      // Y lookup table
extern unsigned long uvTableCb[128];     // Cb lookup table
extern unsigned long uvTableCr[128];     // Cr lookup table

extern unsigned int  crv[256];
extern unsigned int  cbu[256];
extern unsigned int  cgu[256];
extern unsigned int  *cgv[256];

extern unsigned int clpR[512];
extern unsigned int clpG[512];
extern unsigned int clpB[512];

extern long uvQuant[64];// IDCT quantization matrix. Incorporates the IDCT prescale values.

#if 0
extern long uv_Y[16][16];
extern long uv_Cr[8][8];
extern long uv_Cb[8][8];
#endif

void uvdecodeblock( unsigned char *, unsigned long*,int);
void uvdecodeblock16( unsigned char *, unsigned short*,int);
void init_uv2decode(unsigned char *);

void uv2decodeblock(  unsigned long*,int);
void uv2decodeblock_ycbcr(  unsigned long*,int);

int _cdecl checkinter_ycbcr(int *,int*);
void _cdecl uv2decblk_yuv16 ( unsigned short*,int,long *);
void _cdecl uv2decblk_yuv32( unsigned int*,int,long *);

void _cdecl uv2decodeblock16(  unsigned short*,int,long *);
void _cdecl uv2decodeblock32(  unsigned int*,int,long *);

#ifdef __cplusplus
};
#endif

#endif
