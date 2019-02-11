#include <windows.h>
#include "d3dx9.h"
#include "XFileUtil.h"
#include "PhysicalObj.h"
#include"Camera.h"
#include<stdio.h>
#include "terrain.h"
//-----------------------------------------------------------------------------
// ���� ���� 

VOID SetupViewProjection();
//-----------------------------------------------------------------------------
// ���� ���� 
ZCamera*				g_pCamera = NULL;	// Camera Ŭ����
LPDIRECT3D9             g_pD3D = NULL; // Direct3D ��ü 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // ������ ��ġ (����ī��)

LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // ���ؽ� ���� 
PDIRECT3DVERTEXBUFFER9  g_pVBTexture = NULL; // �ؽ��� ��¿���ؽ� ����
PDIRECT3DVERTEXBUFFER9  g_pVBTexture2 = NULL;
PDIRECT3DVERTEXBUFFER9	g_pVBTexture3 = NULL;
LPDIRECT3DTEXTURE9      g_pTexture = NULL; // �ؽ��� �ε��� ����
LPDIRECT3DTEXTURE9      g_pTexture2 = NULL; // �ؽ��� �ε��� ����

CXFileUtil              g_XFile;   // X ���� ����� ���� Ŭ���� ��ü 
CXFileUtil				g_XBall1;
CXFileUtil				g_XBall2;

PhysicalObj				g_PhyObject;
PhysicalObj				g_PhyBall1;
PhysicalObj				g_PhyBall2;
Box						g_board;
bool lbuttonDown = false;
HWND hWnd;
WNDCLASSEX wc;
bool flag = TRUE;
static short pX, pY;
Terrain*				g_pTerrain = NULL;	// Terrain


// Ŀ���� ���ؽ� Ÿ�� ����ü 
struct CUSTOMVERTEX
{
	FLOAT x, y, z;    // 3D ��ǥ��
	DWORD color;      // ���ؽ� ����
};

// Ŀ���� ���ؽ��� ������ ǥ���ϱ� ���� FVF(Flexible Vertex Format) �� 
// D3DFVF_XYZ(3D ���� ��ǥ) ��  D3DFVF_DIFFUSE(���� ����) Ư���� ��������.
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

// �ؽ��� ��ǥ�� ������ ���ؽ� ����ü ����
struct TEXTUREVERTEX
{
	D3DXVECTOR3     position;  // ���ؽ��� ��ġ
	D3DCOLOR        color;     // ���ؽ��� ����
	FLOAT           tu, tv;    // �ؽ��� ��ǥ 
};
TEXTUREVERTEX* pVertices = NULL;

// �� ����ü�� ������ ǥ���ϴ� FVF �� ����
#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)



//-----------------------------------------------------------------------------
// �̸�: InitD3D()
// ���: Direct3D �ʱ�ȭ 
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
	// Direct3D ��ü ���� 
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// ��ġ ������ ����Ÿ �غ�

	D3DPRESENT_PARAMETERS d3dpp;         // ��ġ ������ ���� ����ü ���� ����

	ZeroMemory(&d3dpp, sizeof(d3dpp));                  // ����ü Ŭ����
	d3dpp.BackBufferWidth = 1024;               // ���� �ػ� ���� ����
	d3dpp.BackBufferHeight = 768;               // ���� �ػ� ���� ���� 
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;   // ���� ���� ���� 
	d3dpp.BackBufferCount = 1;                 // ����� �� 
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;  // ���� ��� ����
	d3dpp.hDeviceWindow = hWnd;              // ������ �ڵ� ���� 
	d3dpp.Windowed = true;              // ������ ���� ���� �ǵ��� �� 
	d3dpp.EnableAutoDepthStencil = true;              // ���Ľ� ���۸� ����ϵ��� �� 
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;      // ���Ľ� ���� ���� ���� 


	// D3D��ü�� ��ġ ���� �Լ� ȣ�� (����Ʈ ����ī�� ���, HAL ���)
	
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	// ���� ��ġ�� ���������� �����Ǿ���.

	// zbuffer ����ϵ��� ����
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	SetupViewProjection();
	return S_OK;
}


