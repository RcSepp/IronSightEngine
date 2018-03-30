#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: Texture()
// Desc: Constructor
//-----------------------------------------------------------------------------
Texture::Texture()
{
	tex = NULL;
	srview = NULL;
	rtview = NULL;
	wnd = NULL;
}

//-----------------------------------------------------------------------------
// Name: New()
// Desc: Create a new texture
//-----------------------------------------------------------------------------
Result Texture::CreateNew(OutputWindow* wnd, UINT width, UINT height, Usage usage, bool deviceindependent, DXGI_FORMAT format, UINT miplevels, LPVOID data)
{
	D3D11_TEXTURE2D_DESC texdesc;
	Result rlt;

	this->width = width;
	this->height = height;
	this->wnd = wnd;
	filename.Clear();

	switch(usage)
	{
	case TU_SKIN:
		texdesc.MipLevels = miplevels == 0xFFFFFFFF ? 0 : miplevels; // Full mipmap chain
		texdesc.Usage = D3D11_USAGE_IMMUTABLE; // GPU read only, CPU no access
		texdesc.CPUAccessFlags = NULL; // CPU no access
		texdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as shader resource
		break;

	case TU_SKIN_NOMIPMAP:
		texdesc.MipLevels = miplevels == 0xFFFFFFFF ? 1 : miplevels; // No mipmaps
		texdesc.Usage = D3D11_USAGE_IMMUTABLE; // GPU read only, CPU no access
		texdesc.CPUAccessFlags = NULL; // CPU no access
		texdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as shader resource
		break;

	case TU_SOURCE:
		texdesc.MipLevels = miplevels == 0xFFFFFFFF ? 1 : miplevels; // No mipmaps
		texdesc.Usage = D3D11_USAGE_IMMUTABLE; // GPU read only, CPU no access
		texdesc.CPUAccessFlags = NULL; // CPU no access
		texdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as shader resource
		texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_RENDERTARGET:
		texdesc.MipLevels = miplevels == 0xFFFFFFFF ? 1 : miplevels; // No mipmaps
		texdesc.Usage = D3D11_USAGE_DEFAULT; // GPU read & write, CPU no access
		texdesc.CPUAccessFlags = NULL; // CPU no access
		texdesc.BindFlags = D3D11_BIND_RENDER_TARGET; // Bind as render target
		texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_RENDERTARGETORSOURCE:
		texdesc.MipLevels = miplevels == 0xFFFFFFFF ? 1 : miplevels; // No mipmaps
		texdesc.Usage = D3D11_USAGE_DEFAULT; // GPU read & write, CPU no access
		texdesc.CPUAccessFlags = NULL; // CPU no access
		texdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // Bind as render target and shader resource
		texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_ALLACCESS:
		texdesc.MipLevels = miplevels == 0xFFFFFFFF ? 1 : miplevels; // No mipmaps
		texdesc.Usage = D3D11_USAGE_STAGING; // GPU read & write, CPU read & write
		texdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE; // CPU read & write
		texdesc.BindFlags = 0; // No binding
		texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_CPUWRITE:
		texdesc.MipLevels = miplevels == 0xFFFFFFFF ? 1 : miplevels; // No mipmaps
		texdesc.Usage = D3D11_USAGE_DYNAMIC; // GPU read, CPU write
		texdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU write
		texdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as shader resource
		texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
	}
	texdesc.Width = width;
	texdesc.Height = height;
	texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //EDIT
	texdesc.SampleDesc.Count = 1; //EDIT
	texdesc.SampleDesc.Quality = 0; //EDIT
	texdesc.ArraySize = 1;
	texdesc.MiscFlags = deviceindependent ? D3D11_RESOURCE_MISC_SHARED : NULL;

	// Create texture
	if(data)
	{
		D3D11_SUBRESOURCE_DATA d3ddata;
		d3ddata.pSysMem = data;
		d3ddata.SysMemPitch = 4 * width; //EDIT
		CHKRESULT(wnd->devspec->device->CreateTexture2D(&texdesc, &d3ddata, &tex));
	}
	else
		CHKRESULT(wnd->devspec->device->CreateTexture2D(&texdesc, NULL, &tex));

	if(!texdesc.MipLevels)
		tex->GetDesc(&texdesc); // Update textdesc with the amount of miplevels created
	this->miplevels = texdesc.MipLevels;

	if(texdesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// Create resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srviewdesc;
		ZeroMemory(&srviewdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srviewdesc.Format = texdesc.Format;
		srviewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srviewdesc.Texture2D.MostDetailedMip = 0;
		srviewdesc.Texture2D.MipLevels = texdesc.MipLevels;
		CHKRESULT(wnd->devspec->device->CreateShaderResourceView(tex, &srviewdesc, &srview));
	}

	if(texdesc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		// Create render target view
		D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
		rtviewdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; //EDIT: Use D3D11_RTV_DIMENSION_TEXTURE2DMS if device is multisampled
		rtviewdesc.Texture2D.MipSlice = 0;
		CHKRESULT(wnd->devspec->device->CreateRenderTargetView(tex, &rtviewdesc, &rtview));
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: New()
// Desc: Create a texture by copying an existing ressource (using the GPU)
//-----------------------------------------------------------------------------
Result Texture::CreateCopy(Texture* source, Usage usage, bool deviceindependent)
{
	Result rlt;

	CHKRESULT(CreateNew(source->wnd, source->width, source->height, usage, deviceindependent, DXGI_FORMAT_R8G8B8A8_UNORM, source->miplevels));
	source->wnd->devspec->devicecontext->CopyResource(this->tex, source->tex);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load texture from file
//-----------------------------------------------------------------------------
Result Texture::Load(OutputWindow* wnd, const FilePath& filename, Usage usage, bool deviceindependent)
{
	D3DX11_IMAGE_LOAD_INFO loadinfo;
	bool genmipmaps;
	Result rlt;

	this->wnd = wnd;
	filename.Copy(&this->filename);

	switch(usage)
	{
	case TU_SKIN:
		genmipmaps = true; // Full mipmap chain
		loadinfo.Usage = D3D11_USAGE_IMMUTABLE; // GPU read only, CPU no access
		loadinfo.CpuAccessFlags = NULL; // CPU no access
		loadinfo.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as shader resource
		break;

	case TU_SKIN_NOMIPMAP:
		genmipmaps = false; // No mipmaps
		loadinfo.Usage = D3D11_USAGE_IMMUTABLE; // GPU read only, CPU no access
		loadinfo.CpuAccessFlags = NULL; // CPU no access
		loadinfo.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as shader resource
		break;

	case TU_SOURCE:
		genmipmaps = false; // No mipmaps
		loadinfo.Usage = D3D11_USAGE_IMMUTABLE; // GPU read only, CPU no access
		loadinfo.CpuAccessFlags = NULL; // CPU no access
		loadinfo.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as shader resource
		loadinfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_RENDERTARGET:
		genmipmaps = false; // No mipmaps
		loadinfo.Usage = D3D11_USAGE_DEFAULT; // GPU read & write, CPU no access
		loadinfo.CpuAccessFlags = NULL; // CPU no access
		loadinfo.BindFlags = D3D11_BIND_RENDER_TARGET; // Bind as render target
		loadinfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_RENDERTARGETORSOURCE:
		genmipmaps = false; // No mipmaps
		loadinfo.Usage = D3D11_USAGE_DEFAULT; // GPU read & write, CPU no access
		loadinfo.CpuAccessFlags = NULL; // CPU no access
		loadinfo.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // Bind as render target and shader resource
		loadinfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_ALLACCESS:
		genmipmaps = false; // No mipmaps
		loadinfo.Usage = D3D11_USAGE_STAGING; // GPU read & write, CPU read & write
		loadinfo.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE; // CPU read & write
		loadinfo.BindFlags = 0; // No binding
		loadinfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_CPUWRITE:
		genmipmaps = false; // No mipmaps
		loadinfo.Usage = D3D11_USAGE_DYNAMIC; // GPU read, CPU write
		loadinfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU write
		loadinfo.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as shader resource
		loadinfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Make sure the texture is of type 32bit RGBA
	}

	if(genmipmaps && deviceindependent)
		return ERR("Device-independend mipmapped textures are not supported.");

	loadinfo.MipLevels = (UINT)!genmipmaps;
	loadinfo.MiscFlags = deviceindependent ? D3D11_RESOURCE_MISC_SHARED : NULL;
	CHKALLOC(loadinfo.pSrcInfo = new D3DX11_IMAGE_INFO()); // Prepare dimension retreval

	// Load texture from file
	CHKRESULT(D3DX11CreateTextureFromFile(wnd->devspec->device, filename, &loadinfo, NULL, (ID3D11Resource**)&tex, NULL));

	// Get dimensions
	width = loadinfo.pSrcInfo->Width;
	height = loadinfo.pSrcInfo->Height;
	miplevels = loadinfo.MipLevels;
	delete loadinfo.pSrcInfo;

	if(loadinfo.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// Create resource view
		/*D3D11_SHADER_RESOURCE_VIEW_DESC srviewdesc;
		ZeroMemory(&srviewdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srviewdesc.Format = loadinfo.Format;
		srviewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srviewdesc.Texture2D.MostDetailedMip = 0;
		srviewdesc.Texture2D.MipLevels = loadinfo.MipLevels;
		CHKRESULT(wnd->devspec->device->CreateShaderResourceView(tex, &srviewdesc, &srview));*/
		CHKRESULT(wnd->devspec->device->CreateShaderResourceView(tex, NULL, &srview));
	}

	if(loadinfo.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		// Create render target view
		D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
		rtviewdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; //EDIT: Use D3D11_RTV_DIMENSION_TEXTURE2DMS if device is multisampled
		rtviewdesc.Texture2D.MipSlice = 0;
		CHKRESULT(wnd->devspec->device->CreateRenderTargetView(tex, &rtviewdesc, &rtview));
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Lock()
// Desc: Lock texture and read raw pixel data
//-----------------------------------------------------------------------------
Result Texture::Lock(D3D11_MAPPED_SUBRESOURCE* data)
{
	return CheckResult(wnd->devspec->devicecontext->Map(tex, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ_WRITE, 0, data));
}
//-----------------------------------------------------------------------------
// Name: Unlock()
// Desc: Unlock access to texture
//-----------------------------------------------------------------------------
void Texture::Unlock()
{
	wnd->devspec->devicecontext->Unmap(tex, D3D11CalcSubresource(0, 0, 1));
}

//-----------------------------------------------------------------------------
// Name: MapPixelData()
// Desc: Lock texture and read raw pixel data
//-----------------------------------------------------------------------------
Result Texture::MapPixelData(bool read, bool write, UINT* stride, LPVOID* data)
{
	Result rlt;

	D3D11_MAPPED_SUBRESOURCE mappedtex;
	CHKRESULT(wnd->devspec->devicecontext->Map(tex, D3D11CalcSubresource(0, 0, 1), (write == true && read == false) ? D3D11_MAP_WRITE_DISCARD  : (D3D11_MAP)((read * D3D11_MAP_READ) | (write * D3D11_MAP_WRITE)), 0, &mappedtex)); //EDIT: Let user choose whether to discard
	if(stride)
		*stride = mappedtex.RowPitch;
	*data = mappedtex.pData;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: UnmapData()
// Desc: Unlock access to texture
//-----------------------------------------------------------------------------
void Texture::UnmapData()
{
	wnd->devspec->devicecontext->Unmap(tex, D3D11CalcSubresource(0, 0, 1));
}

//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Save texture to file or memory
//-----------------------------------------------------------------------------
Result Texture::Save(const FilePath& filename, D3DX_IMAGE_FILEFORMAT format)
{
	return Error(D3DX11SaveTextureToFile(wnd->devspec->devicecontext, tex, format, filename));
}
Result Texture::Save(LPD3D10BLOB* datablob, D3DX_IMAGE_FILEFORMAT format)
{
	return Error(D3DX11SaveTextureToMemory(wnd->devspec->devicecontext, tex, format, datablob, NULL));
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Render to texture if it is a render target
//-----------------------------------------------------------------------------
void Texture::Render(LPD3DDEVICE device) //EDIT
{
	if(rtview)
	{
//wnd->devspec->device->OMSetRenderTargets(1, &rtview, NULL);
wnd->devspec->devicecontext->ClearRenderTargetView(rtview, Color(0xFF00FFFF));
//this->Save("out.bmp", D3DX11_IFF_BMP);
//int abc = 0;
	}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Texture::Release()
{
	wnd->textures.remove(this);

	RELEASE(tex);
	RELEASE(srview);
	RELEASE(rtview);

	delete this;
}