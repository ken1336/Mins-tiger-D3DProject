// PhysicalObj.cpp: implementation of the PhysicalObj class.
//
//////////////////////////////////////////////////////////////////////

#include "PhysicalObj.h"
#include<stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


PhysicalObj::PhysicalObj(float x, float y, float z)
{
	p.x = p.y = p.z = 0;
	v.x = v.y = v.z = 0;
	a.x = a.y = a.z = 0;
	a.y = -.8f;
	clock = 0;
	scale = 50;
	min = max = D3DXVECTOR3(0,0,0);
	cPos = p;
	cPos.y = 100;
	
	cLookat = p;
	cLookat.z = 0;
	cLookat.x = 1000;
	cLookat.y = 500;
	direction = FOWARD;
	hSeta = -90;
	vSeta = 0;
}

PhysicalObj::~PhysicalObj()
{

}



D3DXVECTOR3 PhysicalObj::GetLocation()
{
	return p;
}

float PhysicalObj::getDirection() {
	
	float result = 0;
	if (direction == LEFT) {
		//result = 45.0f * (D3DX_PI / 90.0f);
	}
	else if (direction == RIGHT) {
		//result =  -45.0f * (D3DX_PI / 90.0f);
	}
	else if (direction == FOWARD) {
		//result = -90.0f * (D3DX_PI / 90.0f);

	}
	else if (direction == BACK) {
		//result = 0;// 90.0f* (D3DX_PI / 90.0f);
	}
	result = -(hSeta+90) * (D3DX_PI / 180.0f);
	return result;
}


void PhysicalObj::SetPosition(float x, float y, float z)
{
	p.x = x;
	p.y = y;
	p.z = z;
}
void PhysicalObj::SetVelocity(float x, float y, float z)
{
	v.x = x;
	v.y = y;
	v.z = z;
}
void PhysicalObj::AddVelocity(float x, float y, float z)
{
	v.x += x;
	v.y += y;
	v.z += z;
}

void PhysicalObj::SetAcceleration(float x, float y, float z)
{
	a.x = x;
	a.y = y;
	a.z = z;
}