//-----------------------------------------------------------------------------
// �̸�: InitMeshData()
// ���: Mesh�� ���õ� ���� �����͸� �ʱ�ȭ �Ѵ�. 
//-----------------------------------------------------------------------------
HRESULT InitMeshData()
{
	g_XFile.XFileLoad(g_pd3dDevice, "./images/tiger.x");
	g_XBall1.XFileLoad(g_pd3dDevice, "./images/ball.x");
	g_XBall2.XFileLoad(g_pd3dDevice, "./images/ball.x");

	D3DXVECTOR3 min, max;
	D3DXVECTOR3 center;
	float radius;


	BYTE* v = 0;
	g_XFile.GetMesh()->LockVertexBuffer(0, (void**)&v);

	HRESULT hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		g_XFile.GetMesh()->GetNumVertices(),
		D3DXGetFVFVertexSize(g_XFile.GetMesh()->GetFVF()),
		&min,
		&max);
	g_XFile.GetMesh()->UnlockVertexBuffer();
	
	g_PhyObject.SetBoundingBoxObject(min, max);
	g_PhyObject.SetRadius(50);

	



	g_XBall1.GetMesh()->LockVertexBuffer(0, (void**)&v);
	hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		g_XBall1.GetMesh()->GetNumVertices(),
		D3DXGetFVFVertexSize(g_XBall1.GetMesh()->GetFVF()),
		&min,
		&max);
	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		g_XBall1.GetMesh()->GetNumVertices(),
		D3DXGetFVFVertexSize(g_XBall1.GetMesh()->GetFVF()),
		&center,
		&radius);
	g_XBall1.GetMesh()->UnlockVertexBuffer();

	g_PhyBall1.SetBoundingBox(min, max);
	
	g_PhyBall1.SetBoundingSphere(center, radius);
	g_PhyBall1.SetPosition(250, 0, 100);
	// Ball1�� ball2�� �����ϹǷ� Bounding volume�� ���� ����� �ʿ�� ����
	g_PhyBall2.SetBoundingBox(min, max);
	g_PhyBall2.SetBoundingSphere(center, radius);
	g_PhyBall2.SetPosition(-100, 0, -100);



	g_PhyBall1.m = 1;
	g_PhyBall2.m = 10;
	g_PhyObject.m = 10;




	return S_OK;
}


//-----------------------------------------------------------------------------
// �̸�: InitGeometry()
// ���: ���ؽ� ���۸� ������ �� ���ؽ��� ä���. 
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	// ���ؽ� ���ۿ� ���� ���ؽ� �ڷḦ �ӽ÷� �����. 
	CUSTOMVERTEX vertices[] =
	{
		{ -200.0f,  0.0f, 0.0f, 0xff00ff00, }, // x�� ������ ���� ���ؽ� 
	{ 200.0f,  0.0f, 0.0f, 0xff00ff00, },

	{ 0.0f, 0.0f, -200.0f, 0xffffff00, },  // z�� ������ ���� ���ؽ�
	{ 0.0f, 0.0f,  200.0f, 0xffffff00, },

	{ 0.0f, -200.0f, 0.0f, 0xffff0000, },  // y�� ������ ���� ���ؽ�
	{ 0.0f,  200.0f, 0.0f, 0xffff0000, },

	{ 0.0f, 50.0f, 0.0f, 0xffff0000, },  // �ﰢ���� ù ��° ���ؽ� 
	{ -50.0f,  0.0f, 0.0f, 0xffff0000, },  // �ﰢ���� �� ��° ���ؽ� 
	{ 50.0f,  0.0f, 0.0f, 0xffff0000, },  // �ﰡ���� �� ��° ���ؽ� 
	};

	// ���ؽ� ���۸� �����Ѵ�.
	// �� ���ؽ��� ������ D3DFVF_CUSTOMVERTEX ��� �͵� ���� 
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(9 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	// ���ؽ� ���ۿ� ���� �� �� ���ؽ��� �ִ´�. 
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVB->Unlock();








	///////////////���� ������ �ʱ�ȭ///////////////////

	D3DXVECTOR3 directionToLight(0, 1, 0);
	//g_pTerrain = new Terrain(g_pd3dDevice, "./images/coastMountain64.raw", 64, 64, 6, 0.4f);
	g_pTerrain = new Terrain(g_pd3dDevice, "./images/height0.bmp", 45, 0.4f);
	
	//	g_pTerrain->loadTexture("./images/desert.bmp");
	g_pTerrain->loadTexture("./images/tile.tga");
	//	g_pTerrain->genTexture(&directionToLight);



	return S_OK;


}


