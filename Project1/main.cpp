/*****************************************
*                                        *
*        Li Qingquan  10154507139        *
*                                        *
*****************************************/

#include "d3dUtility.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <cstdlib>

using namespace std;

//
// Globals
//
IDirect3DDevice9* Device = 0;

const int Width = 1280;
const int Height = 960;

IDirect3DVertexBuffer9* VB = 0;
IDirect3DIndexBuffer9* IB = 0;

IDirect3DTexture9*      Tex = 0;

int VNUM = 1442;  // 顶点数
int INUM = 0;  // 面的统计量

// 计算三角形的面法向量
void ComputerNormal(D3DXVECTOR3*p0, D3DXVECTOR3*p1, D3DXVECTOR3*p2, D3DXVECTOR3*out) {
	D3DXVECTOR3 u = *p1 - *p0;
	D3DXVECTOR3 v = *p2 - *p0;
	D3DXVec3Cross(out, &u, &v);
	D3DXVec3Normalize(out, out);
}

//
// Classes and Structures
//
struct Vertex {
	Vertex() {};
	Vertex(float x, float y, float z)
	{
		_x = x;
		_y = y;
		_z = z;
	}
	float _x, _y, _z;
	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

//
// Framework Functions
//
bool Setup()
{
	// 计算面的统计量，用于创建索引缓存
	ifstream in_1;
	in_1.open("1.dat");
	if (in_1) {
		while (!in_1.eof()) {
			std::string line;
			getline(in_1, line);
			while (getline(in_1, line)) {
				if (line.empty())
					continue;
				string s1, s2, s3, s4, s5;
				istringstream is(line);
				is >> s1 >> s2 >> s3 >> s4 >> s5;
				if (s5 != "0") {
					INUM += 3;
				}
				else {
					INUM++;
				}
			}
		}
	}
	in_1.close();

	// 创建顶点缓存与索引缓存
	Device->CreateVertexBuffer(
		VNUM * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

	Device->CreateIndexBuffer(
		INUM * 3 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&IB,
		0);

	Vertex* vertices;
	VB->Lock(0, 0, (void**)&vertices, 0);

	// 读取文件2
	ifstream in_2;
	int num_of_vertex = 0;
	in_2.open("2.dat");
	if (in_2) {
		while (!in_2.eof()) {
			std::string line;
			while (getline(in_2, line)) {
				if (line.empty())
					continue;
				string s1, s2, s3, s4;
				istringstream is(line);
				is >> s1 >> s2 >> s3 >> s4;
				float f1, f2, f3, f4;
				f1 = atof(s1.c_str());
				f2 = atof(s2.c_str());
				f3 = atof(s3.c_str());
				f4 = atof(s4.c_str());
				vertices[num_of_vertex++] = Vertex(f2, f3, f4);
			}
		}
	}
	in_2.close();

	VB->Unlock();

	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);

	// 读取文件1
	int num_of_index = 0;
	in_1.open("1.dat");
	if (in_1) {
		while (!in_1.eof()) {
			std::string line;
			getline(in_1, line);
			while (getline(in_1, line)) {
				if (line.empty())
					continue;
				string s1, s2, s3, s4, s5;
				istringstream is(line);
				is >> s1 >> s2 >> s3 >> s4 >> s5;
				int i1, i2, i3, i4, i5;
				i1 = atoi(s1.c_str());
				i2 = atoi(s2.c_str());
				i3 = atoi(s3.c_str());
				i4 = atoi(s4.c_str());
				i5 = atoi(s5.c_str());
				if (i5 == 0) {
					i5 = i4;
				}
				indices[num_of_index] = i2;
				indices[num_of_index+1] = i3;
				indices[num_of_index+2] = i5;
				indices[num_of_index+3] = i4;
				indices[num_of_index+4] = i2;
				indices[num_of_index+5] = i5;
				num_of_index += 6;
			}
		}
	}
	in_1.close();

	IB->Unlock();

	/*D3DXCreateTextureFromFile(Device, "dx5_logo.bmp", &Tex);
	Device->SetTexture(0, Tex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);*/

	// 设置摄像机
	D3DXVECTOR3 position(0.0f, 0.0f, -100.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);

	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

void Cleanup()
{
	// 释放顶点缓存和索引缓存
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
}

bool Display(float timeDelta)
{
	if (Device)
	{
		//// 设置材质  
		//D3DMATERIAL9 mtrl;
		//::ZeroMemory(&mtrl, sizeof(mtrl));
		//mtrl.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.7f, 1.0f);
		//mtrl.Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		//mtrl.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
		//mtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	 //   Device->SetMaterial(&mtrl);
		//
		//D3DLIGHT9 light;
		//::ZeroMemory(&light, sizeof(light));
		//light.Type = D3DLIGHT_DIRECTIONAL;//方向光源  
		//light.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		//light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		//light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		//Device->SetLight(0, &light); //设置光源  
		//Device->LightEnable(0, true); //启用光照 

		//Device->SetRenderState(D3DRS_SPECULARENABLE, true);
		
		D3DXMATRIX Rx, Ry;

		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);

		static  float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;

		if (y >= 6.28f)
			y = 0.0f;

		D3DXMATRIX p = Rx * Ry;

		Device->SetTransform(D3DTS_WORLD, &p);

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

		Device->BeginScene();

		Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		Device->SetIndices(IB);
		Device->SetFVF(Vertex::FVF);

		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, VNUM, 0, INUM);
		
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}
