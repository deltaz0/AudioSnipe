// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <dsound.h>
#include <stdio.h>
#include <MMSystem.h>
#include <time.h>
#include "fftw3.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")



// include the Direct3D Library file
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct CUSTOMVERTEX {FLOAT X, Y, Z, RHW; DWORD COLOR;};
struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

DSBUFFERDESC bufferDesc;

HRESULT CreateBasicBuffer(LPDIRECTSOUND8 lpDirectSound, LPDIRECTSOUNDBUFFER8* ppDsb8);

IDirectSound8* m_DirectSound;
IDirectSoundBuffer* m_primaryBuffer;
IDirectSound3DListener8* m_listener;
IDirectSoundBuffer8* m_secondaryBuffer0;
IDirectSound3DBuffer8* m_secondary3DBuffer0;
IDirectSoundBuffer8* m_secondaryBuffer1;
IDirectSound3DBuffer8* m_secondary3DBuffer1;
IDirectSoundBuffer8* m_secondaryBuffer2;
IDirectSound3DBuffer8* m_secondary3DBuffer2;
IDirectSoundBuffer8* m_secondaryBuffer3;
IDirectSound3DBuffer8* m_secondary3DBuffer3;
IDirectSoundBuffer8* m_secondaryBuffer4;
IDirectSound3DBuffer8* m_secondary3DBuffer4;
IDirectSoundBuffer8* m_secondaryBuffer5;
IDirectSound3DBuffer8* m_secondary3DBuffer5;
IDirectSoundBuffer8* m_secondaryBuffer6;
IDirectSound3DBuffer8* m_secondary3DBuffer6;
IDirectSoundBuffer8* m_secondaryBuffer7;
IDirectSound3DBuffer8* m_secondary3DBuffer7;

IDirect3DVertexBuffer9* vBuffer;

int timer;
bool timeFlag;
RECT tRct;
RECT tRct2;
RECT sRct;
RECT sRct2;
int uiIter;

void DrawAThing(RECT tempRct, int r, int g, int b);
bool InitializeDirectSound(HWND hwnd);
void ShutdownDirectSound();
bool LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer);
void ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer);
bool PlayWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer, float positionX, float positionY, float positionZ);


int counter = 0;
int xPos;
int yPos;
RECT WindowRect;


// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXFONT m_font;



