#include "OpenGL.h"
#include <fstream>

#ifdef _WIN32
typedef ULARGE_INTEGER _TIME;
#define _TIME_EQUALS(t0, t1) ((t0).QuadPart == (t1).QuadPart)
#else
typedef time_t _TIME;
#define _TIME_EQUALS(t0, t1) ((t0) == (t1))
#endif


//-----------------------------------------------------------------------------
// Name: RenderShader()
// Desc: Constructor
//-----------------------------------------------------------------------------
RenderShader::RenderShader()
{
	vertexshader = fragmentshader = shaderprogram = -1;
	/*effect = NULL;
	tech = NULL;
	vtxdecl = NULL;
	vtxdeclcount = 0;
	layouts = NULL;
	passcount = 0;*/
}

//-----------------------------------------------------------------------------
// Name: LoadFX()
// Desc: Load shader from file
//-----------------------------------------------------------------------------
Result RenderShader::LoadFX(const FilePath filename)
{
	void* compiledshader = NULL;
	/*LPD3D10BLOB errormessages = NULL;*/
	Result rlt;

	if(filename.IsEmpty())
		return ERR("Parameter filename is empty");
	if(!filename.Exists())
		return ERR(String("File not found: ") << filename);

	// Get modification time of effect file
	_TIME writetime;
	if(!filename.GetLastWriteTime(&writetime))
		return ERR(String("Error trying to open file ") << filename);

	// >>> Create effect

	// If a precompiled effect file is found, use the precompiled effect
	bool compileshader = true;
#ifdef _DEBUG
	FilePath compiledfilename = FilePath(String(filename) << String("c_d"));
#else
	FilePath compiledfilename = FilePath(String(filename) << String("c"));
#endif
	if(compiledfilename.Exists())
	{
		_TIME compiledwritetime;
		std::ifstream compiledfile(compiledfilename, std::ios::in | std::ios::binary);
		compiledfile.read((char*)&compiledwritetime, sizeof(_TIME));
		if(_TIME_EQUALS(compiledwritetime, writetime))
		{
			// bufsize = end_pos - current_pos
			std::streamsize bufpos = compiledfile.tellg();
			compiledfile.seekg(0, std::ios::end);
			std::streamsize bufsize = (std::streampos)compiledfile.tellg() - (std::streampos)bufpos;
			compiledfile.seekg(bufpos, std::ios::beg);
			//EDIT: Check that bufsize is at least sizeof(GLenum)

			GLenum compiledshaderformat;
			compiledfile.read((char*)&compiledshaderformat, sizeof(GLenum));
			char* compiledshaderbytes = new char[bufsize - sizeof(GLenum)];
			compiledfile.read(compiledshaderbytes, bufsize - sizeof(GLenum));
			compiledfile.close();
			shaderprogram = GLX.glCreateProgram();
			GLX.glProgramBinary(shaderprogram, compiledshaderformat, compiledshaderbytes, bufsize - sizeof(GLenum));
			delete[] compiledshaderbytes;
			int isLinked;
			GLX.glGetProgramiv(shaderprogram, GL_LINK_STATUS, (int*)&isLinked);
			if(isLinked)
				compileshader = false;
		}
		else
			compiledfile.close();
	}

	// If a precompiled effect file wasn't found, compile the effect
	if(compileshader)
	{
		std::ifstream shaderfile(filename, std::ios::in | std::ios_base::binary);
		shaderfile.seekg(0, std::ios::end);
		std::streamsize bufsize = shaderfile.tellg();
		shaderfile.seekg(0, std::ios::beg);
		char* shaderstr = new char[bufsize + 1];
		shaderfile.read(shaderstr, bufsize);
		shaderstr[bufsize] = '\0';
		shaderfile.close();

		// Get #vs and #fs sections from shaderstr
		// output = {vsstart, fsstart}
		const char *vsstart = NULL, *fsstart = NULL;
		char buf[4], *c = shaderstr;
		int buflen = 0;
		while(*c != '\0')
		{
			for(int i = ARRAYSIZE(buf) - buflen; i < ARRAYSIZE(buf); ++i)
				buf[i - 1] = buf[i];
			buf[ARRAYSIZE(buf) - 1] = *c++;
			if(buflen < ARRAYSIZE(buf) - 1)
				++buflen;
			else if(memcmp(buf, "#vs\n", 4) == 0 || memcmp(buf, "#vs\r", 4) == 0)
			{
				if(vsstart)
					LOG("Warning: More than one #vs section defined. Using only first section");
				else
					vsstart = c;
				c[-(int)ARRAYSIZE(buf)] = '\0';
			}
			else if(memcmp(buf, "#fs\n", 4) == 0 || memcmp(buf, "#fs\r", 4) == 0)
			{
				if(fsstart)
					LOG("Warning: More than one #fs section defined. Using only first section");
				else
					fsstart = c;
				c[-(int)ARRAYSIZE(buf)] = '\0';
			}
		}
		int abc2 = 0;//EDIT

		if(!vsstart)
		{
			delete[] shaderstr;
			return ERR(String("No vertex shader section (#vs) defined in shader file ") << filename);
		}
		if(!fsstart)
		{
			delete[] shaderstr;
			return ERR(String("No fragment shader section (#fs) defined in shader file ") << filename);
		}

		int isCompiled_VS, isCompiled_FS, isLinked;
		vertexshader = GLX.glCreateShader(GL_VERTEX_SHADER);
		GLX.glShaderSource(vertexshader, 1, (const char**)&vsstart, NULL);
		GLX.glCompileShader(vertexshader);
		GLX.glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &isCompiled_VS);
		if(!isCompiled_VS)
		{
			int maxLength;
			GLX.glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxLength);
			char* vertexInfoLog = new char[maxLength];
			GLX.glGetShaderInfoLog(vertexshader, maxLength, &maxLength, vertexInfoLog);

			//EDIT: Manually offset error line numbers in vertexInfoLog

			rlt = ERR(String("Error compiling vertex shader:\n") << String(vertexInfoLog));
			delete[] vertexInfoLog;
			delete[] shaderstr;
			return rlt;
		}

		fragmentshader = GLX.glCreateShader(GL_FRAGMENT_SHADER);
		GLX.glShaderSource(fragmentshader, 1, (const char**)&fsstart, NULL);
		GLX.glCompileShader(fragmentshader);
		GLX.glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &isCompiled_FS);
		if(!isCompiled_FS)
		{
			int maxLength;
			GLX.glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);
			char* fragmentInfoLog = new char[maxLength];
			GLX.glGetShaderInfoLog(fragmentshader, maxLength, &maxLength, fragmentInfoLog);

			//EDIT: Manually offset error line numbers in fragmentInfoLog

			rlt = ERR(String("Error compiling fragment shader:\n") << String(fragmentInfoLog));
			delete[] fragmentInfoLog;
			delete[] shaderstr;
			return rlt;
		}

		delete[] shaderstr;

		shaderprogram = GLX.glCreateProgram();
		GLX.glAttachShader(shaderprogram, vertexshader);
		GLX.glAttachShader(shaderprogram, fragmentshader);
		GLX.glProgramParameteri(shaderprogram, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
		GLX.glLinkProgram(shaderprogram);
		GLX.glGetProgramiv(shaderprogram, GL_LINK_STATUS, (int*)&isLinked);
		if(!isLinked)
		{
			int maxLength;
			GLX.glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);
			char* shaderProgramInfoLog = new char[maxLength];
			GLX.glGetProgramInfoLog(shaderprogram, maxLength, &maxLength, shaderProgramInfoLog);

			rlt = ERR(String("Error clinking shader program:\n") << String(shaderProgramInfoLog));
			delete[] shaderProgramInfoLog;
			return rlt;
		}

		// Get compiled effect binary
		GLint binarylength;
		GLenum binaryformat;
		char* binary;
		bool storecompiledeffect = false;
		GLX.glGetProgramiv(shaderprogram, GL_PROGRAM_BINARY_LENGTH, &binarylength);
		if(binarylength == 0)
		{
			// If GL_PROGRAM_BINARY_LENGTH does not succeed, allocate 1MB
			binary = new char[binarylength = 1024 * 1024];
			GLX.glGetProgramBinary(shaderprogram, binarylength, &binarylength, &binaryformat, binary);
			storecompiledeffect = (binarylength != 0 && binaryformat != 0);
		}
		else
		{
			binary = new char[binarylength];
			GLX.glGetProgramBinary(shaderprogram, binarylength, NULL, &binaryformat, binary);
			storecompiledeffect = true;
		}

		if(storecompiledeffect)
		{
			// Store compiled effect in a compiled effect file
			std::ofstream compiledfile(compiledfilename, std::ios::out | std::ios::binary);
			compiledfile.write((char*)&writetime, sizeof(_TIME));
			compiledfile.write((char*)&binaryformat, sizeof(GLenum));
			compiledfile.write(binary, binarylength);
			compiledfile.close();
			delete[] binary;
		}
	}

	// >>> Obtain default parameters

	defparams.world = GLX.glGetUniformLocation(shaderprogram, "World");
	defparams.worldarray = GLX.glGetUniformLocation(shaderprogram, "WorldArray");
	defparams.view = GLX.glGetUniformLocation(shaderprogram, "View");
	defparams.proj = GLX.glGetUniformLocation(shaderprogram, "Proj");
	defparams.viewproj = GLX.glGetUniformLocation(shaderprogram, "ViewProj");
	defparams.worldviewproj = GLX.glGetUniformLocation(shaderprogram, "WorldViewProj");
	defparams.worldviewprojarray = GLX.glGetUniformLocation(shaderprogram, "WorldViewProjArray");
	defparams.worldinvtrans = GLX.glGetUniformLocation(shaderprogram, "WorldInvTrans");
	defparams.worldinvtransarray = GLX.glGetUniformLocation(shaderprogram, "WorldInvTransArray");

	defparams.viewpos = GLX.glGetUniformLocation(shaderprogram, "ViewPos");

	defparams.ambient = GLX.glGetUniformLocation(shaderprogram, "Ambient");
	defparams.diffuse = GLX.glGetUniformLocation(shaderprogram, "Diffuse");
	defparams.specular = GLX.glGetUniformLocation(shaderprogram, "Specular");

	defparams.numlights = GLX.glGetUniformLocation(shaderprogram, "NumLights");
	defparams.lightparams = GLX.glGetUniformLocation(shaderprogram, "LightParams");

	defparams.power = GLX.glGetUniformLocation(shaderprogram, "Power");
	defparams.time = GLX.glGetUniformLocation(shaderprogram, "Time");
	defparams.hastex = GLX.glGetUniformLocation(shaderprogram, "HasTexture");

	defparams.tex = GLX.glGetUniformLocation(shaderprogram, "Texture");

	// >>> Obtain default vertex attributes

	defvtxattrs.pos = GLX.glGetAttribLocation(shaderprogram, "vpos");
	defvtxattrs.nml = GLX.glGetAttribLocation(shaderprogram, "vnml");
	defvtxattrs.texcoord = GLX.glGetAttribLocation(shaderprogram, "vtexcoord");
	defvtxattrs.blendidx = GLX.glGetAttribLocation(shaderprogram, "vblendidx");

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: PrepareRendering()
// Desc: Prepare shader and return the number of passes for the current technique
//-----------------------------------------------------------------------------
void RenderShader::PrepareRendering(Camera* cam, const Matrix* worldmatrix, const Matrix* worldmatrixarray, UINT numblendparts, UINT* passcount)
{
	PrepareRendering(passcount);

	// Set parameters for the whole mesh
	if(defparams.world != -1)
		GLX.glUniformMatrix4fv(defparams.world, 1, false, (GLfloat*)worldmatrix);
	if(defparams.worldinvtrans != -1)
	{
		Matrix mWorldInvTrans;
		MatrixInverseTranspose(&mWorldInvTrans, worldmatrix);
		GLX.glUniformMatrix4fv(defparams.worldinvtrans, 1, false, (GLfloat*)mWorldInvTrans);
	}
	if(defparams.viewpos != -1 && cam)
		GLX.glUniform3f(defparams.viewpos, cam->pos.x, cam->pos.y, cam->pos.z);
	if(defparams.time != -1)
		GLX.glUniform1f(defparams.time, eng->time.t);
	if(numblendparts)
	{
		if(defparams.worldarray != -1)
			GLX.glUniformMatrix4fv(defparams.worldarray, numblendparts, false, (GLfloat*)worldmatrixarray);
		if(defparams.worldviewprojarray != -1 && cam)
		{
			Matrix* mWorldViewProjArray = new Matrix[numblendparts];
			if(cam)
				for(UINT i = 0; i < numblendparts; i++)
					mWorldViewProjArray[i] = worldmatrixarray[i] * cam->viewprojmatrix;
			else
				for(UINT i = 0; i < numblendparts; i++)
					mWorldViewProjArray[i] = worldmatrixarray[i];
			GLX.glUniformMatrix4fv(defparams.worldviewprojarray, numblendparts, false, (GLfloat*)mWorldViewProjArray);
			delete[] mWorldViewProjArray;
		}
		if(defparams.worldinvtransarray != -1)
		{
			Matrix* mWorldInvTransArray = new Matrix[numblendparts];
			for(UINT i = 0; i < numblendparts; i++)
				MatrixInverseTranspose(&mWorldInvTransArray[i], &worldmatrixarray[i]);
			GLX.glUniformMatrix4fv(defparams.worldinvtransarray, numblendparts, false, (GLfloat*)mWorldInvTransArray);
			delete[] mWorldInvTransArray;
		}
	}
	if(cam)
	{
		if(defparams.view != -1)
			GLX.glUniformMatrix4fv(defparams.view, 1, false, (GLfloat*)cam->viewmatrix);
		if(defparams.proj != -1)
			GLX.glUniformMatrix4fv(defparams.proj, 1, false, (GLfloat*)cam->projmatrix);
		if(defparams.viewproj != -1)
			GLX.glUniformMatrix4fv(defparams.viewproj, 1, false, (GLfloat*)cam->viewprojmatrix);
		if(defparams.worldviewproj != -1)
			GLX.glUniformMatrix4fv(defparams.worldviewproj, 1, false, (GLfloat*)(*worldmatrix * cam->viewprojmatrix));
	}
	else
	{
		Matrix matIdentity;
		MatrixIdentity(&matIdentity);
		if(defparams.view != -1)
			GLX.glUniformMatrix4fv(defparams.view, 1, false, (GLfloat*)matIdentity);
		if(defparams.proj != -1)
			GLX.glUniformMatrix4fv(defparams.proj, 1, false, (GLfloat*)matIdentity);
		if(defparams.viewproj)
			GLX.glUniformMatrix4fv(defparams.viewproj, 1, false, (GLfloat*)matIdentity);
		if(defparams.worldviewproj != -1)
			GLX.glUniformMatrix4fv(defparams.worldviewproj, 1, false, (GLfloat*)worldmatrix);
	}
}
void RenderShader::PrepareRendering(UINT* passcount)
{
	GLX.glUseProgram(shaderprogram);

	/*// Set light parameters
	OpenGL* _ogl = (OpenGL*)&*ogl;
	if(defparams.numlights && defparams.lightparams && _ogl->lightparams.size())
	{
		defparams.lightparams->SetRawValue(&_ogl->lightparams[0], 0, min(16, _ogl->lightparams.size()) * sizeof(Light::LightParams));
		defparams.numlights->SetInt(_ogl->lightparams.size());
	}
	else if(defparams.numlights)
		defparams.numlights->SetInt(0);*/

	// Return passcount
	*passcount = 1;
}

