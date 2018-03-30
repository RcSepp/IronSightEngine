#include "Direct3D.h"


#define SPRITECONTAINER_MAX_SLOTS	256 // Must be in sync with sprite shader


//-----------------------------------------------------------------------------
// Name: SpriteContainer()
// Desc: Constructor
//-----------------------------------------------------------------------------
SpriteContainer::SpriteContainer(UINT buffersize)
	: spriteshader(NULL), viewmatrix(NULL), projmatrix(NULL), usecamviewmatrix(false), usecamprojmatrix(false), immediate_spritebuffer(NULL),
	  immediate_buffercapacity(buffersize && buffersize < SPRITECONTAINER_MAX_SLOTS ? buffersize : SPRITECONTAINER_MAX_SLOTS)
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Load sprite shader
//-----------------------------------------------------------------------------
Result SpriteContainer::Init(OutputWindow* wnd, const FilePath filename)
{
	Result rlt;

	this->wnd = wnd;

	// Compile shader
	LPRENDERSHADER lpshd;
	CHKRESULT(wnd->CreateRenderShader(filename, &lpshd));
	spriteshader = (RenderShader*)lpshd;

	// Get custom parameters
	spritesvar = spriteshader->GetVariableBySemantic("Sprites");
	if(!spritesvar->IsValid())
		return ERR("Variable with semantic 'Sprites' not found in Sprite.fx");
	viewprojvar = spriteshader->GetVariableBySemantic("ViewProj")->AsMatrix();
	if(!viewprojvar->IsValid())
		return ERR("Variable with semantic 'ViewProj' not found in Sprite.fx");
	texvar = spriteshader->GetVariableBySemantic("Texture")->AsShaderResource();
	if(!texvar->IsValid())
		return ERR("Variable with semantic 'Texture' not found in Sprite.fx");

	// Prepare shader
	CHKRESULT(spriteshader->SetTechnique(0));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Take over sprite shader from other container
//-----------------------------------------------------------------------------
Result SpriteContainer::Init(OutputWindow* wnd, SpriteContainer* other)
{
	this->wnd = wnd;

	this->spriteshader = other->spriteshader;
	this->spritesvar = other->spritesvar;
	this->viewprojvar = other->viewprojvar;
	this->texvar = other->texvar;

	return R_OK;
}

#pragma region View and projection matrices
//-----------------------------------------------------------------------------
// Name: SetViewTransform()
// Desc: Set custom view matrix
//-----------------------------------------------------------------------------
void SpriteContainer::SetViewTransform(const Matrix* viewmatrix)
{
	REMOVE(this->viewmatrix);
	if(viewmatrix)
		this->viewmatrix = new Matrix(*viewmatrix);
	usecamviewmatrix = false;
}
void SpriteContainer::SetIdentityViewTransform()
{
	REMOVE(this->viewmatrix);
	usecamviewmatrix = false;
}
void SpriteContainer::SetCameraViewTransform()
{
	REMOVE(this->viewmatrix);
	usecamviewmatrix = true;
}

//-----------------------------------------------------------------------------
// Name: SetProjectionTransform()
// Desc: Set custom projection matrix
//-----------------------------------------------------------------------------
void SpriteContainer::SetProjectionTransform(const Matrix* projmatrix)
{
	REMOVE(this->projmatrix);
	if(projmatrix)
		this->projmatrix = new Matrix(*projmatrix);
	usecamprojmatrix = false;
}
void SpriteContainer::SetIdentityProjectionTransform()
{
	REMOVE(this->projmatrix);
	usecamprojmatrix = false;
}
void SpriteContainer::SetCameraProjectionTransform()
{
	REMOVE(this->projmatrix);
	usecamprojmatrix = true;
}

void SpriteContainer::SetMatrices()
{
	if(viewmatrix)
	{
		if(projmatrix)
			viewprojvar->SetMatrix(*viewmatrix * *projmatrix);
		else if(usecamprojmatrix)
			viewprojvar->SetMatrix(*viewmatrix * wnd->cam->projmatrix);
		else
			viewprojvar->SetMatrix(*viewmatrix);
	}
	else if(usecamviewmatrix)
	{
		if(projmatrix)
			viewprojvar->SetMatrix(wnd->cam->viewmatrix * *projmatrix);
		else if(usecamprojmatrix)
			viewprojvar->SetMatrix(wnd->cam->viewprojmatrix);
		else
			viewprojvar->SetMatrix(wnd->cam->viewmatrix);
	}
	else
	{
		if(projmatrix)
			viewprojvar->SetMatrix(*projmatrix);
		else if(usecamprojmatrix)
			viewprojvar->SetMatrix(wnd->cam->projmatrix);
		else
		{
			Matrix identity;
			viewprojvar->SetMatrix(*MatrixIdentity(&identity));
		}
	}
}
#pragma endregion

#pragma region Immediate-drawn sprites
//-----------------------------------------------------------------------------
// Name: Begin()
// Desc: Prepare rendering immediately-drawn sprites
//-----------------------------------------------------------------------------
void SpriteContainer::Begin()
{
	if(!wnd->cam)
		return;

	wnd->devspec->devicecontext->IASetInputLayout(NULL);
	wnd->devspec->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	SetMatrices();
}

//-----------------------------------------------------------------------------
// Name: DrawSpriteDelayed()
// Desc: Add sprite to buffer. The buffer is rendered and cleared when it is full or a sprite of different texture is to be drawn or End() is called.
//		 This should only be called between Start() and End() and no device state changes should be performed inbetween those calls.
//		 This methode is faster than DrawSpriteDelayed(), because it uses preallocated memory of size immediate_buffercapacity (defined in constructor).
//-----------------------------------------------------------------------------
void SpriteContainer::DrawSpriteImmediate(const Sprite* sprite)
{
	// Don't allocate memory for DrawSpriteImmediate() until it's called the first time
	if(!immediate_spritebuffer)
	{
		immediate_spritebuffer = new GpuSprite[immediate_buffercapacity];
		immediate_buffersize = 0;
	}

	ID3D11ShaderResourceView *currenttexture = ((Texture*)sprite->tex)->srview;

	if(immediate_buffersize == 0) // If a new buffer is started
		immediate_tex = currenttexture; // Define texture for this buffer
	else if(immediate_buffersize == immediate_buffercapacity || currenttexture != immediate_tex) // If the buffer is full or sprite has a different texture than the buffered sprites
	{
		End(); // Render and restart buffer
		immediate_tex = currenttexture; // Define texture for the new buffer
	}

	// Append world matrix, texture rect and color
	GpuSprite& s = immediate_spritebuffer[immediate_buffersize++];
	MatrixTranspose(&s.worldmatrix, &sprite->worldmatrix);
	s.texcoord = sprite->texcoord;
	s.texsize = sprite->texsize;
	s.color = sprite->color;
}

//-----------------------------------------------------------------------------
// Name: End()
// Desc: Render remaining immediately-drawn sprites
//-----------------------------------------------------------------------------
void SpriteContainer::End()
{
	if(!wnd->cam || immediate_spritebuffer == 0 || immediate_buffersize == 0)
		return;

	// Render buffer
	texvar->SetResource(immediate_tex);
	spritesvar->SetRawValue(immediate_spritebuffer, 0, immediate_buffersize * sizeof(GpuSprite));
	spriteshader->PreparePass(0, 0);
	wnd->devspec->devicecontext->Draw(immediate_buffersize, 0);

	// Restart buffer
	immediate_buffersize = 0;
}
#pragma endregion

#pragma region Delayed-drawn sprites
//-----------------------------------------------------------------------------
// Name: DrawSpriteDelayed()
// Desc: Add sprite to buffer. The buffer is rendered and cleared when Render(RenderType rendertype) is called.
//		 This methode is slower than DrawSpriteImmediate(), because it allocates memory on the fly.
//-----------------------------------------------------------------------------
void SpriteContainer::DrawSpriteDelayed(const Sprite* sprite)
{
	ID3D11ShaderResourceView *currenttexture = ((Texture*)sprite->tex)->srview;

	// Append texture
	if(delayed_textures.empty() || currenttexture != delayed_textures.back().tex)
		delayed_textures.push_back(TextureAndCount(currenttexture, 1));
	else
		delayed_textures.back().count++;

	// Append world matrix, texture rect and color
	delayed_spritebuffer.push_back(GpuSprite());
	GpuSprite& s = delayed_spritebuffer.back();
	MatrixTranspose(&s.worldmatrix, &sprite->worldmatrix);
	s.texcoord = sprite->texcoord;
	s.texsize = sprite->texsize;
	s.color = sprite->color;
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Render delayed-drawn sprites
//-----------------------------------------------------------------------------
void SpriteContainer::Render(RenderType rendertype)
{
	if(!wnd->cam || delayed_spritebuffer.empty())
		return;

	wnd->devspec->devicecontext->IASetInputLayout(NULL);

	wnd->devspec->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	SetMatrices();

	GpuSprite* spriteptr = &delayed_spritebuffer[0];
	std::list<TextureAndCount>::iterator iter;
	LIST_FOREACH(delayed_textures, iter)
	{
		texvar->SetResource(iter->tex);
		while(iter->count)
		{
			UINT rendercount = min(iter->count, SPRITECONTAINER_MAX_SLOTS);

			spritesvar->SetRawValue(spriteptr, 0, rendercount * sizeof(GpuSprite));

			spriteshader->PreparePass(0, 0);
			wnd->devspec->devicecontext->Draw(rendercount, 0);

			spriteptr += rendercount;
			iter->count -= rendercount;
		}
	}
	delayed_textures.clear();
	delayed_spritebuffer.clear();
}
#pragma endregion

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void SpriteContainer::Release()
{
	wnd->spritecontainers.remove(this);

	if(viewmatrix)
		delete viewmatrix;
	if(projmatrix)
		delete projmatrix;

	if(immediate_spritebuffer)
		delete[] immediate_spritebuffer;

	delete this;
}