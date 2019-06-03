#include <windows.h>
#include <stdio.h>
#include <float.h>
#include "system\memory.h"
#include "system\pak.h"
#include <direct.h>
#include "external\fmod\api\inc\fmod.h"

#include "TheAquarium.h"

#define SLAG		(60.0/150.0)
#define TAKT		(SLAG*4)
#define NUMSCENES	(51)

int StartScene = 0;
int StopScene = NUMSCENES-1;
int StartTime = 0;


extern void *ORoutines[];
extern void *LRoutines[];
extern void *PRoutines[];


//---- Timerplugin
class TimerPlugin : public Plugin
{
public:
	TimerPlugin();
	static void Process(void);

	uint32		NrEvents;
	uint32		EventCounter;
	uint32		Events[100];
	int32		StartTime;
};//TimerPlugin
//----


//---- Div data...
TimerPlugin			Timer;
FSOUND_STREAM		*Music;


//---- Loadfunctions
char*		FMOD_ErrorString(long errcode);
bool		SetupSound();
bool		LoadIntro();

bool		LoadScenes(uint32 Start, uint32 Stop);

//---- Fishlists for all parts...
Scene		*Scenes[NUMSCENES]; 
Fishes		FishLists[NUMSCENES], LightLists[NUMSCENES];




//---- Filenames for all the scenes
char		Filenames [][100]= {
								"data\\intro\\scenes\\cut1.scn",					//0
								"data\\intro\\scenes\\cut2.scn",					//1

								"data\\startpit\\scenes\\cut1.scn",					//2
								"data\\startpit\\scenes\\cut2.scn",					//3
								"data\\startpit\\scenes\\cut3.scn",					//4

								"data\\canyon\\scenes\\cut1.scn", 					//5
								"data\\canyon\\scenes\\cut2.scn", 					//6			
								"data\\canyon\\scenes\\cut3.scn", 					//7			
								"data\\canyon\\scenes\\cut4.scn", 					//8			
								"data\\canyon\\scenes\\cut5.scn",					//9
								"data\\canyon\\scenes\\cut6.scn",					//10
								"data\\canyon\\scenes\\cut7.scn",					//11
								"data\\canyon\\scenes\\cut8.scn",					//12
								"data\\canyon\\scenes\\cut9.scn",					//13
								"data\\canyon\\scenes\\cut10.scn",					//14

								"data\\overlay5\\scenes\\cut1.scn",					//15
								
								"data\\podrace_bigwall\\scenes\\cut1.scn", 			//16					
								"data\\podrace_bigwall\\scenes\\cut2.scn", 			//17				
								"data\\podrace_bigwall\\scenes\\cut3.scn",			//18
								"data\\podrace_bigwall\\scenes\\cut4.scn",			//19

								"data\\overlay5\\scenes\\cut2.scn",					//20

								"data\\city1\\scenes\\cut1.scn",					//21
								"data\\city1\\scenes\\cut1b.scn",					//22
								"data\\city1\\scenes\\cut2.scn",					//23 
								"data\\city1\\scenes\\cut3.scn",					//24
								"data\\city1\\scenes\\cut4.scn",					//25
								"data\\city1\\scenes\\cut5.scn",					//26
								"data\\city1\\scenes\\cut6.scn",					//27
								"data\\city1\\scenes\\cut8.scn",					//28

								"data\\garage\\scenes\\cut1.scn",					//29
								"data\\garage\\scenes\\cut2.scn",					//30
								"data\\garage\\scenes\\cut3.scn",					//31
								"data\\garage\\scenes\\cut3b.scn",					//32
								"data\\garage\\scenes\\cut4.scn",					//33
								"data\\garage\\scenes\\cut5.scn",					//34
								"data\\garage\\scenes\\cut6.scn",					//35
								"data\\garage\\scenes\\cut7.scn",					//36
								"data\\garage\\scenes\\cut8.scn",					//37
								"data\\garage\\scenes\\cut9.scn",					//38
								"data\\garage\\scenes\\cut10.scn",					//39
								"data\\garage\\scenes\\cut11.scn",					//40

								"data\\goal\\scenes\\cut1.scn",					//41
								"data\\goal\\scenes\\cut2.scn",					//42
								"data\\goal\\scenes\\cut5.scn",					//43 
								"data\\goal\\scenes\\cut3.scn",					//44
								"data\\goal\\scenes\\cut4.scn",					//45 
								"data\\goal\\scenes\\cut6.scn",					//46 
								"data\\goal\\scenes\\cut7.scn",					//47 
								"data\\goal\\scenes\\cut8.scn",					//48
								"data\\goal\\scenes\\cut9.scn",					//49
								"data\\scrolltext\\scenes\\scr03.scn",			//50
								};





