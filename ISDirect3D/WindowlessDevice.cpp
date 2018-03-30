#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: WindowlessDevice()
// Desc: Constructor
//-----------------------------------------------------------------------------
WindowlessDevice::WindowlessDevice()
{
	device = NULL;
	devicecontext = NULL;
	shader = NULL;
	desttexture = NULL;
}

//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create new Direct3D 10 device
//-----------------------------------------------------------------------------
Result WindowlessDevice::Create()
{
	Result rlt;

	/*// Create device
	UINT createdeviceflags = NULL;
#ifdef _DEBUG
	createdeviceflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//CHKRESULT(D3D10CreateDevice(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, D3D10_SDK_VERSION, &devspec->device));
CHKRESULT(D3D10CreateDevice1(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, D3D10_FEATURE_LEVEL_10_0, D3D11_1_SDK_VERSION, &device));*/

	// Create device
	UINT createdeviceflags = NULL;
#ifdef _DEBUG
	createdeviceflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL fl[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	CHKRESULT(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, fl, ARRAYSIZE(fl), D3D11_SDK_VERSION, &device, NULL, &devicecontext));

	// Create render quad
	renderquad.Init(device);
	renderquad.PrepareRendering(devicecontext);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetSourceTexture()
// Desc: Apply source texture and set appropriate shader variable
//-----------------------------------------------------------------------------
Result WindowlessDevice::AddSourceTexture(ITexture* tex, String& texsemantic, String& sizesemantic)
{
	if(!tex)
		return ERR("Parameter tex is NULL");
	Result rlt;

	Texture* t = reinterpret_cast<Texture*>(tex);
	if(shader)
	{
		// Store texture, variables and semantics
		LPD3D11EFFECTSHADERRESOURCEVARIABLE vartex = shader->GetVariableBySemantic(texsemantic)->AsShaderResource();
		LPD3D11EFFECTVECTORVARIABLE varsize = shader->GetVariableBySemantic(sizesemantic)->AsVector();
		srctextures.push_back(new TextureAndVariables(t, vartex, varsize, texsemantic, sizesemantic));
/*		CHKRESULT(vartex->SetResource(t->srview));*/ //EDIT11
		float sizevector[] = {(float)t->width, (float)t->height, 0.0f, 0.0f};
		CHKRESULT(varsize->SetFloatVector(sizevector));
	}
	else
		// Store only texture and semantics (variables will be created on a later call to SetShader())
		srctextures.push_back(new TextureAndVariables(t, NULL, NULL, texsemantic, sizesemantic));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetDestinationTexture()
// Desc: Apply destiantion texture and create an aligned viewport
//-----------------------------------------------------------------------------
Result WindowlessDevice::SetDestinationTexture(ITexture* tex, String& sizesemantic)
{
	if(!tex)
		return ERR("Parameter tex is NULL");
	Result rlt;

	Texture* t = reinterpret_cast<Texture*>(tex);

	// Set a new viewport that exactly matches the size of the destination texture
	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)t->width;
	viewport.Height = (FLOAT)t->height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	devicecontext->RSSetViewports(1, &viewport);

	// Set render target
	devicecontext->OMSetRenderTargets(1, &t->rtview, NULL);

	if(shader)
	{
		// Store texture, variables and semantics
		LPD3D11EFFECTVECTORVARIABLE varsize = shader->GetVariableBySemantic(sizesemantic)->AsVector();
		this->desttexture = new TextureAndVariables(t, NULL, varsize, String(), sizesemantic);
		float sizevector[] = {(float)t->width, (float)t->height, 0.0f, 0.0f};
		CHKRESULT(varsize->SetFloatVector(sizevector));
	}
	else
		// Store only texture and semantics (variables will be created on a later call to SetShader())
		this->desttexture = new TextureAndVariables(t, NULL, NULL, String(), sizesemantic);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetShader()
// Desc: Apply shader and sync vertex declaration with it
//-----------------------------------------------------------------------------
Result UpdateTextureAndVariables(RenderShader* shader, WindowlessDevice::TextureAndVariables* tav)
{
	Result rlt;

	if(!tav->vartex && !tav->texsemantic.IsEmpty())
	{
		tav->vartex = shader->GetVariableBySemantic(tav->texsemantic)->AsShaderResource();
/*		CHKRESULT(tav->vartex->SetResource(tav->t->srview));*/ //EDIT11
	}
	if(!tav->varsize && !tav->sizesemantic.IsEmpty())
	{
		tav->varsize = shader->GetVariableBySemantic(tav->sizesemantic)->AsVector();
		float sizevector[] = {(float)tav->t->width, (float)tav->t->height, 0.0f, 0.0f};
/*		CHKRESULT(tav->varsize->SetFloatVector(sizevector));*/ //EDIT11
	}

	return R_OK;
}
Result WindowlessDevice::SetShader(IRenderShader* shader)
{
	if(!shader)
		return ERR("Parameter shader is NULL");
	Result rlt;

	this->shader = reinterpret_cast<RenderShader*>(shader);

	// Get shader parameters
	std::list<TextureAndVariables*>::iterator iter;
	LIST_FOREACH(srctextures, iter)
		UpdateTextureAndVariables(this->shader, (TextureAndVariables*)*iter);
	if(desttexture)
		UpdateTextureAndVariables(this->shader, desttexture);

	return this->shader->SetVtxDecl(RenderQuad::VTX_DECL, ARRAYSIZE(RenderQuad::VTX_DECL)); //EDIT: Vertex declaration may be changed if this shader is used by multiple objects
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Render onto desttexture
//-----------------------------------------------------------------------------
void WindowlessDevice::Render(float bg_r, float bg_g, float bg_b)
{
	if(!desttexture || !desttexture->t->rtview)
		return;

	// Clear render target
	float backcolor[] = {bg_r, bg_g, bg_b, 1.0f};
	devicecontext->ClearRenderTargetView(desttexture->t->rtview, backcolor);

	Render();
}
void WindowlessDevice::Render()
{
	if(!desttexture || !desttexture->t->rtview || !shader)
		return;

	UINT numpasses;
	shader->PrepareRendering(&numpasses);
	for(UINT passidx = 0; passidx < numpasses; passidx++)
	{
		shader->PreparePass(passidx);
		devicecontext->DrawIndexed(6, 0, 0);
	}
}

//-----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: Same as OutputWindow::CreateTexture()
//-----------------------------------------------------------------------------
Result WindowlessDevice::CreateTexture(FilePath& filename, ITexture::Usage usage, bool deviceindependent, ITexture** tex)
{
return ERR("Currently not working");
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->Load((OutputWindow*)this, filename, usage, deviceindependent)); //EDIT!!!

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}
Result WindowlessDevice::CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, ITexture** tex)
{
return ERR("Currently not working");
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->CreateNew((OutputWindow*)this, width, height, usage, deviceindependent, format)); //EDIT!!!

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: RemoveTexture()
// Desc: Unbind and release the given texture
//-----------------------------------------------------------------------------
void WindowlessDevice::RemoveTexture(ITexture* tex)
{
	Texture* basetex = reinterpret_cast<Texture*>(tex);
	if(!basetex)
		return;

	// Unbind texture
	if(basetex == desttexture->t)
	{
		devicecontext->OMSetRenderTargets(0, NULL, NULL); // Remove render target
		delete desttexture;
		desttexture = NULL;
	}
	else
		for(std::list<TextureAndVariables*>::iterator iter = srctextures.begin(); iter != srctextures.end(); iter++)
			if(basetex == (*iter)->t && (*iter)->vartex)
			{
				(*iter)->vartex->SetResource(NULL); // Remove source texture from GPU
				shader->ApplyStateChanges();
				delete *iter;
				srctextures.erase(iter);
				break;
			}

	// Release texture
	textures.remove(basetex);
	basetex->Release();
}

//-----------------------------------------------------------------------------
// Name: CreateRenderShader()
// Desc: Same as OutputWindow::CreateRenderShader()
//-----------------------------------------------------------------------------
Result WindowlessDevice::CreateRenderShader(FilePath& filename, IRenderShader** shader)
{
	Result rlt;

	*shader = NULL;

	RenderShader* newshader;
	CHKALLOC(newshader = new RenderShader());
	CHKRESULT(newshader->LoadFX(filename, device, devicecontext));

	rendershaders.push_back(newshader);
	*shader = newshader;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void WindowlessDevice::Release()
{
	for(std::list<RenderShader*>::iterator iter = rendershaders.begin(); iter != rendershaders.end(); iter++)
		(*iter)->Release();
	rendershaders.clear();

	for(std::list<Texture*>::iterator iter = textures.begin(); iter != textures.end(); iter++)
		(*iter)->Release();
	textures.clear();
	for(std::list<TextureAndVariables*>::iterator iter = srctextures.begin(); iter != srctextures.end(); iter++)
		delete *iter;
	srctextures.clear();
	REMOVE(desttexture);

	RELEASE(device);

	delete this;
}