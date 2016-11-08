#include <list>
#include <cstring>
#include "nwt/startup.h"
#include "nwt/nucwin.h"
#include "3deng_dx8/3deng.h"
#include "timing.h"
#include "tourta_part.h"
#include "scroller_part.h"
#include "fmod.h"
#include <cassert>

int ScreenX;
int ScreenY;

Widget *win;
Engine3D eng3d;
GraphicsContext *gc;
Timer timer;

DemoSystem *wedtro = 0;
TourtaPart *tourta_part = 0;
ScrollerPart *scroller_part = 0;

unsigned long custom_verts_fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

struct HVertex {
	float x, y, z, rhw;
	unsigned long color;

	HVertex() {}

	HVertex(float x, float y) {
		this->x = x;
		this->y = y;
		z = rhw = 0.01f;
		color = 0x7f000000;
	}
} quad[6];

FMUSIC_MODULE *mod;

bool init_done = false;

bool Init();
void MainLoop();
void CleanUp();
int KeyHandler(Widget *win, int key);
int CloseHandler(Widget *win, int arg);
int MouseHandler(Widget *win, int x, int y, bool left, bool middle, bool right);
int WheelHandler(Widget *win, int x, int y, int rot);


int main() {

	win = NWCreateWindow("Wedtro for Cybernoid & Eleni", NWT_CENTERX, NWT_CENTERY, 100, 50, NWT_WIN_TOPMOST);
	SetMainLoopFunc(MainLoop);
	SetHandler(HANDLER_KEY, KeyHandler);
	SetHandler(HANDLER_CLOSE, CloseHandler);
	//SetHandler(HANDLER_MOUSE, (HandlerFunctionPtr)MouseHandler);
	//SetHandler(HANDLER_WHEEL, (HandlerFunctionPtr)WheelHandler);

	if(!Init()) return 0;

	return NWMainLoop(RealTimeLoop);
}

bool Init() {

	ContextInitParameters cip;
	try {
		cip = eng3d.LoadContextParamsConfigFile("n3dinit.conf");
		gc = eng3d.CreateGraphicsContext(win, 0, &cip);
	}
	catch(EngineInitException except) {
		MessageBox(win, except.GetReason().c_str(), "Fatal Error", MB_OK | MB_ICONSTOP);
		return false;
	}
	ScreenX = cip.x;
	ScreenY = cip.y;
	NWResize(win, ScreenX, ScreenY);
	NWResizeClientArea(win, WS_OVERLAPPEDWINDOW);

	ShowCursor(false);

	gc->SetAmbientLight(0.3f);

	wedtro = new DemoSystem(gc);

	{
		// show loading screen
		Object *quad = new Object(gc);
		quad->CreatePlane(4.0f, 0);
		quad->Scale(1.3333f, 1.0f, 1.0f);
		quad->material = Material(1.0f, 1.0f, 1.0f);

		Matrix4x4 ViewMat;
		ViewMat.Translate(0.0f, 0.0f, 3.6f);
		gc->SetViewMatrix(ViewMat);
	
		gc->SetZBuffering(false);
		gc->SetAmbientLight(Color(1.0f, 1.0f, 1.0f));
		
		quad->material.SetTexture(gc->texman->LoadTexture("data/loading.jpg"), TextureMap);

		gc->Clear(0);
		quad->Render();
		gc->Flip();
		
		gc->SetAmbientLight(0l);

		gc->SetBackfaceCulling(true);
		gc->SetZBuffering(true);
		delete quad;
	}

	tourta_part = new TourtaPart(gc);
	tourta_part->SetTimingRel(0, 45000);
	
	scroller_part = new ScrollerPart(gc);
	scroller_part->SetTimingRel(44000, 68000);
	wedtro->AddPart(scroller_part);
	wedtro->AddPart(tourta_part);

	wedtro->Run();

	init_done = true;

	FSOUND_SetHWND(win);
	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
	FSOUND_SetBufferSize(200);
	FSOUND_Init(44100, 32, FSOUND_INIT_GLOBALFOCUS);
	mod = FMUSIC_LoadSong("data/THR-MOON.XM");
	FMUSIC_SetMasterVolume(mod, 250);
	FMUSIC_PlaySong(mod);

	quad[0] = HVertex(0.0f, 0.0f);
	quad[1] = HVertex(800.0f, 0.0f);
	quad[2] = HVertex(800.0f, 600.0f);
	quad[3] = quad[0];
	quad[4] = quad[2];
	quad[5] = HVertex(0.0f, 600.0f);

	
	timer.Start();
	return true;
}

void MainLoop() {
	float t = timer.GetMilliSec() / 1000.0f;
	if(t >= 112.0f) {
		NWCloseWindow(win);
		return;
	}

    gc->Clear(back_color.GetPacked32());
	gc->ClearZBufferStencil(1.0f, 0);

	wedtro->Update();
/*
	if(t > 59.0f) {
		for(int i=0; i<6; i++) {
			float a = (t-59.0f) * 0.5;
			quad[i].color = (int)(a * 255.0f) << 24;
		}

		gc->SetMaterial(Material(1.0f, 1.0f, 1.0f));
		gc->SetAlphaBlending(true);
		gc->SetZBuffering(false);
		gc->SetLighting(false);
		gc->SetBlendFunc(BLEND_SRCALPHA, BLEND_INVSRCALPHA);
		gc->SetVertexProgram(custom_verts_fvf);
	
        gc->D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, quad, sizeof(HVertex));
		gc->SetVertexProgram(VertexFormat);	
		gc->SetLighting(true);
		gc->SetZBuffering(true);
		gc->SetAlphaBlending(false);
	}
*/
	gc->Flip();
	Sleep(10);
}

void CleanUp() {
	ShowCursor(true);
	FMUSIC_FreeSong(mod);
	FSOUND_Close();
	delete wedtro;
}

///////// handlers /////////
int KeyHandler(Widget *win, int key) {
	switch(key) {
	case VK_ESCAPE:
		NWCloseWindow(win);
		break;

	}
	return 0;
}

int CloseHandler(Widget *win, int arg) {
	CleanUp();
	return 0;
}

int WheelHandler(Widget *win, int x, int y, int rot) {
	float tr = rot > 0 ? 0.05f : -0.05f;

	return 0;

	Camera *cam = wedtro->GetActivePart()->GetScene()->GetActiveCamera();

	cam->Zoom(tr);
	
	return 0;
}

int MouseHandler(Widget *win, int x, int y, bool left, bool middle, bool right) {
	static POINT PrevPos;
	POINT MoveDiff;

	Camera *cam = wedtro->GetActivePart()->GetScene()->GetActiveCamera();

	if(left) {
	}
	
	if(right) {
		MoveDiff.x = x - PrevPos.x;
		MoveDiff.y = y - PrevPos.y;
		float xangle = (float)MoveDiff.x / 50.0f;
		float dy = (float)MoveDiff.y;

		cam->Rotate(0.0f, xangle, 0.0f);
		cam->SetPosition(cam->GetPosition() + Vector3(0.0f, dy, 0.0f));
	}

	if(middle) {
	}

	PrevPos.x = x;
	PrevPos.y = y;

	return 0;
}