//---- Common fishes...
Keypressed			K;
ViewCanvas			VC;
ClearCanvas			CC;
SquidReset			Reset;
SquidRun			Run;
RunLightPlugins		LightP;
SquidSort			Sort;
AquaTimer			T;
MessageLoop			MLoop;
Screenshot			SS("shot");
DiskWriter			DW("e:\\tmp\\f");
Fish				Null;
ClearMotionBlur		MB;







int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{

	char *StartSceneParam = lpCmdLine;
	if (StartSceneParam[0] != '\0')
	{
		StartScene = atoi(StartSceneParam);
		if (StartScene >= NUMSCENES) StartScene = 0;

		char *StopSceneParam = strchr(StartSceneParam, ' ');
		if (StopSceneParam != NULL)
		{
			StopSceneParam[0] = '\0';
			StopSceneParam++;
			StopScene = atoi(StopSceneParam);
			if (StopScene < StartScene) StopScene = StartScene;
			if (StopScene >= NUMSCENES) StopScene = NUMSCENES - 1;
		}//if
	}//if
	
	if (StartScene != 0)
		StartTime = Timer.Events[StartScene - 1];

//---- Initialize debug-file...
#ifdef _DEBUG
	AquaErr::DebugInit("debug.txt");
#endif

//	PAK::Open("racer.tls");
//	PAK::LogActivity("filelog.txt");

//---- Setting up general crap
	Null.Type = 16;
	Null.InsertPlugin(&Timer, false);



//---- Setting up sound
	if (!SetupSound())
		Aquarium::Shutdown(AquaErr::ErrorBuffer);


//---- Setting up aquarium and opengl
	DebugMsg1("Setting up OpenGL\n");
	TheAquarium.XRes = 640;
	TheAquarium.YRes = 480;
	TheAquarium.Bits = 32;
    TheAquarium.FullScreen = true;
	TheAquarium.WaitVTR = true;

	if ( !TheAquarium.FillOpenGL((void**)&ORoutines, (void**)&LRoutines, (void**)&PRoutines, hInst) ) return false;


	ClearCanvas::Swim_OpenGl();
	ViewCanvas::Swim_OpenGl();

//---- Loading scenes...
	memset(Scenes, 0, sizeof(Scene*)*NUMSCENES);
	if (!LoadScenes(StartScene, StopScene))
		Aquarium::Shutdown(AquaErr::ErrorBuffer);



	TheAquarium.FishList	= FishLists[StartScene];			//Set the aquarium's objectlist to this one
	TheAquarium.LightList	= LightLists[StartScene];			//Set the aquarium's lightlist to this one

	ClearCanvas::Swim_OpenGl();
	ViewCanvas::Swim_OpenGl();

	Sleep(2000);

//---- Start music
	FSOUND_Stream_SetTime(Music, StartTime);
//	FSOUND_Stream_SetTime(Music, (int)(51.2*1000));
//	FSOUND_Stream_SetTime(Music, (int)(126*1000));
//	FSOUND_Stream_SetTime(Music, (int)(153.6*1000));
	FSOUND_Stream_Play(0, Music);


//---- Setting timerstuff
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	Freq.QuadPart /= 1000;

	LARGE_INTEGER TimerValue;
	QueryPerformanceCounter(&TimerValue);
	TheAquarium.Ticks = (unsigned long)(TimerValue.QuadPart/Freq.QuadPart);

	Scenes[StartScene]->StartTime = TheAquarium.Ticks;
	DebugMsg1("Running mainloop\n");
	Timer.StartTime = TheAquarium.Ticks - StartTime;
	Timer.EventCounter = StartScene;


//---- Setup and run mainloop
	Mainloop M(&TheAquarium, TRUE);						
	M.Feed();
	DebugMsg1("Returned from mainloop\n");



//---- Cleaning up...
	for (int C1 = 0; C1 < NUMSCENES; C1++)
		delete Scenes[C1];


//---- Shutting down...
	DebugMsg1("Aquarium shutdown\n");
	Aquarium::Shutdown(AquaErr::ErrorBuffer);
	return 0;
}//main







/*
-------------------------------- C++ ROUTINE ---------------------------------
  Name		: SetupSound
  Author	: 
  Action	: Loads music and samples...

  Notes:
------------------------------------------------------------------------------
*/
bool SetupSound()
{
//---- Initializing FMOD...
	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
	if (!FSOUND_Init(44100, 32, 0) )
	{
		FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
		if (!FSOUND_Init(44100, 32, 0) )
		{
			AquaErr::ErrorMsg(3, "Game::SetupSound: FMOD error ", FMOD_ErrorString(FSOUND_GetError()), "\n");
			return false;
		}//if
	}//if

	//Register callback on application shutdown 
	Aquarium::RegOnShutdown((void (__cdecl *)(void))FSOUND_Close);


//---- Loading music...
	void *MusicData = PAK::ReadFile("themusic.mp3");
	uint32 FSize = PAK::FileSize("themusic.mp3");
	Music = FSOUND_Stream_OpenFile((const char*)MusicData, FSOUND_2D | FSOUND_LOADMEMORY, FSize);
	if (!Music)
	{
		AquaErr::ErrorMsg(3, "Game::SetupSound: FMOD error ", FMOD_ErrorString(FSOUND_GetError()), "\n");
		return false;
	}

	return true;
}


