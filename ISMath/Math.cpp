#include "ISMath.h"
#include <math.h>
#include <cstring>

float saturate(float val)
{
	return val < 0.0f ? 0.0f : (val > 1.0f ? 1.0f : val);
}

Vector2* Vec2Add(Vector2* vout, const Vector2* v0, const Vector2* v1)
{
	vout->x = v0->x + v1->x;
	vout->y = v0->y + v1->y;
	return vout;
}
Vector2* Vec2Sub(Vector2* vout, const Vector2* v0, const Vector2* v1)
{
	vout->x = v0->x - v1->x;
	vout->y = v0->y - v1->y;
	return vout;
}
Vector2* Vec2Scale(Vector2* vout, const Vector2* vin, float scale)
{
	vout->x = vin->x * scale;
	vout->y = vin->y * scale;
	return vout;
}
Vector2* Vec2AddScaled(Vector2* vout, const Vector2* v0, const Vector2* v1, float v1scale)
{
	vout->x = v0->x + v1->x * v1scale;
	vout->y = v0->y + v1->y * v1scale;
	return vout;
}

Vector3* Vec3Add(Vector3* vout, const Vector3* v0, const Vector3* v1)
{
	vout->x = v0->x + v1->x;
	vout->y = v0->y + v1->y;
	vout->z = v0->z + v1->z;
	return vout;
}
Vector3* Vec3Sub(Vector3* vout, const Vector3* v0, const Vector3* v1)
{
	vout->x = v0->x - v1->x;
	vout->y = v0->y - v1->y;
	vout->z = v0->z - v1->z;
	return vout;
}
Vector3* Vec3Scale(Vector3* vout, const Vector3* vin, float scale)
{
	vout->x = vin->x * scale;
	vout->y = vin->y * scale;
	vout->z = vin->z * scale;
	return vout;
}
Vector3* Vec3AddScaled(Vector3* vout, const Vector3* v0, const Vector3* v1, float v1scale)
{
	vout->x = v0->x + v1->x * v1scale;
	vout->y = v0->y + v1->y * v1scale;
	vout->z = v0->z + v1->z * v1scale;
	return vout;
}

// EDIT: The following functions do not take alpha values into account
Color* ClrAdd(Color* cout, const Color* c0, const Color* c1)
{
	cout->r = c0->r + c1->r;
	cout->g = c0->g + c1->g;
	cout->b = c0->b + c1->b;
	return cout;
}
Color* ClrSub(Color* cout, const Color* c0, const Color* c1)
{
	cout->r = c0->r - c1->r;
	cout->g = c0->g - c1->g;
	cout->b = c0->b - c1->b;
	return cout;
}
Color* ClrMul(Color* cout, const Color* c0, const Color* c1)
{
	cout->r = c0->r * c1->r;
	cout->g = c0->g * c1->g;
	cout->b = c0->b * c1->b;
	return cout;
}
Color* ClrScale(Color* cout, const Color* cin, float scale)
{
	cout->r = cin->r * scale;
	cout->g = cin->g * scale;
	cout->b = cin->b * scale;
	return cout;
}
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, float c1scale)
{
	cout->r = c0->r + c1->r * c1scale;
	cout->g = c0->g + c1->g * c1scale;
	cout->b = c0->b + c1->b * c1scale;
	return cout;
}
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, const Color* c1scale0, float c1scale1)
{
	cout->r = c0->r + c1->r * c1scale0->r * c1scale1;
	cout->g = c0->g + c1->g * c1scale0->g * c1scale1;
	cout->b = c0->b + c1->b * c1scale0->b * c1scale1;
	return cout;
}
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, const Color* c1scale0, const Color* c1scale1, float c1scale2)
{
	cout->r = c0->r + c1->r * c1scale0->r * c1scale1->r * c1scale2;
	cout->g = c0->g + c1->g * c1scale0->g * c1scale1->g * c1scale2;
	cout->b = c0->b + c1->b * c1scale0->b * c1scale1->b * c1scale2;
	return cout;
}

Color3* ClrAdd(Color3* cout, const Color3* c0, const Color3* c1)
{
	cout->r = c0->r + c1->r;
	cout->g = c0->g + c1->g;
	cout->b = c0->b + c1->b;
	return cout;
}
Color3* ClrSub(Color3* cout, const Color3* c0, const Color3* c1)
{
	cout->r = c0->r - c1->r;
	cout->g = c0->g - c1->g;
	cout->b = c0->b - c1->b;
	return cout;
}
Color3* ClrMul(Color3* cout, const Color3* c0, const Color3* c1)
{
	cout->r = c0->r * c1->r;
	cout->g = c0->g * c1->g;
	cout->b = c0->b * c1->b;
	return cout;
}
Color3* ClrScale(Color3* cout, const Color3* cin, float scale)
{
	cout->r = cin->r * scale;
	cout->g = cin->g * scale;
	cout->b = cin->b * scale;
	return cout;
}
Color3* ClrAddScaled(Color3* cout, const Color3* c0, const Color3* c1, float c1scale)
{
	cout->r = c0->r + c1->r * c1scale;
	cout->g = c0->g + c1->g * c1scale;
	cout->b = c0->b + c1->b * c1scale;
	return cout;
}
Color3* ClrAddScaled(Color3* cout, const Color3* c0, const Color3* c1, const Color3* c1scale0, float c1scale1)
{
	cout->r = c0->r + c1->r * c1scale0->r * c1scale1;
	cout->g = c0->g + c1->g * c1scale0->g * c1scale1;
	cout->b = c0->b + c1->b * c1scale0->b * c1scale1;
	return cout;
}
Color3* ClrAddScaled(Color3* cout, const Color3* c0, const Color3* c1, const Color3* c1scale0, const Color3* c1scale1, float c1scale2)
{
	cout->r = c0->r + c1->r * c1scale0->r * c1scale1->r * c1scale2;
	cout->g = c0->g + c1->g * c1scale0->g * c1scale1->g * c1scale2;
	cout->b = c0->b + c1->b * c1scale0->b * c1scale1->b * c1scale2;
	return cout;
}

//-----------------------------------------------------------------------------
// Name: VecSet
// Desc: Set vector components
//-----------------------------------------------------------------------------
Vector2* Vec2Set(Vector2* vout, float x, float y)
{
	vout->x = x;
	vout->y = y;
	return vout;
}
Vector2* Vec2Set(Vector2* vout, const float* xy)
{
	vout->x = xy[0];
	vout->y = xy[1];
	return vout;
}
Vector3* Vec3Set(Vector3* vout, float x, float y, float z)
{
	vout->x = x;
	vout->y = y;
	vout->z = z;
	return vout;
}
Vector3* Vec3Set(Vector3* vout, const float* xyz)
{
	vout->x = xyz[0];
	vout->y = xyz[1];
	vout->z = xyz[2];
	return vout;
}
Vector4* Vec4Set(Vector4* vout, float x, float y, float z, float w)
{
	vout->x = x;
	vout->y = y;
	vout->z = z;
	vout->w = w;
	return vout;
}
Vector4* Vec4Set(Vector4* vout, const float* xyzw)
{
	vout->x = xyzw[0];
	vout->y = xyzw[1];
	vout->z = xyzw[2];
	vout->w = xyzw[3];
	return vout;
}
Color* ClrSet(Color* cout, float r, float g, float b, float a)
{
	cout->r = r;
	cout->g = g;
	cout->b = b;
	cout->a = a;
	return cout;
}
Color* ClrSet(Color* cout, const float* rgba)
{
	cout->r = rgba[0];
	cout->g = rgba[1];
	cout->b = rgba[2];
	cout->a = rgba[3];
	return cout;
}
Color3* ClrSet(Color3* cout, float r, float g, float b)
{
	cout->r = r;
	cout->g = g;
	cout->b = b;
	return cout;
}
Color3* ClrSet(Color3* cout, const float* rgb)
{
	cout->r = rgb[0];
	cout->g = rgb[1];
	cout->b = rgb[2];
	return cout;
}
Quaternion* QuaternionSet(Quaternion* qout, float x, float y, float z, float w)
{
	qout->x = x;
	qout->y = y;
	qout->z = z;
	qout->w = w;
	return qout;
}
Quaternion* QuaternionSet(Quaternion* qout, const float* xyzw)
{
	qout->x = xyzw[0];
	qout->y = xyzw[1];
	qout->z = xyzw[2];
	qout->z = xyzw[3];
	return qout;
}

Color* ClrClone(Color* cout, const Color* c0)
{
	cout->r = c0->r;
	cout->g = c0->g;
	cout->b = c0->b;
	cout->a = c0->a;
	return cout;
}
Color3* ClrClone(Color3* cout, const Color3* c0)
{
	cout->r = c0->r;
	cout->g = c0->g;
	cout->b = c0->b;
	return cout;
}

//-----------------------------------------------------------------------------
// Name: VecDot
// Desc: Dot product
//-----------------------------------------------------------------------------
float Vec2Dot(const Vector2 *v0, const Vector2 *v1)
{
	return v0->x * v1->x + v0->y * v1->y;
}
float Vec3Dot(const Vector3 *v0, const Vector3 *v1)
{
	return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}
float Vec4Dot(const Vector4 *v0, const Vector4 *v1)
{
	return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z + v0->w * v1->w;
}