//-----------------------------------------------------------------------------
// �̸�: InitGeometryTexture()
// ���: �ؽ��� ����� ���� ���ؽ� ���۸� ������ �� ���ؽ��� ä���. 
//-----------------------------------------------------------------------------
HRESULT InitGeometryTexture()
{
	// �ؽ��� �ε� 
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "./Images/sky.bmp", &g_pTexture)))
	{
		return E_FAIL;
	}

	// ���ؽ� ���� ���� 
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(TEXTUREVERTEX), 0,
		D3DFVF_TEXTUREVERTEX, D3DPOOL_DEFAULT, &g_pVBTexture, NULL)))
	{
		return E_FAIL;
	}
	float altitude = g_pTerrain->getHeight(0, 0);
	// ���ؽ� ���� ���� 
	
	if (FAILED(g_pVBTexture->Lock(0, 0, (void**)&pVertices, 0)))
		return E_FAIL;
	pVertices[0].position = D3DXVECTOR3(3000, 3000, 3000);  // ���ؽ� ��ġ 
	pVertices[0].color = 0xffffffff;                  // ���ؽ� ���� �� ���� 
	pVertices[0].tu = 0.0f;                        // ���ؽ� U �ؽ��� ��ǥ 
	pVertices[0].tv = 0.0f;                        // ���ؽ� V �ؽ��� ��ǥ 

	pVertices[1].position = D3DXVECTOR3(3000, 3000, -3000);
	pVertices[1].color = 0xffffffff;
	pVertices[1].tu = 1.0f;
	pVertices[1].tv = 0.0f;

	pVertices[2].position = D3DXVECTOR3(-3000, 3000, 3000);
	pVertices[2].color = 0xffffffff;
	pVertices[2].tu = 0.0f;
	pVertices[2].tv = 1.0f;

	pVertices[3].position = D3DXVECTOR3(-3000, 3000, -3000);
	pVertices[3].color = 0xffffffff;
	pVertices[3].tu = 1.0f;
	pVertices[3].tv = 1.0f;




	g_pVBTexture->Unlock();



	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(TEXTUREVERTEX), 0,
		D3DFVF_TEXTUREVERTEX, D3DPOOL_DEFAULT, &g_pVBTexture2, NULL)))
	{
		return E_FAIL;
	}


	TEXTUREVERTEX* pVertices1;
	if (FAILED(g_pVBTexture2->Lock(0, 0, (void**)&pVertices1, 0)))
		return E_FAIL;
	pVertices1[0].position = D3DXVECTOR3(-1000, 1000, -1000);  // ���ؽ� ��ġ 
	pVertices1[0].color = 0xffffffff;                  // ���ؽ� ���� �� ���� 
	pVertices1[0].tu = 0.0f;                        // ���ؽ� U �ؽ��� ��ǥ 
	pVertices1[0].tv = 0.0f;                        // ���ؽ� V �ؽ��� ��ǥ 

	pVertices1[1].position = D3DXVECTOR3(1000, 1000, -1000);
	pVertices1[1].color = 0xffffffff;
	pVertices1[1].tu = 1.0f;
	pVertices1[1].tv = 0.0f;

	pVertices1[2].position = D3DXVECTOR3(-1000, -1000, -1000);
	pVertices1[2].color = 0xffffffff;
	pVertices1[2].tu = 0.0f;
	pVertices1[2].tv = 1.0f;

	pVertices1[3].position = D3DXVECTOR3(1000, -1000, -1000);
	pVertices1[3].color = 0xffffffff;
	pVertices1[3].tu = 1.0f;
	pVertices1[3].tv = 1.0f;
	g_pVBTexture2->Unlock();
	
	///////////////////////////////////////////////////////////////
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "./Images/background.bmp", &g_pTexture2)))
	{
		return E_FAIL;
	}
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(TEXTUREVERTEX),0,
		D3DFVF_TEXTUREVERTEX,D3DPOOL_DEFAULT, &g_pVBTexture3, NULL)))
	{
		return E_FAIL;
	}



	// �˰����� ����ؼ� �Ǹ���(�� �Ʒ��� ���� ����)�� �����.

	TEXTUREVERTEX* pVertices3;



	if (FAILED(g_pVBTexture3->Lock(0, 0, (void**)&pVertices3, 0)))

		return E_FAIL;



	for (DWORD i = 0; i < 50; i++)

	{

		FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);

		pVertices3[2 * i + 0].position = D3DXVECTOR3(sinf(theta)*3000, -3000.0f, cosf(theta) * 3000);

		// �Ǹ����� �Ʒ��� ������ ��ǥ

		pVertices3[2 * i + 0].color = 0xffffffff;

#ifndef SHOW_HOW_TO_USE_TCI

		// SHOW_HOW_TO_USE_TCI�� ����Ǿ� ������ �ؽ��� ��ǥ�� �������� �ʴ´�.

		pVertices3[2 * i + 0].tu = ((float)i) / (50 - 1); // �ؽ����� u��ǥ

		pVertices3[2 * i + 0].tv = 1.0f; // �ؽ�ó�� v��ǥ 1.0

#endif

		pVertices3[2 * i + 1].position = D3DXVECTOR3(sinf(theta)* 3000, 3000.0f, cosf(theta) * 3000);

		pVertices3[2 * i + 1].color = 0xff808080;

#ifndef SHOW_HOW_TO_USE_TCI

		// SHOW_HOW_TO_USE_TCI�� ����Ǿ� ������ �ؽ�ó ��ǥ�� �������� �ʴ´�

		pVertices3[2 * i + 1].tu = ((float)i) / (50 - 1); // �ؽ����� u��ǥ

		pVertices3[2 * i + 1].tv = 0.0f; // �ؽ����� v��ǥ 0.0

