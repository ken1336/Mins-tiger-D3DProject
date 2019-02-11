// PhysicalObj.h: interface for the PhysicalObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICALOBJ_H__49719AF0_6560_45FB_A928_78DD371377F2__INCLUDED_)
#define AFX_PHYSICALOBJ_H__49719AF0_6560_45FB_A928_78DD371377F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "d3dx9.h"

class Box {
public:
	float x, y, z;
	float with, height;
};

enum DIRECTION{
	LEFT = 0, RIGHT, FOWARD, BACK
};

class PhysicalObj  
{
private:
	D3DXVECTOR3 p, v, a;
	float clock;
	float scale;
	bool jump;
	bool ball = FALSE;
	

	
	
	D3DXVECTOR3 min, max;
	D3DXVECTOR3 center;
	float radius;

	DIRECTION direction = LEFT;

public:
	D3DXVECTOR3 cPos, cLookat;
	float hSeta;
	float vSeta;
	float m = 1;
	float velocity = 0;
	bool PhysicalObj::BoxCollision(Box *rect);
	PhysicalObj(float x=0, float y=0, float z=0);
	void SetPosition(float x, float y, float z);
	void SetVelocity(float x, float y, float z);
	void SetAcceleration(float x, float y, float z);
	void AddVelocity(float x, float y, float z);
	void Move(float current=-1);
	void ObjectMove(float current = -1);
	void BoundCheck();
	void Collision(PhysicalObj *target);
	void CollisionObject(PhysicalObj *target, bool shot);
	void SetBoundingBox(D3DXVECTOR3 m, D3DXVECTOR3 M);
	void SetBoundingBoxObject(D3DXVECTOR3 m, D3DXVECTOR3 M);
	void SetBoundingSphere(D3DXVECTOR3 c, float r);
	D3DXMATRIXA16 GetWorldMatrix();
	virtual ~PhysicalObj();
	D3DXVECTOR3 GetLocation();
	void CameraMove();

	void SetRadius(float set) { radius = set; }

	void setDirection(DIRECTION t) {
		direction = t;
	}
	float getDirection();
	void SetJump(bool set) { jump = set; }
	bool GetJump() { return jump; }
	D3DXVECTOR3 GetVelocityVector() { return v; }
	D3DXVECTOR3 GetCameraPos() {
		return cPos;
	}
	D3DXVECTOR3 GetCameraLookat() {
		return cLookat;
	}

};

#endif // !defined(AFX_PHYSICALOBJ_H__49719AF0_6560_45FB_A928_78DD371377F2__INCLUDED_)
