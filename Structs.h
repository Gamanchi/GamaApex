#pragma once

#include <numbers>
#include <cmath>
#include "Offsets.h"
#include "D3D9Types.h"


#define PI 3.141592653589793238462643

struct Matrix {
	float matrix[16];
};

struct FMatrix {
	float matrix[4][4];
};

struct Vector4 {
	float  x, y, z, w;
};


struct Vector3 {
	float x, y, z;

	float distance(Vector3 vec)
	{
		return sqrt(
			pow(vec.x - x, 2) +
			pow(vec.y - y, 2)
		);
	}

	static float Distance(Vector3 a, Vector3 b) {
		Vector3 vector = Vector3(a.x - b.x, a.y - b.y, a.z - b.z);

		return sqrt(((vector.x * vector.x) + (vector.y * vector.y)) + (vector.z * vector.z));
	}

	Vector3 operator-(Vector3 ape)
	{
		return { x - ape.x, y - ape.y, z - ape.z };
	}

	Vector3 operator+(Vector3 ape)
	{
		return { x + ape.x, y + ape.y, z + ape.z };
	}

	Vector3 operator*(float ape)
	{
		return { x * ape, y * ape, z * ape };
	}

	Vector3 operator/(float ape)
	{
		return { x / ape, y / ape, z / ape };
	}

	Vector3 operator/=(float ape)
	{
		x /= ape;
		y /= ape;
		z /= ape;

		return *this;
	}

	Vector3 operator+=(Vector3 ape)
	{
		return { x += ape.x, y += ape.y, z += ape.z };
	}

	Vector3 operator-=(Vector3 ape)
	{
		return { x -= ape.x, y -= ape.y, z -= ape.z };
	}

	void Normalize()
	{
		while (x > 89.0f)
			x -= 180.f;

		while (x < -89.0f)
			x += 180.f;

		while (y > 180.f)
			y -= 360.f;

		while (y < -180.f)
			y += 360.f;
	}

	float Length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	float Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	float DistTo(Vector3 ape)
	{
		return (*this - ape).Length();
	}

	float Dist2D(Vector3 ape)
	{
		return (*this - ape).Length2D();
	}

	struct Vector3 ScreenPosition(Matrix matrix) {
		struct Vector3 out;
		float _x = matrix.matrix[0] * x + matrix.matrix[1] * y + matrix.matrix[2] * z + matrix.matrix[3];
		float _y = matrix.matrix[4] * x + matrix.matrix[5] * y + matrix.matrix[6] * z + matrix.matrix[7];
		out.z = matrix.matrix[12] * x + matrix.matrix[13] * y + matrix.matrix[14] * z + matrix.matrix[15];

		_x *= 1.f / out.z;
		_y *= 1.f / out.z;

		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		out.x = width * .5f;
		out.y = height * .5f;

		out.x += 0.5f * _x * width + 0.5f;
		out.y -= 0.5f * _y * height + 0.5f;

		return out;
	}

	Vector3 to_angle() {
		Vector3 angles;

		if (y == 0.0f && x == 0.0f)
		{
			angles.x = (z > 0.0f) ? 270.0f : 90.0f;
			angles.y = 0.0f;
		}
		else
		{
			angles.x = std::atan2(-z, Length2D()) * -180 / PI;
			angles.y = std::atan2(y, x) * 180 / PI;

			if (angles.y > 90)
				angles.y -= 180;
			else if (angles.y < 90)
				angles.y += 180;
			else if (angles.y == 90)
				angles.y = 0;
		}

		angles.z = 0.0f;

		return angles;
	}

	Vector3 Clamp() {
		if (x > 180)
			x -= 360;
		else if (x < -180)
			x += 360;

		if (y > 180)
			y -= 360;
		else if (y < -180)
			y += 360;

		if (x < -89)
			x = -89;

		if (x > 89)
			x = 89;

		while (y < -180.0f)
			y += 360.0f;

		while (y > 180.0f)
			y -= 360.0f;

		z = 0;

		return *this;
	}

};



struct Vector2 {
	float x, y;

	Vector2 operator-(Vector2 ape)
	{
		return { x - ape.x, y - ape.y };
	}

	Vector2 operator+(Vector2 ape)
	{
		return { x + ape.x, y + ape.y };
	}

	Vector2 operator*(float ape)
	{
		return { x * ape, y * ape };
	}

	Vector2 operator/(float ape)
	{
		return { x / ape, y / ape };
	}

	Vector2 operator/=(float ape)
	{
		x /= ape;
		y /= ape;

		return *this;
	}

	Vector2 operator+=(Vector2 ape)
	{
		return { x += ape.x, y += ape.y };
	}

	Vector2 operator-=(Vector2 ape)
	{
		return { x -= ape.x, y -= ape.y };
	}

	void Normalize()
	{
		if (x > 89.0f)
			x -= 180.f;

		if (x < -89.0f)
			x += 180.f;

		if (y > 180.f)
			y -= 360.f;

		if (y < -180.f)
			y += 360.f;
	}

	float Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	float Dist2D(Vector2 ape)
	{
		return (*this - ape).Length2D();
	}
};

struct Bone {
	BYTE shit[0xCC];
	float x;
	BYTE shit2[0xC];
	float y;
	BYTE shit3[0xC];
	float z;
};



Vector3 GetBonePos(uintptr_t entity, int id, Vector3 position) {
	Vector3 bone;
	uintptr_t boneArray = Read<uintptr_t>(entity + Offsets::BoneArray);
	UINT32 boneloc = (id * 0x30);
	Bone bo = {};
	bo = Read<Bone>(boneArray + boneloc);

	bone.x = bo.x + position.x;
	bone.y = bo.y + position.y;
	bone.z = bo.z + position.z;
	return bone;
}


