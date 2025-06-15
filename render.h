#ifndef __RENDER_H

#define __RENDER_H

#define RenderHeader ObjectSortHeader
#define _RenderHeader _ObjectSortHeader

#define UNUSED_OBJECT	    (0 | OBJ_SORT_GAME_DRAWN)
#define PLANE_OBJECT        (1 | OBJ_SORT_GAME_DRAWN)
#define BULLET_OBJECT       (2 | OBJ_SORT_GAME_DRAWN)
#define SMOKE_TRAIL_OBJECT	(3 | OBJ_SORT_GAME_DRAWN | OBJ_SORT_HAS_EXTENTS)
#define SPRITE_OBJECT       (4 | OBJ_SORT_GAME_DRAWN)
#define SHADOW_OBJECT		(5 | OBJ_SORT_GAME_DRAWN | OBJ_SORT_LIES_FLAT | OBJ_SORT_MULTI_TILE_OBJECT)
#define BOMB_OBJECT			(6 | OBJ_SORT_GAME_DRAWN)
#define MISSILE_OBJECT		(7 | OBJ_SORT_GAME_DRAWN)
#define FUNNEL_OBJECT		(8 | OBJ_SORT_GAME_DRAWN)
#define AAA_OBJECT			(9 | OBJ_SORT_GAME_DRAWN)
#define SHOCKWAVE_OBJECT	(10 | OBJ_SORT_GAME_DRAWN | OBJ_SORT_LIES_FLAT | OBJ_SORT_MULTI_TILE_OBJECT)
#define CRATER_OBJECT		(11 | OBJ_SORT_GAME_DRAWN | OBJ_SORT_LIES_FLAT | OBJ_SORT_MULTI_TILE_OBJECT)
#define VEHICLE_OBJECT		(12 | OBJ_SORT_GAME_DRAWN)
#define PARTICLE_OBJECT		(13 | OBJ_SORT_GAME_DRAWN)
#define WAYPOINT_OBJECT		(14 | OBJ_SORT_GAME_DRAWN)
#define LANDING_GATE_OBJECT	(15 | OBJ_SORT_GAME_DRAWN)



#define GAME_OBJECT			(0xFFF)

void InitRenderer(void);
void CloseRenderer(void);
void F18Sorter(ObjectSortHeader **tree,ObjectSortHeader *object);
void F18Drawer(ObjectSortHeader *draw_guy);
void Draw3DWorld(CameraInstance *camera);
int LoadPlaneModel(PlaneType *type,char *path_to_model = NULL);
void PauseForDiskHit(void);
void UnPauseFromDiskHit(void);
void RenderBullet(FPoint &location,FPointDouble &vel,BOOL red_tracer);
void SetF18Software(void);
void SetF18Hardware(void);
void DrawLimitedViewWithRoll(GrBuff *buffer,FPointDouble &eye,float heading, float pitch, float roll, ANGLE aperture, BOOL process_to_green, BOOL draw_as_flir, BOOL draw_green_hot = TRUE,BOOL high_detail_only = TRUE,BOOL draw_textures = TRUE,int dot = 0, void *weap = NULL);
void DrawLimitedView(GrBuff *buffer,FPointDouble &eye,float heading, float pitch, ANGLE aperture, BOOL process_to_green, BOOL draw_as_flir, BOOL draw_green_hot = TRUE,BOOL high_detail_only = TRUE,BOOL draw_textures = TRUE,int dot = 0, void *weap = NULL);
void CockpitLightOff();
void CockpitLightOn();
void DrawWeapon(void *W,FPoint &location);
void CalculateRefuelHosePosition(PlaneParams *P,int number);

void GetMatrixForMissileCam(ANGLE r,ANGLE p,ANGLE h,FMatrix &platform_mat,FMatrix &result);
void DrawLimitedViewMatrix(GrBuff *buffer,FPointDouble &eye,FMatrix &matrix,ANGLE aperture, BOOL process_to_green, BOOL draw_as_flir, BOOL draw_green_hot = TRUE,BOOL high_detail_only = TRUE,BOOL draw_textures = TRUE,int dot = 0, void *weap = NULL);


#endif