void PhysicalObj::BoundCheck()
{
	float e = 0.5f;
/*
	if(p.y <= 0) {
		if( fabs(v.y) < 1.0 ) { // stop condition
			p.y = 0;
			v.y = 0;
		} else {
			v.y = (float)fabs(v.y) * e;
		}
	}
	if(p.x < -200) {
		p.x = -200;
		v.x = (float)fabs(v.x) * e;
	}
	if(p.x > 200) {
		p.x = 200;
		v.x = (float)-fabs(v.x) * e;
	}
	if(p.z < -200) {
		p.z = -200;
		v.z = (float)fabs(v.z) * e;
	}
	if(p.z > 200) {
		p.z = 200;
		v.z = (float)-fabs(v.z) * e;
	}
*/

	if(p.y + min.y< 0) {
		if( fabs(v.y) < 1.0) { // stop condition
			p.y = -min.y;
			v.y = 0;
		} else {
			v.y = (float)fabs(v.y) * e;
		}
	}
	if(p.x + min.x < -800) {
		p.x = -800 - min.x;
		v.x = (float)fabs(v.x) * e;
	}
	if(p.x + max.x > 800) {
		p.x = 800 - max.x;
		v.x = (float)-fabs(v.x) * e;
	}
	if(p.z + min.z < -800) {
		p.z = -800 - min.z;
		v.z = (float)fabs(v.z) * e;
	}
	if(p.z + max.z > 800) {
		p.z = 800 - max.z;
		v.z = (float)-fabs(v.z) * e;
	}

}
void PhysicalObj::Move(float current)
{
	float e = 0.992f;
	if(current == -1) { // defafult

		p.x += v.x + 0.5f*a.x;
		p.y += v.y + 0.5f*a.y;
		p.z += v.z + 0.5f*a.z;

		v.x += a.x;
		v.y += a.y;
		v.z += a.z;

		v.x *= e;
		v.y *= e;
		v.z *= e;
		if (fabs(v.x) < 0.1) {
			v.x = .0f;
		}
		if (fabs(v.y) < 0.1) {
			v.y = .0f;
		}
		if (fabs(v.z) < 0.1) {
			v.z = .0f;
		}
		if(v.x!=0.f){
		//char hh[100];
		//sprintf(hh, "%f	%f	%f", v.x,v.y,v.z);
		//MessageBox(NULL, hh, "A", 0);
		}
	}

	BoundCheck();

}
void PhysicalObj::CameraMove() {
	//cPos = p;
	//cPos.y = 300;//r*cos = 
	
	char hh[100];
	sprintf(hh, "%f	%f	%f", cLookat.x, cLookat.y, cLookat.z);
	//MessageBox(NULL, hh, "A", 0);

	cPos.x = p.x + (500 * -cos(hSeta*(D3DX_PI / 180)));
	cPos.z = p.z + (500 * -sin(hSeta*(D3DX_PI / 180)));
	cPos.y = p.y+500;
	cLookat.x = p.x+(1000 * cos(hSeta*(D3DX_PI / 180))*sin(vSeta*(D3DX_PI / 180)));
	cLookat.z =p.z+( 1000 * sin(hSeta*(D3DX_PI / 180))*sin(vSeta*(D3DX_PI / 180)));
	cLookat.y = p.y+(1000 * cos((vSeta-180)*(D3DX_PI / 180)));
	//cLookat.y = p.y+(sin(vSeta*(D3DX_PI / 180))*cLookat.x) / (cos(hSeta*(D3DX_PI / 180))*cos(vSeta*(D3DX_PI / 180)));
	//cLookat.y = p.y  + (1000 * cos(vSeta*(D3DX_PI / 180)));

}
void PhysicalObj::ObjectMove(float current)
{
	CameraMove();
	//cLookat = cPos;
	//cLookat.z += 200;
	//cLookat.y = 0;

	if (current == -1) { // defafult

		if (1) {

			if (velocity <3)
				velocity += 0.3f;
			if (GetAsyncKeyState('A')) {
				
				v.x += 0.1f*velocity * cos((90 + hSeta)*(D3DX_PI / 180));
				v.z += 0.1f*velocity * sin((90 + hSeta)*(D3DX_PI / 180));
			//	p.x += 3 * cos(-80+hSeta*(D3DX_PI / 180));
			//	p.z += 3 * sin(-80 + hSeta*(D3DX_PI / 180));
			}
			if (GetAsyncKeyState('D')) {
			
				v.x -= 0.1f*velocity * cos((90+hSeta)*(D3DX_PI / 180));
				v.z -= 0.1f*velocity * sin((90 + hSeta)*(D3DX_PI / 180));

			//	p.x += 3 * cos(80+hSeta*(D3DX_PI / 180));
			//	p.z += 3 * sin(80+hSeta*(D3DX_PI / 180));
			}
			if (GetAsyncKeyState('W')) {
				
				v.x += 0.1f*velocity * cos(hSeta*(D3DX_PI / 180));
				v.z += 0.1f*velocity * sin(hSeta*(D3DX_PI / 180));
			//	v.x = velocity * cos(hSeta*(D3DX_PI / 180));
			//	v.z = velocity * sin(hSeta*(D3DX_PI / 180));
			//	p.x += velocity*cos(hSeta*(D3DX_PI / 180));
			//	p.z += velocity*sin(hSeta*(D3DX_PI / 180));
			}
			if (GetAsyncKeyState('S')) {
				
				v.x += 0.1f*-velocity * cos(hSeta*(D3DX_PI / 180));
				v.z += 0.1f*-velocity * sin(hSeta*(D3DX_PI / 180));
			
			//	p.x += velocity *-cos(hSeta*(D3DX_PI / 180));
			//	p.z += velocity *-sin(hSeta*(D3DX_PI / 180));
			}
		

		}

		if (velocity != 0.0f) {

			if (velocity <= 0.11f && velocity >= -0.11f) {
				
				velocity = 0.0f;

			}
			else if (velocity < 0.0f) {
				char hh[100];
				sprintf(hh, "%f", velocity);
				//MessageBox(NULL, hh, "A", 0);
				velocity = velocity + 0.1f;
			}
			else if (velocity > 0.0f) {
				char hh[100];
				sprintf(hh, "%f", velocity);
				//MessageBox(NULL, hh, "A", 0);
				velocity = velocity - 0.1f;
			}

		}
		p.x += v.x + 0.5f*a.x;
		p.y += v.y + 0.5f*a.y;
		p.z += v.z + 0.5f*a.z;

		v.x += a.x;
		v.y += a.y;
		v.z += a.z;
		if (v.y != 0.0f) {

			if (p.y < 40) {
				jump = FALSE;
			}
		}
	
		/*
		p.x += v.x + 0.5f*a.x;
		p.y += v.y + 0.5f*a.y;
		p.z += v.z + 0.5f*a.z;
		
		v.x += a.x;
		v.y += a.y;
		v.z += a.z;

		*/
		if (v.x != 0.0f) {
			
			if (v.x <= 0.11f && v.x >= -0.11f) {
				char hh[100];
				sprintf(hh, "%f", v.x);
				//MessageBox(NULL, hh, "A", 0);
				v.x = 0;
				
			}else if(v.x < 0.0f) {
				v.x = v.x + 0.1f;
			}
			else if (v.x > 0.0f) {
				v.x = v.x - 0.1f;
			}
			
		}
		if (v.z != 0.0f) {

			if (v.z <= 0.11f && v.z >= -0.11f) {
				char hh[100];
				sprintf(hh, "%f", v.z);
				//MessageBox(NULL, hh, "A", 0);
				v.z = 0;

			}
			else if (v.z < 0.0f) {
				v.z = v.z + 0.1f;
			}
			else if (v.z > 0.0f) {
				v.z = v.z - 0.1f;
			}

		}
		/*
		if (v.y != 0.0f) {

			if (p.y < 40) {
				jump = FALSE;
			}
			
		

		}*/
		
	}

	BoundCheck();

}