#endif

	}

	g_pVBTexture3->Unlock();


	////////////////////////////////////////////////////////////////

	

	return S_OK;
}



//-----------------------------------------------------------------------------
// �̸�: Cleanup()
// ���: �ʱ�ȭ�Ǿ��� ��� ��ü���� �����Ѵ�. 
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pVB != NULL)
		g_pVB->Release();
	if (g_pVBTexture != NULL)
		g_pVBTexture->Release();
	if (g_pVBTexture2 != NULL)
		g_pVBTexture2->Release();
	if (g_pVBTexture3 != NULL)
		g_pVBTexture3->Release();
	if (g_pTexture != NULL)
		g_pTexture->Release();
	if (g_pTexture2 != NULL)
		g_pTexture2->Release();

	if (g_pd3dDevice != NULL)    // ��ġ ��ü ���� 
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)          // D3D ��ü ���� 
		g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// �̸�: SetupViewProjection()
// ���: �� ��ȯ�� �������� ��ȯ�� �����Ѵ�. 
//-----------------------------------------------------------------------------
VOID SetupViewProjection()
{
	// �� ��ȯ ���� 
	D3DXVECTOR3 vEyePt(100.0f, 250.0f, -400.0f);    // ī�޶��� ��ġ 
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);       // �ٶ󺸴� ���� 
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);          // ������ ���� 
	D3DXMATRIXA16 matView;                           // �亯ȯ�� ��Ʈ���� 
													 // �� ��Ʈ���� ���� 
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	// Direct3D ��ġ�� �� ��Ʈ���� ���� 
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	// �������� ��ȯ ���� 
	D3DXMATRIXA16 matProj;   // �������ǿ� ��Ʈ���� 
							 // �������� ��Ʈ���� ���� 
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 5000.0f);
	// Direct3D ��ġ�� �������� ��Ʈ���� ���� 
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	
	g_pCamera->SetView(&vEyePt, &vLookatPt, &vUpVec);
}
VOID SetupViewProjectionMin()
{
	// �� ��ȯ ���� 
	D3DXVECTOR3 myEyes = g_PhyObject.GetCameraPos();
	D3DXVECTOR3 vLookatPt = g_PhyObject.GetCameraLookat();
	//D3DXVECTOR3 vEyePt(100.0f, 100.0f, 100.0f);
	//myEyes.y = myEyes.y+100;
	//char hh[100];
		//sprintf(hh, "%f	%f	%f", myEyes.x, myEyes.y, myEyes.z);
	//	MessageBox(NULL, hh, "a", 0);
	//D3DXVECTOR3 vEyePt(100.0f, 250.0f, -400.0f);    // ī�޶��� ��ġ 
	//D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);       // �ٶ󺸴� ���� 
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);          // ������ ���� 
	D3DXMATRIXA16 matView;                           // �亯ȯ�� ��Ʈ���� 
													 // �� ��Ʈ���� ���� 
	D3DXMatrixLookAtLH(&matView, &myEyes, &vLookatPt, &vUpVec);
	// Direct3D ��ġ�� �� ��Ʈ���� ���� 
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	// �������� ��ȯ ���� 
	D3DXMATRIXA16 matProj;   // �������ǿ� ��Ʈ���� 
							 // �������� ��Ʈ���� ���� 
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 5000.0f);
	// Direct3D ��ġ�� �������� ��Ʈ���� ���� 
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	g_pCamera->SetView(&myEyes, &vLookatPt, &vUpVec);
}

// ������ �̸� ���� ������ ���ϴ�.
D3DCOLORVALUE black = { 0, 0, 0, 1 };
D3DCOLORVALUE dark_gray = { 0.2f, 0.2f, 0.2f, 1.0f };
D3DCOLORVALUE gray = { 0.5f, 0.5f, 0.5f, 1.0f };
D3DCOLORVALUE red = { 1.0f, 0.0f, 0.0f, 1.0f };
D3DCOLORVALUE white = { 1.0f, 1.0f, 1.0f, 1.0f };
VOID SetupLight()
{
	D3DLIGHT9 light;                         // Direct3D 9 ���� ����ü ���� ����

	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;       // ���� Ÿ���� �𷺼ųη� ����
	light.Diffuse = white;                   // ������ �� ����
	light.Specular = white;
	light.Direction = D3DXVECTOR3(10, -10, 10);       //  ������ ���� (�����ϴ� ����) 
													  //light.Direction = D3DXVECTOR3(20*sin(g_counter*0.01f), -10, 10);       //  ������ ���� (�����ϴ� ����) 
													  //light.Direction = D3DXVECTOR3(10, 25, -40);       //  ������ ���� (�����ϴ� ����) 
	g_pd3dDevice->SetLight(0, &light);      // ����Ʈ ��ȣ ���� (���⿡���� 0��)
	g_pd3dDevice->LightEnable(0, TRUE);     // 0�� ����Ʈ �ѱ�


											// ����Ʈ ��� ����� TRUE�� ��. (�� ����� ���� ��� ����Ʈ ����� ������)
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	// ���������� �����Ʈ ����Ʈ �ѱ� (ȯ�汤�� ���� ����)
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00303030 );
}


