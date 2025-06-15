#ifndef __CLOUDS_H__
#define __CLOUDS_H__



void DrawSpherePreBuilt( float Radius, float hemisphere, FPoint &RelPosition, float alpha, FPoint diffuse, int gop, FMatrix *pMat=NULL);
void BuildSphere( int segments );
void CalcSegsAlpha( float dist, float *pSegs, float *pAlpha);
void SetUVScaleOffset( float Uscale, float Uoffset, float Vscale, float Voffset);

void ProcessClouds( void );
void DisplayClouds( CameraInstance *camera );

void InitRain( void);
void InitClouds( void );
void DisplayRain( void  );



void MoveRain( void);

#endif