bool PhysicalObj::BoxCollision(Box *rect) {
	D3DXVECTOR3 circleDistance;
	circleDistance.x = fabs(center.x - rect->x);
	circleDistance.y = fabs(center.y - rect->y);
	// 녹색 제거
	if(circleDistance.x > (rect->with / 2 + radius))
	{
		return false;
	}
	if (circleDistance.y > (rect->height / 2 + radius))
	{
		return false;
	}
	// 회색 + 노란색 인정
	if (circleDistance.x <= rect->with / 2)
	{
		return true;
	}
	if (circleDistance.y <= (rect->height / 2))
	{
		return true;
	}
	float cornerDistance_sq = (circleDistance.x - (rect->with / 2)) *  (circleDistance.x - (rect->with / 2))
		+ (circleDistance.y - (rect->height / 2))*(circleDistance.y - (rect->height / 2));
	return(cornerDistance_sq <= (radius*radius));


}
void PhysicalObj::Collision(PhysicalObj *target)
{
	
	
	D3DXVECTOR3 distance = (p+center) - (target->p + target->center); // 엄밀하게는 scale도 포함!
	float length = D3DXVec3Length(&distance);
	float rsum = radius + target->radius;

	D3DXVECTOR3 u1, u2;
	

	if(rsum > length) { // collision!

		//exit(0);
		D3DXVECTOR3 d = target->p - p; // normal
		D3DXVec3Normalize(&d, &d);

		D3DXVECTOR3 mv1 = (d * D3DXVec3Dot(&d, &v));
		D3DXVECTOR3 mv2 =( d * D3DXVec3Dot(&d, &target->v));

		u1 = v;
		u2 = target->v;
		
		v = ((v - mv1) + mv2);
		target->v = ((target->v - mv2) + mv1);
		
		v = (u1*(m - target->m) + (2 * target->m*u2)) / (m + target->m);
		target->v = (u2*(target->m - m) + (2 * m*u1)) / (m + target->m);
		
	}

}
void PhysicalObj::CollisionObject(PhysicalObj *target, bool shot)
{

	char hh[100];
	
	D3DXVECTOR3 distance = (p + center) - (target->p + target->center); // 엄밀하게는 scale도 포함!
	float length = D3DXVec3Length(&distance);
	float rsum = radius + target->radius;

	D3DXVECTOR3 u1, u2;


	if (rsum >= length) { // collision!
		if (shot == FALSE ) {
			if (ball == FALSE) {
				ball = TRUE;
				target->p.x = p.x;
				target->p.z = p.z;

			}
		

			D3DXVECTOR3 d = target->p - p; // normal
			D3DXVec3Normalize(&d, &d);

			D3DXVECTOR3 mv1 = (d * D3DXVec3Dot(&d, &v));
			D3DXVECTOR3 mv2 = (d * D3DXVec3Dot(&d, &target->v));

			u1 = v;
			u2 = target->v;

			v = ((v - mv1) + mv2);
			target->v = ((target->v - mv2) + mv1);

			v = (u1*(m - target->m) + (2 * target->m*u2)) / (m + target->m);
			target->v = (u2*(target->m - m) + (2 * m*u1)) / (m + target->m);
		}
		else if(shot == TRUE){
			
			
			
			D3DXVECTOR3 d = target->p - p; // normal
			D3DXVec3Normalize(&d, &d);

			D3DXVECTOR3 mv1 = (d * D3DXVec3Dot(&d, &v));
			D3DXVECTOR3 mv2 = (d * D3DXVec3Dot(&d, &target->v));

			u1 = v;
			u2 = target->v;

			v = ((v - mv1) + mv2);
			target->v = ((target->v - mv2) + mv1);

			//sprintf(hh, "%f	%f", m, target->v.z);
			//MessageBox(NULL, hh, "A", 0);
			
			v = (u1*(m - target->m) + (2 * target->m*u2)) / (m + target->m);
			target->v = (u2*(target->m - m) + (2 * m*u1)) / (m + target->m);

			if (ball == TRUE) {
				ball = FALSE;
				target->p.x += 100 * cos(hSeta*(D3DX_PI / 180));
				target->p.z += 100 * sin(hSeta*(D3DX_PI / 180));
				target->v.x += 20 * cos(hSeta*(D3DX_PI / 180));
				target->v.z += 20 * sin(hSeta*(D3DX_PI / 180));
				v.x = 0;
				v.z = 0;
			}
		

			//sprintf(hh, "%f	%f", target->v.x,target->v.z);
			//MessageBox(NULL, hh, "A", 0);
		}
		
		
			//exit(0);
			////MessageBox(NULL, "A", "A", 0);
			//D3DXVECTOR3 d = target->p - p; // normal
			//D3DXVec3Normalize(&d, &d);

			//D3DXVECTOR3 mv1 = (d * D3DXVec3Dot(&d, &v));
			//D3DXVECTOR3 mv2 = (d * D3DXVec3Dot(&d, &target->v));

			//u1 = v;
			//u2 = target->v;

			//v = ((v - mv1) + mv2);
			//target->v = ((target->v - mv2) + mv1);

			//v = (u1*(m - target->m) + (2 * target->m*u2)) / (m + target->m);
			//target->v = (u2*(target->m - m) + (2 * m*u1)) / (m + target->m);


			
			//target->p.x += 20* cos(hSeta*(D3DX_PI / 180));
			//target->p.z += 20* sin(hSeta*(D3DX_PI / 180));

			//target->v.x += 2+10 * cos(hSeta*(D3DX_PI / 180));
			//target->v.z += 2+10 * sin(hSeta*(D3DX_PI / 180));
		/*	if (1) {
				if (GetAsyncKeyState('A')) {
					target->p.x += -10;
					target->v.x = -15;
					v.x = 0;
				}
				if (GetAsyncKeyState('D')) {
					target->p.x += 10;
					target->v.x = 15;
					v.x = 0;
				}
				if (GetAsyncKeyState('W')) {
					target->p.z += 10;
					target->v.z = 15;
					v.z = 0;
				}
				if (GetAsyncKeyState('S')) {
					target->p.z += -10;
					target->v.z = -15;
					v.z = 0;
				}
				
			}*/
		
	}
	

}
void PhysicalObj::SetBoundingBox(D3DXVECTOR3 m, D3DXVECTOR3 M)
{
	min = m*scale;
	max = M*scale;
}
void PhysicalObj::SetBoundingBoxObject(D3DXVECTOR3 m, D3DXVECTOR3 M)
{
	//float scale2 = 100;
	min = m * scale;
	max = M * scale;
}

void PhysicalObj::SetBoundingSphere(D3DXVECTOR3 c, float r)
{
	center = c * scale;
	radius = r*scale;
}

D3DXMATRIXA16 PhysicalObj::GetWorldMatrix()
{
	D3DXMATRIXA16 matWorld, matScale;
	D3DXMatrixTranslation(&matWorld, p.x, p.y, p.z);
	D3DXMatrixScaling(&matScale, scale, scale, scale);

	D3DXMatrixMultiply(&matWorld, &matScale, &matWorld);
	return matWorld;
}