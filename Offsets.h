#pragma once

namespace Offsets {
	enum Offsets {
		EntityList = 0x19ed718,//cl_entitylist

		ViewMatrix = 0x11a210,//view_matrix 0x5f2891

		LocalPlayer = 0x1d9e418,//local_player

		TeamId = 0x448,//m_iTeamNum

		Health = 0x438,//m_iHealth
		MaxHealth = 0x578,//m_iMaxHealth

		m_shieldHealth = 0x0170,
		m_shieldHealthMax = 0x0174,

		Origin = 0x14C,//m_localOrigin 0x58   m_vecAbsOrigin  	0x4      0x14C is good

		Knocked = 0x2728,//m_bleedoutState
		Dead = 0x0798,//m_lifeState

		BoneArray = 0x0f38,//m_bConstrainBetweenEndpoints //may be bonematrix 0xf38

		Bones = 0xef0,//m_nForceBone + 0x48

		GlowEnable = 0xcd0, // glow_enable

		GlowType = 0x2c0, // glow_type

		GlowColor = 0x1d0, // glow_color

		OViewAngleX = 0x20f0,//m_angEyeAngles.x float

		OViewAngleY = 0x20f4,// m_angEyeAngles.y float

		ViewAngle = 0x490, // m_localAngles

		PlayerName = 0x589, // m_iName

		ViewRender = 0x74bad90, //view_render

		FOV = 0x930, //m_usableFOV
		cinput = 0x1d0d600, //cinput

		//namelist = 0xb9315d0,
	};
}