//-----------------------------------------------------------------------------
// Name: PreparePass()
// Desc: Prepare current shader pass
//-----------------------------------------------------------------------------
void RenderShader::PreparePass(UINT passindex, UINT techidx)
{
	/*LPD3D11EFFECTPASS pass = (techidx == 0xFFFFFFFF ? tech : effect->GetTechniqueByIndex(techidx))->GetPassByIndex(passindex);
	if(!pass->IsValid())
		return;

	// EDIT

	devicecontext->IASetInputLayout(layouts[passindex]);
	pass->Apply(0, devicecontext);*/
}

//-----------------------------------------------------------------------------
// Name: PrepareSubset()
// Desc: Prepare current mesh subset
//-----------------------------------------------------------------------------
void RenderShader::PrepareSubset(const Texture* tex, const Color* ambient, const Color* diffuse, const Color* specular, float power, UINT passindex)
{
	/*devicecontext->IASetInputLayout(layouts[passindex]);*/

	// Set parameters for the current mesh subset
	if(defparams.hastex != -1)
		GLX.glUniform1i(defparams.hastex, (int)(tex != NULL));
	if(defparams.tex != -1 && tex)
	{
		glEnable(GL_TEXTURE_2D);
		GLX.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex->tex);
		GLX.glUniform1i(defparams.tex, 0);
	}
	else
		glDisable(GL_TEXTURE_2D);
	if(defparams.ambient != -1)
		GLX.glUniform3f(defparams.ambient, ambient->r, ambient->g, ambient->b);
	if(defparams.diffuse != -1)
		GLX.glUniform3f(defparams.diffuse, diffuse->r, diffuse->g, diffuse->b);
	if(defparams.specular != -1)
		GLX.glUniform4f(defparams.specular, specular->r, specular->g, specular->b, power);

