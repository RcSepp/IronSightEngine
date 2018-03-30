#define RCP_GET_HIT_POS		0x1
#define RCP_GET_HIT_NML		0x2
#define RCP_GET_HIT_UV		0x4
#define RCP_GET_CURVATURE	0x8
class Surface;
class Material;
class Scene;
struct RayCastParams
{
	// Input
	Vector3 raypos, raydir;
	unsigned char flags;

	// Output
	float hitdist, hitcurv;
	const Surface* hitsfc;
	unsigned int hitidx; // Primitive index (i.e. face index for mesh hits)
	const Material* hitmat;
	Vector3 hitpos, hitnml, hittgt, hitbnm;
	Vector2 hituv;

	// Internal
	Vector3 transraypos, transraydir;

	// Misc
	LPVOID user;
const Scene* scene;

	// Culling
	Intersection::RAY_TRIANGLE_INTERSECTION_FUNCTION RayTriangle;
	Intersection::RAY_SPHERE_INTERSECTION_FUNCTION RaySphere;
	void SetCullMode(Intersection::CullMode cm)
	{
		switch(cm)
		{
		case Intersection::_CM_NONE:
			RayTriangle = Intersection::RayTriangleNC;
			RaySphere = Intersection::RaySphereBFC;
			break;
		case Intersection::CM_FRONT:
			RayTriangle = Intersection::RayTriangleFFC;
			RaySphere = Intersection::RaySphereFFC;
			break;
		case Intersection::CM_BACK:
			RayTriangle = Intersection::RayTriangleBFC;
			RaySphere = Intersection::RaySphereBFC;
		}
	}
	Intersection::CullMode GetCullMode()
	{
		if(RayTriangle == Intersection::RayTriangleFFC)
			return Intersection::CM_FRONT;
		if(RayTriangle == Intersection::RayTriangleBFC)
			return Intersection::CM_BACK;
		return Intersection::_CM_NONE;
	}
	bool IsCullingDisabled() {return RayTriangle == Intersection::RayTriangleNC;}
	bool IsFrontFaceCullingEnabled() {return RayTriangle == Intersection::RayTriangleFFC;}
	bool IsBackFaceCullingEnabled() {return RayTriangle == Intersection::RayTriangleBFC;}
	void FlipCullMode()
	{
		// Switch between front- and backface culling
		if(RayTriangle == Intersection::RayTriangleFFC)
		{
			RayTriangle = Intersection::RayTriangleBFC;
			RaySphere = Intersection::RaySphereBFC;
		}
		else
		{
			RayTriangle = Intersection::RayTriangleFFC;
			RaySphere = Intersection::RaySphereFFC;
		}
	}

	RayCastParams(Intersection::CullMode cm) {SetCullMode(cm);}
};
struct BatchRayCastParams
{
	// Input
	Vector3 tl, tr, bl, br;
	Size<int> resolution;
	Vector2 pixeloffset;
//const Matrix* invviewproj;

	// Internal
	struct Hit
	{
		float hitdist;
		Vector2 hituv;
		const Surface* hitsfc;
		unsigned int hitidx;
	}* hits;
	const Surface* currentsfc;
	const Matrix *currenttransform, *currentinvtransform;

	// Preallocated memory
	RayCastParams params;
	mutable Vector3 lerpvec1, lerpvec2;

	void SetRayDirs(const Matrix* viewmatrix, float fovx, float fovy)
	{
		Vec3Set(&tr, 1.0f * tan(fovx), 1.0f * tan(fovy), 1.0f);
		Vec3Normalize(&tr);
		Vec3Set(&tl, -tr.x,  tr.y, tr.z);
		Vec3Set(&bl, -tr.x, -tr.y, tr.z);
		Vec3Set(&br,  tr.x, -tr.y, tr.z);
		Vec3TransformNormal(&tr, &tr, viewmatrix);
		Vec3TransformNormal(&tl, &tl, viewmatrix);
		Vec3TransformNormal(&bl, &bl, viewmatrix);
		Vec3TransformNormal(&br, &br, viewmatrix);
	}
	Vector3* GetRayDir(Vector3* vResult, int x, int y) const
	{
		Vec3Lerp(&lerpvec1, &tl, &tr, ((float)x + pixeloffset.x) / (float)resolution.width);
		Vec3Lerp(&lerpvec2, &bl, &br, ((float)x + pixeloffset.x) / (float)resolution.width);
		Vec3Lerp(vResult, &lerpvec1, &lerpvec2, ((float)y + pixeloffset.y) / (float)resolution.height);
		return vResult;
	}

	BatchRayCastParams(Intersection::CullMode cm) : params(cm) {}
};
typedef void(*ONRAYHIT_CALLBACK)(RayCastParams& params, Point<int>& pos, float frameblendfactor);