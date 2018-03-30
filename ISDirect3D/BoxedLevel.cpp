#include "Direct3D.h"


D3D11_INPUT_ELEMENT_DESC ilboxes[] =
{
    {"PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};


//-----------------------------------------------------------------------------
// Name: BoxedLevel()
// Desc: Constructor
//-----------------------------------------------------------------------------
BoxedLevel::BoxedLevel(const UINT (&chunksize)[3], LPTEXTURE texboxes, LPRENDERSHADER blevelshader)
	: wnd(NULL), blevelshader((RenderShader*)blevelshader), boxtypes(NULL), numboxtypes(0), texboxes((Texture*)texboxes)
{
	memcpy(this->chunksize, chunksize, 3 * sizeof(UINT));
	totalchunksize = chunksize[0] * chunksize[1] * chunksize[2];
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize boxed level
//-----------------------------------------------------------------------------
Result BoxedLevel::Init(OutputWindow* wnd, UINT numchunkbuffers)
{
	Result rlt;

	this->wnd = wnd;
	if(!wnd)
		return ERR("Parameter wnd is NULL");

	// Get custom parameters
	boxtypesvar = blevelshader->GetVariableBySemantic("BoxTypes");
	if(!boxtypesvar->IsValid())
		return ERR("Variable with semantic BoxTypes not found in boxed level shader");
	chunkposvar = blevelshader->GetVariableBySemantic("ChunkPosition")->AsVector();
	if(!chunkposvar->IsValid())
		return ERR("Variable with semantic ChunkPosition not found in boxed level shader");
	LPD3D11EFFECTVECTORVARIABLE chunksizevar = blevelshader->GetVariableBySemantic("ChunkSize")->AsVector();
	if(!chunksizevar->IsValid())
		return ERR("Variable with semantic ChunkSize not found in boxed level shader");
	worldviewprojvar = blevelshader->GetVariableBySemantic("ViewProj")->AsMatrix();
	if(!worldviewprojvar->IsValid())
		return ERR("Variable with semantic ViewProj not found in boxed level shader");

	// Prepare shader
	CHKRESULT(blevelshader->SetTechnique(0));
	chunksizevar->SetRawValue(chunksize, 0, 3 * sizeof(UINT));

	// Create chunk buffers
	ChunkBuffer* newchunkbuffer;
	for(UINT i = 0; i < numchunkbuffers; i++)
	{
		CHKALLOC(newchunkbuffer = new ChunkBuffer());

		D3D11_BUFFER_DESC vbdesc;
		vbdesc.ByteWidth = totalchunksize * sizeof(BoxedLevelBoxDesc);
		vbdesc.Usage = D3D11_USAGE_DEFAULT;
		vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbdesc.CPUAccessFlags = 0;
		vbdesc.MiscFlags = 0;

		CHKRESULT(wnd->devspec->device->CreateBuffer(&vbdesc, NULL, &newchunkbuffer->buffer));
		unusedchunkbuffers.push(newchunkbuffer);
	}

	// Create input layout
	CHKRESULT(blevelshader->SetVtxDecl(ilboxes, ARRAYSIZE(ilboxes)));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetBoxTypes()
// Desc: Send a list of available box types to the GPU. The array must be kept alive until the boxed level gets destroyed
//-----------------------------------------------------------------------------
void BoxedLevel::SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes)
{
	this->boxtypes = boxtypes;
	this->numboxtypes = numtypes;
	boxtypesvar->SetRawValue(boxtypes, 0, numtypes * sizeof(BoxedLevelBoxType));
}

//-----------------------------------------------------------------------------
// Name: AssignChunk()
// Desc: Assign chunk to a free chunk buffer and load content onto GPU
//-----------------------------------------------------------------------------
void BoxedLevel::AssignChunk(BoxedLevelChunk* chunk)
{
	// Check whether the chunk is already assigned
	std::map<BoxedLevelChunk*, ChunkBuffer*>::iterator pair = usedchunkbuffers.find(chunk);
	if(pair != usedchunkbuffers.end())
	{
		Result::PrintLogMessage("BoxedLevelChunk already bound to a buffer");
		return;
	}

	// Check whether a chunk buffer is free
	if(unusedchunkbuffers.empty())
	{
		Result::PrintLogMessage("Exceeded maximum amount of simultaneously used chunk buffers");
		return;
	}

	// Get chunk buffer
	ChunkBuffer* chunkbuffer = unusedchunkbuffers.top();
	unusedchunkbuffers.pop();
	usedchunkbuffers[chunk] = chunkbuffer;

	// Load chunk into vertex buffer
	D3D11_BOX destbox;
	destbox.left = 0;
	destbox.right = totalchunksize * sizeof(BoxedLevelBoxDesc);
	destbox.top = destbox.front = 0;
	destbox.bottom = destbox.back = 1;
	wnd->devspec->devicecontext->UpdateSubresource(chunkbuffer->buffer, 0, &destbox, chunk->boxmask, 0, 0);
}

//-----------------------------------------------------------------------------
// Name: UnassignChunk()
// Desc: Free assigned chunk buffer
//-----------------------------------------------------------------------------
void BoxedLevel::UnassignChunk(BoxedLevelChunk* chunk)
{
	std::map<BoxedLevelChunk*, ChunkBuffer*>::iterator pair = usedchunkbuffers.find(chunk);
	if(pair != usedchunkbuffers.end())
	{
		unusedchunkbuffers.push(pair->second);
		usedchunkbuffers.erase(pair);
	}
}

//-----------------------------------------------------------------------------
// Name: UpdateChunk()
// Desc: Sync subregion with the GPU
//-----------------------------------------------------------------------------
void BoxedLevel::UpdateChunk(BoxedLevelChunk* chunk, const UINT (&boxpos)[3])
{
	// Get chunk buffer
	std::map<BoxedLevelChunk*, ChunkBuffer*>::iterator pair = usedchunkbuffers.find(chunk);
	if(pair == usedchunkbuffers.end())
		return; // Ignore update on unassigned chunk

	// Format: [[[x]y]z]
	//UINT boxidx = boxpos[2] * chunksize[0] * chunksize[1] + boxpos[1] * chunksize[0] + boxpos[0];
	// Format: [[[y]z]x] (binvox format)
	UINT boxidx = boxpos[0] * chunksize[1] * chunksize[2] + boxpos[2] * chunksize[1] + boxpos[1];

	// Load chunk into vertex buffer
	D3D11_BOX destbox;
	destbox.left = boxidx * sizeof(BoxedLevelBoxDesc);
	destbox.right = destbox.left + sizeof(BoxedLevelBoxDesc);
	destbox.top = destbox.front = 0;
	destbox.bottom = destbox.back = 1;
	wnd->devspec->devicecontext->UpdateSubresource(pair->second->buffer, 0, &destbox, chunk->boxmask + boxidx, 0, 0);

/* // Update whole chunk (for debugging)
D3D11_BOX destbox;
destbox.left = 0 * sizeof(BoxedLevelBoxDesc);
destbox.right = chunk->numboxes * sizeof(BoxedLevelBoxDesc);
destbox.top = destbox.front = 0;
destbox.bottom = destbox.back = 1;
wnd->devspec->devicecontext->UpdateSubresource(pair->second->buffer, 0, &destbox, chunk->boxmask, 0, 0);*/
}
void BoxedLevel::UpdateChunk(BoxedLevelChunk* chunk, const UINT (&boxpos_min)[3], const UINT (&boxpos_max)[3])
{
	Result::PrintLogMessage("UpdateChunk(BoxedLevelChunk*, const UINT (&)[3], const UINT (&)[3]) NOT IMPLEMENTED");
}

//-----------------------------------------------------------------------------
// Name: GetAssignedChunks()
// Desc: Create a list of currently assigned (non-empty) chunks
//-----------------------------------------------------------------------------
void BoxedLevel::GetAssignedChunks(std::list<BoxedLevelChunk*>* chunks)
{
	std::map<BoxedLevelChunk*, ChunkBuffer*>::iterator iter;
	LIST_FOREACH(usedchunkbuffers, iter)
		if(iter->first->numboxes)
			chunks->push_back(iter->first);
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Draw boxed level
//-----------------------------------------------------------------------------
void BoxedLevel::Render(RenderType rendertype)
{
	// Prepare rendering
	wnd->devspec->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	worldviewprojvar->SetMatrix(wnd->cam->viewprojmatrix);

	// Render used chunk buffers
	std::map<BoxedLevelChunk*, ChunkBuffer*>::iterator iter;
	LIST_FOREACH(usedchunkbuffers, iter)
		if(iter->first->numboxes)
		{
			//EDIT: Add visibility check of chunk bounds against view frustum

			const UINT stride = 4;
			const UINT offset = 0;
			wnd->devspec->devicecontext->IASetVertexBuffers(0, 1, &iter->second->buffer, &stride, &offset);

			chunkposvar->SetFloatVector(Vector4(iter->first->GetPosX(chunksize), iter->first->GetPosY(chunksize), iter->first->GetPosZ(chunksize), 1.0f));

			UINT numpasses;
			blevelshader->PrepareRendering(&numpasses);
			blevelshader->PrepareSubset(texboxes, &Color(0.2f, 0.2f, 0.2f, 1.0f), &Color(0.8f, 0.8f, 0.8f, 1.0f), &Color(0.0f, 0.0f, 0.0f, 1.0f), 2.0f, 0);
			wnd->devspec->devicecontext->Draw(totalchunksize, 0);
		}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void BoxedLevel::Release()
{
	// Remove unused chunk buffers
	while(unusedchunkbuffers.size())
	{
		unusedchunkbuffers.top()->buffer->Release();
		delete unusedchunkbuffers.top();
		unusedchunkbuffers.pop();
	}

	// Remove used chunk buffers
	std::map<BoxedLevelChunk*, ChunkBuffer*>::iterator iter;
	LIST_FOREACH(usedchunkbuffers, iter)
	{
		iter->second->buffer->Release();
		delete iter->second;
	}
	usedchunkbuffers.clear();

	delete this;
}