void LeftMove(PhysicalObj obj) {

	D3DXMATRIXA16 matWorld = obj.GetWorldMatrix();
	D3DXMATRIXA16 matWorld2;

	D3DXMatrixRotationY(&matWorld2, -45.0f * (D3DX_PI / 90.0f));

	D3DXMatrixMultiply(&matWorld, &matWorld2, &matWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


	g_XFile.XFileDisplay(g_pd3dDevice);
}
//-----------------------------------------------------------------------------
// �̸�: Render()
// ���: ȭ���� �׸���.
//-----------------------------------------------------------------------------
VOID Render()
{
	if (NULL == g_pd3dDevice)  // ��ġ ��ü�� �������� �ʾ����� ���� 
		return;

	// �� �� �������� ��ȯ ����
	//SetupViewProjection();




	// �ﰢ���� ��/�� ���� ��� �������ϵ��� �ø� ����� ����.
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ������ ����. (���� ���� ������ �̳���, ���ؽ� ��ü�� ������ ����ϵ���)
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ����۸� ������ �������� �����.
	// ����۸� Ŭ����
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// ȭ�� �׸��� ���� 
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		///// ���ؽ� ��� 
		// ���ؽ� ���� ���� 
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX); // ���ؽ� ���� ���� 

		D3DXMATRIXA16 matWorld;  // ���� ��ȯ�� ��Ʈ���� ���� 







	///////////////���� ����/////////////

		D3DXMatrixScaling(&matWorld, 1.0f, 1.0f, 1.0f);
		g_pTerrain->draw(&matWorld, FALSE);	// TRUE for edge
		//////////////////////////////




		for (float x = -200; x <= 200; x += 20) {  // z �࿡ ������ ������ ���� �� �׸��� 
			D3DXMatrixTranslation(&matWorld, x, 0.0, 0.0);  // x�࿡ ���� ��ġ �̵� ��Ʈ����   
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld); // ��ȯ��Ʈ���� ���� 
			g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 2, 1);  // z�� ���� �׸��� 
		}

		for (float z = -200; z <= 200; z += 20) {  // x �࿡ ������ ������ ���� �� �׸��� 
			D3DXMatrixTranslation(&matWorld, 0.0, 0.0, z);  // z �࿡ ���� ��ġ �̵� ��Ʈ����
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);  // ��ȯ��Ʈ���� ���� 
			g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 1);   // x�� ���� �׸��� 
		}

		D3DXMatrixIdentity(&matWorld);   // ��Ʈ������ ���� ��ķ� ���� 
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);  // ��ȯ ��Ʈ���� ���� 
		g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 4, 1);   // y �� �׸��� 


		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		SetupLight();
		// X ���� ���
		//D3DXMatrixScaling(&matWorld, 100.0f, 100.0f, 100.0f);
		// tiger
		matWorld = g_PhyObject.GetWorldMatrix();
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		
		////////ȣ���� ����////////////////
		
		D3DXMATRIXA16 matWorld2;

		D3DXMatrixRotationY(&matWorld2, g_PhyObject.getDirection());
		D3DXMatrixMultiply(&matWorld, &matWorld2, &matWorld);
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
			////////ȣ���� ����////////////////




		g_XFile.XFileDisplay(g_pd3dDevice);
		// ball1
		matWorld = g_PhyBall1.GetWorldMatrix();
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_XBall1.XFileDisplay(g_pd3dDevice);

		// ball2
		matWorld = g_PhyBall2.GetWorldMatrix();
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_XBall2.XFileDisplay(g_pd3dDevice);


		// �ؽ�ó ���� ����

		// ���� ����
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		// �ؽ��� ���� (�ؽ��� ������ ���Ͽ� g_pTexture�� ����Ͽ���.)
		g_pd3dDevice->SetTexture(0, g_pTexture);

		// �ؽ��� ��� ȯ�� ����
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		// ����� ���ؽ� ���� ����
		
		g_pd3dDevice->SetStreamSource(0, g_pVBTexture, 0, sizeof(TEXTUREVERTEX));
		
		// FVF �� ����
		g_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
		// �簢�� ���� (�ﰢ�� 2���� �̿��Ͽ� �簢�� ������ �������) ��� 
		D3DXMatrixIdentity(&matWorld);   // ��Ʈ������ ���� ��ķ� ����
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);



		/*���� ����*/
	
	//	g_pd3dDevice->SetStreamSource(0, g_pVBTexture2, 0, sizeof(TEXTUREVERTEX));
		// FVF �� ����
		//g_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
		// �簢�� ���� (�ﰢ�� 2���� �̿��Ͽ� �簢�� ������ �������) ��� 
	//	D3DXMatrixIdentity(&matWorld);   // ��Ʈ������ ���� ��ķ� ����
	//	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	//	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		/*�������*/
		
		g_pd3dDevice->SetTexture(0, g_pTexture2);
		
		//g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g_pd3dDevice->SetStreamSource(0, g_pVBTexture3, 0, sizeof(TEXTUREVERTEX));

		g_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
		D3DXMatrixIdentity(&matWorld);   // ��Ʈ������ ���� ��ķ� ����
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);


		/*���⵵*/



		// �ؽ��� ���� ����
		g_pd3dDevice->SetTexture(0, NULL);


		// ȭ�� �׸��� �� 
		g_pd3dDevice->EndScene();
	}

	// ������� ������ ȭ������ ������. 
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}