// function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);    // closes Direct3D and releases memory

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    // the handle for the window, filled by a function
    HWND hWnd;
    // this struct holds information for the window class
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass1";

    // register the window class
    RegisterClassEx(&wc);

    // create the window and use the result as the handle
    hWnd = CreateWindowEx(NULL,
                          "WindowClass1",    // name of the window class
                          "AudioSnipe",   // title of the window
                          WS_OVERLAPPEDWINDOW,    // window style
						  //WS_EX_TOPMOST | WS_POPUP,    // fullscreen values
                          0, 0,    // the starting x and y positions should be 0
                          SCREEN_WIDTH,    // width of the window
                          SCREEN_HEIGHT,    // height of the window
                          NULL,    // we have no parent window, NULL
                          NULL,    // we aren't using menus, NULL
                          hInstance,// application handle
                          NULL);    // used with multiple windows, NULL

	ShowCursor(false);

	//Set Rectangle for constraining cursor
	GetWindowRect(hWnd, &WindowRect);

    // display the window on the screen
    ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
    initD3D(hWnd);

	//vertex buffer for drawing rectangles
	d3ddev->CreateVertexBuffer(4*sizeof(CUSTOMVERTEX), NULL, CUSTOMFVF, D3DPOOL_DEFAULT, &vBuffer, NULL);
	
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_listener = 0;
	m_secondaryBuffer1 = 0;
	m_secondary3DBuffer1 = 0;
	timer = 0;
	timeFlag = false;
	uiIter = 0;
	bufferDesc.dwFlags = DSBCAPS_CTRLFREQUENCY;
	tRct.left = tRct.right = tRct.bottom = tRct.top = 0;
	tRct2.left = tRct2.right = tRct2.bottom = tRct2.top = 0;
	sRct.left = sRct.right = sRct.bottom = sRct.top = 0;
	sRct2.left = sRct2.right = sRct2.bottom = sRct2.top = 0;

	InitializeDirectSound(hWnd);

	LoadWaveFile("../a0.wav", &m_secondaryBuffer0, &m_secondary3DBuffer0);
	LoadWaveFile("../a1.wav", &m_secondaryBuffer1, &m_secondary3DBuffer1);
	LoadWaveFile("../a2.wav", &m_secondaryBuffer2, &m_secondary3DBuffer2);
	LoadWaveFile("../a3.wav", &m_secondaryBuffer3, &m_secondary3DBuffer3);
	LoadWaveFile("../a4.wav", &m_secondaryBuffer4, &m_secondary3DBuffer4);
	LoadWaveFile("../a5.wav", &m_secondaryBuffer5, &m_secondary3DBuffer5);
	LoadWaveFile("../a6.wav", &m_secondaryBuffer6, &m_secondary3DBuffer6);
	LoadWaveFile("../a7.wav", &m_secondaryBuffer7, &m_secondary3DBuffer7);
	//PlayWaveFile();
	//PlayWaveFile(0.0f,0.0f,0.0f);
	


    // enter the main loop:

    // this struct holds Windows event messages
    MSG msg;

    // wait for the next message in the queue, store the result in 'msg'
    // Enter the infinite message loop
    while(TRUE)
    {
        // Check to see if any messages are waiting in the queue
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // translate keystroke messages into the right format
            TranslateMessage(&msg);

            // send the message to the WindowProc function
            DispatchMessage(&msg);
        }

		//constrain cursor
		ClipCursor(&WindowRect);

		if(msg.wParam == VK_ESCAPE)
			break;

        // If the message is WM_QUIT, exit the while loop
        if(msg.message == WM_QUIT)
            break;

		sRct.left = SCREEN_WIDTH - (tRct.top*((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT));
		sRct.right = sRct.left + (SCREEN_WIDTH*0.1f);
		sRct.top = (tRct.left*((float)SCREEN_HEIGHT / (float)SCREEN_WIDTH));
		sRct.bottom = sRct.top + (SCREEN_HEIGHT*0.1f);

		sRct2.left = (tRct.top*((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT));
		sRct2.right = sRct2.left + (SCREEN_WIDTH*0.1f);
		sRct2.top = SCREEN_HEIGHT - (tRct.left*((float)SCREEN_HEIGHT / (float)SCREEN_WIDTH));
		sRct2.bottom = sRct2.top + (SCREEN_HEIGHT*0.1f);

		xPos = GET_X_LPARAM(msg.lParam);
		yPos = GET_Y_LPARAM(msg.lParam);

		float cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4 = 0.0f;

		cx1 = (((tRct.left-SCREEN_WIDTH*0.5f) + (sRct.left-SCREEN_WIDTH*0.5f))*0.5f)*1.414213562f;
		cy1 = (((tRct.top-SCREEN_HEIGHT*0.5f) + (sRct.bottom-SCREEN_HEIGHT*0.5f))*0.5f)*1.414213562f;
		cx3 = cx1*(-1.0f);
		cy3 = cy1*(-1.0f);
		cx2 = cy1*(-1.0f);
		cy2 = cx1;
		cx4 = cx3*(-1.0f);
		cy4 = cy3*(-1.0f);



		if(timeGetTime() - timer >= 30){
			timer = timeGetTime();
			//if(!timeFlag){
				//timeFlag = true;
				if(uiIter==0){
					uiIter = 1;
					PlayWaveFile(&m_secondaryBuffer7,&m_secondary3DBuffer7,(tRct.left - SCREEN_WIDTH*0.5f)*0.02f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
				}
				else if(uiIter==1){
					uiIter = 2;
					//PlayWaveFile(&m_secondaryBuffer3,&m_secondary3DBuffer3,(tRct2.left - SCREEN_WIDTH*0.5f)*0.02f,(tRct2.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
					PlayWaveFile(&m_secondaryBuffer6,&m_secondary3DBuffer6,cx1*0.02f,cy1*0.02f,0.0f);
				}
				else if(uiIter==2){
					uiIter = 3;
					//PlayWaveFile(&m_secondaryBuffer6,&m_secondary3DBuffer6,(sRct.left - SCREEN_WIDTH*0.5f)*0.02f,(sRct.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
					PlayWaveFile(&m_secondaryBuffer5,&m_secondary3DBuffer5,cx4*0.02f,cy4*0.02f,0.0f);
				}
				else if(uiIter==3){
					uiIter = 4;
					//PlayWaveFile(&m_secondaryBuffer2,&m_secondary3DBuffer2,(sRct2.left - SCREEN_WIDTH*0.5f)*0.02f,(sRct2.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
					PlayWaveFile(&m_secondaryBuffer4,&m_secondary3DBuffer4,(sRct.left - SCREEN_WIDTH*0.5f)*0.02f,(sRct.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
				}
				else if(uiIter==4){
					uiIter = 5;
					//PlayWaveFile(&m_secondaryBuffer5,&m_secondary3DBuffer5,cx1*0.02f,cy1*0.02f,0.0f);
					PlayWaveFile(&m_secondaryBuffer3,&m_secondary3DBuffer3,(sRct2.left - SCREEN_WIDTH*0.5f)*0.02f,(sRct2.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
				}
				else if(uiIter==5){
					uiIter = 6;
					//PlayWaveFile(&m_secondaryBuffer1,&m_secondary3DBuffer1,cx2*0.02f,cy2*0.02f,0.0f);
					PlayWaveFile(&m_secondaryBuffer2,&m_secondary3DBuffer2,cx3*0.02f,cy3*0.02f,0.0f);
				}
				else if(uiIter==6){
					uiIter = 7;
					//PlayWaveFile(&m_secondaryBuffer4,&m_secondary3DBuffer4,cx3*0.02f,cy3*0.02f,0.0f);
					PlayWaveFile(&m_secondaryBuffer1,&m_secondary3DBuffer1,cx2*0.02f,cy2*0.02f,0.0f);
				}
				else{
					uiIter = 0;
					PlayWaveFile(&m_secondaryBuffer0,&m_secondary3DBuffer0,(tRct2.left - SCREEN_WIDTH*0.5f)*0.02f,(tRct2.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
				}
			//}
				/*
			else{
				timeFlag = false;
				if(uiIter==0)
					PlayWaveFile(&m_secondaryBuffer0x,&m_secondary3DBuffer0x,(tRct.left - SCREEN_WIDTH*0.5f)*0.02f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
				else if(uiIter==1)
					PlayWaveFile(&m_secondaryBuffer1x,&m_secondary3DBuffer1x,(tRct2.left - SCREEN_WIDTH*0.5f)*0.02f,(tRct2.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
				else if(uiIter==2)
					PlayWaveFile(&m_secondaryBuffer2x,&m_secondary3DBuffer2x,(sRct.left - SCREEN_WIDTH*0.5f)*0.02f,(sRct.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
				else
					PlayWaveFile(&m_secondaryBuffer3x,&m_secondary3DBuffer3x,(sRct2.left - SCREEN_WIDTH*0.5f)*0.02f,(sRct2.top - SCREEN_HEIGHT*0.5f)*0.02f,0.0f);
			}
			*/
		}





		/*
		int ySec = 0;
		int yIter = (SCREEN_HEIGHT/6.0f);
		for(int co=0; co<6; co++){
			if((yPos>=yIter*co)&&(yPos<yIter*(co+1)))
				ySec = co;
		}
		
		ySec = 5-ySec;
		*/
		/*
		if(timeGetTime() - timer >= 125){
			timer = timeGetTime();
			if(!timeFlag){
				timeFlag = true;
				switch(ySec){
				case 0:
					PlayWaveFile(&m_secondaryBuffern2,&m_secondary3DBuffern2,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 1:
					PlayWaveFile(&m_secondaryBuffern1,&m_secondary3DBuffern1,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 2:
					PlayWaveFile(&m_secondaryBuffer0,&m_secondary3DBuffer0,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 3:
					PlayWaveFile(&m_secondaryBuffer1,&m_secondary3DBuffer1,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 4:
					PlayWaveFile(&m_secondaryBuffer2,&m_secondary3DBuffer2,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 5:
					PlayWaveFile(&m_secondaryBuffer3,&m_secondary3DBuffer3,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				default:
					PlayWaveFile(&m_secondaryBuffer0,&m_secondary3DBuffer0,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				}
			}
			else{
				timeFlag = false;
				switch(ySec){
				case 0:
					PlayWaveFile(&m_secondaryBuffern2x,&m_secondary3DBuffern2x,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 1:
					PlayWaveFile(&m_secondaryBuffern1x,&m_secondary3DBuffern1x,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 2:
					PlayWaveFile(&m_secondaryBuffer0x,&m_secondary3DBuffer0x,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 3:
					PlayWaveFile(&m_secondaryBuffer1x,&m_secondary3DBuffer1x,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 4:
					PlayWaveFile(&m_secondaryBuffer2x,&m_secondary3DBuffer2x,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				case 5:
					PlayWaveFile(&m_secondaryBuffer3x,&m_secondary3DBuffer3x,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				default:
					PlayWaveFile(&m_secondaryBuffer0x,&m_secondary3DBuffer0x,(tRct.left - SCREEN_WIDTH*0.5f)*0.01f,(tRct.top - SCREEN_HEIGHT*0.5f)*0.01f,0.0f);
					break;
				}
			}
		}
		*/

        // Run game code here
        render_frame();
    }

	//unconstrain cursor
	ClipCursor(0);
	ShowCursor(true);

	// clean up DirectX and COM
    cleanD3D();

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch(message)
    {
        // this message is read when the window is closed
        case WM_DESTROY:
            {
                // close the application entirely
                PostQuitMessage(0);
                return 0;
            } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}

// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface


    D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	//d3dpp.Windowed = FALSE; //program fullscreen, not windowed
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit
    d3dpp.BackBufferWidth = SCREEN_WIDTH;    // set the width of the buffer
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;    // set the height of the buffer

    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);

	// Create a D3DX font object
	D3DXCreateFont( d3ddev, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font );

}


// this is the function used to render a single frame
void render_frame(void)
{
    // clear the window to a deep blue
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

    d3ddev->BeginScene();    // begins the 3D scene

    
	//--------------------------------------------------
	//--------------------------------------------------
	// do 3D rendering on the back buffer here
	//--------------------------------------------------
	//--------------------------------------------------

	D3DCOLOR fontColor = D3DCOLOR_ARGB(200,200,200,255);    

	// Create a rectangle to indicate where on the screen it should be drawn
	RECT rct;
	rct.left=(SCREEN_WIDTH*0.45f);
	rct.right=rct.left+100;
	rct.top=(SCREEN_HEIGHT*0.4f);
	rct.bottom=rct.top+20;


	char tmpStrX[16];
	itoa(xPos,tmpStrX,10);
 
	// Draw some text 
	m_font->DrawText(NULL, tmpStrX, -1, &rct, 0, fontColor );

	char tmpStrY[16];
	itoa(yPos,tmpStrY,10);

	rct.left=(SCREEN_WIDTH*0.45f);
	rct.right=rct.left+100;
	rct.top=(SCREEN_HEIGHT*0.5f);
	rct.bottom=rct.top+20;

	// Draw some text 
	m_font->DrawText(NULL, tmpStrY, -1, &rct, 0, fontColor );

	rct.left=(SCREEN_WIDTH*0.45f);
	rct.right=(SCREEN_WIDTH*0.55f);
	rct.top=(SCREEN_HEIGHT*0.45f);
	rct.bottom=(SCREEN_HEIGHT*0.55f);

	DrawAThing(rct, 127, 0, 0);

	RECT mRct;

	mRct.left = xPos - (SCREEN_WIDTH*0.05f);
	mRct.right = xPos + (SCREEN_WIDTH*0.05f);
	mRct.top = yPos - (SCREEN_HEIGHT*0.05f);
	mRct.bottom = yPos + (SCREEN_HEIGHT*0.05f);

	DrawAThing(mRct, 0, 127, 0);


	tRct.left = rct.left + ((mRct.left - rct.left)*2.0f);
	tRct.right = tRct.left + (SCREEN_WIDTH*0.1f);
	tRct.top = rct.top + ((mRct.top - rct.top)*2.0f);
	tRct.bottom = tRct.top + (SCREEN_WIDTH*0.1f);

	DrawAThing(tRct, 80, 100, 120);

	tRct2.left = rct.left - ((mRct.left - rct.left)*1.0f);
	tRct2.right = tRct2.left + (SCREEN_WIDTH*0.1f);
	tRct2.top = rct.top - ((mRct.top - rct.top)*1.0f);
	tRct2.bottom = tRct2.top + (SCREEN_WIDTH*0.1f);

	DrawAThing(tRct2, 120, 100, 80);

	DrawAThing(sRct, 80, 100, 120);

	DrawAThing(sRct2, 100, 80, 120);



	//--------------------------------------------------
	//--------------------------------------------------
	//end 3d rendering
	//--------------------------------------------------
	//--------------------------------------------------


    d3ddev->EndScene();    // ends the 3D scene

    d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	ShutdownWaveFile(&m_secondaryBuffer7, &m_secondary3DBuffer7);
	ShutdownWaveFile(&m_secondaryBuffer6, &m_secondary3DBuffer6);
	ShutdownWaveFile(&m_secondaryBuffer5, &m_secondary3DBuffer5);
	ShutdownWaveFile(&m_secondaryBuffer4, &m_secondary3DBuffer4);
	ShutdownWaveFile(&m_secondaryBuffer3, &m_secondary3DBuffer3);
	ShutdownWaveFile(&m_secondaryBuffer2, &m_secondary3DBuffer2);
	ShutdownWaveFile(&m_secondaryBuffer1, &m_secondary3DBuffer1);
	ShutdownWaveFile(&m_secondaryBuffer0, &m_secondary3DBuffer0);

	ShutdownDirectSound();
	vBuffer->Release();
	m_font->Release();
    d3ddev->Release();    // close and release the 3D device
    d3d->Release();    // close and release Direct3D
}

void DrawAThing(RECT tempRct, int r, int g, int b){
	D3DCOLOR rColor = D3DCOLOR_XRGB(r,g,b);

	CUSTOMVERTEX quad[] =
	{
		{tempRct.left, tempRct.top, 0, 1.0f, rColor,},
		{tempRct.right, tempRct.top, 0, 1.0f, rColor,},
		{tempRct.left, tempRct.bottom, 0, 1.0f, rColor,},
		{tempRct.right, tempRct.bottom, 0, 1.0f, rColor,},
	};

	//d3ddev->CreateVertexBuffer(4*sizeof(CUSTOMVERTEX), NULL, CUSTOMFVF, D3DPOOL_DEFAULT, &vBuffer, NULL);

	VOID* pVoid;
	vBuffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, quad, sizeof(quad));
	vBuffer->Unlock();

	d3ddev->SetStreamSource(0, vBuffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->SetFVF(CUSTOMFVF);
	d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

bool InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	WAVEFORMATEX waveFormat;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if(FAILED(result))
	{
		return false;
	}

	// Obtain a listener interface.
	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener);
	if(FAILED(result))
	{
		return false;
	}

	// Set the initial position of the listener to be in the middle of the scene.
	m_listener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);

	return true;
}


void ShutdownDirectSound()
{

	// Release the listener interface.
	if(m_listener)
	{
		m_listener->Release();
		m_listener = 0;
	}

	// Release the primary sound buffer pointer.
	if(m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if(m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}

bool LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;


	// Open the wave file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the wave file header.
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Check that the chunk ID is the RIFF format.
	if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
	   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
	   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
	   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in mono format.
	if(waveFileHeader.numChannels != 1)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
	   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if(!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if(count != waveFileHeader.dataSize)
	{
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if(FAILED(result))
	{
		return false;
	}
	
	// Release the wave data since it was copied into the secondary buffer.
	delete [] waveData;
	waveData = 0;


	// Get the 3D interface to the secondary sound buffer.
	result = (*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{

	// Release the 3D interface to the secondary sound buffer.
	if(*secondary3DBuffer)
	{
		(*secondary3DBuffer)->Release();
		*secondary3DBuffer = 0;
	}

	// Release the secondary sound buffer.
	if(*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}


bool PlayWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer, float positionX, float positionY, float positionZ)
{
	HRESULT result;
	

	// Set position at the beginning of the sound buffer.
	result = (*secondaryBuffer)->SetCurrentPosition(0);
	if(FAILED(result))
	{
		return false;
	}

	// Set volume of the buffer to 100%.
	result = (*secondaryBuffer)->SetVolume(DSBVOLUME_MAX);
	if(FAILED(result))
	{
		return false;
	}

	// Set the 3D position of the sound.
	(*secondary3DBuffer)->SetPosition(positionX, positionY, positionZ, DS3D_IMMEDIATE);

	// Play the contents of the secondary sound buffer.
	result = (*secondaryBuffer)->Play(0, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}