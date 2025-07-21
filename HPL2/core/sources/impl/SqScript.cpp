/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "impl/SqScript.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"
#include "math/Math.h"
#include <stdio.h>
#include "impl/scripthelper.h"
#include "resources/BinaryBuffer.h"
#include "resources/Resources.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC DATA
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	#define kEncryptKey 0x4516FFDD

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSqScript::cSqScript(const tString& asName,asIScriptEngine *apScriptEngine,
							CScriptBuilder *apScriptBuilder,cScriptOutput *apScriptOutput, int alHandle)
		: iScript(asName, _W(""))
	{
		mpScriptEngine = apScriptEngine;
		mpScriptBuilder = apScriptBuilder;
		mpScriptOutput = apScriptOutput;
		mlHandle = alHandle;

		mpContext = mpScriptEngine->CreateContext();
		mpContext->SetExceptionCallback(asMETHOD(cSqScript, HandleException), this, asCALL_THISCALL);

		//Create a unique module name
		msModuleName = "Module_"+cString::ToString(cMath::RandRectl(0,1000000))+
						"_"+cString::ToString(mlHandle);

	}

	cSqScript::~cSqScript()
	{
		mpScriptEngine->DiscardModule(msModuleName.c_str());
		mpContext->Release();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cSqScript::CreateFromFile(const tWString& asFileName, tString *apCompileMessages)
	{
		mbPreparedFunction = false;
		SetFullPath(asFileName);

		tWString sExt = cString::ToLowerCaseW(cString::GetFileExtW(asFileName));

		/////////////////////////////////////////
		// Load file
		int lLength = 0;
		char *pCharBuffer = NULL;
		
		/////////////////////////////////////
		// Normal load
		if(sExt == _W("hps"))
		{
			pCharBuffer = LoadCharBuffer(asFileName,lLength);
			if(pCharBuffer==NULL){
				Error("Couldn't load script '%s'!\n",asFileName.c_str());
				return false;
			}
		}
		/////////////////////////////////////
		// Compressed load
		else if(cResources::GetCreateAndLoadCompressedMaps())
		{
			cBinaryBuffer compBuffer;
			if(compBuffer.Load(asFileName)==false)
			{
				//Log("Could not load compressed map!\n");
				return false;
			}

			int lKey = kEncryptKey;
			compBuffer.XorTransform((char*)&lKey, sizeof(lKey));

			cBinaryBuffer textBuff;
			if(textBuff.DecompressAndAddFromBuffer(&compBuffer, false)==false)
			{
				//Log("Could not decompress map!\n");
				return false;
			}

			textBuff.SetPos(0);
			lLength = (int)textBuff.GetSize();
			pCharBuffer = hplNewArray(char,lLength);
			textBuff.GetCharArray(pCharBuffer, lLength);
		}

		/////////////////////////////////////////
		// Save compressed
		if(sExt == _W("hps") && cResources::GetCreateAndLoadCompressedMaps())
		{
			tWString sCompFile = cString::SetFileExtW(asFileName,_W("chps"));

			//Only recreate if file does not exist or if out of date.
			if(	cPlatform::FileExists(sCompFile)==false || 
				cPlatform::FileModifiedDate(sCompFile) < cPlatform::FileModifiedDate(asFileName))
			{
				cBinaryBuffer textBuff;
				textBuff.AddCharArray(pCharBuffer, lLength);

				cBinaryBuffer compBuff;
				compBuff.CompressAndAdd(textBuff.GetDataPointer(), textBuff.GetSize());

				int lKey = kEncryptKey;
				compBuff.XorTransform((char*)&lKey, sizeof(lKey));

				compBuff.Save(sCompFile);
			}
		}
		
		/////////////////////////////////////////
		// Create module
		mpScriptBuilder->SetIncludeCallback(cLowLevelSystemSDL::IncludeScript, this);
		int r = mpScriptBuilder->StartNewModule(mpScriptEngine, msModuleName.c_str());
		r = mpScriptBuilder->AddSectionFromMemory(pCharBuffer, "main");
		if(r<0)
		{
			Error("Couldn't add script '%s'!\n", asFileName.c_str());
			hplDeleteArray(pCharBuffer);
			mpScriptBuilder->SetIncludeCallback(NULL, NULL);
			return false;
		}

		r = mpScriptBuilder->BuildModule();
		mpModule = mpScriptEngine->GetModule(msModuleName.c_str());
		if (apCompileMessages) *apCompileMessages = mpScriptOutput->GetMessage();

		if(r<0)
		{
			Error("Couldn't build script '%s'!\n",cString::To8Char(asFileName).c_str());
			Log("------- SCRIPT OUTPUT BEGIN --------------------------\n");
			mpScriptOutput->Display();
			mpScriptOutput->Clear();
			Log("------- SCRIPT OUTPUT END ----------------------------\n");
			
			hplDeleteArray(pCharBuffer);
			mpScriptBuilder->SetIncludeCallback(NULL, NULL);
			return false;
		}
		mpScriptOutput->Clear();

		hplDeleteArray(pCharBuffer);
		mpScriptBuilder->SetIncludeCallback(NULL, NULL);
		return true;
	}

	//-----------------------------------------------------------------------

	int cSqScript::GetFuncHandle(const tString& asFunc)
	{
		return mpModule->GetFunctionIdByName(asFunc.c_str());
	}

	//-----------------------------------------------------------------------

	void cSqScript::AddArg(const tString& asArg)
	{

	}

	//-----------------------------------------------------------------------

	bool cSqScript::Run(const tString& asFuncLine)
	{
		ExecuteString(mpScriptEngine, asFuncLine.c_str(), mpModule);

		return true;
	}

	//-----------------------------------------------------------------------

	bool cSqScript::Run(int alHandle)
	{
		mpContext->Prepare(alHandle);

		/* Set all the args here */

		mpContext->Execute();

		return true;
	}

	//-----------------------------------------------------------------------

	bool cSqScript::PrepareRunFunc(const tString& asFuncName)
	{
		if (mbPreparedFunction) return false;

		int alHandle = mpModule->GetFunctionIdByName(asFuncName.c_str());
		if (alHandle == asNO_FUNCTION) {
			return false;
		}

		int alResult = mpContext->Prepare(alHandle);
		if (alResult == asCONTEXT_ACTIVE)
		{
			Error("Context active???");
			return false;
		}
		mbPreparedFunction = true;

		return true;
	}

	//-----------------------------------------------------------------------

	void cSqScript::SetPreparedFuncArg(int alArgID, void* aObj)
	{
		if(!mbPreparedFunction) return;

		mpContext->SetArgObject(alArgID, aObj);
	}

	void cSqScript::SetPreparedFuncArg(int alArgID, tString asString)
	{
		if (!mbPreparedFunction) return;

		mpContext->SetArgObject(alArgID, &asString);
	}

	void cSqScript::SetPreparedFuncArg(int alArgID, float afValue)
	{
		if (!mbPreparedFunction) return;

		mpContext->SetArgFloat(alArgID, afValue);
	}

	void cSqScript::SetPreparedFuncArg(int alArgID, bool abX)
	{
		if (!mbPreparedFunction) return;

		mpContext->SetArgByte(alArgID, abX);
	}

	void cSqScript::SetPreparedFuncArg(int alArgID, int alValue)
	{
		if (!mbPreparedFunction) return;

		mpContext->SetArgDWord(alArgID, alValue);
	}

	void cSqScript::SetPreparedFuncArg(int alArgID, double alValue)
	{
		if (!mbPreparedFunction) return;

		mpContext->SetArgDouble(alArgID, alValue);
	}

	//-----------------------------------------------------------------------

	bool cSqScript::RunPreparedFunc()
	{
		mpContext->Execute();
		mbPreparedFunction = false;

		return true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	char* cSqScript::LoadCharBuffer(const tWString& asFileName, int& alLength)
	{
		FILE *pFile = cPlatform::OpenFile(asFileName, _W("rb"));
		if(pFile==NULL){
			return NULL;
		}

		fseek(pFile,0,SEEK_END);
		size_t lLength = ftell(pFile);
		rewind(pFile);

		//There's some weird stuff going on, looks like the string finalizer character wasn't created, and the new script loader no likey reading garbage data
		alLength = (int)++lLength;

		char *pBuffer = hplNewArray(char,lLength);
		fread(pBuffer, lLength-1, 1, pFile);
		pBuffer[lLength-1] = *"\0";

		fclose(pFile);

		return pBuffer;
	}

	//-----------------------------------------------------------------------

	void cSqScript::HandleException(asIScriptContext* ctx)
	{
		asIScriptEngine* engine = ctx->GetEngine();

		Error("Script Exception: %s\n", ctx->GetExceptionString());
		int function = ctx->GetExceptionFunction();
		Log("Function: %s\n", mpModule->GetFunctionDescriptorById(function));
		Log("Module: %s\n", mpModule->GetName());
		Log("Line: %s\n", ctx->GetExceptionLineNumber());
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// STATIC PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

}