/*int foo = GLX.glGetAttribLocation(shaderprogram, "vpos");
GLX.glEnableVertexAttribArray(foo);
//GLX.glEnableVertexAttribArray(GLX.glGetAttribLocation(shaderprogram, "vnml"));
//GLX.glEnableVertexAttribArray(GLX.glGetAttribLocation(shaderprogram, "vtexcoord"));
GLX.glVertexAttribPointer(foo, 3, GL_FLOAT, GL_FALSE, sizeof(IObject::Vertex), (void*)offsetof(IObject::Vertex, pos));
//GLX.glVertexAttribPointer(GLX.glGetAttribLocation(shaderprogram, "vnml"), 3, GL_FLOAT, GL_FALSE, sizeof(IObject::Vertex), (void*)offsetof(IObject::Vertex, nml));
//GLX.glVertexAttribPointer(GLX.glGetAttribLocation(shaderprogram, "vtexcoord"), 2, GL_FLOAT, GL_FALSE, sizeof(IObject::Vertex), (void*)offsetof(IObject::Vertex, texcoord));*/
}

//-----------------------------------------------------------------------------
// Name: ApplyStateChanges()
// Desc: Send state changes for the given pass or all passes (if pass == (UINT)-1) to the GPU
//-----------------------------------------------------------------------------
void RenderShader::ApplyStateChanges(UINT passindex)
{
	/*if(passindex == (UINT)-1)
		for(UINT p = 0; p < passcount; p++)
			tech->GetPassByIndex(p)->Apply(0, devicecontext);
	else
		tech->GetPassByIndex(passindex)->Apply(0, devicecontext);*/
}

