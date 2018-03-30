#include "OpenGL.h"
#include <ISImage.h>

#ifdef _DEBUG
	#pragma comment (lib, "ISImage_d.lib")
#else
	#pragma comment (lib, "ISImage.lib")
#endif

//-----------------------------------------------------------------------------
// Name: Texture()
// Desc: Constructor
//-----------------------------------------------------------------------------
Texture::Texture()
{
	/*tex = NULL;
	srview = NULL;
	rtview = NULL;*/
	wnd = NULL;
}

int GetPow2(int v)
{
	int p2 = 1;
	while(p2 < v)
		p2 <<= 1;
	return p2;
}

//-----------------------------------------------------------------------------
// Name: New()
// Desc: Create a new texture
//-----------------------------------------------------------------------------
Result Texture::CreateNew(OutputWindow* wnd, UINT width, UINT height, Usage usage, UINT miplevels, LPVOID data)
{
	return ERR("Not implemented");
	/*D3D11_TEXTURE2D_DESC texdesc;
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

	return R_OK;*/
}

//-----------------------------------------------------------------------------
// Name: New()
// Desc: Create a texture by copying an existing ressource (using the GPU)
//-----------------------------------------------------------------------------
Result Texture::CreateCopy(Texture* source, Usage usage)
{
	return ERR("Not implemented");
	/*Result rlt;

	CHKRESULT(CreateNew(source->wnd, source->width, source->height, usage, deviceindependent, DXGI_FORMAT_R8G8B8A8_UNORM, source->miplevels));
	source->wnd->devspec->devicecontext->CopyResource(this->tex, source->tex);

	return R_OK;*/
}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load texture from file
//-----------------------------------------------------------------------------
Result Texture::Load(OutputWindow* wnd, const FilePath& filename, Usage usage)
{
	if(filename.IsEmpty())
		return ERR("Parameter filename is empty");
	if(!filename.Exists())
		return ERR(String("File not found: ") << filename);

	Image* img;
	Result rlt;

	CHKRESULT(LoadImage(filename, &img));

	this->wnd = wnd;
	filename.Copy(&this->filename);

	// Get dimensions
	width = img->width;
	height = img->height;
	miplevels = 1;

	// Get required properties from usage
	switch(usage)
	{
	case TU_SKIN:
		caps.genmipmaps = true; // Full mipmap chain
		// GPU read only
		// CPU no access
		caps.bind_resource = true; // Bind as shader resource
		// Flexible pixelformat
		break;

	case TU_SKIN_NOMIPMAP:
		// No mipmaps
		// GPU read only
		// CPU no access
		caps.bind_resource = true; // Bind as shader resource
		// Flexible pixelformat
		break;

	case TU_SOURCE:
		// No mipmaps
		// GPU read only
		// CPU no access
		caps.bind_resource = true; // Bind as shader resource
		caps.force_rgba = true; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_RENDERTARGET:
		// No mipmaps
		caps.gpu_write = true; // GPU read & write
		// CPU no access
		caps.bind_rendertarget = true; // Bind as render target
		caps.force_rgba = true; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_RENDERTARGETORSOURCE:
		// No mipmaps
		caps.gpu_write = true; // GPU read & write
		// CPU no access
		caps.bind_resource = caps.bind_rendertarget = true; // Bind as render target and shader resource
		caps.force_rgba = true; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_ALLACCESS:
		// No mipmaps
		caps.gpu_write = true; // GPU read & write
		caps.cpu_read = caps.cpu_write = true; // CPU read & write
		// No binding
		caps.force_rgba = true; // Make sure the texture is of type 32bit RGBA
		break;

	case TU_CPUWRITE:
		// No mipmaps
		// GPU read only
		caps.cpu_write = true; // CPU write
		caps.bind_resource = true; // Bind as shader resource
		caps.force_rgba = true; // Make sure the texture is of type 32bit RGBA
	}

	// Create and bind texture
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	// Rescale texture if it's dimmensions aren't a power of 2 (OpenGL doesn't support non-pow2 textures)
	int width_pow2 = width;//GetPow2(width);
	int height_pow2 = height;//GetPow2(height);
	unsigned char* data_pow2;

	if(width != width_pow2 || height != height_pow2)
	{
		data_pow2 = new unsigned char[width_pow2 * height_pow2 * img->bpp];

		for(int y = 0; y < height; y++)
			memcpy(data_pow2 + y * width_pow2 * img->bpp, img->data + y * width * img->bpp, width * img->bpp);
	}
	else
		data_pow2 = NULL;

	/*this->scalex = (float)width / (float)width_pow2;
	this->scaley = (float)height / (float)height_pow2;
	this->width = (float)width;
	this->height = (float)height;
	this->width_pow2 = (float)width_pow2;
	this->height_pow2 = (float)height_pow2;*/

	// Copy image to texture
	GLint pixelformat;
	switch(img->bpp)
	{
	case 1: pixelformat = GL_LUMINANCE; break;
	case 3: pixelformat = GL_RGB; break;
	case 4: pixelformat = GL_RGBA; break;
	default:
		REMOVE_ARRAY(data_pow2);
		return ERR(String("Invalid number of bits per pixel: ") << String(img->bpp * 8) << String(" (supported bpp = {8, 24, 32})"));
	}
	glTexImage2D(GL_TEXTURE_2D, 0, pixelformat, width_pow2, height_pow2, 0, pixelformat, GL_UNSIGNED_BYTE, data_pow2 ? data_pow2 : img->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Disable mip mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Disable mip mapping
	glEnable(GL_TEXTURE_2D); // Apply changes

	//EDIT: Create mip-chain if caps.genmipmaps == true

	// Cleanup
	REMOVE_ARRAY(data_pow2);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: MapPixelData()
// Desc: Lock texture and read raw pixel data
//-----------------------------------------------------------------------------
Result Texture::MapPixelData(bool read, bool write, UINT* stride, LPVOID* data)
{
	return ERR("Not implemented");
	/*Result rlt;

	D3D11_MAPPED_SUBRESOURCE mappedtex;
	CHKRESULT(wnd->devspec->devicecontext->Map(tex, D3D11CalcSubresource(0, 0, 1), (write == true && read == false) ? D3D11_MAP_WRITE_DISCARD  : (D3D11_MAP)((read * D3D11_MAP_READ) | (write * D3D11_MAP_WRITE)), 0, &mappedtex)); //EDIT: Let user choose whether to discard
	if(stride)
		*stride = mappedtex.RowPitch / width;
	*data = mappedtex.pData;

	return R_OK;*/
}

//-----------------------------------------------------------------------------
// Name: UnmapData()
// Desc: Unlock access to texture
//-----------------------------------------------------------------------------
void Texture::UnmapData()
{
	LOG("Not implemented");
	//wnd->devspec->devicecontext->Unmap(tex, D3D11CalcSubresource(0, 0, 1));
}

/*//-----------------------------------------------------------------------------
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
}*/

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Render to texture if it is a render target
//-----------------------------------------------------------------------------
void Texture::Render()
{
	LOG("Not implemented");
	/*if(rtview)
	{
//wnd->devspec->device->OMSetRenderTargets(1, &rtview, NULL);
wnd->devspec->devicecontext->ClearRenderTargetView(rtview, Color(0xFF00FFFF));
//this->Save("out.bmp", D3DX11_IFF_BMP);
//int abc = 0;
	}*/
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Texture::Release()
{
	wnd->textures.remove(this);

	//EDIT: Not implemented
	/*RELEASE(tex);
	RELEASE(srview);
	RELEASE(rtview);*/

	delete this;
}