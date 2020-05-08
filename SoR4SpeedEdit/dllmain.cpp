#include "stdafx.h"
#include <windows.h>
#include <timeapi.h>
#include "include\detours.h"
#include <cstdint>
#include <tchar.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Lib/detours.lib")
#pragma pack(1)

int speedModifier = 115;

DWORD (WINAPI *origGetTickCount)() = GetTickCount;
BOOL (WINAPI *origQueryPerformanceCounter)(LARGE_INTEGER* lpPerformanceCount) = QueryPerformanceCounter;

DWORD initialTick;
DWORD WINAPI detourGetTickCount ()
{
	const auto currentTick = origGetTickCount ();
	return (initialTick + (currentTick - initialTick) * speedModifier) / 100;
}

int64_t initialPerformanceCounter;
BOOL WINAPI detourQueryPerformanceCounter (LARGE_INTEGER* lp_performance_count)
{
	int64_t currentPerformanceCounter;
	if (!origQueryPerformanceCounter (reinterpret_cast<LARGE_INTEGER*>(&currentPerformanceCounter)))
		return FALSE;

	long long new_time = (currentPerformanceCounter + (currentPerformanceCounter - initialPerformanceCounter) * speedModifier) /100;
	*lp_performance_count = *reinterpret_cast<LARGE_INTEGER*>(&new_time);

	return TRUE;
}

void Setup ()
{
	initialTick = GetTickCount ();
	QueryPerformanceCounter (reinterpret_cast<LARGE_INTEGER*>(&initialPerformanceCounter));

	DetourTransactionBegin ();
	DetourUpdateThread (GetCurrentThread ());
	DetourAttach (&reinterpret_cast<PVOID&>(origGetTickCount), detourGetTickCount);
	DetourAttach (&reinterpret_cast<PVOID&>(origQueryPerformanceCounter), detourQueryPerformanceCounter);
	DetourTransactionCommit ();
}

INT APIENTRY DllMain (HMODULE h_dll_handle, DWORD n_reason, LPVOID reserved)
{
	static HINSTANCE hL;
	HANDLE hThread = nullptr;
	switch (n_reason)
	{
		case DLL_PROCESS_ATTACH:
			hL = LoadLibrary (_T (".\\MojoShader_Original.dll"));
			if (!hL) return false;

			hThread = CreateThread (0, 0, (LPTHREAD_START_ROUTINE)Setup, 0, 0, 0);
			break;

		case DLL_PROCESS_DETACH:
			FreeLibrary (hL);
			break;

		default:
			break;
	}

	if (hThread != nullptr)
	{
		return TRUE;
	}

	return FALSE;
}