int windowHeight = 1080, windowWidth = 1920;

D3DMATRIX vMatrix;

struct Vector3 WorldToScreen22(struct Vector3& vIn, Vector2& vOut)
{
	struct Vector3 out;

	vOut.x = vMatrix.m[0][0] * vIn.x + vMatrix.m[0][1] * vIn.y + vMatrix.m[0][2] * vIn.z + vMatrix.m[0][3];
	vOut.y = vMatrix.m[1][0] * vIn.x + vMatrix.m[1][1] * vIn.y + vMatrix.m[1][2] * vIn.z + vMatrix.m[1][3];

	float w = vMatrix.m[3][0] * vIn.x + vMatrix.m[3][1] * vIn.y + vMatrix.m[3][2] * vIn.z + vMatrix.m[3][3];

	//if (w < 0.01)
		//return false;

	float invw = 1.0f / w;

	vOut.x *= invw;
	vOut.y *= invw;

	float x = windowWidth / 2;
	float y = windowHeight / 2;

	x += 0.5 * vOut.x * windowWidth + 0.5;
	y -= 0.5 * vOut.y * windowHeight + 0.5;

	vOut.x = x;
	vOut.y = y;

	return out;
}

struct Vector3 _WorldToScreen(const struct Vector3 pos, struct Matrix matrix) {
	struct Vector3 out;

	float _x = matrix.matrix[0] * pos.x + matrix.matrix[1] * pos.y + matrix.matrix[2] * pos.z + matrix.matrix[3];
	float _y = matrix.matrix[4] * pos.x + matrix.matrix[5] * pos.y + matrix.matrix[6] * pos.z + matrix.matrix[7];

	out.z = matrix.matrix[12] * pos.x + matrix.matrix[13] * pos.y + matrix.matrix[14] * pos.z + matrix.matrix[15];

	if (out.z < 0.1f)
		return out;

	_x *= 1.f / out.z;
	_y *= 1.f / out.z;


	int width = 1920;
	int height = 1080;


	out.x = width * .5f;
	out.y = height * .5f;


	out.x += 0.5f * _x * width + 0.5f;
	out.y -= 0.5f * _y * height + 0.5f;


	return out;
}


struct Vector3 WorldToScreen(struct Vector3 pos, Matrix matrix, int SWidth, int SHeight) {
	struct Vector3 out;

	float _x = matrix.matrix[0] * pos.x + matrix.matrix[1] * pos.y + matrix.matrix[2] * pos.z + matrix.matrix[3];
	float _y = matrix.matrix[4] * pos.x + matrix.matrix[5] * pos.y + matrix.matrix[6] * pos.z + matrix.matrix[7];

	out.z = matrix.matrix[12] * pos.x + matrix.matrix[13] * pos.y + matrix.matrix[14] * pos.z + matrix.matrix[15];

	_x *= 1.f / out.z;
	_y *= 1.f / out.z;

	out.x = SWidth * .5f;
	out.y = SHeight * .5f;

	out.x += 0.5f * _x * SWidth + 0.5f;
	out.y -= 0.5f * _y * SHeight + 0.5f;

	return out;
};






float matrix[16];

struct Vector3 WorldToScreen2(struct Vector3 pos, Vector2& screen)
{
	screen.x = matrix[0] * pos.x + matrix[1] * pos.y + matrix[2] * pos.z + matrix[3];
	screen.y = matrix[4] * pos.x + matrix[5] * pos.y + matrix[6] * pos.z + matrix[7];

	const float w = matrix[12] * pos.x + matrix[13] * pos.y + matrix[14] * pos.z + matrix[15];

	//if (w < 0.1f)
	//	return false;

	screen.x /= w;
	screen.y /= w;



	screen.x = windowWidth * 0.5f/*.97f*/ + screen.x * windowWidth * 0.5f/*.97f*/;
	screen.y = windowHeight * 0.5f/*1.047f*/ - screen.y * windowHeight * 0.5f/*1.047f*/;//1.f original

	//screen.x = 0.5f * (1.f + screen.x / w);
	//screen.y = 0.5f * (1.f - screen.y / w);

	//screen.x = screen.x * screen.x;
	//screen.y = screen.y * screen.y;

	//return true;
	return pos;
}

struct GlowMode {
	int8_t GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
	//int GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};

float CenterDistance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

static bool mouse_move(int x, int y) {
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dx = x;
	input.mi.dy = y;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(input));
	return true;
}

void AimAtPos(int screenwidth, int screenheight, float x, float y, float speed, float lock, bool smooth) {

	int ScreenCenterX = screenwidth / 2, ScreenCenterY = screenheight / 2;

	float AimSpeed = (float)speed + 0.2f;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0) {
		if (x > ScreenCenterX) {
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			TargetX += lock;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			TargetX -= lock;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			TargetY += lock;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			TargetY -= lock;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	if (!smooth)
	{
		mouse_move((int)TargetX, (int)(TargetY));
		return;
	}

	TargetX /= 10;
	TargetY /= 10;

	if (abs(TargetX) < 1)
	{
		if (TargetX > 0)
		{
			TargetX = 1;
		}
		if (TargetX < 0)
		{
			TargetX = -1;
		}
	}
	if (abs(TargetY) < 1)
	{
		if (TargetY > 0)
		{
			TargetY = 1;
		}
		if (TargetY < 0)
		{
			TargetY = -1;
		}
	}
	mouse_move((int)TargetX, (int)(TargetY));
	//mouse_event(MOUSEEVENTF_MOVE, (int)TargetX, (int)(TargetY), NULL, NULL);
}
