#include "Direct3D.h"
#include <fstream>


//-----------------------------------------------------------------------------
// Name: RenderShader()
// Desc: Constructor
//-----------------------------------------------------------------------------
RenderShader::RenderShader()
{
	effect = NULL;
	tech = NULL;
	device = NULL;
	devicecontext = NULL;
	vtxdecl = NULL;
	vtxdeclcount = 0;
	layouts = NULL;
	passcount = 0;
}

//-----------------------------------------------------------------------------
// Name: LoadFX()
// Desc: Load shader from file
//-----------------------------------------------------------------------------
Result RenderShader::LoadFX(const FilePath filename, LPD3DDEVICE device, LPD3DDEVICECONTEXT devicecontext)
{
	ID3D10Blob* compiledshader = 0;
	LPD3D10BLOB errormessages = NULL;
	Result rlt;

	this->device = device;
	if(!device)
		return ERR("Parameter device is NULL");
	this->devicecontext = devicecontext;
	if(!devicecontext)
		return ERR("Parameter devicecontext is NULL");
	if(filename.IsEmpty())
		return ERR("Parameter filename is empty");

	// Get modification time of effect file
	ULARGE_INTEGER writetime;
	if(!filename.GetLastWriteTime(&writetime))
		return ERR(String("Error trying to open file ") << filename);

	// >>> Create effect

	// If a precompiled effect file is found, use the precompiled effect
	bool compileshader = true;
#ifdef _DEBUG
	/*String filenameext;
	filename.GetExtension(&filenameext);
	FilePath compiledfilename;
	filename.Substring(0, filename.length() - filenameext.length() - 1, &compiledfilename);
	compiledfilename <<= "_d.";
	compiledfilename <<= filenameext;
	compiledfilename <<= "c";*/
	FilePath compiledfilename = FilePath(String(filename) << String("c_d"));
#else
	FilePath compiledfilename = FilePath(String(filename) << String("c"));
#endif
	if(compiledfilename.Exists())
	{
		ULARGE_INTEGER compiledwritetime;
		std::ifstream compiledfile(compiledfilename, std::ios::in | std::ios::binary);
		compiledfile.read((char*)&compiledwritetime, sizeof(ULARGE_INTEGER));
		if(compiledwritetime.QuadPart == writetime.QuadPart)
		{
			// bufsize = end_pos - current_pos
			std::streamsize bufpos = compiledfile.tellg();
			compiledfile.seekg(0, std::ios::end);
			std::streamsize bufsize = compiledfile.tellg() - bufpos;
			compiledfile.seekg(bufpos, std::ios::beg);

			char* compiledshaderbytes = new char[(size_t)bufsize];
			compiledfile.read(compiledshaderbytes, bufsize);
			compiledfile.close();
			CHKRESULT(D3DX11CreateEffectFromMemory(compiledshaderbytes, (size_t)bufsize, 0, device, &effect));
			delete[] compiledshaderbytes;
			compileshader = false;
		}
		else
			compiledfile.close();
	}

	// If a precompiled effect file wasn't found, compile the effect
	if(compileshader)
	{
		UINT shaderflags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
		shaderflags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#else
		shaderflags |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif
		D3D10_SHADER_MACRO defines[] = {{"PI", "3.141592654f"}, {0, 0}};

		HRESULT hr = D3DX11CompileFromFile(filename, defines, NULL, NULL, "fx_5_0", shaderflags, 0, NULL, &compiledshader, &errormessages, NULL);
		if(FAILED(hr))
		{
			if(errormessages)
				return ERR((char*)errormessages->GetBufferPointer());
			else
				return Error(hr);
		}

		CHKRESULT(D3DX11CreateEffectFromMemory(compiledshader->GetBufferPointer(), compiledshader->GetBufferSize(), 0, device, &effect));

		// Store compiled effect in a compiled effect file
		std::ofstream compiledfile(compiledfilename, std::ios::out | std::ios::binary);
		compiledfile.write((char*)&writetime, sizeof(ULARGE_INTEGER));
		compiledfile.write((char*)compiledshader->GetBufferPointer(), compiledshader->GetBufferSize());
		compiledfile.close();
	}


	// >>> Obtain default parameters

	defparams.world = effect->GetVariableBySemantic("World")->AsMatrix();							if(!defparams.world->IsValid())					defparams.world = NULL;
	defparams.worldarray = effect->GetVariableBySemantic("WorldArray")->AsMatrix();					if(!defparams.worldarray->IsValid())			defparams.worldarray = NULL;
	defparams.view = effect->GetVariableBySemantic("View")->AsMatrix();								if(!defparams.view->IsValid())					defparams.view = NULL;
	defparams.proj = effect->GetVariableBySemantic("Proj")->AsMatrix();								if(!defparams.proj->IsValid())					defparams.proj = NULL;
	defparams.viewproj = effect->GetVariableBySemantic("ViewProj")->AsMatrix();						if(!defparams.viewproj->IsValid())				defparams.viewproj = NULL;
	defparams.worldviewproj = effect->GetVariableBySemantic("WorldViewProj")->AsMatrix();			if(!defparams.worldviewproj->IsValid())			defparams.worldviewproj = NULL;
	defparams.worldviewprojarray = effect->GetVariableBySemantic("WorldViewProjArray")->AsMatrix();	if(!defparams.worldviewprojarray->IsValid())	defparams.worldviewprojarray = NULL;
	defparams.worldinvtrans = effect->GetVariableBySemantic("WorldInvTrans")->AsMatrix();			if(!defparams.worldinvtrans->IsValid())			defparams.worldinvtrans = NULL;
	defparams.worldinvtransarray = effect->GetVariableBySemantic("WorldInvTransArray")->AsMatrix();	if(!defparams.worldinvtransarray->IsValid())	defparams.worldinvtransarray = NULL;

	defparams.viewpos = effect->GetVariableBySemantic("ViewPos")->AsVector();						if(!defparams.viewpos->IsValid())				defparams.viewpos = NULL;

	defparams.ambient = effect->GetVariableBySemantic("Ambient")->AsVector();						if(!defparams.ambient->IsValid())				defparams.ambient = NULL;
	defparams.diffuse = effect->GetVariableBySemantic("Diffuse")->AsVector();						if(!defparams.diffuse->IsValid())				defparams.diffuse = NULL;
	defparams.specular = effect->GetVariableBySemantic("Specular")->AsVector();						if(!defparams.specular->IsValid())				defparams.specular = NULL;

	defparams.numlights = effect->GetVariableBySemantic("NumLights")->AsScalar();					if(!defparams.numlights->IsValid())				defparams.numlights = NULL;
	defparams.lightparams = effect->GetVariableBySemantic("LightParams");							if(!defparams.lightparams->IsValid())			defparams.lightparams = NULL;

	defparams.power = effect->GetVariableBySemantic("Power")->AsScalar();							if(!defparams.power->IsValid())					defparams.power = NULL;
	defparams.time = effect->GetVariableBySemantic("Time")->AsScalar();								if(!defparams.time->IsValid())					defparams.time = NULL;
	defparams.hastex = effect->GetVariableBySemantic("HasTexture")->AsScalar();						if(!defparams.hastex->IsValid())				defparams.hastex = NULL;

	defparams.tex = effect->GetVariableBySemantic("Texture")->AsShaderResource();					if(!defparams.tex->IsValid())					defparams.tex = NULL;

	// Default to first technique
	CHKRESULT(SetTechnique(0)); //EDIT11

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: PrepareRendering()
// Desc: Prepare shader and return the number of passes for the current technique
//-----------------------------------------------------------------------------
void RenderShader::PrepareRendering(Camera* cam, const Matrix* worldmatrix, const Matrix* worldmatrixarray, UINT numblendparts, UINT* passcount)
{
	// Set parameters for the whole mesh
	if(defparams.world)
		defparams.world->SetMatrix((float*)worldmatrix);
	if(defparams.worldinvtrans)
	{
		Matrix mWorldInvTrans;
		MatrixInverseTranspose(&mWorldInvTrans, worldmatrix);
		defparams.worldinvtrans->SetMatrix((float*)mWorldInvTrans);
	}
	if(defparams.viewpos && cam)
		defparams.viewpos->SetFloatVector((float*)Vector4(cam->pos.x, cam->pos.y, cam->pos.z, 1.0f));
	if(defparams.time)
		defparams.time->SetFloat(eng->time.t);
	if(numblendparts)
	{
		if(defparams.worldarray)
			defparams.worldarray->SetMatrixArray((float*)worldmatrixarray, 0, numblendparts);
		if(defparams.worldviewprojarray && cam)
		{
			Matrix* mWorldViewProjArray = new Matrix[numblendparts];
			if(cam)
				for(UINT i = 0; i < numblendparts; i++)
					mWorldViewProjArray[i] = worldmatrixarray[i] * cam->viewprojmatrix;
			else
				for(UINT i = 0; i < numblendparts; i++)
					mWorldViewProjArray[i] = worldmatrixarray[i];
			defparams.worldviewprojarray->SetMatrixArray((float*)mWorldViewProjArray, 0, numblendparts);
			delete[] mWorldViewProjArray;
		}
		if(defparams.worldinvtransarray)
		{
			Matrix* mWorldInvTransArray = new Matrix[numblendparts];
			for(UINT i = 0; i < numblendparts; i++)
				MatrixInverseTranspose(&mWorldInvTransArray[i], &worldmatrixarray[i]);
			defparams.worldinvtransarray->SetMatrixArray((float*)mWorldInvTransArray, 0, numblendparts);
			delete[] mWorldInvTransArray;
		}
	}
	if(cam)
	{
		if(defparams.view)
			defparams.view->SetMatrix((float*)cam->viewmatrix);
		if(defparams.proj)
			defparams.proj->SetMatrix((float*)cam->projmatrix);
		if(defparams.viewproj)
			defparams.viewproj->SetMatrix((float*)cam->viewprojmatrix);
		if(defparams.worldviewproj)
			defparams.worldviewproj->SetMatrix((float*)(*worldmatrix * cam->viewprojmatrix));
	}
	else
	{
		Matrix matIdnetity;
		MatrixIdentity(&matIdnetity);
		if(defparams.view)
			defparams.view->SetMatrix((float*)matIdnetity);
		if(defparams.proj)
			defparams.proj->SetMatrix((float*)matIdnetity);
		if(defparams.viewproj)
			defparams.viewproj->SetMatrix((float*)matIdnetity);
		if(defparams.worldviewproj)
			defparams.worldviewproj->SetMatrix((float*)worldmatrix);
	}

	PrepareRendering(passcount);
}
void RenderShader::PrepareRendering(UINT* passcount)
{
	// Set light parameters
	Direct3D* _d3d = (Direct3D*)&*d3d;
	if(defparams.numlights && defparams.lightparams && _d3d->lightparams.size())
	{
		defparams.lightparams->SetRawValue(&_d3d->lightparams[0], 0, min(16, _d3d->lightparams.size()) * sizeof(Light::LightParams));
		defparams.numlights->SetInt(_d3d->lightparams.size());
	}
	else if(defparams.numlights)
		defparams.numlights->SetInt(0);

	// Return passcount
	*passcount = this->passcount;
}

//-----------------------------------------------------------------------------
// Name: PreparePass()
// Desc: Prepare current shader pass
//-----------------------------------------------------------------------------
void RenderShader::PreparePass(UINT passindex, UINT techidx)
{
	LPD3D11EFFECTPASS pass = (techidx == 0xFFFFFFFF ? tech : effect->GetTechniqueByIndex(techidx))->GetPassByIndex(passindex);
	if(!pass->IsValid())
		return;

	// EDIT

	devicecontext->IASetInputLayout(layouts[passindex]);
	pass->Apply(0, devicecontext);
}

//-----------------------------------------------------------------------------
// Name: PrepareSubset()
// Desc: Prepare current mesh subset
//-----------------------------------------------------------------------------
void RenderShader::PrepareSubset(const Texture* tex, const Color* ambient, const Color* diffuse, const Color* specular, float power, UINT passindex)
{
	devicecontext->IASetInputLayout(layouts[passindex]);

	// Set parameters for the current mesh subset
	if(defparams.hastex)
		defparams.hastex->SetBool(tex != NULL);
	if(defparams.tex && tex)
		defparams.tex->SetResource(tex->srview);
	if(defparams.ambient)
		defparams.ambient->SetFloatVector((float*)ambient);
	if(defparams.diffuse)
		defparams.diffuse->SetFloatVector((float*)diffuse);
	if(defparams.specular)
		defparams.specular->SetFloatVector((float*)specular);
	if(defparams.power)
		defparams.power->SetFloat(power);

	tech->GetPassByIndex(passindex)->Apply(0, devicecontext);
}

//-----------------------------------------------------------------------------
// Name: ApplyStateChanges()
// Desc: Send state changes for the given pass or all passes (if pass == (UINT)-1) to the GPU
//-----------------------------------------------------------------------------
void RenderShader::ApplyStateChanges(UINT passindex)
{
	if(passindex == (UINT)-1)
		for(UINT p = 0; p < passcount; p++)
			tech->GetPassByIndex(p)->Apply(0, devicecontext);
	else
		tech->GetPassByIndex(passindex)->Apply(0, devicecontext);
}

//-----------------------------------------------------------------------------
// Name: ReCreateInputLayouts()
// Desc: Recreate input layouts for the current technique
//-----------------------------------------------------------------------------
Result RenderShader::ReCreateInputLayouts()
{
	D3DX11_PASS_DESC passdesc;
	Result rlt;

	for(UINT passidx = 0; passidx < passcount; passidx++)
	{
		tech->GetPassByIndex(passidx)->GetDesc(&passdesc);
		CHKRESULT(device->CreateInputLayout(vtxdecl, vtxdeclcount, passdesc.pIAInputSignature, passdesc.IAInputSignatureSize, &layouts[passidx]));
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetTechnique()
// Desc: Set rendering technique
//-----------------------------------------------------------------------------
Result RenderShader::SetTechnique(UINT techidx)
{
	// Set technique
	LPD3D11EFFECTTECHNIQUE newtech = effect->GetTechniqueByIndex(techidx);
	if(newtech->IsValid())
		tech = newtech;
	else
		return ERR("Technique index out of bounds");

	// Get passcount
	D3DX11_TECHNIQUE_DESC techdesc;
	tech->GetDesc(&techdesc);
	passcount = techdesc.Passes;

	CHKALLOC(layouts = (ID3D11InputLayout**)new LPVOID[passcount]);
	ZeroMemory(layouts, passcount * sizeof(LPVOID));

	if(vtxdecl)
		return ReCreateInputLayouts();
	else
		return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetVtxDecl()
// Desc: Called from Object::SetShader() to sync its vtxdecl with the shader
//-----------------------------------------------------------------------------
Result RenderShader::SetVtxDecl(D3D11_INPUT_ELEMENT_DESC* vtxdecl, UINT vtxdeclcount)
{
	if(this->vtxdecl == vtxdecl)
		return R_OK; // Nothing to do

	this->vtxdeclcount = vtxdeclcount;
	this->vtxdecl = vtxdecl;

	return ReCreateInputLayouts();
}

IEffectScalarVariable* RenderShader::GetScalarVariable(const String& semantic)
{
	if(!effect)
		return NULL;
	LPD3D11EFFECTVARIABLE var = effect->GetVariableBySemantic(semantic);
	return var->IsValid() ? new EffectScalarVariable(var->AsScalar()) : NULL;
}
IEffectVectorVariable* RenderShader::GetVectorVariable(const String& semantic)
{
	if(!effect)
		return NULL;
	LPD3D11EFFECTVARIABLE var = effect->GetVariableBySemantic(semantic);
	return var->IsValid() ? new EffectVectorVariable(var->AsVector()) : NULL;
}
IEffectMatrixVariable* RenderShader::GetMatrixVariable(const String& semantic)
{
	if(!effect)
		return NULL;
	LPD3D11EFFECTVARIABLE var = effect->GetVariableBySemantic(semantic);
	return var->IsValid() ? new EffectMatrixVariable(var->AsMatrix()) : NULL;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void RenderShader::Release()
{
	//REMOVE(tech);
	RELEASE(effect);

	vtxdecl = NULL;
	vtxdeclcount = 0;

	for(UINT passidx = 0; passidx < passcount; passidx++)
		RELEASE(layouts[passidx]);
	REMOVE_ARRAY(layouts);

	delete this;
}