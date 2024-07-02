#pragma once
#include "../globals.h"

class Dormant_esp
{
	float m_round_start_time = 0.0f;
public:
	virtual void start();
	virtual void set_round_start_time(float time) { m_round_start_time = time; }
	virtual bool adjust_sound(crypt_ptr <Player> player);
	virtual void setup_adjust(crypt_ptr <Player> player, SndInfo_t& sound);
	virtual bool valid_sound(SndInfo_t& sound);

	virtual void reset()
	{
		m_utlvecSoundBuffer.RemoveAll();
		m_utlCurSoundList.RemoveAll();
	}

	struct SoundPlayer
	{
		virtual void reset(bool store_data = false, const Vector& origin = Vector(0.0f, 0.0f, 0.0f), int flags = 0)
		{
			if (store_data)
			{
				m_iReceiveTime = globals->realtime;
				m_vecOrigin = origin;
				m_nFlags = flags;
			}
			else
			{
				m_iReceiveTime = 0.0f;
				m_vecOrigin.Zero();
				m_nFlags = 0;
			}
		}

		virtual void Override(SndInfo_t& sound)
		{
			m_iReceiveTime = globals->realtime;
			m_vecOrigin = *sound.m_pOrigin;
		}

		bool found = false;
		bool from_shared = false;
		float m_iReceiveTime = 0.0f;
		
		Vector m_vecOrigin;
		int m_nFlags = 0;
	}
	m_cSoundPlayers[65];

	CUtlVector <SndInfo_t> m_utlvecSoundBuffer;
	CUtlVector <SndInfo_t> m_utlCurSoundList;
};