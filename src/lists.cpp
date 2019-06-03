#include "TheAquarium.h"

void *ORoutines[] = {
	NULL,							//00
	Mainloop::Swim,					//01
	Keypressed::Swim,				//02
	ViewCanvas::Swim_OpenGl,		//03
	ClearMotionBlur::Swim_OpenGl,	//04
	SquidReset::Swim,				//05
	SquidRun::Swim,					//06
	Scene::Swim,					//07
	RunLightPlugins::Swim,			//08
	DiskWriter::Swim,				//09
	AquaTimer::Swim,				//10
	SquidSort::Swim,				//11
	(void*)(-1),					//12 Not used
	Vector_Calls::OpenGL,			//13
	Vector_Calls::OpenGL_NV,		//14
	(void*)(-1),					//15
	Vector_Calls::NullObject,		//16
	(void*)(-1),					//17 Reserved for Gamescene
	(void*)(-1),					//18 Reserved for Control
	Screenshot::Swim,				//19
	ClearCanvas::Swim_OpenGl,		//20
	(void*)(-1),					//21
	(void*)(-1),					//22
	(void*)(-1),					//23
	(void*)(-1),					//24
	MessageLoop::Swim,				//25
	NULL,
};

void *LRoutines[] = {
	NULL,
	Light_Calls::DistantLight,
	Light_Calls::Pointlight,
	Light_Calls::Spotlight,
	NULL,
};

void *PRoutines[] = {
	NULL,
	(void*)(-1),//OpenGL_Face_Calls::Flat,
	(void*)(-1),//OpenGL_Face_Calls::Flat_Perspective_TMap,
	(void*)(-1),//OpenGL_Face_Calls::Flat_Perspective_TMap,
	(void*)(-1),//OpenGL_Face_Calls::Flat_Transparent,
	(void*)(-1),//OpenGL_Face_Calls::Flat_Transparent_Perspective_TMap_Additive,
	(void*)(-1),//OpenGL_Face_Calls::Flat_Transparent_Perspective_TMap,
	OpenGL_Face_Calls::Gourad,
	OpenGL_Face_Calls::Gourad_Perspective_TMap,
	OpenGL_Face_Calls::Gourad_Transparent,
	OpenGL_Face_Calls::Gourad_Transparent_Perspective_TMap_Additive,
	OpenGL_Face_Calls::Gourad_Transparent_Perspective_TMap,
	Sprite::Sprite_OpenGL_Additive,
	AquaText::Draw,
	OpenGL_Face_Calls::Gourad_Transparent_Additive,
	NULL,
};//
