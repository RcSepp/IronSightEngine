#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: SkyBox()
// Desc: Constructor
//-----------------------------------------------------------------------------
SkyBox::SkyBox()
{
	wnd = NULL;
	spritecontainer = NULL;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize
//-----------------------------------------------------------------------------
Result SkyBox::Init(OutputWindow* wnd)
{
	Result rlt;

	this->wnd = wnd;
	if(!wnd)
		return ERR("Parameter wnd is NULL");

	// Create spritecontainer
	CHKRESULT(wnd->CreateSpriteContainer(6, &spritecontainer));
	spritecontainer->SetCameraProjectionTransform();

	return R_OK;
}

Result PrepareLayer(OutputWindow* wnd, const FilePath& filename, Texture* tex, Sprite* sprt)
{
	Result rlt;

	HRESULT* hr = new HRESULT();
	CHKRESULT(tex->Load(wnd, filename, Texture::TU_SOURCE, false));
	sprt->tex = tex;

	sprt->texcoord = Vector2(0.0f, 0.0f);
	sprt->texsize = Vector2(1.0f, 1.0f);
	sprt->color = 0xFFFFFFFF;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateLayer()
// Desc: Create and store skybox layer from up to 6 image files (distance: 0xFFFFFFF == infinity)
//-----------------------------------------------------------------------------
Result SkyBox::CreateLayer(const FilePath& north, const FilePath& east, const FilePath& south, const FilePath& west, const FilePath& top, const FilePath& bottom, UINT distance)
{
	Result rlt;

	Layer* newlayer;
	CHKALLOC(newlayer = new Layer());

	newlayer->distance = distance == 0xFFFFFFFF ? 0.0f : (float)distance;

	newlayer->numfaces = !north.IsEmpty() + !east.IsEmpty() + !south.IsEmpty() +
						 !west.IsEmpty() + !top.IsEmpty() + !bottom.IsEmpty();
	CHKALLOC(newlayer->faces = new Sprite[newlayer->numfaces]);
	CHKALLOC(newlayer->textures = new Texture[newlayer->numfaces]);

	float boxdist = (wnd->cam->clipnear + wnd->cam->clipfar) / 2.0f; //EDIT: Warn if cam not yet set or register for clipnear-, clipfar change
	Matrix mScale, mTranslate, mRotate;
	byte i = 0;

	MatrixScaling(&mScale, boxdist * 2.0f, boxdist * 2.0f, boxdist * 2.0f); // EDIT: Adjust for FOV
	MatrixTranslation(&mTranslate, 0.0f, 0.0f, boxdist);

	if(!north.IsEmpty())
	{
		IFSUCCEEDED(PrepareLayer(wnd, north, &newlayer->textures[i], &newlayer->faces[i]))
			newlayer->faces[i++].worldmatrix = mScale * mTranslate;
		else
			bool abc = rlt == Error(D3D11_ERROR_FILE_NOT_FOUND);
	}
	if(!east.IsEmpty())
	{
		IFSUCCEEDED(PrepareLayer(wnd, east, &newlayer->textures[i], &newlayer->faces[i]))
		{
			MatrixRotationY(&mRotate, PI / 2.0f);
			newlayer->faces[i++].worldmatrix = mScale * mTranslate * mRotate;
		}
		else
			int abc = 0;
	}
	if(!south.IsEmpty())
	{
		IFSUCCEEDED(PrepareLayer(wnd, south, &newlayer->textures[i], &newlayer->faces[i]))
		{
			MatrixRotationY(&mRotate, PI);
			newlayer->faces[i++].worldmatrix = mScale * mTranslate * mRotate;
		}
		else
			int abc = 0;
	}
	if(!west.IsEmpty())
	{
		IFSUCCEEDED(PrepareLayer(wnd, west, &newlayer->textures[i], &newlayer->faces[i]))
		{
			MatrixRotationY(&mRotate, -PI / 2.0f);
			newlayer->faces[i++].worldmatrix = mScale * mTranslate * mRotate;
		}
		else
			int abc = 0;
	}
	if(!top.IsEmpty())
	{
		IFSUCCEEDED(PrepareLayer(wnd, top, &newlayer->textures[i], &newlayer->faces[i]))
		{
			MatrixRotationX(&mRotate, -PI / 2.0f);
			newlayer->faces[i++].worldmatrix = mScale * mTranslate * mRotate;
		}
		else
			int abc = 0;
	}
	if(!bottom.IsEmpty())
	{
		IFSUCCEEDED(PrepareLayer(wnd, bottom, &newlayer->textures[i], &newlayer->faces[i]))
		{
			MatrixRotationX(&mRotate, PI / 2.0f);
			newlayer->faces[i++].worldmatrix = mScale * mTranslate * mRotate;
		}
		else
			int abc = 0;
	}

	// Store layer
	layers.push_back(newlayer);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Draw HUD to screen
//-----------------------------------------------------------------------------
void SkyBox::Render(RenderType rendertype)
{
	if(layers.empty())
		return;

	float parralaxfactor;
	Matrix mRot, mTranslation, mNewviewmatrix;
	if(wnd->cam->orientmode == OM_EULER)
	{
		Matrix mRx, mRy, mRz;
		MatrixRotationY(&mRy, wnd->cam->rot.y);
		MatrixRotationX(&mRx, wnd->cam->rot.x);
		MatrixRotationZ(&mRz, wnd->cam->rot.z);
		mRot = mRy * mRx * mRz;
	}
	else // orientmode == OM_QUAT
		MatrixRotationQuaternion(&mRot, &wnd->cam->qrot);

	/*ID3D11BlendState* bstate; // Edit: Move to OutputWindow
	D3D11_BLEND_DESC bdesc;
	bdesc.AlphaToCoverageEnable = FALSE;
	for(byte i = 0; i < 8; i++)
	{
		bdesc.BlendEnable[i] = TRUE;
		bdesc.RenderTargetWriteMask[i] = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	bdesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bdesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bdesc.BlendOp = D3D11_BLEND_OP_ADD;
	bdesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
	bdesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	bdesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	wnd->devspec->device->CreateBlendState(&bdesc, &bstate);
	wnd->devspec->device->OMSetBlendState(bstate, NULL, 0xFFFFFFFF);*/

	spritecontainer->Begin();
	std::list<Layer*>::const_iterator layer_itr;
	LIST_FOREACH(layers, layer_itr)
	{
		parralaxfactor = (*layer_itr)->distance == 0.0f ? 0.0f : 1.0f / (*layer_itr)->distance;

		MatrixTranslation(&mTranslation, -wnd->cam->pos.x * parralaxfactor,
											 -wnd->cam->pos.y * parralaxfactor,
											 -wnd->cam->pos.z * parralaxfactor);
		spritecontainer->SetViewTransform(&(mTranslation * mRot));

		for(BYTE i = 0; i < (*layer_itr)->numfaces; i++)
			spritecontainer->DrawSpriteImmediate((*layer_itr)->faces + i);
	}
	spritecontainer->End();
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void SkyBox::Release()
{

	delete this;
}