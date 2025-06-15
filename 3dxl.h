#include "F18.h"

BOOL _3dxlGrCopyRectNegativeMask8Color(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color);
BOOL _3dxlGrCopyRectMask8Color(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color);
BOOL _3dxlGrCopyRectMask8ColorCmpVal(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color, int CmpVal);
BOOL _3dxlGrCopyRectMask8Fast(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh);
void _3dxlSetupNavFlirZValues(float &z_value,float &ooz_value,float &oow_value,float &light_value);
void _3dxlDisplayPage(GrBuff *buff);
float _3dxlMaxBulletAsLineDistance();
void _3dxlPrepareForSpecialPalette(unsigned char *pal,int table,int FXID);
void _3dxlFinishSpecialPalette(unsigned char *pal);
void _3dxlBeginFrame();
void _3dxlEndFrame();