char *FMOD_ErrorString(long errcode)
{
	switch (errcode)
	{


		case FMOD_ERR_NONE:				return "No errors";
		case FMOD_ERR_BUSY:				return "Cannot call this command after FSOUND_Init.  Call FSOUND_Close first.";
		case FMOD_ERR_UNINITIALIZED:	return "This command failed because FSOUND_Init was not called";
		case FMOD_ERR_PLAY:				return "Playing the sound failed.";
		case FMOD_ERR_INIT:				return "Error initializing output device.";
		case FMOD_ERR_ALLOCATED:		return "The output device is already in use and cannot be reused.";
		case FMOD_ERR_OUTPUT_FORMAT:	return "Soundcard does not support the features needed for this soundsystem (16bit stereo output)";
		case FMOD_ERR_COOPERATIVELEVEL:	return "Error setting cooperative level for hardware.";
		case FMOD_ERR_CREATEBUFFER:		return "Error creating hardware sound buffer.";
		case FMOD_ERR_FILE_NOTFOUND:	return "File not found";
		case FMOD_ERR_FILE_FORMAT:		return "Unknown file format";
		case FMOD_ERR_FILE_BAD:			return "Error loading file";
		case FMOD_ERR_MEMORY:			return "Not enough memory ";
		case FMOD_ERR_VERSION:			return "The version number of this file format is not supported";
		case FMOD_ERR_INVALID_PARAM:	return "An invalid parameter was passed to this function";
		case FMOD_ERR_NO_EAX:			return "Tried to use an EAX command on a non EAX enabled channel or output.";
		case FMOD_ERR_NO_EAX2:		    return "Tried to use an advanced EAX2 command on a non EAX2 enabled channel or output";
		case FMOD_ERR_CHANNEL_ALLOC:	return "Failed to allocate a new channel";
		case FMOD_ERR_RECORD:			return "Recording is not supported on this machine";
		case FMOD_ERR_MEDIAPLAYER:		return "Windows Media Player not installed so cant play wma or use internet streaming.";
		default :						return "Unknown error";
	}
}



