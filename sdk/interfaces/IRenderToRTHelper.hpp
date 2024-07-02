#pragma once
#include "VTFTexture.h"
class IRenderToRTHelperObject
{
public:
	virtual void Draw(const matrix3x4_t& matRootToWorld) = 0;
	virtual bool BoundingSphere(Vector& vecCenter, float& flRadius) = 0;
	virtual ITexture* GetEnvCubeMap() = 0;
};

struct ALIGN16 Camera_t
{
	void Init(const Vector& origin, const QAngle& angles, float flNear, float flFar, float flFOV, float flAspect);
	void InitOrtho(const Vector& origin, const QAngle& angles, float flNear, float flFar, float flWidth, float flHeight);

	void InitViewParameters(const Vector& vOrigin, const QAngle& vAngles);

	void InitOrthoProjection(float flZNear, float flZFar, float flWidth, float flHeight);
	bool IsOrthographic() const;
	void InitPerspectiveProjection(float flZNear, float flZFar, float flFOVX, float flAspect);
	// generates 8 vertices of the frustum
	// vertex order is near plane (UL, UR, LL, LR), far plane (UL, UR, LL, LR)
	void ComputeGeometry(Vector* pVertsOut8) const;
	void ComputeGeometry(Vector* pVertsOut8, const Vector& vForward, const Vector& vLeft, const Vector& vUp) const;

	inline bool operator ==(const Camera_t& other) const;
	inline bool operator !=(const Camera_t& other) const;

	Vector m_origin;
	QAngle m_angles;

	// FOV for X/width. 
	// This should be set to -1 to get an ortho projection,
	// in which case it'll use m_flWidth and m_flHeight.
	float m_flFOVX;
	float m_flAspect; // For Perspective

	float m_flZNear;
	float m_flZFar;

	float m_flWidth;	// For ortho.
	float m_flHeight;
} ALIGN16_POST;
struct RenderToRTData_t
{
	RenderToRTData_t()
		: m_pObject(NULL)
		, m_pResultVTF(NULL)
		, m_pszIconNameSuffix(NULL)
		, m_stage(RENDER_TO_RT_STAGE_UNDEFINED)
	{
	}

	IRenderToRTHelperObject* m_pObject;
	IVTFTexture* m_pResultVTF;

	MaterialLightingState_t m_LightingState;
	QAngle m_cameraAngles;
	Vector m_cameraOffset;
	float m_cameraFOV;
	Camera_t m_Camera;
	matrix3x4_t m_rootToWorld;

	RenderToRTStage_t m_stage;

	const char* m_pszIconNameSuffix;

	bool m_bUsingExplicitModelCameraPosAnglesFromAttachment;

};
class IRenderToRTHelper
{
public:
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual RenderToRTData_t* CreateRenderToRTData(IRenderToRTHelperObject* pObject, IVTFTexture* pResultVTF) = 0;
	virtual void StartRenderToRT(RenderToRTData_t* pRendertoRTData) = 0;
	virtual void DestroyRenderToRTData(RenderToRTData_t* pRendertoRTData) = 0;
	virtual bool Process() = 0;
};