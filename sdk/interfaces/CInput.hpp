#pragma once

#include "../misc/CUserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class CInput
{
public:
	char pad0[0xC]; // 0x0
	bool m_fTrackIRAvailable; // 0xC
	bool m_fMouseInitialized; // 0xD
	bool m_fMouseActive; // 0xE
	char pad1[0x9A]; // 0xF
	bool m_fCameraInThirdPerson; // 0xAD
	char pad2[0x2]; // 0xAE
	Vector m_vecCameraOffset; // 0xB0
	char pad3[0x38]; // 0xBC
	CUserCmd* m_pCommands; // 0xF4
	CVerifiedUserCmd* m_pVerifiedCommands; // 0xF8

	CUserCmd* GetUserCmd(int sequence_number)
	{
		return &m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
	}

	CVerifiedUserCmd* GetVerifiedUserCmd(int sequence_number)
	{
		return &m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
	}
};