/*
Loads scenes from [Start] to and including [Stop]
*/
bool LoadScenes(uint32 Start, uint32 Stop)
{
	uint32 C1, C2;

	for (C2 = Start; C2 <= Stop; C2++)
	{
		Scenes[C2] = new Scene;
		if ( !Scenes[C2]->Load(Filenames[C2], SCENE_FILE_FlagDefault) ) 
			return false;

		if ( !Scenes[C2]->SetupOpenGL() )
		{
			AquaErr::ErrorMsg(1, "Main: Error setting up scene for OpenGL\n");
			return false;
		}

	//---- Building objectlist
		if ( C2 != 0)
		{
			if ( !FishLists[C2].InsertFish(&CC, FALSE) ) return false;
		}//if
		else
			if ( !FishLists[C2].InsertFish(&MB, FALSE) ) return false;



		if ( !FishLists[C2].InsertFish(&Reset, FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(Scenes[C2], FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(&LightP, FALSE) ) return false;
		for ( C1 = 0; C1 < Scenes[C2]->NrVectors; C1++)
			if ( !FishLists[C2].InsertFish(Scenes[C2]->Vectors[C1].V, FALSE) ) return false;

		if ( !FishLists[C2].InsertFish(&Sort, FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(&Run, FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(&SS, FALSE) ) return false;
//		if ( !FishLists[C2].InsertFish(&DW, FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(&VC, FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(&T, FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(&K, FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(&MLoop, FALSE) ) return false;
		if ( !FishLists[C2].InsertFish(&Null, FALSE) ) return false;


	//---- Building lightlist
		for ( C1 = 0; C1 < Scenes[C2]->NrLights; C1++)
			if ( !LightLists[C2].InsertFish(Scenes[C2]->Lights[C1].L, FALSE) ) return false;

	}//For



	if (Scenes[0])
	{
	}//if

	return true;
}//LoadCity






TimerPlugin::TimerPlugin()
{
	Run			= Process;

	NrEvents		= 51;
	EventCounter	= 0;

	Events[ 0]		= (unsigned long)(32*1000);			//Intro, cut2

	Events[ 1]		= (unsigned long)(37 *1000);			//Startpit, cut1
	Events[ 2]		= (unsigned long)(44*1000);			//Startpit, cut2
	Events[ 3]		= (unsigned long)(48*1000);			//Startpit, cut3

	Events[ 4]		= (unsigned long)(51.2*1000);		//Canyon, cut1
	Events[ 5]		= (unsigned long)(53.4*1000);		//Canyon, cut2
	Events[ 6]		= (unsigned long)(57.4*1000);		//Canyon, cut3
	Events[ 7]		= (unsigned long)(59.4*1000);		//Canyon, cut4
	Events[ 8]		= (unsigned long)(63.9*1000);		//Canyon, cut5
	Events[ 9]		= (unsigned long)(68.0*1000);		//Canyon, cut6
	Events[10]		= (unsigned long)(72.0*1000);		//Canyon, cut7
	Events[11]		= (unsigned long)(73.7*1000);		//Canyon, cut8
	Events[12]		= (unsigned long)(76.8*1000);		//Canyon, cut9
	Events[13]		= (unsigned long)(83.4*1000);		//Canyon, cut10

	Events[14]		= (unsigned long)(87.6*1000);		//Overlay5, cut1

	Events[15]		= (unsigned long)( 97.4*1000);		//bigwall, cut1
	Events[16]		= (unsigned long)(102.4*1000);		//bigwall, cut2
	Events[17]		= (unsigned long)(105.4*1000);		//bigwall, cut3
	Events[18]		= (unsigned long)(111.4*1000);		//bigwall, cut1

	Events[19]		= (unsigned long)(117.4*1000);		//Overlay5, cut2

	Events[20]		= (unsigned long)(124*1000);		//city, cut1
	Events[21]		= (unsigned long)(128*1000);		//city, cut1b
	Events[22]		= (unsigned long)(131*1000);		//city, cut2
	Events[23]		= (unsigned long)(134*1000);		//city, cut3
	Events[24]		= (unsigned long)(137*1000);		//city, cut4
	Events[25]		= (unsigned long)(142*1000);		//city, cut5
	Events[26]		= (unsigned long)(145*1000);		//city, cut6
	Events[27]		= (unsigned long)(150*1000);		//city, cut8

	Events[28]		= (unsigned long)(153.6*1000);			//->Cut1
	Events[29]		= (unsigned long)(162.6*1000);			//->Cut2
	Events[30]		= (unsigned long)(170.6*1000);			//->Cut3
	Events[31]		= (unsigned long)(179.6*1000);			//->Cut3b
	Events[32]		= (unsigned long)(188.6*1000);			//->Cut4
	Events[33]		= (unsigned long)(191.6*1000);			//->Cut5
	Events[34]		= (unsigned long)(194.1*1000);			//->Cut6
	Events[35]		= (unsigned long)(197.1*1000);			//->Cut7
	Events[36]		= (unsigned long)(199.1*1000);			//->Cut8
	Events[37]		= (unsigned long)(200.6*1000);			//->Cut9
	Events[38]		= (unsigned long)(202.1*1000);			//->Cut10
	Events[39]		= (unsigned long)(207.4*1000);			//->Cut11

	Events[40]		= (unsigned long)(211.2*1000);			//->Cut1
	Events[41]		= (unsigned long)(214.2*1000);			//->Cut2
	Events[42]		= (unsigned long)(215.7*1000);			//->Cut5
	Events[43]		= (unsigned long)(218.7*1000);			//->Cut3
	Events[44]		= (unsigned long)(221.7*1000);			//->Cut4
	Events[45]		= (unsigned long)(226.4*1000);			//->Cut6
	Events[46]		= (unsigned long)(228.4*1000);			//->Cut7
	Events[47]		= (unsigned long)(232.8*1000);			//->Cut8
	Events[48]		= (unsigned long)(236.8*1000);			//->Cut9
	Events[49]		= (unsigned long)(247.4*1000);			//->Cut9

	Events[50]		= (unsigned long)(297.4*1000);			//->quit
}

void TimerPlugin::Process(void)
{
	TimerPlugin* P = (TimerPlugin*)TheAquarium.CurrentPlugin;
	if (P->EventCounter == P->NrEvents) return;

	if ((TheAquarium.Ticks - P->StartTime) >= P->Events[P->EventCounter])
	{
		if (P->EventCounter == (P->NrEvents - 1))
		{
			TheAquarium.QuitFlag = 1;
		}//if
		else
		{
			TheAquarium.FishList	= FishLists[P->EventCounter+1];			//Set the aquarium's objectlist to this one
			TheAquarium.LightList	= LightLists[P->EventCounter+1];			//Set the aquarium's lightlist to this one
			Scenes[P->EventCounter+1]->StartTime = P->Events[P->EventCounter] + P->StartTime;

			P->EventCounter++;
		}//else
	}//if
//	TheAquarium.Ticks += 40;
}