//-----------------------------------------------------------------------------
// Name: ReCreateInputLayouts()
// Desc: Recreate input layouts for the current technique
//-----------------------------------------------------------------------------
Result RenderShader::ReCreateInputLayouts()
{
	/*D3DX11_PASS_DESC passdesc;
	Result rlt;

	for(UINT passidx = 0; passidx < passcount; passidx++)
	{
		tech->GetPassByIndex(passidx)->GetDesc(&passdesc);
		CHKRESULT(device->CreateInputLayout(vtxdecl, vtxdeclcount, passdesc.pIAInputSignature, passdesc.IAInputSignatureSize, &layouts[passidx]));
	}*/

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetVtxDecl()
// Desc: Called from Object::SetShader() to sync its vtxdecl with the shader
//-----------------------------------------------------------------------------
Result RenderShader::SetVtxDecl(/*D3D11_INPUT_ELEMENT_DESC* vtxdecl, UINT vtxdeclcount*/)
{
	/*if(this->vtxdecl == vtxdecl)
		return R_OK; // Nothing to do

	this->vtxdeclcount = vtxdeclcount;
	this->vtxdecl = vtxdecl;

	return ReCreateInputLayouts();*/

if(defvtxattrs.pos != -1)
{
	GLX.glEnableVertexAttribArray(defvtxattrs.pos);
	GLX.glVertexAttribPointer(defvtxattrs.pos, 3, GL_FLOAT, GL_FALSE, sizeof(IObject::Vertex), (void*)offsetof(IObject::Vertex, pos));
}
if(defvtxattrs.nml != -1)
{
	GLX.glEnableVertexAttribArray(defvtxattrs.nml);
	GLX.glVertexAttribPointer(defvtxattrs.nml, 3, GL_FLOAT, GL_FALSE, sizeof(IObject::Vertex), (void*)offsetof(IObject::Vertex, nml));
}
if(defvtxattrs.texcoord != -1)
{
	GLX.glEnableVertexAttribArray(defvtxattrs.texcoord);
	GLX.glVertexAttribPointer(defvtxattrs.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(IObject::Vertex), (void*)offsetof(IObject::Vertex, texcoord));
}
if(defvtxattrs.blendidx != -1)
{
	GLX.glEnableVertexAttribArray(defvtxattrs.texcoord);
	GLX.glVertexAttribPointer(defvtxattrs.blendidx, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(IObject::Vertex), (void*)offsetof(IObject::Vertex, blendidx));
}

return R_OK;
}

/*IEffectScalarVariable* RenderShader::GetScalarVariable(const String& semantic)
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
}*/

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void RenderShader::Release()
{
	if(shaderprogram != -1)
	{
		GLX.glUseProgram(0); // Unbind shader
		if(vertexshader != -1)
		{
			GLX.glDetachShader(shaderprogram, vertexshader);
			GLX.glDeleteShader(vertexshader);
		}
		if(fragmentshader != -1)
		{
			GLX.glDetachShader(shaderprogram, fragmentshader);
			GLX.glDeleteShader(fragmentshader);
		}
		GLX.glDeleteProgram(shaderprogram);
	}
	/*RELEASE(effect);

	vtxdecl = NULL;
	vtxdeclcount = 0;

	for(UINT passidx = 0; passidx < passcount; passidx++)
		RELEASE(layouts[passidx]);
	REMOVE_ARRAY(layouts);*/

	delete this;
}