//-----------------------------------------------------------------------------
// �̸� : MsgProc()
// ��� : ������ �޽��� �ڵ鷯 
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	static HANDLE hEditInfo = NULL;

	
	switch (msg)
	{
	case WM_CREATE:
	/*	hEditInfo= CreateWindowA("edit", "aaaaaaaaaaaaaaaaaaaaaaaaaaa", WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL, 10, 10, 200, 25, hWnd,NULL, NULL,NULL);*/
		return 0 ;

	case WM_LBUTTONDOWN:
		lbuttonDown = TRUE;
		pX = LOWORD(lParam);
		pY = HIWORD(lParam);
		break;
	case WM_MOUSEMOVE:

		if (lbuttonDown == TRUE) {
			short mX,mY;
			mX = LOWORD(lParam);
			mY = HIWORD(lParam);
			//char hh[100];
			//sprintf(hh, "pre: %d	cur: %d", pY, mY);
			//MessageBox(NULL, hh, "a", 0);
			if (pX - mX >25) {
				char hh[100];
				//sprintf(hh, "pre: %d	cur: %d", pX, mX);
				//MessageBox(NULL, hh, "a", 0);
				g_pCamera->RotateLocalY(-.02f);
				
			//	sprintf(hh, "pre: %d	cur: %d", pX, mX);
			//	MessageBox(NULL, hh, "b", 0);
				
			}
			if(mX - pX >25) {
				char hh[100];
				sprintf(hh, "pre: %d	cur: %d", pX, mX);
				//MessageBox(NULL, hh, "a", 0);
				g_pCamera->RotateLocalY(.02f);
				
			}
			if (pY - mY > 25) {
				char hh[100];
			//	sprintf(hh, "pre: %d	cur: %d", pY, mY);
				//MessageBox(NULL, hh, "a", 0);
				g_pCamera->RotateLocalX(-.02f);
			}
			if (mY - pY > 25) {
				g_pCamera->RotateLocalX(.02f);
			}
			//pX = mX;
		//	pY = pY;
			
			
			
		}
		break;
	case WM_LBUTTONUP:
		lbuttonDown = FALSE;
		break;
	case WM_DESTROY:
		Cleanup();   // ���α׷� ����� ��ü ������ ���Ͽ� ȣ���� 
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		if(flag)
			Render();    // ȭ�� ����� ����ϴ� ������ �Լ� ȣ�� 
		ValidateRect(hWnd, NULL);
		return 0;
		
	case WM_KEYDOWN:
		D3DXVECTOR3 location = g_PhyObject.GetLocation();
		switch (wParam)
		{

			//case VK_SPACE:
			//	
			//	g_PhyObject.SetPosition(location.x, location.y+=5, location.z);
			//	//g_PhyObject.AddVelocity(0.0f, 3.3f, 0);
			//	//g_PhyBall1.AddVelocity(0.0f, 3.3f, 0);
			//	break;
			//case VK_LEFT:
			//	//g_PhyObject.SetPosition(location.x, location.y, location.z);
			//	//g_PhyObject.AddVelocity(-0.1f,0,0);
			//	//g_PhyObject.SetAcceleration(-1.0f, 0, 0);
			//	g_PhyObject.AddVelocity(-0.5f, 0, 0);
			//	g_PhyBall1.AddVelocity(-0.1f, 0, 0);
			//	g_PhyObject.setDirection(LEFT);
			//	LeftMove(g_PhyObject);
			//	break;
			//case VK_RIGHT:
			//	g_PhyObject.setDirection(RIGHT);
			//	g_PhyObject.AddVelocity(0.5f, 0, 0);

			//	g_PhyBall1.AddVelocity(0.1f, 0, 0);
			//	break;
			//case VK_UP:
			//	g_PhyObject.setDirection(FOWARD);
			//	g_PhyObject.AddVelocity(0, 0, 0.5f);

			//	g_PhyBall1.AddVelocity(0, 0, 0.1f);
			//	break;
			//case VK_DOWN:
			//	g_PhyObject.setDirection(BACK);
			//	g_PhyObject.AddVelocity(0, 0, -0.5f);

			//	g_PhyBall1.AddVelocity(0, 0, -0.1f);
			//	break;
			//}
			break;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
static POINT p;
void ProcessKey(void)
{
	char hh[100];
	
	GetCursorPos(&p);
	
	ScreenToClient(hWnd, &p);
	
	
	RECT lpRect;
	GetClientRect(hWnd, &lpRect);

	//sprintf(hh, " %d,%d", lpRect.left,lpRect.right);
	//MessageBox(NULL, hh, "a", 0);
	POINT center;
	center.x =(lpRect.left+lpRect.right)/2;
	center.y = (lpRect.top + lpRect.bottom) / 2;
	
	
	if (p.x -pX< -20 && lbuttonDown == TRUE) {
		//g_pCamera->RotateLocalY(-.02f);
		g_PhyObject.hSeta+=2;
		//g_PhyObject.cLookat.x--;
		sprintf(hh, " %f", g_PhyObject.cLookat.y);
		//MessageBox(NULL, hh, "a", 0);
	}
	else if (p.x -pX> 20&& lbuttonDown == TRUE) {
		g_PhyObject.hSeta-=2;
		//g_PhyObject.cLookat.x++;
	}
	if (p.y - pY< -20 && lbuttonDown == TRUE) {
		g_PhyObject.vSeta += 2;
		//g_PhyObject.cLookat.y += 15;
		//g_PhyObject.cLookat.y++;
	}else if (p.y-pY > 20 && lbuttonDown == TRUE) {
		//g_PhyObject.cLookat.y -= 15;
		g_PhyObject.vSeta-=2;
		//g_PhyObject.cLookat.y--;
	}
	//sprintf(hh, " %d,%d",p.x,center.x);
	//MessageBox(NULL, hh, "a", 0);
	static bool jump;
	if (GetAsyncKeyState('W')) g_pCamera->MoveLocalZ(2.5f);	// ī�޶� ����!
	if (GetAsyncKeyState('S')) g_pCamera->MoveLocalZ(-2.5f);	// ī�޶� ����!
	if (GetAsyncKeyState('A')) g_pCamera->MoveLocalX(-2.5f);	// ī�޶� ����
	if (GetAsyncKeyState('D')) g_pCamera->MoveLocalX(2.5f);	// ī�޶� ������
/*
	if (GetAsyncKeyState('A')) g_pCamera->RotateLocalY(-.02f);
	if (GetAsyncKeyState('D')) g_pCamera->RotateLocalY(.02f);
	if (GetAsyncKeyState('W')) g_pCamera->RotateLocalX(-.02f);
	if (GetAsyncKeyState('S')) g_pCamera->RotateLocalX(.02f);*/
	D3DXVECTOR3 location = g_PhyObject.GetLocation();
	static float preY;
	if (g_PhyObject.GetJump() == TRUE) {
		
		
		
		
	}
	/*
		
	char hh[100];
	sprintf(hh, " %F", g_PhyObject.GetLocation().y);
	MessageBox(NULL, hh, "a", 0);

	*/
	if (GetAsyncKeyState(VK_SPACE) && g_PhyObject.GetJump() == FALSE ) {
		g_PhyObject.AddVelocity(0, 10, 0);
		g_PhyObject.SetJump(TRUE);
		
		

		
		//g_PhyObject.SetPosition(location.x, location.y += 5, location.z);
	}
	if (GetAsyncKeyState('A')) {
		
	//	g_PhyObject.AddVelocity(-0.2f, 0, 0);
	//	g_PhyBall1.AddVelocity(-0.1f, 0, 0);
		g_PhyObject.setDirection(LEFT);
		
		
	}
	if (GetAsyncKeyState('D')) {
		//SetupViewProjectionMin();
		g_PhyObject.setDirection(RIGHT);
	//	g_PhyObject.AddVelocity(0.2f, 0, 0);
		

		//g_PhyBall1.AddVelocity(0.1f, 0, 0);
	}
	if (GetAsyncKeyState('W')) {
		//SetupViewProjectionMin();
		g_PhyObject.setDirection(FOWARD);
	//	g_PhyObject.AddVelocity(0, 0, 0.2f);
		

		//g_PhyBall1.AddVelocity(0, 0, 0.1f);
	}
	if (GetAsyncKeyState('S')) {
		//SetupViewProjectionMin();
		g_PhyObject.setDirection(BACK);
	//	g_PhyObject.AddVelocity(0, 0, -0.2f);
	

		//g_PhyBall1.AddVelocity(0, 0, -0.1f);
	}


	D3DXMATRIXA16*	pmatView = g_pCamera->GetViewMatrix();		// ī�޶� ����� ��´�.
	g_pd3dDevice->SetTransform(D3DTS_VIEW, pmatView);			// ī�޶� ��� ����

}


void Action()
{

	//pVertices[0].position.x = g_PhyObject.cLookat.x - (1000* cos((90 + g_PhyObject.hSeta)*(D3DX_PI / 180)));
	//pVertices[1].position.x = g_PhyObject.cLookat.x + (1000 * cos((90 + g_PhyObject.hSeta)*(D3DX_PI / 180)));
	//pVertices[2].position.x = g_PhyObject.cLookat.x - (1000 * cos((90 + g_PhyObject.hSeta)*(D3DX_PI / 180)));
	//pVertices[3].position.x = g_PhyObject.cLookat.x + (1000 * cos((90 + g_PhyObject.hSeta)*(D3DX_PI / 180)));

	//pVertices[0].position.z = g_PhyObject.cLookat.z -(1000* sin((90 + g_PhyObject.hSeta)*(D3DX_PI / 180)));
	//pVertices[1].position.z = g_PhyObject.cLookat.z + (1000 * sin((90 + g_PhyObject.hSeta)*(D3DX_PI / 180)));
	//pVertices[2].position.z = g_PhyObject.cLookat.z - (1000 * sin((90 + g_PhyObject.hSeta)*(D3DX_PI / 180)));
	//pVertices[3].position.z = g_PhyObject.cLookat.z + (1000 * sin((90 + g_PhyObject.hSeta)*(D3DX_PI / 180)));

	//pVertices[0].position.y =  (sin((g_PhyObject.vSeta+30)*(D3DX_PI / 180))*g_PhyObject.cLookat.x) / (cos(g_PhyObject.hSeta*(D3DX_PI / 180))*cos((g_PhyObject.vSeta + 30)*(D3DX_PI / 180)));
	//pVertices[1].position.y =   (sin((g_PhyObject.vSeta + 30)*(D3DX_PI / 180))*g_PhyObject.cLookat.x) / (cos(g_PhyObject.hSeta*(D3DX_PI / 180))*cos((g_PhyObject.vSeta + 30)*(D3DX_PI / 180)));
//	pVertices[2].position.y =  (sin((g_PhyObject.vSeta - 30)*(D3DX_PI / 180))*g_PhyObject.cLookat.x) / (cos(g_PhyObject.hSeta*(D3DX_PI / 180))*cos((g_PhyObject.vSeta - 30)*(D3DX_PI / 180)));
	//pVertices[3].position.y =  (sin((g_PhyObject.vSeta - 30)*(D3DX_PI / 180))*g_PhyObject.cLookat.x) / (cos(g_PhyObject.hSeta*(D3DX_PI / 180))*cos((g_PhyObject.vSeta - 30)*(D3DX_PI / 180)));
	ProcessKey(); 
	SetupViewProjectionMin();
	g_PhyObject.ObjectMove();
	g_PhyBall1.Move();
	g_PhyBall2.Move();
	g_PhyBall1.Collision(&g_PhyBall2);
//	g_PhyObject.Collision(
	g_PhyObject.CollisionObject(&g_PhyBall2, GetAsyncKeyState('R'));
	g_PhyObject.CollisionObject(&g_PhyBall1, GetAsyncKeyState('R'));

	//flag = FALSE;
	g_board.x = 0;
	g_board.y = 50;
	g_board.z = 100;
	g_board.with = 1000;
	g_board.height = 55;
	if (g_PhyBall1.BoxCollision(&g_board)) {
		flag = false;
	}
}

//-----------------------------------------------------------------------------
// �̸�: WinMain()
// ���: ���α׷��� ������ 
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// ������ Ŭ���� ���� ���� �� ���� 
	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Game", NULL };
	// ������ Ŭ���� ��� 
	RegisterClassEx(&wc);
	g_pCamera = new ZCamera;
	// ������ ���� 
	hWnd = CreateWindow("D3D Game", "D3D Game Program",
		WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	// Direct3D �ʱ�ȭ�� �����ϸ� �����ϰ�, �����ϸ� �����Ѵ�.
	if (SUCCEEDED(InitD3D(hWnd)) &&       // Direct3D�� �ʱ�ȭ�� �����ϰ� 
		SUCCEEDED(InitGeometry()) &&         // ���ؽ� ���� ������ �����ϰ�
		SUCCEEDED(InitGeometryTexture()) &&  // �ؽ��� ���ؽ� ���� ������ �����ϰ� 
		SUCCEEDED(InitMeshData()))          // ��Ÿ ���� ����Ÿ �ε� 
	{
		// ������ ��� 
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		// �޽��� ���� �����ϱ�
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			// �޽��ڰ� ������ ���� �´�. 
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				Action();
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
	}

	delete g_pCamera;
	delete g_pTerrain;
	UnregisterClass("D3D Game", wc.hInstance);
	return 0;
}
