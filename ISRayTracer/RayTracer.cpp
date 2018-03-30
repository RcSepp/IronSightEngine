#include "RayTracer.h"
#include "RaySpace.h"


//-----------------------------------------------------------------------------
// Name: CreateRayTracer()
// Desc: DLL API for creating an instance of RayTracer
//-----------------------------------------------------------------------------
RAYTRACER_EXTERN_FUNC LPRAYTRACER __cdecl CreateRayTracer()
{
	return new RayTracer();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void RayTracer::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: CreateScene()
// Desc: Create an empty scene
//-----------------------------------------------------------------------------
Result RayTracer::CreateScene(ISRayTracer::IScene** scene)
{
	Result rlt;

	*scene = NULL;

	Scene* newscene;
	CHKALLOC(newscene = new Scene(this));

	scenes.push_back(newscene);
	*scene = newscene;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateCamera()
// Desc: Create a camera
//-----------------------------------------------------------------------------
Result RayTracer::CreateCamera(ISRayTracer::ICamera** cam)
{
	Result rlt;

	*cam = NULL;

	Camera* newcam;
	CHKALLOC(newcam = new Camera(this));

	cameras.push_back(newcam);
	*cam = newcam;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreatePointLight()
// Desc: Create a point light source
//-----------------------------------------------------------------------------
Result RayTracer::CreatePointLight(ISRayTracer::IPointLight** light)
{
	Result rlt;

	*light = NULL;

	PointLight* newlight;
	CHKALLOC(newlight = new PointLight(this));

	lights.push_back(newlight);
	*light = newlight;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateDirLight()
// Desc: Create a directional light source
//-----------------------------------------------------------------------------
Result RayTracer::CreateDirLight(ISRayTracer::IDirLight** light)
{
	Result rlt;

	*light = NULL;

	DirLight* newlight;
	CHKALLOC(newlight = new DirLight(this));

	lights.push_back(newlight);
	*light = newlight;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateSpotLight()
// Desc: Create a spot light source
//-----------------------------------------------------------------------------
Result RayTracer::CreateSpotLight(ISRayTracer::ISpotLight** light)
{
	Result rlt;

	*light = NULL;

	SpotLight* newlight;
	CHKALLOC(newlight = new SpotLight(this));

	lights.push_back(newlight);
	*light = newlight;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateSphereSurface()
// Desc: Create a spherical object
//-----------------------------------------------------------------------------
Result RayTracer::CreateSphereSurface(const Vector3* pos, float radius, ISRayTracer::ISphereSurface** sfc)
{
	Result rlt;

	*sfc = NULL;

	SphereSurface* newsfc;
	CHKALLOC(newsfc = new SphereSurface(this, pos, radius));

	surfaces.push_back(newsfc);
	*sfc = newsfc;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateMeshSurface()
// Desc: Create a triangulated object
//-----------------------------------------------------------------------------
Result RayTracer::CreateMeshSurface(ISRayTracer::IMeshSurface** sfc)
{
	Result rlt;

	*sfc = NULL;

	MeshSurface* newsfc;
	CHKALLOC(newsfc = new MeshSurface(this));

	surfaces.push_back(newsfc);
	*sfc = newsfc;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateVMeshSurface()
// Desc: Create a voxelized object
//-----------------------------------------------------------------------------
Result RayTracer::CreateVMeshSurface(ISRayTracer::IVMeshSurface** sfc)
{
	Result rlt;

	*sfc = NULL;

	VMeshSurface* newsfc;
	CHKALLOC(newsfc = new VMeshSurface(this));

	surfaces.push_back(newsfc);
	*sfc = newsfc;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateSolidMaterial()
// Desc: Create an untextured material
//-----------------------------------------------------------------------------
Result RayTracer::CreateSolidMaterial(ISRayTracer::IMaterial** mat)
{
	Result rlt;

	*mat = NULL;

	Material* newmat;
	CHKALLOC(newmat = new Material(this));

	materials.push_back(newmat);
	*mat = newmat;

	return R_OK;
}

#pragma region Render()
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Creates and executes a render sequence
//-----------------------------------------------------------------------------
void RayTracer::Render(const ISRayTracer::IScene* scene, const RayTracingOptions& options)
{
	RenderSequence sequence(options, (const Scene*)scene);

if(options.IsBlendingRequired())
	sequence.blendbuffer = new Color[sequence.region.size.width * sequence.region.size.height];
else
	sequence.blendbuffer = NULL;

sequence.BeginFrame(1, 1);
if(sequence.blendbuffer) // If the blendbuffer is used
	memset(sequence.blendbuffer, 0, sequence.region.size.width * sequence.region.size.height * sizeof(Color)); // Clear the blendbuffer
sequence.RayTraceFrame(1.0f);
sequence.EndFrame(1, 1);


// Free memory
if(sequence.blendbuffer)
	delete[] sequence.blendbuffer;
}

//-----------------------------------------------------------------------------
// Name: RayTraceFrame()
// Desc: Calls RayTraceFrame(float, const Vector2*) with pixel offsets according to supersampling
//-----------------------------------------------------------------------------
void RayTracer::RenderSequence::RayTraceFrame(float frameblendfactor)
{
	Vector2 pixeloffset;
	switch(options.supersamplingfactor)
	{
	case 1:
		// Render the frame only once. Sample point is the center of the pixel
		Vec2Set(&pixeloffset, 0.5f, 0.5f);
		RayTraceFrame(frameblendfactor, &pixeloffset);
		break;

	case 4:
		// Render the frame four times. Sampel points follow the rotated grid pattern
		if(!options.quietmodeenabled) Result::PrintLogMessage("Pass 1/4");
		Vec2Set(&pixeloffset, 0.7f, 0.1f);
		RayTraceFrame(frameblendfactor / 4.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 2/4"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.9f, 0.7f);
		RayTraceFrame(frameblendfactor / 4.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 3/4"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.3f, 0.9f);
		RayTraceFrame(frameblendfactor / 4.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 4/4"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.1f, 0.3f);
		RayTraceFrame(frameblendfactor / 4.0f, &pixeloffset);
		break;

	case 16:
		// Render the frame 16 times. Sampel points follow the rotated grid pattern
		if(!options.quietmodeenabled) Result::PrintLogMessage("Pass 1/16");
		Vec2Set(&pixeloffset, 0.35f, 0.05f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 2/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.45f, 0.35f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 3/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.15f, 0.45f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 4/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.05f, 0.15f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;

		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 5/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.85f, 0.05f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 6/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.95f, 0.35f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 7/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.65f, 0.45f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 8/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.55f, 0.15f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;

		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 9/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.35f, 0.55f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 10/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.45f, 0.85f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 11/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.15f, 0.95f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 12/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.05f, 0.65f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;

		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 13/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.85f, 0.55f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 14/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.95f, 0.85f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 15/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.65f, 0.95f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		if(!options.keepalive) return;
		if(!options.quietmodeenabled) {Result::PrintLogMessage("Pass 16/16"); numpixelsdrawn = 0;}
		Vec2Set(&pixeloffset, 0.55f, 0.65f);
		RayTraceFrame(frameblendfactor / 16.0f, &pixeloffset);
		break;

	default:
		Result::PrintLogMessage(String("Unsupported supersampling factor: ") << String((int)options.supersamplingfactor));
	}
}

//-----------------------------------------------------------------------------
// Name: RayTraceFrame()
// Desc: Renders a frame with a given prmultiplied blendfactor and a given pixel offset
//-----------------------------------------------------------------------------
void RayTracer::RenderSequence::RayTraceFrame(float frameblendfactor, const Vector2* pixeloffset)
{
	// Prepare ray cast params for view rays
	RayCastParams params(Intersection::CM_BACK);
	params.scene = scene;
	params.raypos = scene->cam->pos;

	// Prepare ray cast params for view rays (batch mode)
	BatchRayCastParams batchparams(Intersection::CM_BACK);
	batchparams.params.scene = scene;
	batchparams.params.user = this;
	batchparams.params.raypos = scene->cam->pos;
	batchparams.resolution = scene->cam->size;
	batchparams.pixeloffset = *pixeloffset;
	batchparams.SetRayDirs(&scene->cam->transform, scene->cam->fovx, scene->cam->fovy);
/*Matrix matinvview, matproj, matviewproj;
MatrixInvLookAtRH(&matinvview, &scene->cam->pos, &scene->cam->target, &scene->cam->up);
MatrixPerspectiveFovRH(&matproj, 2.0f * scene->cam->fovy, (float)scene->cam->size.width / (float)scene->cam->size.height, 0.01f, 1000.0f);
matviewproj = matinvview * matproj;
batchparams.invviewproj = &matviewproj;*/

	Point<> pixelpos;
	int xfrom = region.x, xto = region.x + region.width;
	int yfrom = region.y, yto = region.y + region.height;
#define ITERATE_PIXELS(pixelpos) for(pixelpos.y = yfrom; pixelpos.y < yto; pixelpos.y++) for(pixelpos.x = xfrom; pixelpos.x < xto; pixelpos.x++)
	Color finalclr;

	switch(options.technique)
	{
	case RayTracingOptions::RTT_DEBUG:
		// Used only for debugging
		batchparams.params.flags = params.flags = RCP_GET_HIT_POS;
		ITERATE_PIXELS(pixelpos)
		{
			scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
			if(CastRay(params))
			{
				float foo = (params.hitpos.x / 4.0f) + 0.5f;
				foo = saturate(foo);
				ClrSet(&finalclr, foo, foo, foo);
				SetPixel(pixelpos, &finalclr, frameblendfactor);
			}
			if(!options.keepalive)
				return;
		}
		break;

	case RayTracingOptions::RTT_SHOW_COLORS:
		// Render only surface color
		batchparams.params.flags = params.flags = RCP_GET_HIT_UV;
		if(options.batchviewrays)
			scene->BatchCastRay(batchparams, sampler, frameblendfactor, &OnRayHit_RTT_SHOW_COLORS, NULL);
		else
		{
			Color ambdiffclr;
			ITERATE_PIXELS(pixelpos)
			{
				scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
				if(CastRay(params))
				{
					params.hitmat->Sample(&finalclr, &params.hituv, sampler);
					ClrClone(&ambdiffclr, &params.hitmat->ambientclr);
					ClrAdd(&ambdiffclr, &ambdiffclr, &params.hitmat->diffuseclr);
					ClrMul(&finalclr, &finalclr, &ambdiffclr);
					SetPixel(pixelpos, &finalclr, frameblendfactor);
				}
				if(!options.keepalive)
					return;
			}
		}
		break;

	case RayTracingOptions::RTT_SHOW_NORMALS:
		// Render only surface normals
		batchparams.params.flags = params.flags = RCP_GET_HIT_NML;
		if(options.batchviewrays)
			scene->BatchCastRay(batchparams, sampler, frameblendfactor, &OnRayHit_RTT_SHOW_NORMALS, NULL);
		else
			ITERATE_PIXELS(pixelpos)
			{
//if(pixelpos.x != 512 || pixelpos.y != 512)
//	continue;
				scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
				if(CastRay(params))
				{
					ClrSet(&finalclr, (params.hitnml.x + 1.0f) / 2.0f, (params.hitnml.y + 1.0f) / 2.0f, (params.hitnml.z + 1.0f) / 2.0f);
					SetPixel(pixelpos, &finalclr, frameblendfactor);
				}
				if(!options.keepalive)
					return;
			}
		break;

	case RayTracingOptions::RTT_SHOW_UV:
		// Render only surface normals
		batchparams.params.flags = params.flags = RCP_GET_HIT_UV;
		if(options.batchviewrays)
			scene->BatchCastRay(batchparams, sampler, frameblendfactor, &OnRayHit_RTT_SHOW_UV, NULL);
		else
			ITERATE_PIXELS(pixelpos)
			{
				scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
				if(CastRay(params))
				{
					ClrSet(&finalclr, params.hituv.x, params.hituv.y, 0.0f);
					SetPixel(pixelpos, &finalclr, frameblendfactor);
				}
				if(!options.keepalive)
					return;
			}
		break;

	case RayTracingOptions::RTT_SHOW_REFLECT_DIRS:
		// Render only surface reflection directions
		batchparams.params.flags = params.flags = RCP_GET_HIT_NML;
		if(options.batchviewrays)
			scene->BatchCastRay(batchparams, sampler, frameblendfactor, &OnRayHit_RTT_SHOW_REFLECT_DIRS, NULL);
		else
			ITERATE_PIXELS(pixelpos)
			{
				scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
				if(CastRay(params))
				{
					Vec3Reflect(&params.hitnml, &params.hitnml, &params.raydir);
					ClrSet(&finalclr, (params.hitnml.x + 1.0f) / 2.0f, (params.hitnml.y + 1.0f) / 2.0f, (params.hitnml.z + 1.0f) / 2.0f);
					SetPixel(pixelpos, &finalclr, frameblendfactor);
				}
				if(!options.keepalive)
					return;
			}
		break;

	case RayTracingOptions::RTT_SHOW_REFRACT_DIRS:
		// Render only surface refraction directions
		batchparams.params.flags = params.flags = RCP_GET_HIT_NML;
		if(options.batchviewrays)
			scene->BatchCastRay(batchparams, sampler, frameblendfactor, &OnRayHit_RTT_SHOW_REFRACT_DIRS, NULL);
		else
			ITERATE_PIXELS(pixelpos)
			{
				scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
				if(CastRay(params))
				{
					if(Vec3Refract(&params.hitpos, &params.hitnml, &params.raydir, REFRACT_IDX_AIR / params.hitmat->refraction))
						ClrSet(&finalclr, (params.hitpos.x + 1.0f) / 2.0f, (params.hitpos.y + 1.0f) / 2.0f, (params.hitpos.z + 1.0f) / 2.0f);
					else
						ClrSet(&finalclr, (params.hitnml.x + 1.0f) / 2.0f, (params.hitnml.y + 1.0f) / 2.0f, (params.hitnml.z + 1.0f) / 2.0f);
					SetPixel(pixelpos, &finalclr, frameblendfactor);
				}
				if(!options.keepalive)
					return;
			}
		break;

	case RayTracingOptions::RTT_SHOW_CURVATURE:
		// Render only surface curvature
		batchparams.params.flags = params.flags = RCP_GET_CURVATURE;
		if(options.batchviewrays)
			scene->BatchCastRay(batchparams, sampler, frameblendfactor, &OnRayHit_RTT_SHOW_CURVATURE, NULL);
		else
			ITERATE_PIXELS(pixelpos)
			{
				scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
				if(CastRay(params))
				{
					float transformed_curvature = pow(params.hitcurv, 0.3f); // Brighten areas of low curvature
					ClrSet(&finalclr, transformed_curvature, transformed_curvature, transformed_curvature);
					SetPixel(pixelpos, &finalclr, frameblendfactor);
				}
				else
					SetPixel(pixelpos, &scene->bgclr, frameblendfactor);
				if(!options.keepalive)
					return;
			}
		break;

	case RayTracingOptions::RTT_WHITTED_RT:
		// Trace rays from camera according to the Whitted Ray Tracing model
		batchparams.params.flags = params.flags = RCP_GET_HIT_POS | RCP_GET_HIT_UV | RCP_GET_HIT_NML;
		if(options.batchviewrays)
			scene->BatchCastRay(batchparams, sampler, frameblendfactor, &OnRayHit_RTT_WHITTED_RT, OnRayMiss_RTT_WHITTED_RT);
		else
			ITERATE_PIXELS(pixelpos)
			{
//if(pixelpos.x != 256 || pixelpos.y != 200)
//	continue;
				scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
				if(CastRay(params))
				{
					ClrSet(&finalclr, 0.0f, 0.0f, 0.0f);
					RecursiveWhittedRT(params, finalclr, 1.0f, scene->cam->maxbounces);
					params.raypos = scene->cam->pos; // Reset ray pos to camera pos
					SetPixel(pixelpos, &finalclr, frameblendfactor);
				}
				else
					SetPixel(pixelpos, &scene->bgclr, frameblendfactor);
				if(!options.keepalive)
					return;
			}
		break;

	case RayTracingOptions::RTT_VDA_GI:
		// Trace rays from camera according to the VDA global illumination ray tracing algorithm
		if(!options.rayspace)
		{
			Result::PrintLogMessage("Attempting to render VDA GI technique without rayspace");
			break;
		}
		batchparams.params.flags = params.flags = RCP_GET_HIT_UV | RCP_GET_HIT_NML;
		if(options.batchviewrays)
		{
			Result::PrintLogMessage("Batch view rays not supported for VDA GI");
			break;
		}

		ITERATE_PIXELS(pixelpos)
		{
			scene->cam->SetRayDir(&params.raydir, pixelpos, pixeloffset);
			if(CastRay(params))
			{
				ClrSet(&finalclr, 0.0f, 0.0f, 0.0f);
				((RaySpace*)options.rayspace)->SampleLattice(&finalclr, params, sampler);
				SetPixel(pixelpos, &finalclr, frameblendfactor);
			}
			else
				SetPixel(pixelpos, &scene->bgclr, frameblendfactor);
			if(!options.keepalive)
				return;
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: OnRayHit_..., OnRayMiss...
// Desc: Callbacks for batch view ray casting
//-----------------------------------------------------------------------------
void RayTracer::RenderSequence::OnRayHit_RTT_SHOW_NORMALS(RayCastParams& params, Point<int>& pos, float frameblendfactor)
{
	/*static*/ Color finalclr;
	ClrSet(&finalclr, (params.hitnml.x + 1.0f) / 2.0f, (params.hitnml.y + 1.0f) / 2.0f, (params.hitnml.z + 1.0f) / 2.0f);
	((RayTracer::RenderSequence*)params.user)->SetPixel(pos, &finalclr, frameblendfactor);
}
void RayTracer::RenderSequence::OnRayHit_RTT_SHOW_UV(RayCastParams& params, Point<int>& pos, float frameblendfactor)
{
	/*static*/ Color finalclr;
	ClrSet(&finalclr, params.hituv.x, params.hituv.y, 0.0f);
	((RayTracer::RenderSequence*)params.user)->SetPixel(pos, &finalclr, frameblendfactor);
}
void RayTracer::RenderSequence::OnRayHit_RTT_SHOW_COLORS(RayCastParams& params, Point<int>& pos, float frameblendfactor)
{
	/*static*/ Color finalclr, ambdiffclr;
	params.hitmat->Sample(&finalclr, &params.hituv, ((RayTracer::RenderSequence*)params.user)->sampler);
	ClrClone(&ambdiffclr, &params.hitmat->ambientclr);
	ClrAdd(&ambdiffclr, &ambdiffclr, &params.hitmat->diffuseclr);
	ClrMul(&finalclr, &finalclr, &ambdiffclr);
	((RayTracer::RenderSequence*)params.user)->SetPixel(pos, &finalclr, frameblendfactor);
}
void RayTracer::RenderSequence::OnRayHit_RTT_SHOW_REFLECT_DIRS(RayCastParams& params, Point<int>& pos, float frameblendfactor)
{
	/*static*/ Color finalclr;
	Vec3Reflect(&params.hitnml, &params.hitnml, &params.raydir);
	ClrSet(&finalclr, (params.hitnml.x + 1.0f) / 2.0f, (params.hitnml.y + 1.0f) / 2.0f, (params.hitnml.z + 1.0f) / 2.0f);
	((RayTracer::RenderSequence*)params.user)->SetPixel(pos, &finalclr, frameblendfactor);
}
void RayTracer::RenderSequence::OnRayHit_RTT_SHOW_REFRACT_DIRS(RayCastParams& params, Point<int>& pos, float frameblendfactor)
{
	/*static*/ Color finalclr;
	if(Vec3Refract(&params.hitpos, &params.hitnml, &params.raydir, REFRACT_IDX_AIR / params.hitmat->refraction))
		ClrSet(&finalclr, (params.hitpos.x + 1.0f) / 2.0f, (params.hitpos.y + 1.0f) / 2.0f, (params.hitpos.z + 1.0f) / 2.0f);
	else
		ClrSet(&finalclr, (params.hitnml.x + 1.0f) / 2.0f, (params.hitnml.y + 1.0f) / 2.0f, (params.hitnml.z + 1.0f) / 2.0f);
	((RayTracer::RenderSequence*)params.user)->SetPixel(pos, &finalclr, frameblendfactor);
}
void RayTracer::RenderSequence::OnRayHit_RTT_SHOW_CURVATURE(RayCastParams& params, Point<int>& pos, float frameblendfactor)
{
	/*static*/ Color finalclr;
	float transformed_curvature = pow(params.hitcurv, 0.3f); // Brighten areas of low curvature
	ClrSet(&finalclr, transformed_curvature, transformed_curvature, transformed_curvature);
	((RayTracer::RenderSequence*)params.user)->SetPixel(pos, &finalclr, frameblendfactor);
}
void RayTracer::RenderSequence::OnRayHit_RTT_WHITTED_RT(RayCastParams& params, Point<int>& pos, float frameblendfactor)
{
	/*static*/ Color finalclr;
	ClrSet(&finalclr, 0.0f, 0.0f, 0.0f);
	((RayTracer::RenderSequence*)params.user)->RecursiveWhittedRT(params, finalclr, 1.0f, params.scene->cam->maxbounces);
	params.raypos = params.scene->cam->pos; // Reset ray pos to camera pos
	((RayTracer::RenderSequence*)params.user)->SetPixel(pos, &finalclr, frameblendfactor);
}
void RayTracer::RenderSequence::OnRayMiss_RTT_WHITTED_RT(RayCastParams& params, Point<int>& pos, float frameblendfactor)
{
	((RayTracer::RenderSequence*)params.user)->SetPixel(pos, &params.scene->bgclr, frameblendfactor);
}

#ifndef _WIN32
timespec timediff(timespec start, timespec end)
{
	timespec temp;
	if((end.tv_nsec - start.tv_nsec) < 0)
	{
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
	else
	{
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}
#endif
void RayTracer::RenderSequence::BeginFrame(int frame, int numframes)
{
	if(!options.quietmodeenabled)
	{
		Result::PrintLogMessage(String("Rendering frame ") << String(frame) << String(" of ") << String(numframes));

		// Reset pixel counter (only for logging number of pixels)
		numpixelsdrawn = 0;

		// Reset frame timer (only for logging render time)
#ifdef _WIN32
		QueryPerformanceFrequency(&timerfreq);
		QueryPerformanceCounter(&framestarttime);
#else
		clock_getres(CLOCK_REALTIME, &timerfreq);
		clock_gettime(CLOCK_REALTIME, &framestarttime);
#endif
	}
}
void RayTracer::RenderSequence::EndFrame(int frame, int numframes)
{
	if(!options.quietmodeenabled)
	{
		// Log frame time
#ifdef _WIN32
		QueryPerformanceCounter(&frameendtime);
		Result::PrintLogMessage(String("The frame took ") << String((int)((frameendtime.QuadPart - framestarttime.QuadPart) * 1000 / timerfreq.QuadPart)) << String("ms to render."));
#else
		clock_gettime(CLOCK_REALTIME, &frameendtime);
		timespec frametime = timediff(framestarttime, frameendtime);
		printf("The frame took %ims to render.\n", (int)(frametime.tv_sec * 1000 + frametime.tv_nsec / 1000000));
#endif
	}

	// Call user callback
	if(options.onframedone)
		options.onframedone(frame, numframes);
}

void RayTracer::RenderSequence::SetPixel(const Point<>& pos, const Color* clr, float pixelblendfactor)
{
	if(!options.onsetpixel)
		return;

	if(blendbuffer && pixelblendfactor < 1.0f)
	{
		Color* finalclr = &blendbuffer[(pos.x - region.location.x) + (pos.y - region.location.y) * region.size.width];
		ClrAddScaled(finalclr, finalclr, clr, pixelblendfactor);
		options.onsetpixel(pos, finalclr->ToDWORD());
	}
	else
		options.onsetpixel(pos, clr->ToDWORD());

	// Log number of pixels every 10000 pixels
	numpixelsdrawn++;
	if(!options.quietmodeenabled && (numpixelsdrawn / 10000) * 10000 == numpixelsdrawn)
		Result::PrintLogMessage(String(numpixelsdrawn) << String(" pixels drawn"));
}
bool RayTracer::RenderSequence::CastRay(RayCastParams& params)
{
	if(options.oncastray)
	{
		params.flags |= RCP_GET_HIT_POS;
		bool result = scene->CastRay(params, sampler);
		if(result)
			options.oncastray((float*)&params.raypos, (float*)&params.hitpos);
		else
		{
			Vector3 rayendpos;
			Vec3Scale(&rayendpos, &params.raydir, 100.0f);
			options.oncastray((float*)&params.raypos, (float*)&rayendpos);
		}
		return result;
	}
	else
		return scene->CastRay(params, sampler);
}

void RayTracer::RenderSequence::RecursiveWhittedRT(RayCastParams& params, Color& finalclr, float factor, int maxbounces)
{
	const Surface* hitsfc = params.hitsfc;
	const Material* hitmat = params.hitmat;

	if(params.IsBackFaceCullingEnabled()) // Only illuminate rays outside solid objects
	{
		/*static*/ Color surfaceclr;
		/*static*/ RayCastParams newparams(params.GetCullMode());
		newparams.scene = params.scene;

		// Ambient light = scene_ambient_color * surface_color * ka
		hitmat->Sample(&surfaceclr, &params.hituv, sampler);
		ClrAddScaled(&finalclr, &finalclr, &scene->ambientclr, &surfaceclr, &hitmat->ambientclr, factor * hitmat->absorbance);

		Vector3* V = &params.raydir;
		Vector3* N = &params.hitnml;

		// >>> Cast rays towards each light source (shadow ray)

		newparams.raypos = params.hitpos;
		newparams.flags = 0;
		std::list<Light*>::const_iterator iter;
		float exposure;
		LIST_FOREACH(scene->lights, iter)
		{
			if((exposure = (*iter)->GetExposure(&params.hitpos, newparams)) != 0.0f)
			{
				Vector3* L = &newparams.raydir;
				Vector3 LR;
				Vec3Reflect(&LR, &params.hitnml, L);

				// Specular light = light_color * surface_color * ks * dot(view_dir, light_reflect_dir)
				float dot = pow(saturate(Vec3Dot(V, &LR)), hitmat->specularpwr);
				ClrAddScaled(&finalclr, &finalclr, (*iter)->lightcolor, &surfaceclr, &hitmat->specularclr, dot * exposure * factor * hitmat->absorbance);

				// Diffuse light = light_color * surface_color * kd * dot(light_dir, surface_normal)
				dot = saturate(Vec3Dot(L, N));
				ClrAddScaled(&finalclr, &finalclr, (*iter)->lightcolor, &surfaceclr, &hitmat->diffuseclr, dot * exposure * factor * hitmat->absorbance);
			}
		}
	}

	if(!maxbounces)
		return;

// Debug: show hit normal
//options.oncastray((float*)&params.hitpos, (float*)&(params.hitpos + params.hitnml));

	// Store some ray cast parameters that would get overwritten during the refraction trace
	Vector3 raydir = params.raydir, hitpos = params.hitpos, hitnml = params.hitnml;

	// >>> Cast ray towards refraction direction (transmitted ray)

	float reflectance = hitmat->reflectance;
	if(hitmat->transmittance)
	{
		params.raypos = hitpos;
		if(options.usethinrefractions)
		{
			if(Vec3Refract(&params.raydir, &hitnml, &params.raydir, params.IsFrontFaceCullingEnabled() ?
			   hitmat->refraction / REFRACT_IDX_AIR : REFRACT_IDX_AIR / hitmat->refraction))
			{
				// The ray source passes culling. Move ray forward to avoid self-casting.
				Vector3 voffset;
				Vec3Add(&params.raypos, &params.raypos, Vec3Scale(&voffset, &params.raydir, 10.0f * FLOAT_TOLERANCE)); // 1x FLOAT_TOLERANCE may not be enough when casting through steep surfaces

				if(CastRay(params))
					RecursiveWhittedRT(params, finalclr, factor * hitmat->transmittance, maxbounces - 1);
				else
					ClrAddScaled(&finalclr, &finalclr, &scene->bgclr, factor * hitmat->transmittance);
			}
			else // If total internal reflection occures
				reflectance += hitmat->transmittance; // Add transmittance factor to reflectance factor
		}
		else
		{
			if(Vec3Refract(&params.raydir, &hitnml, &params.raydir, params.IsFrontFaceCullingEnabled() ?
			   hitmat->refraction / REFRACT_IDX_AIR : REFRACT_IDX_AIR / hitmat->refraction))
			{
				params.FlipCullMode(); // Ray entered/exited object. Switch between frontface culling (for rays inside objects) and backface culling (for rays outside objects)
				if(CastRay(params))
					RecursiveWhittedRT(params, finalclr, factor * hitmat->transmittance, maxbounces - 1);
				else
					ClrAddScaled(&finalclr, &finalclr, &scene->bgclr, factor * hitmat->transmittance);
				params.FlipCullMode(); // Ray entered/exited object. Switch between frontface culling (for rays inside objects) and backface culling (for rays outside objects)
			}
			else // If total internal reflection occures
				reflectance += hitmat->transmittance; // Add transmittance factor to reflectance factor
		}
	}

	// >>> Cast ray towards reflection direction (reflected ray)

	if(reflectance)
	{
		params.raypos = hitpos;
		Vec3Reflect(&params.raydir, &hitnml, &raydir);
		if(CastRay(params))
			RecursiveWhittedRT(params, finalclr, factor * reflectance, maxbounces - 1);
		else
			ClrAddScaled(&finalclr, &finalclr, &scene->bgclr, factor * reflectance);
	}
}
#pragma endregion

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void RayTracer::Release()
{
	while(scenes.size())
		scenes.front()->Release();
	while(cameras.size())
		cameras.front()->Release();

	delete this;
}