//-----------------------------------------------------------------------------
// Name: Vec3Cross
// Desc: Cross product
//-----------------------------------------------------------------------------
Vector3* Vec3Cross(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1)
{
	Vector3 v(v0->y * v1->z - v0->z * v1->y,
			  v0->z * v1->x - v0->x * v1->z,
			  v0->x * v1->y - v0->y * v1->x);
	*vResult = v;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3Reflect
// Desc: Compute the reflection of a vector v on a surface with normal n (r = v - 2 * (n . v) * n).
//-----------------------------------------------------------------------------
Vector3* Vec3Reflect(Vector3 *vResult, const Vector3 *n, const Vector3 *v)
{
	Vec3Scale(vResult, n, 2.0f * Vec3Dot(n, v)); // vResult = 2 * (n . v) * n
	Vec3Sub(vResult, v, vResult); // vResult = v - 2 * (n . v) * n
	return vResult;
}

Vector3* Vec3Refract(Vector3 *vResult, const Vector3 *n, const Vector3 *v, float ratio)
{
/*float cosv = Vec3Dot(v, n);
float k = 1.0 - ratio * ratio * (1.0 - cosv*cosv);
if (k < 0.0)
	return 0;

Vector3 t;
Vec3Scale(&t, v, ratio);
Vec3Scale(vResult, n, ratio * cosv - sqrt(fabs(k)));
Vec3Sub(vResult, &t, vResult);
Vec3Normalize(vResult);
return vResult;*/

	float costheta1 = -Vec3Dot(v, n);
	float costheta2sq = 1.0f - ratio*ratio * (1.0f - costheta1*costheta1);
if(costheta2sq < 0.0f)
	return 0;
	Vector3 t;
	Vec3Scale(&t, v, ratio);
	Vec3Scale(vResult, n, ratio * costheta1 - sqrt(costheta2sq));
	Vec3Add(vResult, vResult, &t);
	Vec3Normalize(vResult);
	return vResult;

	//Vec3Scale(vResult, n, 2.0f * Vec3Dot(n, v)); // vResult = 2 * (n . v) * n
	//Vec3Sub(vResult, v, vResult); // vResult = v - 2 * (n . v) * n
	/*Vec3Scale(vResult, n, ri2 - ri1);
	Vec3Add(vResult, vResult, v);
	Vec3Normalize(vResult);
	return vResult;*/

/*float cosphi1 = -Vec3Dot(n, v);
float cosphi2 = 1.0f - ratio*ratio * (1.0f - cosphi1*cosphi1);
if (cosphi2 < 0.0f)
	return 0;
cosphi2 = sqrt(cosphi2);
Vector3 t;
Vec3Scale(&t, v, ratio);
Vec3Scale(vResult, n, ratio * cosphi1 + cosphi2);
Vec3Add(vResult, vResult, &t);
return vResult;*/
}
/*{
  float cosv = -dot(v, n);
  float cost2 = 1.0f - ratio * ratio * (1.0f - cosv*cosv);
  float3 t = ratio * v + ((ratio * cosv - sqrt(fabs(cost2))) * n);
  return t * (float3)(cost2 > 0);
}*/

//-----------------------------------------------------------------------------
// Name: GetUV
// Desc: Get the barycentric coordinates of a triangles point in 2D space
//-----------------------------------------------------------------------------
Vector2* GetUV(Vector2 t0, Vector2 t1, Vector2 t2, Vector2 tP, Vector2 *uv)
{
	if(!(t2.y * t0.x - t2.y * t1.x - t0.y * t0.x + t0.y * t1.x - //BUGFIX: ERROR UNKNOWN
		 t2.x * t0.y + t2.x * t1.y + t0.x * t0.y - t0.x * t1.y)) //BUGFIX: ERROR UNKNOWN
	{ //BUGFIX: ERROR UNKNOWN
		uv->x = uv->y = 0.0f; //BUGFIX: ERROR UNKNOWN
		return uv; //BUGFIX: ERROR UNKNOWN
	} //BUGFIX: ERROR UNKNOWN
	uv->y = ((t0.x - tP.x) * (t0.y - t1.y) - (t0.y - tP.y) * (t0.x - t1.x)) /
			(t2.y * t0.x - t2.y * t1.x - t0.y * t0.x + t0.y * t1.x -
			 t2.x * t0.y + t2.x * t1.y + t0.x * t0.y - t0.x * t1.y);
	if(fabs(t0.x - t1.x) > fabs(t0.y - t1.y))
		uv->x = (t0.x - t0.x * uv->y + t2.x * uv->y - tP.x) / (t0.x - t1.x);
	else
		uv->x = (t0.y - t0.y * uv->y + t2.y * uv->y - tP.y) / (t0.y - t1.y);
	return uv;
}

//-----------------------------------------------------------------------------
// Name: VecLength
// Desc: Return the length of the given vector
//-----------------------------------------------------------------------------
float Vec2Length(const Vector2 *v0)
{
	return sqrt(v0->x*v0->x + v0->y*v0->y);
}
float Vec2Length(const float x, const float y)
{
	return sqrt(x*x + y*y);
}
float Vec3Length(const Vector3 *v0)
{
	return sqrt(v0->x*v0->x + v0->y*v0->y + v0->z*v0->z);
}
float Vec3Length(const float x, const float y, const float z)
{
	return sqrt(x*x + y*y + z*z);
}
float Vec4Length(const Vector4 *v0)
{
	return sqrt(v0->x*v0->x + v0->y*v0->y + v0->z*v0->z + v0->w*v0->w);
}
float Vec4Length(const float x, const float y, const float z, const float w)
{
	return sqrt(x*x + y*y + z*z + w*w);
}

//-----------------------------------------------------------------------------
// Name: VecLengthSq
// Desc: Return the squared length of the given vector
//-----------------------------------------------------------------------------
float Vec2LengthSq(const Vector2 *v0)
{
	return v0->x*v0->x + v0->y*v0->y;
}
float Vec2LengthSq(const float x, const float y)
{
	return x*x + y*y;
}
float Vec3LengthSq(const Vector3 *v0)
{
	return v0->x*v0->x + v0->y*v0->y + v0->z*v0->z;
}
float Vec3LengthSq(const float x, const float y, const float z)
{
	return x*x + y*y + z*z;
}
float Vec4LengthSq(const Vector4 *v0)
{
	return v0->x*v0->x + v0->y*v0->y + v0->z*v0->z + v0->w*v0->w;
}
float Vec4LengthSq(const float x, const float y, const float z, const float w)
{
	return x*x + y*y + z*z + w*w;
}

//-----------------------------------------------------------------------------
// Name: Vec3Inverse
// Desc: Return an inversed form of the given vector
//-----------------------------------------------------------------------------
Vector2* Vec2Inverse(Vector2* vResult, const Vector2* v0)
{
	vResult->x = -v0->x;
	vResult->y = -v0->y;
	return vResult;
}
Vector3* Vec3Inverse(Vector3* vResult, const Vector3* v0)
{
	vResult->x = -v0->x;
	vResult->y = -v0->y;
	vResult->z = -v0->z;
	return vResult;
}
Vector4* Vec4Inverse(Vector4* vResult, const Vector4* v0)
{
	vResult->x = -v0->x;
	vResult->y = -v0->y;
	vResult->z = -v0->z;
	vResult->w = -v0->w;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3Invert
// Desc: Invert the vector
//-----------------------------------------------------------------------------
Vector2* Vec2Invert(Vector2* v0)
{
	v0->x = -v0->x;
	v0->y = -v0->y;
	return v0;
}
Vector3* Vec3Invert(Vector3* v0)
{
	v0->x = -v0->x;
	v0->y = -v0->y;
	v0->z = -v0->z;
	return v0;
}
Vector4* Vec4Invert(Vector4* v0)
{
	v0->x = -v0->x;
	v0->y = -v0->y;
	v0->z = -v0->z;
	v0->w = -v0->w;
	return v0;
}

//-----------------------------------------------------------------------------
// Name: Vec3Normalize
// Desc: Return a normalized form of the given vector
//-----------------------------------------------------------------------------
Vector2* Vec2Normalize(Vector2 *vResult, const Vector2 *v0)
{
	float len = sqrt(v0->x*v0->x + v0->y*v0->y);
	Vector2 v(v0->x / len,v0->y / len);
	*vResult = v;
	return vResult;
}
Vector2* Vec2Normalize(Vector2 *v0)
{
	float len = sqrt(v0->x*v0->x + v0->y*v0->y);
	*v0 /= len;
	return v0;
}
Vector3* Vec3Normalize(Vector3 *vResult, const Vector3 *v0)
{
	float len = sqrt(v0->x*v0->x + v0->y*v0->y + v0->z*v0->z);
	Vector3 v(v0->x / len,v0->y / len, v0->z / len);
	*vResult = v;
	return vResult;
}
Vector3* Vec3Normalize(Vector3 *v0)
{
	float len = sqrt(v0->x*v0->x + v0->y*v0->y + v0->z*v0->z);
	*v0 /= len;
	return v0;
}
Vector4* Vec4Normalize(Vector4 *vResult, const Vector4 *v0)
{
	float len = sqrt(v0->x*v0->x + v0->y*v0->y + v0->z*v0->z + v0->w*v0->w);
	Vector4 v(v0->x / len,v0->y / len, v0->z / len, v0->w / len);
	*vResult = v;
	return vResult;
}
Vector4* Vec4Normalize(Vector4 *v0)
{
	float len = sqrt(v0->x*v0->x + v0->y*v0->y + v0->z*v0->z + v0->w*v0->w);
	*v0 /= len;
	return v0;
}

//-----------------------------------------------------------------------------
// Name: Vec3Transform
// Desc: Transform by the given matrix assuming v0.w = 1.0f
//-----------------------------------------------------------------------------
Vector4* Vec3Transform(Vector4 *vResult, const Vector3 *v0, const Matrix *m0)
{
	Vector4 v(v0->x * m0->_11 + v0->y * m0->_21 + v0->z * m0->_31 + m0->_41,
			  v0->x * m0->_12 + v0->y * m0->_22 + v0->z * m0->_32 + m0->_42,
			  v0->x * m0->_13 + v0->y * m0->_23 + v0->z * m0->_33 + m0->_43,
			  v0->x * m0->_14 + v0->y * m0->_24 + v0->z * m0->_34 + m0->_44);
	*vResult = v;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3TransformCoord
// Desc: Transform by the given matrix assuming v0.w = 1.0f and project the result back into w = 1.0f
//-----------------------------------------------------------------------------
Vector3* Vec3TransformCoord(Vector3 *vResult, const Vector3 *v0, const Matrix *m0)
{
	float w = v0->x * m0->_14 + v0->y * m0->_24 + v0->z * m0->_34 + m0->_44;
	Vector3 v((v0->x * m0->_11 + v0->y * m0->_21 + v0->z * m0->_31 + m0->_41) / w,
			  (v0->x * m0->_12 + v0->y * m0->_22 + v0->z * m0->_32 + m0->_42) / w,
			  (v0->x * m0->_13 + v0->y * m0->_23 + v0->z * m0->_33 + m0->_43) / w);
	*vResult = v;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3TransformNormal
// Desc: Transform by the given matrix ignoring all w components
//-----------------------------------------------------------------------------
Vector3* Vec3TransformNormal(Vector3 *vResult, const Vector3 *v0, const Matrix *m0)
{
	Vector3 v(v0->x * m0->_11 + v0->y * m0->_21 + v0->z * m0->_31,
			  v0->x * m0->_12 + v0->y * m0->_22 + v0->z * m0->_32,
			  v0->x * m0->_13 + v0->y * m0->_23 + v0->z * m0->_33);
	*vResult = v;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec4Transform
// Desc: Transform by the given matrix assuming v0.w = 1.0f
//-----------------------------------------------------------------------------
Vector4* Vec4Transform(Vector4 *vResult, const Vector4 *v0, const Matrix *m0)
{
	Vector4 v(v0->x * m0->_11 + v0->y * m0->_21 + v0->z * m0->_31 + v0->w * m0->_41,
			  v0->x * m0->_12 + v0->y * m0->_22 + v0->z * m0->_32 + v0->w * m0->_42,
			  v0->x * m0->_13 + v0->y * m0->_23 + v0->z * m0->_33 + v0->w * m0->_43,
			  v0->x * m0->_14 + v0->y * m0->_24 + v0->z * m0->_34 + v0->w * m0->_44);
	*vResult = v;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3Unproject
// Desc: Transform from normalized device space (x = [-1.0f, 2.0], y = [-1.0f, 2.0], z = [0.0f, 1.0]) to world space
//-----------------------------------------------------------------------------
/*Vector3* Vec3Unproject(Vector3 *vResult, const Vector3* v, const Matrix* worldviewproj)
{
	Matrix invtransform;
	MatrixInverse(&invtransform, worldviewproj);

	Vec3TransformCoord(vResult, v, &invtransform);

	return vResult;
}*/

//-----------------------------------------------------------------------------
// Name: Vec2Lerp
// Desc: Return v0 * (1 - f) + v1 * f
//-----------------------------------------------------------------------------
Vector2* Vec2Lerp(Vector2 *vResult, const Vector2 *v0, const Vector2 *v1, float f)
{
	float g = 1.0f - f;
	vResult->x = v0->x * g + v1->x * f;
	vResult->y = v0->y * g + v1->y * f;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3Lerp
// Desc: Return v0 * (1 - f) + v1 * f
//-----------------------------------------------------------------------------
Vector3* Vec3Lerp(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1, float f)
{
	float g = 1.0f - f;
	vResult->x = v0->x * g + v1->x * f;
	vResult->y = v0->y * g + v1->y * f;
	vResult->z = v0->z * g + v1->z * f;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: ClrLerp
// Desc: Return c0 * (1 - f) + c1 * f
//-----------------------------------------------------------------------------
Color* ClrLerp(Color* cResult, const Color* c0, const Color* c1, float f)
{
	float g = 1.0f - f;
	cResult->r = c0->r * g + c1->r * f;
	cResult->g = c0->g * g + c1->g * f;
	cResult->b = c0->b * g + c1->b * f;
	return cResult;
}
Color3* ClrLerp(Color3* cResult, const Color3* c0, const Color3* c1, float f)
{
	float g = 1.0f - f;
	cResult->r = c0->r * g + c1->r * f;
	cResult->g = c0->g * g + c1->g * f;
	cResult->b = c0->b * g + c1->b * f;
	return cResult;
}

//-----------------------------------------------------------------------------
// Name: Vec2BaryCentric
// Desc: Return v0 * (1 - u - v) + v1 * u + v2 * v
//-----------------------------------------------------------------------------
Vector2* Vec2BaryCentric(Vector2* vResult, const Vector2* v0, const Vector2* v1, const Vector2* v2, const Vector2* uv)
{
	float u = uv->x, v = uv->y;
	float w = 1.0f - u - v;
	vResult->x = v0->x * w + v1->x * u + v2->x * v;
	vResult->y = v0->y * w + v1->y * u + v2->y * v;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3BaryCentric
// Desc: Return v0 * (1 - u - v) + v1 * u + v2 * v
//-----------------------------------------------------------------------------
Vector3* Vec3BaryCentric(Vector3* vResult, const Vector3* v0, const Vector3* v1, const Vector3* v2, const Vector2* uv)
{
	float u = uv->x, v = uv->y;
	float w = 1.0f - u - v;
	vResult->x = v0->x * w + v1->x * u + v2->x * v;
	vResult->y = v0->y * w + v1->y * u + v2->y * v;
	vResult->z = v0->z * w + v1->z * u + v2->z * v;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec2InvBaryCentric
// Desc: Return [u, v] for v0 * (1 - u - v) + v1 * u + v2 * v
//-----------------------------------------------------------------------------
Vector2* Vec2InvBaryCentric(Vector2* vResult, const Vector2* v0, const Vector2* v1, const Vector2* v2, const Vector2* uv)
{
	float div = 1.0f / (v0->x * v1->y + v1->x * v2->y + v2->x * v0->y - v0->y * v1->x - v1->y * v2->x - v2->y * v0->x);
	vResult->x = div * ((v2->x * v0->y - v0->x * v2->y) + (v2->y - v0->y) * uv->x + (v0->x - v2->x) * uv->y);
	vResult->y = div * ((v0->x * v1->y - v1->x * v0->y) + (v0->y - v1->y) * uv->x + (v1->x - v0->x) * uv->y);
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3Min
// Desc: Return component-wise minimum of v0 and v1
//-----------------------------------------------------------------------------
Vector3* Vec3Min(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1)
{
	vResult->x = v0->x < v1->x ? v0->x : v1->x;
	vResult->y = v0->y < v1->y ? v0->y : v1->y;
	vResult->z = v0->z < v1->z ? v0->z : v1->z;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: Vec3Max
// Desc: Return component-wise minimum of v0 and v1
//-----------------------------------------------------------------------------
Vector3* Vec3Max(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1)
{
	vResult->x = v0->x > v1->x ? v0->x : v1->x;
	vResult->y = v0->y > v1->y ? v0->y : v1->y;
	vResult->z = v0->z > v1->z ? v0->z : v1->z;
	return vResult;
}

//-----------------------------------------------------------------------------
// Name: QuaternionIdentity
// Desc: Identity quaternion
//-----------------------------------------------------------------------------
Quaternion* QuaternionIdentity(Quaternion *qResult)
{
	qResult->x = qResult->y = qResult->z = 0.0f;
	qResult->w = 1.0f;
	return qResult;
}

//-----------------------------------------------------------------------------
// Name: QuaternionMultiply
// Desc: Quaternion multiplication and in-place multiplication
//-----------------------------------------------------------------------------
Quaternion* QuaternionMultiply(Quaternion *qResult, const Quaternion *q0, const Quaternion *q1)
{
	if(q0 == qResult)
		return QuaternionMultiply(qResult, q1);

	const Vector3* v0 = (const Vector3*)q0;
	const Vector3* v1 = (const Vector3*)q1;
	Vector3* vResult = (Vector3*)qResult;

	// Multiply (x, y, z)-part
	Vec3Cross(vResult, v1, v0);
	Vec3AddScaled(vResult, vResult, v0, q1->w);
	Vec3AddScaled(vResult, vResult, v1, q0->w);

	// Multiply w-part
	qResult->w = q0->w * q1->w - Vec3Dot(v0, v1);

	return qResult;
}
Quaternion* QuaternionMultiply(Quaternion *qResult, const Quaternion *q1)
{
	const Vector3* v1 = (const Vector3*)q1;
	Vector3* vResult = (Vector3*)qResult;
	Vector3 vtemp;

	Vec3Cross(&vtemp, v1, vResult);
	Vec3AddScaled(&vtemp, &vtemp, v1, qResult->w);
	qResult->w = qResult->w * q1->w - Vec3Dot(vResult, v1);
	Vec3AddScaled(vResult, &vtemp, vResult, q1->w);

	return qResult;
}

//-----------------------------------------------------------------------------
// Name: QuaternionInverse/QuaternionInvert
// Desc: Quaternion inversion and in-place inversion
//-----------------------------------------------------------------------------
Quaternion* QuaternionInverse(Quaternion *qResult, const Quaternion *q0)
{
	float invdot = q0->x*q0->x + q0->y*q0->y + q0->z*q0->z + q0->w*q0->w;
	if(invdot)
	{
		invdot = 1.0f / invdot;
		qResult->x = -q0->x * invdot;
		qResult->y = -q0->y * invdot;
		qResult->z = -q0->z * invdot;
		qResult->w = q0->w * invdot;
	}
	else
		qResult->x = qResult->y = qResult->z = qResult->w = 0.0f;
	return qResult;
}
Quaternion* QuaternionInvert(Quaternion *q0)
{
	float invdot = q0->x*q0->x + q0->y*q0->y + q0->z*q0->z + q0->w*q0->w;
	if(invdot)
	{
		invdot = 1.0f / invdot;
		q0->x = -q0->x * invdot;
		q0->y = -q0->y * invdot;
		q0->z = -q0->z * invdot;
		q0->w = q0->w * invdot;
	}
	else
		q0->x = q0->y = q0->z = q0->w = 0.0f;
	return q0;
}

//-----------------------------------------------------------------------------
// Name: QuaternionNormalize
// Desc: Quaternion normalization and in-place normalization (identical to Vector4 normalization)
//-----------------------------------------------------------------------------
Quaternion* QuaternionNormalize(Quaternion *qResult, const Quaternion *q0)
{
	float len = sqrt(q0->x*q0->x + q0->y*q0->y + q0->z*q0->z + q0->w*q0->w);
	QuaternionSet(qResult, q0->x / len,q0->y / len, q0->z / len, q0->w / len);
	return qResult;
}
Quaternion* QuaternionNormalize(Quaternion *q0)
{
	float len = sqrt(q0->x*q0->x + q0->y*q0->y + q0->z*q0->z + q0->w*q0->w);
	*q0 /= len;
	return q0;
}

//-----------------------------------------------------------------------------
// Name: QuaternionRotationX
// Desc: Build a quaternion that rotates pitch degrees around the x-axis
//-----------------------------------------------------------------------------
Quaternion* QuaternionRotationX(Quaternion *qResult, float pitch)
{
	pitch /= 2.0f;
	qResult->x = sin(pitch);
	qResult->y = 0.0f;
	qResult->z = 0.0f;
	qResult->w = cos(pitch);
	return qResult;
}

//-----------------------------------------------------------------------------
// Name: QuaternionRotationY
// Desc: Build a quaternion that rotates yaw degrees around the y-axis
//-----------------------------------------------------------------------------
Quaternion* QuaternionRotationY(Quaternion *qResult, float yaw)
{
	yaw /= 2.0f;
	qResult->x = 0.0f;
	qResult->y = sin(yaw);
	qResult->z = 0.0f;
	qResult->w = cos(yaw);
	return qResult;
}

//-----------------------------------------------------------------------------
// Name: QuaternionRotationZ
// Desc: Build a quaternion that rotates roll degrees around the z-axis
//-----------------------------------------------------------------------------
Quaternion* QuaternionRotationZ(Quaternion *qResult, float roll)
{
	roll /= 2.0f;
	qResult->x = 0.0f;
	qResult->y = 0.0f;
	qResult->z = sin(roll);
	qResult->w = cos(roll);
	return qResult;
}

//-----------------------------------------------------------------------------
// Name: QuaternionRotationAxis
// Desc: Build a quaternion that rotates angle degrees around an arbitrary axis
//-----------------------------------------------------------------------------
Quaternion* QuaternionRotationAxis(Quaternion *qResult, const Vector3* axis, float angle)
{
	angle /= 2.0f;
	float s = sin(angle);
	qResult->x = s * axis->x;
	qResult->y = s * axis->y;
	qResult->z = s * axis->z;
	qResult->w = cos(angle);
	return qResult;
}

//-----------------------------------------------------------------------------
// Name: QuaternionSlerp
// Desc: Interpolate between two quaternions, using spherical linear interpolation
//-----------------------------------------------------------------------------
Quaternion* QuaternionSlerp(Quaternion *qResult, const Quaternion *q0, const Quaternion *q1, float f)
{
float ax = q0->x, ay = q0->y, az = q0->z, aw = q0->w, bx = q1->x, by = q1->y, bz = q1->z, bw = q1->w;
float omega, cosom, sinom, scale0, scale1;

// calc cosine
cosom = ax * bx + ay * by + az * bz + aw * bw;
// adjust signs (if necessary)
if (cosom < 0.0f) {
	cosom = -cosom;
	bx = -bx;
	by = -by;
	bz = -bz;
	bw = -bw;
}
// calculate coefficients
if ((1.0f - cosom) > 0.000001f) {
	// standard case (slerp)
	omega = acos(cosom);
	sinom = sin(omega);
	scale0 = sin((1.0f - f) * omega) / sinom;
	scale1 = sin(f * omega) / sinom;
}
else {
	// "from" and "to" quaternions are very close 
	//  ... so we can do a linear interpolation
	scale0 = 1.0f - f;
	scale1 = f;
}
// calculate final values
qResult->x = scale0 * ax + scale1 * bx;
qResult->y = scale0 * ay + scale1 * by;
qResult->z = scale0 * az + scale1 * bz;
qResult->w = scale0 * aw + scale1 * bw;

return qResult;
};

//-----------------------------------------------------------------------------
// Name: QuaternionToAxisAngle
// Desc: Extract a rotation axis and an angle that together represent the rotation of the quaternion
//-----------------------------------------------------------------------------
void QuaternionToAxisAngle(const Quaternion *q0, Vector3* axis, float* angle)
{
	if(q0->w > 1) // If q0 isn't normalized
	{
		Quaternion q0n;
		QuaternionNormalize(&q0n, q0);
		*angle = 2 * acos(q0n.w);
		register float s = sqrt(1.0f - q0n.w*q0n.w);
		if(s < FLOAT_TOLERANCE)
		{
			axis->x = q0n.x;
			axis->y = q0n.y;
			axis->z = q0n.z;
		}
		else
		{
			axis->x = q0n.x / s;
			axis->y = q0n.y / s;
			axis->z = q0n.z / s;
		}
	}
	else // If q0 is normalized
	{
		*angle = 2 * acos(q0->w);
		register float s = sqrt(1.0f - q0->w*q0->w);
		if(s < FLOAT_TOLERANCE)
		{
			axis->x = q0->x;
			axis->y = q0->y;
			axis->z = q0->z;
		}
		else
		{
			axis->x = q0->x / s;
			axis->y = q0->y / s;
			axis->z = q0->z / s;
		}
	}
}

Vector3* Quaternion::ComputeAxisWrapper() const
{
	Vector3* axis = new Vector3();
	if(this->w > 1) // If this isn't normalized
	{
		Quaternion q0n;
		QuaternionNormalize(&q0n, this);
		register float s = sqrt(1.0f - q0n.w*q0n.w);
		if(s < FLOAT_TOLERANCE)
		{
			axis->x = q0n.x;
			axis->y = q0n.y;
			axis->z = q0n.z;
		}
		else
		{
			axis->x = q0n.x / s;
			axis->y = q0n.y / s;
			axis->z = q0n.z / s;
		}
	}
	else // If this is normalized
	{
		register float s = sqrt(1.0f - this->w*this->w);
		if(s < FLOAT_TOLERANCE)
		{
			axis->x = this->x;
			axis->y = this->y;
			axis->z = this->z;
		}
		else
		{
			axis->x = this->x / s;
			axis->y = this->y / s;
			axis->z = this->z / s;
		}
	}
	return axis;
}
float Quaternion::ComputeAngleWrapper() const
{
	if(this->w > 1) // If this isn't normalized
	{
		Quaternion q0n;
		QuaternionNormalize(&q0n, this);
		return 2 * acos(q0n.w);
	}
	else // If this is normalized
		return 2 * acos(this->w);
}

//-----------------------------------------------------------------------------
// Name: PlaneFromPointNormal
// Desc: Create plane given a point on the plane and the plane normal
//-----------------------------------------------------------------------------
Plane* PlaneFromPointNormal(Plane *pResult, const Vector3 *vPt, const Vector3 *vNml)
{
	//Result::PrintLogMessage("PlaneFromPointNormal() not implemented"); //EDIT11
	return NULL;
}

//-----------------------------------------------------------------------------
// Name: PlaneNormalize
// Desc: Normalize the plane normal
//-----------------------------------------------------------------------------
Plane* PlaneNormalize(Plane *pResult, const Plane *p0)
{
	float len = sqrt(p0->a*p0->a + p0->b*p0->b + p0->c*p0->c);
	pResult->a /= len;
	pResult->b /= len;
	pResult->c /= len;
	pResult->d /= len;
	return pResult;
}
Plane* PlaneNormalize(Plane *pResult)
{
	float len = sqrt(pResult->a*pResult->a + pResult->b*pResult->b + pResult->c*pResult->c);
	pResult->a /= len;
	pResult->b /= len;
	pResult->c /= len;
	pResult->d /= len;
	return pResult;
}

//-----------------------------------------------------------------------------
// Name: PlaneDotCoord
// Desc: Dot product of p0[a, b, c, d] and v0[x, y, z, 1]
//-----------------------------------------------------------------------------
float PlaneDotCoord(const Plane *p0,const Vector3 *v0)
{
	//Result::PrintLogMessage("PlaneDotCoord() not tested"); //EDIT11
	return p0->a * v0->x + p0->b * v0->y + p0->c * v0->z + p0->d;
}

//-----------------------------------------------------------------------------
// Name: D3DXPlaneIntersectLine
// Desc: Find the intersection point between the plane p0 and a line going from v0 to v1
//-----------------------------------------------------------------------------
Vector3* PlaneIntersectLine(Vector3 *vResult, const Plane *p0, const Vector3 *v0, const Vector3 *v1)
{
	// >>> ISMath version

	/*float v0dot = Vec3Dot(v0, (Vector3*)p0);
	float v1dot = Vec3Dot(v1, (Vector3*)p0);
	if((v0dot <= p0->d + FLOAT_TOLERANCE && v1dot <= p0->d + FLOAT_TOLERANCE) || (v0dot + FLOAT_TOLERANCE >= p0->d && v1dot + FLOAT_TOLERANCE >= p0->d))
		return NULL; // Both v0 and v1 lie on the same side of the plane

	Vec3Scale(vResult, (Vector3*)p0, p0->d); // vResult = point on plane
	Vec3Sub(vResult, vResult, v0); // vResult = v0 ---> point on plane
	float num = Vec3Dot(vResult, (Vector3*)p0);

	Vec3Sub(vResult, v1, v0); // vResult = v0 ---> v1
	Vec3Normalize(vResult); // vResult = dir of v0 ---> v1
	float div = Vec3Dot(vResult, (Vector3*)p0);

	Vec3AddScaled(vResult, v0, vResult, num / div); // vResult = v0 + (dir of v0 ---> v1) * (num / div)

	//Result::PrintLogMessage("PlaneIntersectLine() not tested"); //EDIT11
	return vResult;*/

// >>> D3DXPlaneDotCoord version (returns true, even if the intersection is not between v0 and v1; also defines p0->d inverse)

Vec3Scale(vResult, (Vector3*)p0, -p0->d); // vResult = point on plane
Vec3Sub(vResult, vResult, v0); // vResult = v0 ---> point on plane
float num = Vec3Dot(vResult, (Vector3*)p0);

Vec3Sub(vResult, v1, v0); // vResult = v0 ---> v1
Vec3Normalize(vResult); // vResult = dir of v0 ---> v1
float div = Vec3Dot(vResult, (Vector3*)p0);
if(div < FLOAT_TOLERANCE && div > -FLOAT_TOLERANCE)
	return NULL; // v0 ---> v1 is parallel to p0

Vec3AddScaled(vResult, v0, vResult, num / div); // vResult = v0 + (dir of v0 ---> v1) * (num / div)

//Result::PrintLogMessage("PlaneIntersectLine() not tested"); //EDIT11
return vResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixIdentity
// Desc: Identity matrix
//-----------------------------------------------------------------------------
Matrix* MatrixIdentity(Matrix *mResult)
{
	Matrix m = Matrix(1.0f, 0.0f, 0.0f, 0.0f,
					  0.0f, 1.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f, 0.0f,
					  0.0f, 0.0f, 0.0f, 1.0f);
	*mResult = m;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixTranslation
// Desc: Translation matrix
//-----------------------------------------------------------------------------
Matrix* MatrixTranslation(Matrix *mResult, float x, float y, float z)
{
	mResult->_11 = mResult->_22 = mResult->_33 = mResult->_44 = 1.0f;
	mResult->_12 = mResult->_13 = mResult->_14 = 0.0f;
	mResult->_21 = mResult->_23 = mResult->_24 = 0.0f;
	mResult->_31 = mResult->_32 = mResult->_34 = 0.0f;
	mResult->_41 = x;
	mResult->_42 = y;
	mResult->_43 = z;
	return mResult;
}
Matrix* MatrixTranslation(Matrix *mResult, const Vector3 *v0)
{
	mResult->_11 = mResult->_22 = mResult->_33 = mResult->_44 = 1.0f;
	mResult->_12 = mResult->_13 = mResult->_14 = 0.0f;
	mResult->_21 = mResult->_23 = mResult->_24 = 0.0f;
	mResult->_31 = mResult->_32 = mResult->_34 = 0.0f;
	mResult->_41 = v0->x;
	mResult->_42 = v0->y;
	mResult->_43 = v0->z;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixTranslate
// Desc: Translate m0 by v0
//-----------------------------------------------------------------------------
Matrix* MatrixTranslate(Matrix *mResult, const Matrix *m0, float x, float y, float z)
{
	/*mResult->_41 = m0->_41 + x;
	mResult->_42 = m0->_42 + y;
	mResult->_43 = m0->_43 + z;
	if(mResult != m0)
	{
		memcpy(mResult, m0, 12 * sizeof(float));
		mResult->_44 = m0->_44;
	}*/

if(mResult == m0)
{
mResult->_41 += m0->_11 * x + m0->_21 * y + m0->_31 * z;
mResult->_42 += m0->_12 * x + m0->_22 * y + m0->_32 * z;
mResult->_43 += m0->_13 * x + m0->_23 * y + m0->_33 * z;
mResult->_44 += m0->_14 * x + m0->_24 * y + m0->_34 * z;
}
//else
//	Result::PrintLogMessage("MatrixTranslate() not implemented");

	return mResult;
}
Matrix* MatrixTranslate(Matrix *mResult, const Matrix *m0, const Vector3 *v0)
{
	mResult->_41 = m0->_41 + v0->x;
	mResult->_42 = m0->_42 + v0->y;
	mResult->_43 = m0->_43 + v0->z;
	if(mResult != m0)
	{
		memcpy(mResult, m0, 12 * sizeof(float));
		mResult->_44 = m0->_44;
	}
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixScaling
// Desc: Scaling matrix
//-----------------------------------------------------------------------------
Matrix* MatrixScaling(Matrix *mResult, float x, float y, float z)
{
	mResult->_12 = mResult->_13 = mResult->_14 = 0.0f;
	mResult->_21 = mResult->_23 = mResult->_24 = 0.0f;
	mResult->_31 = mResult->_32 = mResult->_34 = 0.0f;
	mResult->_41 = mResult->_42 = mResult->_43 = 0.0f;
	mResult->_11 = x;
	mResult->_22 = y;
	mResult->_33 = z;
	mResult->_44 = 1.0f;
	return mResult;
}
Matrix* MatrixScaling(Matrix *mResult, const Vector3 *v0)
{
	mResult->_12 = mResult->_13 = mResult->_14 = 0.0f;
	mResult->_21 = mResult->_23 = mResult->_24 = 0.0f;
	mResult->_31 = mResult->_32 = mResult->_34 = 0.0f;
	mResult->_41 = mResult->_42 = mResult->_43 = 0.0f;
	mResult->_11 = v0->x;
	mResult->_22 = v0->y;
	mResult->_33 = v0->z;
	mResult->_44 = 1.0f;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixScale
// Desc: Scale m0 by v0
//-----------------------------------------------------------------------------
Matrix* MatrixScale(Matrix *mResult, const Matrix *m0, float x, float y, float z)
{
	mResult->_11 = m0->_11 * x; mResult->_12 = m0->_12 * x; mResult->_13 = m0->_13 * x; mResult->_14 = m0->_14 * x;
	mResult->_21 = m0->_21 * y; mResult->_22 = m0->_22 * y; mResult->_23 = m0->_23 * y; mResult->_24 = m0->_24 * y;
	mResult->_31 = m0->_31 * z; mResult->_32 = m0->_32 * z; mResult->_33 = m0->_33 * z; mResult->_34 = m0->_34 * z;
	if(mResult != m0)
		memcpy(&mResult->_41, &m0->_41, 4 * sizeof(float));

	return mResult;
}
Matrix* MatrixScale(Matrix *mResult, const Matrix *m0, const Vector3 *v0)
{
	mResult->_11 = m0->_11 * v0->x; mResult->_12 = m0->_12 * v0->x; mResult->_13 = m0->_13 * v0->x; mResult->_14 = m0->_14 * v0->x;
	mResult->_21 = m0->_21 * v0->y; mResult->_22 = m0->_22 * v0->y; mResult->_23 = m0->_23 * v0->y; mResult->_24 = m0->_24 * v0->y;
	mResult->_31 = m0->_31 * v0->z; mResult->_32 = m0->_32 * v0->z; mResult->_33 = m0->_33 * v0->z; mResult->_34 = m0->_34 * v0->z;
	if(mResult != m0)
		memcpy(&mResult->_41, &m0->_41, 4 * sizeof(float));
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixTranspose
// Desc: Return the matrix transpose of the given matrix
//-----------------------------------------------------------------------------
Matrix* MatrixTranspose(Matrix *mResult, const Matrix *m0)
{
	Matrix m = Matrix(m0->_11, m0->_21, m0->_31, m0->_41,
					  m0->_12, m0->_22, m0->_32, m0->_42,
					  m0->_13, m0->_23, m0->_33, m0->_43,
					  m0->_14, m0->_24, m0->_34, m0->_44);
	*mResult = m;
	return mResult;
}

Matrix* MatrixInverseTranspose(Matrix *mResult, const Matrix *m0)
{
float b00 = m0->_11 * m0->_22 - m0->_12 * m0->_21;
float b01 = m0->_11 * m0->_23 - m0->_13 * m0->_21;
float b02 = m0->_11 * m0->_24 - m0->_14 * m0->_21;
float b03 = m0->_12 * m0->_23 - m0->_13 * m0->_22;
float b04 = m0->_12 * m0->_24 - m0->_14 * m0->_22;
float b05 = m0->_13 * m0->_24 - m0->_14 * m0->_23;
float b06 = m0->_31 * m0->_42 - m0->_32 * m0->_41;
float b07 = m0->_31 * m0->_43 - m0->_33 * m0->_41;
float b08 = m0->_31 * m0->_44 - m0->_34 * m0->_41;
float b09 = m0->_32 * m0->_43 - m0->_33 * m0->_42;
float b10 = m0->_32 * m0->_44 - m0->_34 * m0->_42;
float b11 = m0->_33 * m0->_44 - m0->_34 * m0->_43;

// Calculate the determinant
float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

if(!det)
return mResult;
det = 1.0f / det;

mResult->_11 = (m0->_22 * b11 - m0->_23 * b10 + m0->_24 * b09) * det;
mResult->_12 = (m0->_23 * b08 - m0->_21 * b11 - m0->_24 * b07) * det;
mResult->_13 = (m0->_21 * b10 - m0->_22 * b08 + m0->_24 * b06) * det;
mResult->_14 = 0.0f;

mResult->_21 = (m0->_13 * b10 - m0->_12 * b11 - m0->_14 * b09) * det;
mResult->_22 = (m0->_11 * b11 - m0->_13 * b08 + m0->_14 * b07) * det;
mResult->_23 = (m0->_12 * b08 - m0->_11 * b10 - m0->_14 * b06) * det;
mResult->_24 = 0.0f;

mResult->_31 = (m0->_42 * b05 - m0->_43 * b04 + m0->_44 * b03) * det;
mResult->_32 = (m0->_43 * b02 - m0->_41 * b05 - m0->_44 * b01) * det;
mResult->_33 = (m0->_41 * b04 - m0->_42 * b02 + m0->_44 * b00) * det;
mResult->_34 = 0.0f;

mResult->_41 = mResult->_42 = mResult->_43 = mResult->_44 = 0.0f;

return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixDeterminant
// Desc: Compute the determinat of a matrix
//-----------------------------------------------------------------------------
float MatrixDeterminant(const Matrix *m0)
{
	float b00 = m0->_11 * m0->_22 - m0->_12 * m0->_21;
	float b01 = m0->_11 * m0->_23 - m0->_13 * m0->_21;
	float b02 = m0->_11 * m0->_24 - m0->_14 * m0->_21;
	float b03 = m0->_12 * m0->_23 - m0->_13 * m0->_22;
	float b04 = m0->_12 * m0->_24 - m0->_14 * m0->_22;
	float b05 = m0->_13 * m0->_24 - m0->_14 * m0->_23;
	float b06 = m0->_31 * m0->_42 - m0->_32 * m0->_41;
	float b07 = m0->_31 * m0->_43 - m0->_33 * m0->_41;
	float b08 = m0->_31 * m0->_44 - m0->_34 * m0->_41;
	float b09 = m0->_32 * m0->_43 - m0->_33 * m0->_42;
	float b10 = m0->_32 * m0->_44 - m0->_34 * m0->_42;
	float b11 = m0->_33 * m0->_44 - m0->_34 * m0->_43;

	return b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
};

//-----------------------------------------------------------------------------
// Name: MatrixInverse
// Desc: Compute the inverse of a matrix (returns NULL if the matrix has no inverse)
//-----------------------------------------------------------------------------
Matrix* MatrixInverse(Matrix* mResult, const Matrix* m0)
{
	float b00 = m0->_11 * m0->_22 - m0->_12 * m0->_21;
	float b01 = m0->_11 * m0->_23 - m0->_13 * m0->_21;
	float b02 = m0->_11 * m0->_24 - m0->_14 * m0->_21;
	float b03 = m0->_12 * m0->_23 - m0->_13 * m0->_22;
	float b04 = m0->_12 * m0->_24 - m0->_14 * m0->_22;
	float b05 = m0->_13 * m0->_24 - m0->_14 * m0->_23;
	float b06 = m0->_31 * m0->_42 - m0->_32 * m0->_41;
	float b07 = m0->_31 * m0->_43 - m0->_33 * m0->_41;
	float b08 = m0->_31 * m0->_44 - m0->_34 * m0->_41;
	float b09 = m0->_32 * m0->_43 - m0->_33 * m0->_42;
	float b10 = m0->_32 * m0->_44 - m0->_34 * m0->_42;
	float b11 = m0->_33 * m0->_44 - m0->_34 * m0->_43;

	float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
	if(!det)
		return NULL; 
	det = 1.0f / det;

	if(m0 == mResult)
	{
		Matrix m0_ = *m0;
		mResult->_11 = (m0_._22 * b11 - m0_._23 * b10 + m0_._24 * b09) * det;
		mResult->_12 = (m0_._13 * b10 - m0_._12 * b11 - m0_._14 * b09) * det;
		mResult->_13 = (m0_._42 * b05 - m0_._43 * b04 + m0_._44 * b03) * det;
		mResult->_14 = (m0_._33 * b04 - m0_._32 * b05 - m0_._34 * b03) * det;
		mResult->_21 = (m0_._23 * b08 - m0_._21 * b11 - m0_._24 * b07) * det;
		mResult->_22 = (m0_._11 * b11 - m0_._13 * b08 + m0_._14 * b07) * det;
		mResult->_23 = (m0_._43 * b02 - m0_._41 * b05 - m0_._44 * b01) * det;
		mResult->_24 = (m0_._31 * b05 - m0_._33 * b02 + m0_._34 * b01) * det;
		mResult->_31 = (m0_._21 * b10 - m0_._22 * b08 + m0_._24 * b06) * det;
		mResult->_32 = (m0_._12 * b08 - m0_._11 * b10 - m0_._14 * b06) * det;
		mResult->_33 = (m0_._41 * b04 - m0_._42 * b02 + m0_._44 * b00) * det;
		mResult->_34 = (m0_._32 * b02 - m0_._31 * b04 - m0_._34 * b00) * det;
		mResult->_41 = (m0_._22 * b07 - m0_._21 * b09 - m0_._23 * b06) * det;
		mResult->_42 = (m0_._11 * b09 - m0_._12 * b07 + m0_._13 * b06) * det;
		mResult->_43 = (m0_._42 * b01 - m0_._41 * b03 - m0_._43 * b00) * det;
		mResult->_44 = (m0_._31 * b03 - m0_._32 * b01 + m0_._33 * b00) * det;
	}
	else
	{
		mResult->_11 = (m0->_22 * b11 - m0->_23 * b10 + m0->_24 * b09) * det;
		mResult->_12 = (m0->_13 * b10 - m0->_12 * b11 - m0->_14 * b09) * det;
		mResult->_13 = (m0->_42 * b05 - m0->_43 * b04 + m0->_44 * b03) * det;
		mResult->_14 = (m0->_33 * b04 - m0->_32 * b05 - m0->_34 * b03) * det;
		mResult->_21 = (m0->_23 * b08 - m0->_21 * b11 - m0->_24 * b07) * det;
		mResult->_22 = (m0->_11 * b11 - m0->_13 * b08 + m0->_14 * b07) * det;
		mResult->_23 = (m0->_43 * b02 - m0->_41 * b05 - m0->_44 * b01) * det;
		mResult->_24 = (m0->_31 * b05 - m0->_33 * b02 + m0->_34 * b01) * det;
		mResult->_31 = (m0->_21 * b10 - m0->_22 * b08 + m0->_24 * b06) * det;
		mResult->_32 = (m0->_12 * b08 - m0->_11 * b10 - m0->_14 * b06) * det;
		mResult->_33 = (m0->_41 * b04 - m0->_42 * b02 + m0->_44 * b00) * det;
		mResult->_34 = (m0->_32 * b02 - m0->_31 * b04 - m0->_34 * b00) * det;
		mResult->_41 = (m0->_22 * b07 - m0->_21 * b09 - m0->_23 * b06) * det;
		mResult->_42 = (m0->_11 * b09 - m0->_12 * b07 + m0->_13 * b06) * det;
		mResult->_43 = (m0->_42 * b01 - m0->_41 * b03 - m0->_43 * b00) * det;
		mResult->_44 = (m0->_31 * b03 - m0->_32 * b01 + m0->_33 * b00) * det;
	}

	return mResult;
};

//-----------------------------------------------------------------------------
// Name: Matrix3x3Determinant
// Desc: Compute the determinat of a 3x3 submatrix
//-----------------------------------------------------------------------------
float Matrix3x3Determinant(const Matrix *m0)
{
	return m0->_11 * m0->_22 * m0->_33 + m0->_12 * m0->_23 * m0->_31 + m0->_13 * m0->_21 * m0->_32 -
		   m0->_13 * m0->_22 * m0->_31 - m0->_11 * m0->_23 * m0->_32 - m0->_12 * m0->_21 * m0->_33;
}

//-----------------------------------------------------------------------------
// Name: Matrix3x3Inverse
// Desc: Return the inverted 3x3 submatrix embedded into a 4x4 matrix
//-----------------------------------------------------------------------------
Matrix* Matrix3x3Inverse(Matrix *mResult, float *det, const Matrix *m0)
{
	float d = Matrix3x3Determinant(m0);
	if(!d)
	{
		if(det)
			*det = 0.0f;
		return mResult = 0;
	}

	/*Matrix m = Matrix(m0->_11 / d, m0->_21 / d, m0->_31 / d, 0.0f,
					  m0->_12 / d, m0->_22 / d, m0->_32 / d, 0.0f,
					  m0->_13 / d, m0->_23 / d, m0->_33 / d, 0.0f,
					  0.0f,        0.0f,        0.0f,        1.0f);*/
	Matrix m = Matrix((m0->_22 * m0->_33 - m0->_23 * m0->_32) / d, (m0->_13 * m0->_32 - m0->_12 * m0->_33) / d, (m0->_12 * m0->_23 - m0->_13 * m0->_22) / d, 0.0f,
					  (m0->_23 * m0->_31 - m0->_21 * m0->_33) / d, (m0->_11 * m0->_33 - m0->_13 * m0->_31) / d, (m0->_13 * m0->_21 - m0->_11 * m0->_23) / d, 0.0f,
					  (m0->_21 * m0->_32 - m0->_22 * m0->_31) / d, (m0->_12 * m0->_31 - m0->_11 * m0->_32) / d, (m0->_11 * m0->_22 - m0->_12 * m0->_21) / d, 0.0f,
					  0.0f,        0.0f,        0.0f,        1.0f);
	if(det)
		*det = d;
	*mResult = m;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixPerspective
// Desc: Perspective projection matrix
//-----------------------------------------------------------------------------
Matrix* MatrixPerspectiveLH(Matrix *mResult, float width, float height, float zn, float zf)
{
	Matrix m = Matrix(2.0f*zn/width, 0.0f,           0.0f,          0.0f,
					  0.0f,          2.0f*zn/height, 0.0f,          0.0f,
					  0.0f,          0.0f,           zf/(zf-zn),    1.0f,
					  0.0f,          0.0f,           zn*zf/(zn-zf), 0.0f);
	*mResult = m;
	return mResult;
}
Matrix* MatrixPerspectiveRH(Matrix *mResult, float width, float height, float zn, float zf)
{
	Matrix m = Matrix(2.0f*zn/width, 0.0f,           0.0f,          0.0f,
					  0.0f,          2.0f*zn/height, 0.0f,          0.0f,
					  0.0f,          0.0f,           zf/(zn-zf),   -1.0f,
					  0.0f,          0.0f,           zn*zf/(zn-zf), 0.0f);
	*mResult = m;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixPerspectiveFov
// Desc: Perspective projection matrix
//-----------------------------------------------------------------------------
Matrix* MatrixPerspectiveFovLH(Matrix *mResult, float fovy, float aspect, float zn, float zf)
{
	float yscale = 1.0f / tan(fovy / 2.0f);
	Matrix m = Matrix(yscale/aspect, 0.0f,           0.0f,          0.0f,
					  0.0f,          yscale,         0.0f,          0.0f,
					  0.0f,          0.0f,           zf/(zf-zn),    1.0f,
					  0.0f,          0.0f,           zn*zf/(zn-zf), 0.0f);
	*mResult = m;
	return mResult;
}
Matrix* MatrixPerspectiveFovRH(Matrix *mResult, float fovy, float aspect, float zn, float zf)
{
	float yscale = 1.0f / tan(fovy / 2.0f);
	Matrix m = Matrix(yscale/aspect, 0.0f,           0.0f,          0.0f,
					  0.0f,          yscale,         0.0f,          0.0f,
					  0.0f,          0.0f,           zf/(zn-zf),   -1.0f,
					  0.0f,          0.0f,           zn*zf/(zn-zf), 0.0f);
	*mResult = m;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixOrthographic
// Desc: Orthographic projection matrix
//-----------------------------------------------------------------------------
Matrix* MatrixOrthographicLH(Matrix *mResult, float width, float height, float zn, float zf)
{
	Matrix m = Matrix(2.0f/width,    0.0f,           0.0f,          0.0f,
					  0.0f,          2.0f/height,    0.0f,          0.0f,
					  0.0f,          0.0f,           1.0f/(zf-zn),  0.0f,
					  0.0f,          0.0f,           zn/(zn-zf),    1.0f);
	*mResult = m;
	return mResult;
}
Matrix* MatrixOrthographicRH(Matrix *mResult, float width, float height, float zn, float zf)
{
	Matrix m = Matrix(2.0f/width,    0.0f,           0.0f,          0.0f,
					  0.0f,          2.0f/height,    0.0f,          0.0f,
					  0.0f,          0.0f,           1.0f/(zn-zf),  0.0f,
					  0.0f,          0.0f,           zn/(zn-zf),    1.0f);
	*mResult = m;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixRotationX
// Desc: Rotation matrix around the x-axis
//-----------------------------------------------------------------------------
Matrix* MatrixRotationX(Matrix *mResult, float angle)
{
	Matrix m = Matrix(1.0f,        0.0f,        0.0f,        0.0f,
					  0.0f,        cos(angle),  sin(angle),  0.0f,
					  0.0f,        -sin(angle), cos(angle),  0.0f,
					  0.0f,        0.0f,        0.0f,        1.0f);
	*mResult = m;
	return mResult;
}
Matrix* MatrixRotateX(Matrix* mResult, const Matrix* m0, float angle)
{
	float s = sin(angle), c = cos(angle);
	float* row[2];

	if(m0 == mResult)
	{
		row[0] = new float[4]; memcpy(row[0], &m0->_21, 4 * sizeof(float)); // Row 1
		row[1] = new float[4]; memcpy(row[1], &m0->_31, 4 * sizeof(float)); // Row 2
	}
	else // m0 != mResult
	{
		memcpy(&mResult->_11, &m0->_11, 4 * sizeof(float)); // Row 0
		row[0] = &mResult->_21; // Row 2
		row[1] = &mResult->_31; // Row 3
		memcpy(&mResult->_41, &m0->_41, 4 * sizeof(float)); // Row 3
	}

	mResult->_21 = row[0][0] * c + row[1][0] * s;
	mResult->_22 = row[0][1] * c + row[1][1] * s;
	mResult->_23 = row[0][2] * c + row[1][2] * s;
	mResult->_24 = row[0][3] * c + row[1][3] * s;
	mResult->_31 = row[1][0] * c - row[0][0] * s;
	mResult->_32 = row[1][1] * c - row[0][1] * s;
	mResult->_33 = row[1][2] * c - row[0][2] * s;
	mResult->_34 = row[1][3] * c - row[0][3] * s;

	if(m0 == mResult)
	{
		delete row[0];
		delete row[1];
	}

	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixRotationY
// Desc: Rotation matrix around the y-axis
//-----------------------------------------------------------------------------
Matrix* MatrixRotationY(Matrix *mResult, float angle)
{
	Matrix m = Matrix(cos(angle),  0.0f,        -sin(angle), 0.0f,
					  0.0f,        1.0f,        0.0f,        0.0f,
					  sin(angle),  0.0f,        cos(angle),  0.0f,
					  0.0f,        0.0f,        0.0f,        1.0f);
	*mResult = m;
	return mResult;
}
Matrix* MatrixRotateY(Matrix* mResult, const Matrix* m0, float angle)
{
	float s = sin(angle), c = cos(angle);
	float* row[2];

	if(m0 == mResult)
	{
		row[0] = new float[4]; memcpy(row[0], &m0->_11, 4 * sizeof(float)); // Row 0
		row[1] = new float[4]; memcpy(row[1], &m0->_31, 4 * sizeof(float)); // Row 2
	}
	else // m0 != mResult
	{
		row[0] = &mResult->_11; // Row 0
		memcpy(&mResult->_21, &m0->_21, 4 * sizeof(float)); // Rows 1 and 3
		row[1] = &mResult->_31; // Row 2
		memcpy(&mResult->_41, &m0->_41, 4 * sizeof(float)); // Rows 1 and 3
	}

	mResult->_11 = row[0][0] * c - row[1][0] * s;
	mResult->_12 = row[0][1] * c - row[1][1] * s;
	mResult->_13 = row[0][2] * c - row[1][2] * s;
	mResult->_14 = row[0][3] * c - row[1][3] * s;
	mResult->_31 = row[0][0] * s + row[1][0] * c;
	mResult->_32 = row[0][1] * s + row[1][1] * c;
	mResult->_33 = row[0][2] * s + row[1][2] * c;
	mResult->_34 = row[0][3] * s + row[1][3] * c;

	if(m0 == mResult)
	{
		delete row[0];
		delete row[1];
	}

	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixRotationZ
// Desc: Rotation matrix around the z-axis
//-----------------------------------------------------------------------------
Matrix* MatrixRotationZ(Matrix *mResult, float angle)
{
	Matrix m = Matrix(cos(angle),  sin(angle),  0.0f,        0.0f,
					  -sin(angle), cos(angle),  0.0f,        0.0f,
					  0.0f,        0.0f,        1.0f,        0.0f,
					  0.0f,        0.0f,        0.0f,        1.0f);
	*mResult = m;
	return mResult;
}
Matrix* MatrixRotateZ(Matrix* mResult, const Matrix* m0, float angle)
{
	float s = sin(angle), c = cos(angle);
	float* row[2];

	if(m0 == mResult)
	{
		row[0] = new float[4]; memcpy(row[0], &m0->_11, 4 * sizeof(float)); // Row 0
		row[1] = new float[4]; memcpy(row[1], &m0->_21, 4 * sizeof(float)); // Row 1
	}
	else // m0 != mResult
	{
		row[0] = &mResult->_11; // Row 0
		row[1] = &mResult->_21; // Row 1
		memcpy(&mResult->_31, &m0->_31, 8 * sizeof(float)); // Rows 2 and 3
	}

	mResult->_11 = row[0][0] * c + row[1][0] * s;
	mResult->_12 = row[0][1] * c + row[1][1] * s;
	mResult->_13 = row[0][2] * c + row[1][2] * s;
	mResult->_14 = row[0][3] * c + row[1][3] * s;
	mResult->_21 = row[1][0] * c - row[0][0] * s;
	mResult->_22 = row[1][1] * c - row[0][1] * s;
	mResult->_23 = row[1][2] * c - row[0][2] * s;
	mResult->_24 = row[1][3] * c - row[0][3] * s;

	if(m0 == mResult)
	{
		delete row[0];
		delete row[1];
	}

	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixRotationYawPitchRoll
// Desc: Build a matrix by yaw, pitch and roll
//-----------------------------------------------------------------------------
Matrix* MatrixRotationYawPitchRoll(Matrix *mResult, float yaw, float pitch, float roll)
{
	Matrix m = Matrix(cos(roll) * cos(yaw) + sin(pitch) * sin(roll) * sin(yaw), cos(roll) * sin(pitch) * sin(yaw) - cos(yaw) * sin(roll), cos(pitch) * sin(yaw), 0.0f,
					  cos(pitch) * sin(roll), cos(pitch) * cos(roll), -sin(pitch), 0.0f,
					  cos(yaw) * sin(pitch) * sin(roll) - cos(roll) * sin(yaw), cos(roll) * cos(yaw) * sin(pitch) + sin(roll) * sin(yaw), cos(pitch) * cos(yaw), 0.0f,
					  0.0f, 0.0f, 0.0f, 1.0f);
	*mResult = m;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixRotationQuaternion
// Desc: Build a rotation matrix, given a quaternion
//-----------------------------------------------------------------------------
Matrix* MatrixRotationQuaternion(Matrix *mResult, const Quaternion* qrot)
{
	float xx = 2.0f * qrot->x * qrot->x;
	float xy = 2.0f * qrot->x * qrot->y;
	float xz = 2.0f * qrot->x * qrot->z;
	float xw = 2.0f * qrot->x * qrot->w;
	float yy = 2.0f * qrot->y * qrot->y;
	float yz = 2.0f * qrot->y * qrot->z;
	float yw = 2.0f * qrot->y * qrot->w;
	float zz = 2.0f * qrot->z * qrot->z;
	float zw = 2.0f * qrot->z * qrot->w;
	float ww = 2.0f * qrot->w * qrot->w;

	mResult->_11 = 1.0f - yy - zz;
	mResult->_12 = xy + zw;
	mResult->_13 = xz - yw;

	mResult->_21 = xy - zw;
	mResult->_22 = 1.0f - xx - zz;
	mResult->_23 = yz + xw;

	mResult->_31 = xz + yw;
	mResult->_32 = yz - xw;
	mResult->_33 = 1.0f - xx - yy;

	mResult->_14 = mResult->_24 = mResult->_34 = 0.0f;
	mResult->_41 = mResult->_42 = mResult->_43 = 0.0f;
	mResult->_44 = 1.0f;

	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixLookAt
// Desc: Build a matrix with origin at pos and looking at target
//-----------------------------------------------------------------------------
Matrix* MatrixLookAtLH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up)
{
	Vector3 vlook, vright, vup;
	Vec3Sub(&vlook, target, pos);
	Vec3Normalize(&vlook);
	Vec3Cross(&vright, up, &vlook);
	Vec3Normalize(&vright);
	Vec3Cross(&vup, &vlook, &vright);

	mResult->_11 = vright.x; mResult->_12 = vright.y; mResult->_13 = vright.z; mResult->_14 = 0.0;
	mResult->_21 = vup.x; mResult->_22 = vup.y; mResult->_23 = vup.z; mResult->_24 = 0.0;
	mResult->_31 = vlook.x; mResult->_32 = vlook.y; mResult->_33 = vlook.z; mResult->_34 = 0.0;
	mResult->_41 = pos->x; mResult->_42 = pos->y; mResult->_43 = pos->z; mResult->_44 = 1.0;
	return mResult;
}
Matrix* MatrixLookAtRH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up)
{
	Vector3 vlook, vright, vup;
	Vec3Sub(&vlook, target, pos);
	Vec3Normalize(&vlook);
	Vec3Cross(&vright, &vlook, up);
	Vec3Normalize(&vright);
	Vec3Cross(&vup, &vright, &vlook);

	mResult->_11 = vright.x; mResult->_12 = vright.y; mResult->_13 = vright.z; mResult->_14 = 0.0;
	mResult->_21 = vup.x; mResult->_22 = vup.y; mResult->_23 = vup.z; mResult->_24 = 0.0;
	mResult->_31 = vlook.x; mResult->_32 = vlook.y; mResult->_33 = vlook.z; mResult->_34 = 0.0;
	mResult->_41 = pos->x; mResult->_42 = pos->y; mResult->_43 = pos->z; mResult->_44 = 1.0;
	return mResult;
}

//-----------------------------------------------------------------------------
// Name: MatrixInvLookAt
// Desc: Build a view matrix with origin at pos and looking at target
//-----------------------------------------------------------------------------
Matrix* MatrixInvLookAtLH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up)
{
	Vector3 vlook, vright, vup;
	Vec3Sub(&vlook, target, pos);
	Vec3Normalize(&vlook);
	Vec3Cross(&vright, up, &vlook);
	Vec3Normalize(&vright);
	Vec3Cross(&vup, &vlook, &vright);

	mResult->_11 = vright.x;			   mResult->_12 = vup.x;			   mResult->_13 = vlook.x;				 mResult->_14 = 0.0;
	mResult->_21 = vright.y;			   mResult->_22 = vup.y;			   mResult->_23 = vlook.y;				 mResult->_24 = 0.0;
	mResult->_31 = vright.z;			   mResult->_32 = vup.z;			   mResult->_33 = vlook.z;				 mResult->_34 = 0.0;
	mResult->_41 = -Vec3Dot(&vright, pos); mResult->_42 = -Vec3Dot(&vup, pos); mResult->_43 = -Vec3Dot(&vlook, pos); mResult->_44 = 1.0;
	return mResult;
}
Matrix* MatrixInvLookAtRH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up)
{
	Vector3 vlook, vright, vup;
	Vec3Sub(&vlook, pos, target);
	Vec3Normalize(&vlook);
	Vec3Cross(&vright, up, &vlook);
	Vec3Normalize(&vright);
	Vec3Cross(&vup, &vlook, &vright);

	mResult->_11 = vright.x;			   mResult->_12 = vup.x;			   mResult->_13 = vlook.x;				 mResult->_14 = 0.0;
	mResult->_21 = vright.y;			   mResult->_22 = vup.y;			   mResult->_23 = vlook.y;				 mResult->_24 = 0.0;
	mResult->_31 = vright.z;			   mResult->_32 = vup.z;			   mResult->_33 = vlook.z;				 mResult->_34 = 0.0;
	mResult->_41 = -Vec3Dot(&vright, pos); mResult->_42 = -Vec3Dot(&vup, pos); mResult->_43 = -Vec3Dot(&vlook, pos); mResult->_44 = 1.0;
	return mResult;
}

bool RayIntersectsBox(const Vector3* raypos, const Vector3* raydir, const Vector3* boxmin, const Vector3* boxmax)
{
Vector3 dirfrac(1.0f / raydir->x, 1.0f / raydir->y, 1.0f / raydir->z);

// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
// r.org is origin of ray
float t1 = (boxmin->x - raypos->x)*dirfrac.x;
float t2 = (boxmax->x - raypos->x)*dirfrac.x;
float t3 = (boxmin->y - raypos->y)*dirfrac.y;
float t4 = (boxmax->y - raypos->y)*dirfrac.y;
float t5 = (boxmin->z - raypos->z)*dirfrac.z;
float t6 = (boxmax->z - raypos->z)*dirfrac.z;

//float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
float tmin, tmax, tmp;
tmin = t1 < t2 ? t1 : t2;
tmp = t3 < t4 ? t3 : t4;
tmin = tmin > tmp ? tmin : tmp;
tmp = t5 < t6 ? t5 : t6;
tmin = tmin > tmp ? tmin : tmp;
//float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
tmax = t1 > t2 ? t1 : t2;
tmp = t3 > t4 ? t3 : t4;
tmax = tmax < tmp ? tmax : tmp;
tmp = t5 > t6 ? t5 : t6;
tmax = tmax < tmp ? tmax : tmp;

// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
if (tmax < 0)
{
//	t = tmax;
	return false;
}

// if tmin > tmax, ray doesn't intersect AABB
if (tmin > tmax)
{
//	t = tmax;
	return false;
}

//t = tmin;
return true;
}

namespace Intersection
{
	// RaySphere - Backface culling version:
	float RaySphereBFC(const Vector3* raypos, const Vector3* raydir, const Vector3* spherecenter, float sphereradius)
	{
		Vector3 v0;
		Vec3Sub(&v0, raypos, spherecenter);
		float v0_lensq = Vec3LengthSq(&v0);
		if(v0_lensq <= sphereradius*sphereradius) // If raypos is inside the sphere
			return FLOAT_INFINITY; // Sphere is invisible from the inside
		float rdv0 = Vec3Dot(raydir, &v0);
		float discriminant = rdv0*rdv0 - v0_lensq + sphereradius*sphereradius;
		if(discriminant < 0.0)
			return FLOAT_INFINITY;
		else
		{
			rdv0 = -rdv0 - sqrt(discriminant); // Compute final hit distance (take closest intersection)
			return rdv0 > FLOAT_TOLERANCE ? rdv0 : FLOAT_INFINITY; // Allow only hits in positive distance
		}
	}
	// RaySphere - Frontface culling version:
	float RaySphereFFC(const Vector3* raypos, const Vector3* raydir, const Vector3* spherecenter, float sphereradius)
	{
		Vector3 v0;
		Vec3Sub(&v0, raypos, spherecenter);
		float rdv0 = Vec3Dot(raydir, &v0);
		float discriminant = rdv0*rdv0 - Vec3LengthSq(&v0) + sphereradius*sphereradius;
		if(discriminant < 0.0)
			return FLOAT_INFINITY;
		else
		{
			rdv0 = -rdv0 + sqrt(discriminant); // Compute final hit distance (take furthest intersection)
			return rdv0 > FLOAT_TOLERANCE ? rdv0 : FLOAT_INFINITY; // Allow only hits in positive distance
		}
	}

	// RayTriangle - No culling version:
	float RayTriangleNC(const Vector3* raypos, const Vector3* raydir, const Vector3* v0, const Vector3* v1, const Vector3* v2, Vector2* uv)
	{
		Vector3 v01, v02, h, s, q;
		float a, f;
		Vec3Sub(&v01, v1, v0);
		Vec3Sub(&v02, v2, v0);

		Vec3Cross(&h, raydir, &v02);
		a = Vec3Dot(&v01, &h);

		if(a > -FLOAT_TOLERANCE && a < FLOAT_TOLERANCE)
			return FLOAT_INFINITY;

		f = 1.0f / a;
		Vec3Sub(&s, raypos, v0);
		uv->x = f * Vec3Dot(&s, &h);

		if(uv->x < -FLOAT_TOLERANCE || uv->x > 1.0f + FLOAT_TOLERANCE)
			return FLOAT_INFINITY;

		Vec3Cross(&q, &s, &v01);
		uv->y = f * Vec3Dot(raydir, &q);

		if(uv->y < -FLOAT_TOLERANCE || uv->x + uv->y > 1.0f + FLOAT_TOLERANCE)
			return FLOAT_INFINITY;
		float t = f * Vec3Dot(&v02, &q);

		return t > 0.0f ? t : FLOAT_INFINITY; // Allow only hits in positive distance
	}
	// RayTriangle - Backface culling version:
	float RayTriangleBFC(const Vector3* raypos, const Vector3* raydir, const Vector3* v0, const Vector3* v1, const Vector3* v2, Vector2* uv)
	{
		Vector3 v01, v02, h, s, q;
		float a, f;
		Vec3Sub(&v01, v1, v0);
		Vec3Sub(&v02, v2, v0);

		Vec3Cross(&h, raydir, &v02);
		a = Vec3Dot(&v01, &h);

		if(a < FLOAT_TOLERANCE)
			return FLOAT_INFINITY;

		Vec3Sub(&s, raypos, v0);
		uv->x = Vec3Dot(&s, &h);
		if(uv->x < -FLOAT_TOLERANCE || uv->x > a + FLOAT_TOLERANCE)
			return FLOAT_INFINITY;

		Vec3Cross(&q, &s, &v01);
		uv->y = Vec3Dot(raydir, &q);
		if(uv->y < -FLOAT_TOLERANCE || uv->x + uv->y > a + FLOAT_TOLERANCE)
			return FLOAT_INFINITY;
		float t = Vec3Dot(&v02, &q);

		f = 1.0f / a;
		t *= f;
		uv->x *= f;
		uv->y *= f;

		return t > 0.0f ? t : FLOAT_INFINITY; // Allow only hits in positive distance
	}
	// RayTriangle - Frontface culling version:
	float RayTriangleFFC(const Vector3* raypos, const Vector3* raydir, const Vector3* v0, const Vector3* v1, const Vector3* v2, Vector2* uv)
	{
		Vector3 v01, v02, h, s, q;
		float a, f;
		Vec3Sub(&v01, v1, v0);
		Vec3Sub(&v02, v2, v0);

		Vec3Cross(&h, raydir, &v02);
		a = Vec3Dot(&v01, &h);

		if(a > FLOAT_TOLERANCE)
			return FLOAT_INFINITY;

		Vec3Sub(&s, raypos, v0);
		uv->x = Vec3Dot(&s, &h);
		if(uv->x < a - FLOAT_TOLERANCE || uv->x > FLOAT_TOLERANCE)
			return FLOAT_INFINITY;

		Vec3Cross(&q, &s, &v01);
		uv->y = Vec3Dot(raydir, &q);
		if(uv->x + uv->y < a - FLOAT_TOLERANCE || uv->y > FLOAT_TOLERANCE)
			return FLOAT_INFINITY;
		float t = Vec3Dot(&v02, &q);

		f = 1.0f / a;
		t *= f;
		uv->x *= f;
		uv->y *= f;

		return t > 0.0f ? t : FLOAT_INFINITY; // Allow only hits in positive distance
	}

	// RayMovingSphere - Backface culling version:
	bool IntersectLines(const Vector3* p00, const Vector3* p01, const Vector3* p10, const Vector3* p11, float* sc, float* tc) //EDIT: Not fully tested; Not optimized
	{
		Vector3 u, v, w, dP;
		float a, b, c, d, e, D;
		u = *p01 - *p00;
		v = *p11 - *p10;
		w = *p00 - *p10;
		a = Vec3Dot(&u, &u); // always >= 0
		b = Vec3Dot(&u, &v);
		c = Vec3Dot(&v, &v); // always >= 0
		d = Vec3Dot(&u, &w);
		e = Vec3Dot(&v, &w);
		D = a * c - b * b; // always >= 0

		// Compute the line parameters of the two closest points
		if(D < FLOAT_TOLERANCE) // the lines are almost parallel
		{
			*sc = 0.0f;
			*tc = b > c ? d / b : e / c; // use the largest denominator
		}
		else
		{
			*sc = (b * e - c * d) / D;
			*tc = (a * e - b * d) / D;
		}

		// Get the difference of the two closest points
		dP = w + *Vec3Scale(&u, &u, *sc) - *Vec3Scale(&v, &v, *tc); // = line1(sc) - line2(tc)

		if(Vec3LengthSq(&dP) > FLOAT_TOLERANCE*FLOAT_TOLERANCE)
			return false;

		return true;
	}
	float RayMovingSphereBFC(const Vector3* raypos, const Vector3* raydir, const Vector3* spherecenter, float sphereradius, const Vector3* spherevelocity) //EDIT: Not fully tested
	{
		// Shift raypos towards spherecenter by radius
		Vector3 v;
		Vec3Sub(&v, spherecenter, raypos); // spheredir = raypos - spherecenter
		Vec3Cross(&v, &v, raydir); // cross = spheredir x raydir
		Vec3Cross(&v, &v, raydir); // spheredir (orthogonal to raydir) = cross x raydir
		Vec3Scale(&v, Vec3Normalize(&v), sphereradius); // rayshift = spheredir * sphereradius
		if(Vec3Dot(&v, spherevelocity) > 0.0) Vec3Invert(&v); // Assure rayshift points towards spherecenter, not away from it
		Vec3Add(&v, raypos, &v); // newraypos = raypos + rayshift

		// Perform ray collision between:
		// Ray 1 = spherecenter to (spherecenter + spherevelocity)
		// Ray 2 = newraypos to (newraypos + raydir)
		float sc, tc;
		Vector3 p01, p11;
		Vec3Add(&p01, spherecenter, spherevelocity);
		Vec3Add(&p11, raydir, &v);
		if(!IntersectLines(spherecenter, &p01, &v, &p11, &sc, &tc) || sc < 0.0 || sc > 1.0 || tc < 0.0 || tc > 1.0)
			return FLOAT_INFINITY;
		// Results:
		// tc = ray distance to first collission with sphere
		// sc = relative time of impact (0.0 == sphere at spherecenter; 1.0 == sphere at spherecenter + spherevelocity)
		return tc;
	}

	RAY_TRIANGLE_INTERSECTION_FUNCTION RayTriangle = RayTriangleBFC; // Default to backface culling
	RAY_SPHERE_INTERSECTION_FUNCTION RaySphere = RaySphereBFC; // Default to backface culling
	void SetCullMode(CullMode cm)
	{
		switch(cm)
		{
		case _CM_NONE:
			RayTriangle = RayTriangleNC;
			RaySphere = RaySphereBFC;
			break;
		case CM_FRONT:
			RayTriangle = RayTriangleFFC;
			RaySphere = RaySphereFFC;
			break;
		case CM_BACK:
			RayTriangle = RayTriangleBFC;
			RaySphere = RaySphereBFC;
		}
	}
	CullMode GetCullMode()
	{
		if(RayTriangle == RayTriangleFFC)
			return CM_FRONT;
		if(RayTriangle == RayTriangleBFC)
			return CM_BACK;
		return _CM_NONE;
	}
	bool IsCullingDisabled() {return RayTriangle == RayTriangleNC;}
	bool IsFrontFaceCullingEnabled() {return RayTriangle == RayTriangleFFC;}
	bool IsBackFaceCullingEnabled() {return RayTriangle == RayTriangleBFC;}
	void FlipCullMode()
	{
		// Switch between front- and backface culling
		if(RayTriangle == RayTriangleFFC)
		{
			RayTriangle = RayTriangleBFC;
			RaySphere = RaySphereBFC;
		}
		else
		{
			RayTriangle = RayTriangleFFC;
			RaySphere = RaySphereFFC;
		}
	}
}