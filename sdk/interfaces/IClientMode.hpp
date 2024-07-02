#pragma once

#include "../math/VMatrix.hpp"

class IPanel;
class Entity;

enum class ClearFlags_t
{
    VIEW_CLEAR_COLOR = 0x1,
    VIEW_CLEAR_DEPTH = 0x2,
    VIEW_CLEAR_FULL_TARGET = 0x4,
    VIEW_NO_DRAW = 0x8,
    VIEW_CLEAR_OBEY_STENCIL = 0x10,
    VIEW_CLEAR_STENCIL = 0x20,
};

enum class MotionBlurMode_t
{
    MOTION_BLUR_DISABLE = 1,
    MOTION_BLUR_GAME = 2,
    MOTION_BLUR_SFM = 3
};
/*
class CViewSetup
{
public:
    __int32   x;                  //0x0000 
    __int32   x_old;              //0x0004 
    __int32   y;                  //0x0008 
    __int32   y_old;              //0x000C 
    __int32   width;              //0x0010 
    __int32   width_old;          //0x0014 
    __int32   height;             //0x0018 
    __int32   height_old;         //0x001C 
    bool	  bOrtho;
    std::byte	pad0[0x8F];
    float     fov;                //0x00B0 
    float     viewmodel_fov;      //0x00B4 
    Vector    origin;             //0x00B8 
    Vector    angles;             //0x00C4 
    float		flNearZ;
    float		flFarZ;
    float		flNearViewmodelZ;
    float		flFarViewmodelZ;
    float		flAspectRatio;
    float		flNearBlurDepth;
    float		flNearFocusDepth;
    float		flFarFocusDepth;
    float		flFarBlurDepth;
    float		flNearBlurRadius;
    float		flFarBlurRadius;
    float		flDoFQuality;
    int			nMotionBlurMode;
    float		flShutterTime;
    Vector		vecShutterOpenPosition;
    Vector		vecShutterOpenAngles;
    Vector		vecShutterClosePosition;
    Vector		vecShutterCloseAngles;
    float		flOffCenterTop;
    float		flOffCenterBottom;
    float		flOffCenterLeft;
    float		flOffCenterRight;
    bool		bOffCenter : 1;
    bool		bRenderToSubrectOfLargerScreen : 1;
    bool		bDoBloomAndToneMapping : 1;
    bool		bDoDepthOfField : 1;
    bool		bHDRTarget : 1;
    bool		bDrawWorldNormal : 1;
    bool		bCullFontFaces : 1;
    bool		bCacheFullSceneState : 1;
    bool		bCSMView : 1;

};//Size=0x014C
*/
class CViewSetup
{
public:
    int			x;
    int			x_old;
    int			y;
    int			y_old;
    int			width;
    int			width_old;
    int			height;
    int			height_old;
    bool		bOrtho;
    std::byte	pad0[0x8F];
    float		fov;
    float		viewmodel_fov;
    Vector		origin;
    Vector		angles;
    float		flNearZ;
    float		flFarZ;
    float		flNearViewmodelZ;
    float		flFarViewmodelZ;
    float		flAspectRatio;
    float		flNearBlurDepth;
    float		flNearFocusDepth;
    float		flFarFocusDepth;
    float		flFarBlurDepth;
    float		flNearBlurRadius;
    float		flFarBlurRadius;
    float		flDoFQuality;
    int			nMotionBlurMode;
    float		flShutterTime;
    Vector		vecShutterOpenPosition;
    Vector		vecShutterOpenAngles;
    Vector		vecShutterClosePosition;
    Vector		vecShutterCloseAngles;
    float		flOffCenterTop;
    float		flOffCenterBottom;
    float		flOffCenterLeft;
    float		flOffCenterRight;
    bool		bOffCenter : 1;
    bool		bRenderToSubrectOfLargerScreen : 1;
    bool		bDoBloomAndToneMapping : 1;
    bool		bDoDepthOfField : 1;
    bool		bHDRTarget : 1;
    bool		bDrawWorldNormal : 1;
    bool		bCullFontFaces : 1;
    bool		bCacheFullSceneState : 1;
    bool		bCSMView : 1;
};





class IClientMode
{
public:
    virtual             ~IClientMode() {}
    virtual int         ClientModeCSNormal(void *) = 0;
    virtual void        Init() = 0;
    virtual void        InitViewport() = 0;
    virtual void        Shutdown() = 0;
    virtual void        Enable() = 0;
    virtual void        Disable() = 0;
    virtual void        Layout() = 0;
    virtual IPanel*     GetViewport() = 0;
    virtual void*       GetViewportAnimationController() = 0;
    virtual void        ProcessInput(bool bActive) = 0;
    virtual bool        ShouldDrawDetailObjects() = 0;
    virtual bool        ShouldDrawEntity(Entity *e) = 0;
    virtual bool        ShouldDrawLocalPlayer(Entity *pPlayer) = 0;
    virtual bool        ShouldDrawParticles() = 0;
    virtual bool        ShouldDrawFog(void) = 0;
    virtual void        OverrideView(CViewSetup *pSetup) = 0;
    virtual int         KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0;
    virtual void        StartMessageMode(int iMessageModeType) = 0;
    virtual IPanel*     GetMessagePanel() = 0;
    virtual void        OverrideMouseInput(float *x, float *y) = 0;
    virtual bool        CreateMove(float flInputSampleTime, void* usercmd) = 0;
    virtual void        LevelInit(const char *newmap) = 0;
    virtual void        LevelShutdown(void) = 0;
};