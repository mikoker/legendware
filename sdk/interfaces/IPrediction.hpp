#pragma once

#include "../math/QAngle.hpp"
#include "../misc/CUserCmd.hpp"
#include "IMoveHelper.hpp"

class CMoveData {
public:
	bool    m_bFirstRunOfFunctions : true;
	bool    m_bGameCodeMovedPlayer : true;
	int     m_nPlayerHandle;        // edict index on server, client entity handle on client=
	int     m_nImpulseCommand;      // Impulse command issued.
	Vector  m_vecViewAngles;        // Command view angles (local space)
	Vector  m_vecAbsViewAngles;     // Command view angles (world space)
	int     m_nButtons;             // Attack buttons.
	int     m_nOldButtons;          // From host_client->oldbuttons;
	float   m_flForwardMove;
	float   m_flSideMove;
	float   m_flUpMove;
	float   m_flMaxSpeed;
	float   m_flClientMaxSpeed;
	Vector  m_vecVelocity;          // edict::velocity        // Current movement direction.
	Vector  m_vecAngles;            // edict::angles
	Vector  m_vecOldAngles;
	float   m_outStepHeight;        // how much you climbed this move
	Vector  m_outWishVel;           // This is where you tried 
	Vector  m_outJumpVel;           // This is your jump velocity
	Vector  m_vecConstraintCenter;
	float   m_flConstraintRadius;
	float   m_flConstraintWidth;
	float   m_flConstraintSpeedFactor;
	float   m_flUnknown[5];
	Vector  m_vecAbsOrigin;
};

class Player;

class IGameMovement {
public:
	virtual					  ~IGameMovement(void) { }

	virtual void	          ProcessMovement(Player* pPlayer, CMoveData* pMove) = 0;
	virtual void	          Reset(void) = 0;
	virtual void	          StartTrackPredictionErrors(Player* pPlayer) = 0;
	virtual void	          FinishTrackPredictionErrors(Player* pPlayer) = 0;
	virtual void	          DiffPrint(char const* fmt, ...) = 0;
	virtual Vector const& GetPlayerMins(bool ducked) const = 0;
	virtual Vector const& GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector const& GetPlayerViewOffset(bool ducked) const = 0;
	virtual bool		      IsMovingPlayerStuck(void) const = 0;
	virtual Player* GetMovingPlayer(void) const = 0;
	virtual void		      UnblockPusher(Player* pPlayer, Player* pPusher) = 0;
	virtual void			  SetupMovementBounds(CMoveData* pMove) = 0;
};

class CGameMovement
	: public IGameMovement {
public:
	virtual ~CGameMovement(void) { }
};

class IPrediction
{
public:
	void Update(int start_frame, bool valid_frame, int inc_ack, int out_cmd)
	{
		typedef void(__thiscall* oUpdate)(void*, int, bool, int, int);
		return call_virtual<oUpdate>(this, 3)(this, start_frame, valid_frame, inc_ack, out_cmd);
	}

	void CheckMovingGround(Entity* player, double frametime)
	{
		typedef void(__thiscall* oCheckMovingGround)(void*, Entity*, double);
		return call_virtual<oCheckMovingGround>(this, 18)(this, player, frametime);
	}

	void RunCommand(Player* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
	{
		typedef void(__thiscall* oRunCommand)(void*, Player*, CUserCmd*, IMoveHelper*);
		return call_virtual<oRunCommand>(this, 19)(this, player, ucmd, moveHelper);
	}

	void SetupMove(Player* player, CUserCmd* ucmd, IMoveHelper* moveHelper, void* pMoveData)
	{
		typedef void(__thiscall* oSetupMove)(void*, Player*, CUserCmd*, IMoveHelper*, void*);
		return call_virtual<oSetupMove>(this, 20)(this, player, ucmd, moveHelper, pMoveData);
	}

	void FinishMove(Player* player, CUserCmd* ucmd, void* pMoveData)
	{
		typedef void(__thiscall* oFinishMove)(void*, Player*, CUserCmd*, void*);
		return call_virtual<oFinishMove>(this, 21)(this, player, ucmd, pMoveData);
	}

	void SetLocalViewAngles(Vector& Angles)
	{
		typedef void(__thiscall* Fn)(void*, Vector&);
		return call_virtual<Fn>(this, 13)(this, Angles);
	}

	char pad00[0x4];
	int m_hLastGround;
	bool m_bInPrediction;
	bool m_bFirstTimePredicted;
	bool m_bEnginePaused;
	bool m_bOldCLPredictValue;
	int m_nPreviousStartFrame;
	int m_nCommandsPredicted;
	char pad01[0x4];
	bool m_bPreviousAckHadErrors;
};