#pragma comment(linker, "/export:MOJOSHADER_version=MojoShader_Original.MOJOSHADER_version")
#pragma comment(linker, "/export:MOJOSHADER_changeset=MojoShader_Original.MOJOSHADER_changeset")
#pragma comment(linker, "/export:MOJOSHADER_maxShaderModel=MojoShader_Original.MOJOSHADER_maxShaderModel")
#pragma comment(linker, "/export:MOJOSHADER_freePreshader=MojoShader_Original.MOJOSHADER_freePreshader")
#pragma comment(linker, "/export:MOJOSHADER_parse=MojoShader_Original.MOJOSHADER_parse")
#pragma comment(linker, "/export:MOJOSHADER_parsePreshader=MojoShader_Original.MOJOSHADER_parsePreshader")
#pragma comment(linker, "/export:MOJOSHADER_freeParseData=MojoShader_Original.MOJOSHADER_freeParseData")
#pragma comment(linker, "/export:MOJOSHADER_glGetError=MojoShader_Original.MOJOSHADER_glGetError")
#pragma comment(linker, "/export:MOJOSHADER_glAvailableProfiles=MojoShader_Original.MOJOSHADER_glAvailableProfiles")
#pragma comment(linker, "/export:MOJOSHADER_glBestProfile=MojoShader_Original.MOJOSHADER_glBestProfile")
#pragma comment(linker, "/export:MOJOSHADER_glMakeContextCurrent=MojoShader_Original.MOJOSHADER_glMakeContextCurrent")
#pragma comment(linker, "/export:MOJOSHADER_glMaxUniforms=MojoShader_Original.MOJOSHADER_glMaxUniforms")
#pragma comment(linker, "/export:MOJOSHADER_glCompileShader=MojoShader_Original.MOJOSHADER_glCompileShader")
#pragma comment(linker, "/export:MOJOSHADER_glGetShaderParseData=MojoShader_Original.MOJOSHADER_glGetShaderParseData")
#pragma comment(linker, "/export:MOJOSHADER_glLinkProgram=MojoShader_Original.MOJOSHADER_glLinkProgram")
#pragma comment(linker, "/export:MOJOSHADER_glBindProgram=MojoShader_Original.MOJOSHADER_glBindProgram")
#pragma comment(linker, "/export:MOJOSHADER_glCreateContext=MojoShader_Original.MOJOSHADER_glCreateContext")
#pragma comment(linker, "/export:MOJOSHADER_glBindShaders=MojoShader_Original.MOJOSHADER_glBindShaders")
#pragma comment(linker, "/export:MOJOSHADER_glSetVertexShaderUniformF=MojoShader_Original.MOJOSHADER_glSetVertexShaderUniformF")
#pragma comment(linker, "/export:MOJOSHADER_glGetVertexShaderUniformF=MojoShader_Original.MOJOSHADER_glGetVertexShaderUniformF")
#pragma comment(linker, "/export:MOJOSHADER_glSetVertexShaderUniformI=MojoShader_Original.MOJOSHADER_glSetVertexShaderUniformI")
#pragma comment(linker, "/export:MOJOSHADER_glGetVertexShaderUniformI=MojoShader_Original.MOJOSHADER_glGetVertexShaderUniformI")
#pragma comment(linker, "/export:MOJOSHADER_glSetVertexShaderUniformB=MojoShader_Original.MOJOSHADER_glSetVertexShaderUniformB")
#pragma comment(linker, "/export:MOJOSHADER_glGetVertexShaderUniformB=MojoShader_Original.MOJOSHADER_glGetVertexShaderUniformB")
#pragma comment(linker, "/export:MOJOSHADER_glSetPixelShaderUniformF=MojoShader_Original.MOJOSHADER_glSetPixelShaderUniformF")
#pragma comment(linker, "/export:MOJOSHADER_glGetPixelShaderUniformF=MojoShader_Original.MOJOSHADER_glGetPixelShaderUniformF")
#pragma comment(linker, "/export:MOJOSHADER_glSetPixelShaderUniformI=MojoShader_Original.MOJOSHADER_glSetPixelShaderUniformI")
#pragma comment(linker, "/export:MOJOSHADER_glGetPixelShaderUniformI=MojoShader_Original.MOJOSHADER_glGetPixelShaderUniformI")
#pragma comment(linker, "/export:MOJOSHADER_glSetPixelShaderUniformB=MojoShader_Original.MOJOSHADER_glSetPixelShaderUniformB")
#pragma comment(linker, "/export:MOJOSHADER_glGetPixelShaderUniformB=MojoShader_Original.MOJOSHADER_glGetPixelShaderUniformB")
#pragma comment(linker, "/export:MOJOSHADER_glGetVertexAttribLocation=MojoShader_Original.MOJOSHADER_glGetVertexAttribLocation")
#pragma comment(linker, "/export:MOJOSHADER_glSetVertexAttribute=MojoShader_Original.MOJOSHADER_glSetVertexAttribute")
#pragma comment(linker, "/export:MOJOSHADER_glSetVertexAttribDivisor=MojoShader_Original.MOJOSHADER_glSetVertexAttribDivisor")
#pragma comment(linker, "/export:MOJOSHADER_glSetLegacyBumpMapEnv=MojoShader_Original.MOJOSHADER_glSetLegacyBumpMapEnv")
#pragma comment(linker, "/export:MOJOSHADER_glProgramReady=MojoShader_Original.MOJOSHADER_glProgramReady")
#pragma comment(linker, "/export:MOJOSHADER_glDeleteProgram=MojoShader_Original.MOJOSHADER_glDeleteProgram")
#pragma comment(linker, "/export:MOJOSHADER_glDeleteShader=MojoShader_Original.MOJOSHADER_glDeleteShader")
#pragma comment(linker, "/export:MOJOSHADER_glDestroyContext=MojoShader_Original.MOJOSHADER_glDestroyContext")
#pragma comment(linker, "/export:MOJOSHADER_glProgramViewportFlip=MojoShader_Original.MOJOSHADER_glProgramViewportFlip")
#pragma comment(linker, "/export:MOJOSHADER_glCompileEffect=MojoShader_Original.MOJOSHADER_glCompileEffect")
#pragma comment(linker, "/export:MOJOSHADER_glDeleteEffect=MojoShader_Original.MOJOSHADER_glDeleteEffect")
#pragma comment(linker, "/export:MOJOSHADER_glEffectBegin=MojoShader_Original.MOJOSHADER_glEffectBegin")
#pragma comment(linker, "/export:MOJOSHADER_glEffectCommitChanges=MojoShader_Original.MOJOSHADER_glEffectCommitChanges")
#pragma comment(linker, "/export:MOJOSHADER_glEffectBeginPass=MojoShader_Original.MOJOSHADER_glEffectBeginPass")
#pragma comment(linker, "/export:MOJOSHADER_glEffectEndPass=MojoShader_Original.MOJOSHADER_glEffectEndPass")
#pragma comment(linker, "/export:MOJOSHADER_glEffectEnd=MojoShader_Original.MOJOSHADER_glEffectEnd")
#pragma comment(linker, "/export:MOJOSHADER_parseEffect=MojoShader_Original.MOJOSHADER_parseEffect")
#pragma comment(linker, "/export:MOJOSHADER_freeEffect=MojoShader_Original.MOJOSHADER_freeEffect")
#pragma comment(linker, "/export:MOJOSHADER_cloneEffect=MojoShader_Original.MOJOSHADER_cloneEffect")
#pragma comment(linker, "/export:MOJOSHADER_effectSetRawValueHandle=MojoShader_Original.MOJOSHADER_effectSetRawValueHandle")
#pragma comment(linker, "/export:MOJOSHADER_effectSetRawValueName=MojoShader_Original.MOJOSHADER_effectSetRawValueName")
#pragma comment(linker, "/export:MOJOSHADER_effectGetCurrentTechnique=MojoShader_Original.MOJOSHADER_effectGetCurrentTechnique")
#pragma comment(linker, "/export:MOJOSHADER_effectSetTechnique=MojoShader_Original.MOJOSHADER_effectSetTechnique")
#pragma comment(linker, "/export:MOJOSHADER_effectFindNextValidTechnique=MojoShader_Original.MOJOSHADER_effectFindNextValidTechnique")