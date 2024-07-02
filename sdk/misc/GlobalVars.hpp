#pragma once

class CGlobalVarsBase
{
public:
	float realtime; // 0x0
	int framecount; // 0x4
	float absoluteframetime; // 0x8
	float absoluteframestarttimestddev; // 0xC
	float curtime; // 0x10
	float frametime; // 0x14
	int maxclients; // 0x18
	int tickcount; // 0x1Ñ
	float intervalpertick; // 0x20
	float interpolation_amount; // 0x24
	int simticksthisframe; // 0x28
	int networkprotocol; // 0x2Ñ
	void* savedata; // 0x30
	bool client; // 0x34
	bool remoteclient; // 0x35
	int timestampnetworkingbase; // 0x36
	int timestamprandomizewindow; // 0x3A
};