#ifndef __ISMATH_H
#define __ISMATH_H


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#define FLOAT_INFINITY 1e20f
#define FLOAT_TOLERANCE 1e-6f
#ifndef PI
#define PI 3.141592654f
#endif


//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

struct Vector2;
struct Vector3;
struct Vector4;
struct Quaternion;
struct Color;
struct Color3;
struct Plane;
struct Matrix;

float saturate(float val);
Vector2* Vec2Set(Vector2* vout, float x, float y);
Vector2* Vec2Set(Vector2* vout, const float* xy);
Vector3* Vec3Set(Vector3* vout, float x, float y, float z);
Vector3* Vec3Set(Vector3* vout, const float* xyz);
Vector4* Vec4Set(Vector4* vout, float x, float y, float z, float w);
Vector4* Vec4Set(Vector4* vout, const float* xyzw);
Color* ClrSet(Color* cout, float r, float g, float b, float a = 1.0f);
Color* ClrSet(Color* cout, const float* rgba);
Color3* ClrSet(Color3* cout, float r, float g, float b);
Color3* ClrSet(Color3* cout, const float* rgb);
Quaternion* QuaternionSet(Quaternion* qout, float x, float y, float z, float w);
Quaternion* QuaternionSet(Quaternion* qout, const float* xyzw);
Color* ClrClone(Color* cout, const Color* c0);
Color3* ClrClone(Color3* cout, const Color3* c0);
float Vec2Dot(const Vector2 *v0, const Vector2 *v1);
float Vec3Dot(const Vector3 *v0, const Vector3 *v1);
float Vec4Dot(const Vector4 *v0, const Vector4 *v1);
Vector3* Vec3Cross(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1);
Vector3* Vec3Reflect(Vector3 *vResult, const Vector3 *n, const Vector3 *v);
Vector3* Vec3Refract(Vector3 *vResult, const Vector3 *n, const Vector3 *v, float ratio);
Vector2* GetUV(Vector2 t0, Vector2 t1, Vector2 t2, Vector2 tP, Vector2 *uv);
float Vec2Length(const Vector2 *v0);
float Vec2Length(const float x, const float y);
float Vec3Length(const Vector3 *v0);
float Vec3Length(const float x, const float y, const float z);
float Vec4Length(const Vector4 *v0);
float Vec4Length(const float x, const float y, const float z, const float w);
float Vec2LengthSq(const Vector2 *v0);
float Vec2LengthSq(const float x, const float y);
float Vec3LengthSq(const Vector3 *v0);
float Vec3LengthSq(const float x, const float y, const float z);
float Vec4LengthSq(const Vector4 *v0);
float Vec4LengthSq(const float x, const float y, const float z, const float w);
Vector2* Vec2Inverse(Vector2* vResult, const Vector2* v0);
Vector3* Vec3Inverse(Vector3* vResult, const Vector3* v0);
Vector4* Vec4Inverse(Vector4* vResult, const Vector4* v0);
Vector2* Vec2Invert(Vector2* v0);
Vector3* Vec3Invert(Vector3* v0);
Vector4* Vec4Invert(Vector4* v0);
Vector2* Vec2Normalize(Vector2 *vResult, const Vector2 *v0);
Vector2* Vec2Normalize(Vector2 *v0);
Vector3* Vec3Normalize(Vector3 *vResult, const Vector3 *v0);
Vector3* Vec3Normalize(Vector3 *v0);
Vector4* Vec4Normalize(Vector4 *vResult, const Vector4 *v0);
Vector4* Vec4Normalize(Vector4 *v0);
Vector4* Vec3Transform(Vector4 *vResult, const Vector3 *v0, const Matrix *m0);
Vector3* Vec3TransformCoord(Vector3 *vResult, const Vector3 *v0, const Matrix *m0);
Vector3* Vec3TransformNormal(Vector3 *vResult, const Vector3 *v0, const Matrix *m0);
Vector4* Vec4Transform(Vector4 *vResult, const Vector4 *v0, const Matrix *m0);
//Vector3* Vec3Unproject(Vector3 *vResult, const Vector3* v, const Matrix* worldviewproj);
Vector2* Vec2Lerp(Vector2 *vResult, const Vector2 *v0, const Vector2 *v1, float f);
Vector3* Vec3Lerp(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1, float f);
Color* ClrLerp(Color* cResult, const Color* c0, const Color* c1, float f);
Color3* ClrLerp(Color3* cResult, const Color3* c0, const Color3* c1, float f);
Vector2* Vec2BaryCentric(Vector2* vResult, const Vector2* v0, const Vector2* v1, const Vector2* v2, const Vector2* uv);
Vector3* Vec3BaryCentric(Vector3* vResult, const Vector3* v0, const Vector3* v1, const Vector3* v2, const Vector2* uv);
Vector2* Vec2InvBaryCentric(Vector2* vResult, const Vector2* v0, const Vector2* v1, const Vector2* v2, const Vector2* uv);
Vector3* Vec3Min(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1);
Vector3* Vec3Max(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1);
Quaternion* QuaternionIdentity(Quaternion *qResult);
Quaternion* QuaternionMultiply(Quaternion *qResult, const Quaternion *q0, const Quaternion *q1);
Quaternion* QuaternionMultiply(Quaternion *qResult, const Quaternion *q1);
Quaternion* QuaternionInverse(Quaternion *qResult, const Quaternion *q0);
Quaternion* QuaternionInvert(Quaternion *q0);
Quaternion* QuaternionNormalize(Quaternion *qResult, const Quaternion *q0);
Quaternion* QuaternionNormalize(Quaternion *q0);
Quaternion* QuaternionRotationX(Quaternion *qResult, float pitch);
Quaternion* QuaternionRotationY(Quaternion *qResult, float yaw);
Quaternion* QuaternionRotationZ(Quaternion *qResult, float roll);
Quaternion* QuaternionRotationAxis(Quaternion *qResult, const Vector3* axis, float angle);
Quaternion* QuaternionSlerp(Quaternion *qResult, const Quaternion *q0, const Quaternion *q1, float f);
void QuaternionToAxisAngle(const Quaternion *q0, Vector3* axis, float* angle);
Plane* PlaneFromPointNormal(Plane *pResult, const Vector3 *vPt, const Vector3 *vNml);
Plane* PlaneNormalize(Plane *pResult, const Plane *p0);
Plane* PlaneNormalize(Plane *pResult);
float PlaneDotCoord(const Plane *p0,const Vector3 *v0);
Vector3* PlaneIntersectLine(Vector3 *vResult, const Plane *p0, const Vector3 *v0, const Vector3 *v1);
Matrix* MatrixIdentity(Matrix *mResult);
Matrix* MatrixTranslation(Matrix *mResult, float x, float y, float z);
Matrix* MatrixTranslation(Matrix *mResult, const Vector3 *v0);
Matrix* MatrixTranslate(Matrix *mResult, const Matrix *m0, float x, float y, float z);
Matrix* MatrixTranslate(Matrix *mResult, const Matrix *m0, const Vector3 *v0);
Matrix* MatrixScaling(Matrix *mResult, float x, float y, float z);
Matrix* MatrixScaling(Matrix *mResult, const Vector3 *v0);
Matrix* MatrixScale(Matrix *mResult, const Matrix *m0, float x, float y, float z);
Matrix* MatrixScale(Matrix *mResult, const Matrix *m0, const Vector3 *v0);
Matrix* MatrixTranspose(Matrix *mResult, const Matrix *m0);
Matrix* MatrixInverseTranspose(Matrix *mResult, const Matrix *m0);
float MatrixDeterminant(const Matrix *m0);
//Matrix* MatrixInverse(Matrix *mResult, float *det, const Matrix *m0);
Matrix* MatrixInverse(Matrix *mResult, const Matrix *m0);
Matrix* MatrixPerspectiveLH(Matrix *mResult, float width, float height, float zn, float zf);
Matrix* MatrixPerspectiveRH(Matrix *mResult, float width, float height, float zn, float zf);
Matrix* MatrixPerspectiveFovLH(Matrix *mResult, float fovy, float aspect, float zn, float zf);
Matrix* MatrixPerspectiveFovRH(Matrix *mResult, float fovy, float aspect, float zn, float zf);
Matrix* MatrixOrthographicLH(Matrix *mResult, float width, float height, float zn, float zf);
Matrix* MatrixOrthographicRH(Matrix *mResult, float width, float height, float zn, float zf);
Matrix* MatrixRotationX(Matrix *mResult, float angle);
Matrix* MatrixRotateX(Matrix* mResult, const Matrix* m0, float angle);
Matrix* MatrixRotationY(Matrix *mResult, float angle);
Matrix* MatrixRotateY(Matrix* mResult, const Matrix* m0, float angle);
Matrix* MatrixRotationZ(Matrix *mResult, float angle);
Matrix* MatrixRotateZ(Matrix* mResult, const Matrix* m0, float angle);
Matrix* MatrixRotationYawPitchRoll(Matrix *mResult, float yaw, float pitch, float roll);
Matrix* MatrixRotationQuaternion(Matrix *mResult, const Quaternion* qrot);
Matrix* MatrixLookAtLH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up);
Matrix* MatrixLookAtRH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up);
Matrix* MatrixInvLookAtLH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up);
Matrix* MatrixInvLookAtRH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up);
bool RayIntersectsBox(const Vector3* raypos, const Vector3* raydir, const Vector3* boxmin, const Vector3* boxmax);


//-----------------------------------------------------------------------------
// CLASSES
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: Vector2
// Desc: Two component vector class
//-----------------------------------------------------------------------------
struct Vector2
{
public:
	float x, y;
	Vector2(){};
	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	};
	Vector2(const float* xy)
	{
		this->x = xy[0];
		this->y = xy[1];
	};
	operator float*() {return &x;}
	operator const float* () const {return &x;}
	// Binary operators
	Vector2 operator+(const Vector2& summand) const
	{
		return Vector2(x + summand.x, y + summand.y);
	};
	Vector2 operator-(const Vector2& subtrahend) const
	{
		return Vector2(x - subtrahend.x, y - subtrahend.y);
	};
	Vector2 operator*(const Vector2& factor) const
	{
		return Vector2(x * factor.x, y * factor.y);
	};
	Vector2 operator*(float factor) const
	{
		return Vector2(x * factor, y * factor);
	};
	Vector2 operator/(const Vector2& divisor) const
	{
		return Vector2(x / divisor.x, y / divisor.y);
	};
	Vector2 operator/(float divisor) const
	{
		float invdiv = 1.0f / divisor;
		return Vector2(x * invdiv, y * invdiv);
	};
	// Assignment operators
	/*Vector2& operator=(const Vector2 &reference)
	{
		if(this == &reference)
			return *this;
		this->x = reference.x;
		this->y = reference.y;
		return *this;
	};*/
	Vector2& operator+=(const Vector2& summand)
	{
		x += summand.x; y += summand.y;
		return *this;
	};
	Vector2& operator+=(float summand)
	{
		x += summand; y += summand;
		return *this;
	};
	Vector2& operator-=(const Vector2& subtrahend)
	{
		x -= subtrahend.x; y -= subtrahend.y;
		return *this;
	};
	Vector2& operator-=(float subtrahend)
	{
		x -= subtrahend; y -= subtrahend;
		return *this;
	};
	Vector2& operator*=(const Vector2& factor)
	{
		x *= factor.x; y *= factor.y;
		return *this;
	};
	Vector2& operator*=(float factor)
	{
		x *= factor; y *= factor;
		return *this;
	};
	Vector2& operator/=(const Vector2& divisor)
	{
		x /= divisor.x; y /= divisor.y;
		return *this;
	};
	Vector2& operator/=(float divisor)
	{
		float invdiv = 1.0f / divisor;
		x *= invdiv; y *= invdiv;
		return *this;
	};
	// Comparison operators
	bool operator==(const Vector2& reference) const
	{
		return x == reference.x && y == reference.y;
	};
	bool operator!=(const Vector2& reference) const
	{
		return x != reference.x || y != reference.y;
	};
	bool operator<(const Vector2& reference) const
	{
		return x*x + y*y < reference.x*reference.x + reference.y*reference.y;
	};
	bool operator>(const Vector2& reference) const
	{
		return x*x + y*y > reference.x*reference.x + reference.y*reference.y;
	};
	bool operator<=(const Vector2& reference) const
	{
		return x*x + y*y <= reference.x*reference.x + reference.y*reference.y;
	};
	bool operator>=(const Vector2& reference) const
	{
		return x*x + y*y >= reference.x*reference.x + reference.y*reference.y;
	};
};
Vector2* Vec2Add(Vector2* vout, const Vector2* v0, const Vector2* v1);
Vector2* Vec2Sub(Vector2* vout, const Vector2* v0, const Vector2* v1);
Vector2* Vec2Scale(Vector2* vout, const Vector2* vin, float scale);
Vector2* Vec2AddScaled(Vector2* vout, const Vector2* v0, const Vector2* v1, float v1scale);

//-----------------------------------------------------------------------------
// Name: Vector3
// Desc: Three component vector class
//-----------------------------------------------------------------------------
class hkVector4;
struct Vector3
{
public:
	float x, y, z;
	Vector3(){};
	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	};
	Vector3(const float* xyz)
	{
		this->x = xyz[0];
		this->y = xyz[1];
		this->z = xyz[2];
	};
	operator float*() {return &x;}
	operator const float* () const {return &x;}
	Vector3(const hkVector4& hkvec);
	operator hkVector4() const;
	// Binary operators
	Vector3 operator+(const Vector3& summand) const
	{
		return Vector3(x + summand.x, y + summand.y, z + summand.z);
	};
	Vector3 operator-(const Vector3& subtrahend) const
	{
		return Vector3(x - subtrahend.x, y - subtrahend.y, z - subtrahend.z);
	};
	Vector3 operator*(const Vector3& factor) const
	{
		return Vector3(x * factor.x, y * factor.y, z * factor.z);
	};
	Vector3 operator*(float factor) const
	{
		return Vector3(x * factor, y * factor, z * factor);
	};
	Vector3 operator/(const Vector3& divisor) const
	{
		return Vector3(x / divisor.x, y / divisor.y, z / divisor.z);
	};
	Vector3 operator/(float divisor) const
	{
		float invdiv = 1.0f / divisor;
		return Vector3(x * invdiv, y * invdiv, z * invdiv);
	};
	// Assignment operators
	/*Vector3& operator=(const Vector3& reference)
	{
		if(this == &reference)
			return *this;
		this->x = reference.x;
		this->y = reference.y;
		this->z = reference.z;
		return *this;
	};*/
	Vector3& operator+=(const Vector3& summand)
	{
		x += summand.x; y += summand.y; z += summand.z;
		return *this;
	};
	Vector3& operator+=(float summand)
	{
		x += summand; y += summand; z += summand;
		return *this;
	};
	Vector3& operator-=(const Vector3& subtrahend)
	{
		x -= subtrahend.x; y -= subtrahend.y; z -= subtrahend.z;
		return *this;
	};
	Vector3& operator-=(float subtrahend)
	{
		x -= subtrahend; y -= subtrahend; z -= subtrahend;
		return *this;
	};
	Vector3& operator*=(const Vector3& factor)
	{
		x *= factor.x; y *= factor.y; z *= factor.z;
		return *this;
	};
	Vector3& operator*=(float factor)
	{
		x *= factor; y *= factor; z *= factor;
		return *this;
	};
	Vector3& operator/=(const Vector3& divisor)
	{
		x /= divisor.x; y /= divisor.y; z /= divisor.z;
		return *this;
	};
	Vector3& operator/=(float divisor)
	{
		float invdiv = 1.0f / divisor;
		x *= invdiv; y *= invdiv; z *= invdiv;
		return *this;
	};
	// Comparison operators
	bool operator==(const Vector3& reference) const
	{
		return x == reference.x && y == reference.y && z == reference.z;
	};
	bool operator!=(const Vector3& reference) const
	{
		return x != reference.x || y != reference.y || z != reference.z;
	};
	bool operator<(const Vector3& reference) const
	{
		return x*x + y*y + z*z < reference.x*reference.x + reference.y*reference.y +
								 reference.z*reference.z;
	};
	bool operator>(const Vector3& reference) const
	{
		return x*x + y*y + z*z > reference.x*reference.x + reference.y*reference.y +
								 reference.z*reference.z;
	};
	bool operator<=(const Vector3& reference) const
	{
		return x*x + y*y + z*z <= reference.x*reference.x + reference.y*reference.y +
								  reference.z*reference.z;
	};
	bool operator>=(const Vector3& reference) const
	{
		return x*x + y*y + z*z >= reference.x*reference.x + reference.y*reference.y +
								  reference.z*reference.z;
	};
};
Vector3* Vec3Add(Vector3* vout, const Vector3* v0, const Vector3* v1);
Vector3* Vec3Sub(Vector3* vout, const Vector3* v0, const Vector3* v1);
Vector3* Vec3Scale(Vector3* vout, const Vector3* vin, float scale);
Vector3* Vec3AddScaled(Vector3* vout, const Vector3* v0, const Vector3* v1, float v1scale);

//-----------------------------------------------------------------------------
// Name: Vector4
// Desc: Four component vector class
//-----------------------------------------------------------------------------
struct Vector4
{
public:
	float x, y, z, w;
	Vector4(){};
	Vector4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	};
	Vector4(const float* xyzw)
	{
		this->x = xyzw[0];
		this->y = xyzw[1];
		this->z = xyzw[2];
		this->w = xyzw[3];
	};
	Vector4(const Vector3* v0, float w)
	{
		this->x = v0->x;
		this->y = v0->y;
		this->z = v0->z;
		this->w = w;
	};
	operator float*() {return &x;}
	operator const float* () const {return &x;}
	// Binary operators
	Vector4 operator+(const Vector4& summand) const
	{
		return Vector4(x + summand.x, y + summand.y, z + summand.z, w + summand.w);
	};
	Vector4 operator-(const Vector4& subtrahend) const
	{
		return Vector4(x - subtrahend.x, y - subtrahend.y, z - subtrahend.z, w - subtrahend.w);
	};
	Vector4 operator*(const Vector4& factor) const
	{
		return Vector4(x * factor.x, y * factor.y, z * factor.z, w * factor.w);
	};
	Vector4 operator*(float factor) const
	{
		return Vector4(x * factor, y * factor, z * factor, w * factor);
	};
	Vector4 operator/(const Vector4& divisor) const
	{
		return Vector4(x / divisor.x, y / divisor.y, z / divisor.z, w / divisor.w);
	};
	Vector4 operator/(float divisor) const
	{
		float invdiv = 1.0f / divisor;
		return Vector4(x * invdiv, y * invdiv, z * invdiv, w * invdiv);
	};
	// Assignment operators
	/*Vector4& operator=(const Vector4& reference)
	{
		if(this == &reference)
			return *this;
		this->x = reference.x;
		this->y = reference.y;
		this->z = reference.z;
		this->w = reference.w;
		return *this;
	};*/
	Vector4& operator+=(const Vector4& summand)
	{
		x += summand.x; y += summand.y; z += summand.z; w += summand.w;
		return *this;
	};
	Vector4& operator+=(float summand)
	{
		x += summand; y += summand; z += summand; w += summand;
		return *this;
	};
	Vector4& operator-=(const Vector4& subtrahend)
	{
		x -= subtrahend.x; y -= subtrahend.y; z -= subtrahend.z; w -= subtrahend.w;
		return *this;
	};
	Vector4& operator-=(float subtrahend)
	{
		x -= subtrahend; y -= subtrahend; z -= subtrahend; w -= subtrahend;
		return *this;
	};
	Vector4& operator*=(const Vector4& factor)
	{
		x *= factor.x; y *= factor.y; z *= factor.z; w *= factor.w;
		return *this;
	};
	Vector4& operator*=(float factor)
	{
		x *= factor; y *= factor; z *= factor; w *= factor;
		return *this;
	};
	Vector4& operator/=(const Vector4& divisor)
	{
		x /= divisor.x; y /= divisor.y; z /= divisor.z; w /= divisor.w;
		return *this;
	};
	Vector4& operator/=(float divisor)
	{
		float invdiv = 1.0f / divisor;
		x *= invdiv; y *= invdiv; z *= invdiv; w *= invdiv;
		return *this;
	};
	// Comparison operators
	bool operator==(const Vector4& reference) const
	{
		return x == reference.x && y == reference.y && z == reference.z && w == reference.w;
	};
	bool operator!=(const Vector4& reference) const
	{
		return x != reference.x || y != reference.y || z != reference.z || w != reference.w;
	};
	bool operator<(const Vector4& reference) const
	{
		return x*x + y*y + z*z + w*w < reference.x*reference.x + reference.y*reference.y +
									 reference.z*reference.z + reference.w*reference.w;
	};
	bool operator>(const Vector4& reference) const
	{
		return x*x + y*y + z*z + w*w > reference.x*reference.x + reference.y*reference.y +
									 reference.z*reference.z + reference.w*reference.w;
	};
	bool operator<=(const Vector4& reference) const
	{
		return x*x + y*y + z*z + w*w <= reference.x*reference.x + reference.y*reference.y +
									  reference.z*reference.z + reference.w*reference.w;
	};
	bool operator>=(const Vector4& reference) const
	{
		return x*x + y*y + z*z + w*w >= reference.x*reference.x + reference.y*reference.y +
									  reference.z*reference.z + reference.w*reference.w;
	};
};

//-----------------------------------------------------------------------------
// Name: Quaternion
// Desc: Four component quaternion class
//-----------------------------------------------------------------------------
class hkQuaternion;
struct Quaternion
{
public:
	float x, y, z, w;
	Quaternion(){};
	Quaternion(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	};
	Quaternion(const float* xyzw)
	{
		this->x = xyzw[0];
		this->y = xyzw[1];
		this->z = xyzw[2];
		this->w = xyzw[3];
	};
	operator float*() {return &x;}
	operator const float* () const {return &x;}
	Quaternion(const hkQuaternion& hkquat);
	operator hkQuaternion() const;
	// Binary operators
	Quaternion operator+(const Quaternion& summand) const
	{
		return Quaternion(x + summand.x, y + summand.y, z + summand.z, w + summand.w);
	};
	Quaternion operator-(const Quaternion& subtrahend) const
	{
		return Quaternion(x - subtrahend.x, y - subtrahend.y, z - subtrahend.z, w - subtrahend.w);
	};
	Quaternion operator*(const Quaternion& factor) const
	{
		Quaternion qResult;
		QuaternionMultiply(&qResult, this, &factor);
		return qResult;
	};
	Quaternion operator*(float factor) const
	{
		return Quaternion(x * factor, y * factor, z * factor, w * factor);
	};
	Quaternion operator/(float divisor) const
	{
		float invdiv = 1.0f / divisor;
		return Quaternion(x * invdiv, y * invdiv, z * invdiv, w * invdiv);
	};
	// Assignment operators
	/*Quaternion& operator=(const Quaternion& reference)
	{
		if(this == &reference)
			return *this;
		this->x = reference.x;
		this->y = reference.y;
		this->z = reference.z;
		this->w = reference.w;
		return *this;
	};*/
	Quaternion& operator+=(const Quaternion& summand)
	{
		x += summand.x; y += summand.y; z += summand.z; w += summand.w;
		return *this;
	};
	Quaternion& operator-=(const Quaternion& subtrahend)
	{
		x -= subtrahend.x; y -= subtrahend.y; z -= subtrahend.z; w -= subtrahend.w;
		return *this;
	};
	Quaternion& operator*=(const Quaternion& factor)
	{
		QuaternionMultiply(this, &factor);
		return *this;
	};
	Quaternion& operator*=(float factor)
	{
		x *= factor; y *= factor; z *= factor; w *= factor;
		return *this;
	};
	Quaternion& operator/=(float divisor)
	{
		float invdiv = 1.0f / divisor;
		x *= invdiv; y *= invdiv; z *= invdiv; w *= invdiv;
		return *this;
	};
	// Comparison operators
	bool operator==(const Quaternion& reference) const
	{
		return x == reference.x && y == reference.y && z == reference.z && w == reference.w;
	};
	bool operator!=(const Quaternion& reference) const
	{
		return x != reference.x || y != reference.y || z != reference.z || w != reference.w;
	};

	Vector3* ComputeAxisWrapper() const;
	float ComputeAngleWrapper() const;
};

//-----------------------------------------------------------------------------
// Name: Color
// Desc: Four component color class
//-----------------------------------------------------------------------------
struct Color
{
public:
	float r, g, b, a;
	Color(){};
	Color(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	};
	Color(const float* rgba)
	{
		this->r = rgba[0];
		this->g = rgba[1];
		this->b = rgba[2];
		this->a = rgba[3];
	};
	Color(unsigned int argb)
	{
		this->r = ((argb >> 16) & 0xFF) / 255.0f;
		this->g = ((argb >>  8) & 0xFF) / 255.0f;
		this->b = ((argb >>  0) & 0xFF) / 255.0f;
		this->a = ((argb >> 24) & 0xFF) / 255.0f;
	};
	operator unsigned int() const
	{
		return ((unsigned int)(saturate(r) * 255.0f)) | ((unsigned int)(saturate(g) * 255.0f) << 8) | ((unsigned int)(saturate(b) * 255.0f) << 16) | ((unsigned int)(saturate(a) * 255.0f) << 24);
	}
	operator float*() {return &r;}
	operator const float* () const {return &r;}

	unsigned int ToDWORD() const
	{
		return (unsigned int)(saturate(r) * 255.0f) | ((unsigned int)(saturate(g) * 255.0f) << 8) | ((unsigned int)(saturate(b) * 255.0f) << 16);
	}
	unsigned int ToDWORD(float blendfactor) const
	{
		blendfactor = saturate(blendfactor);
		return (unsigned int)(saturate(r) * 255.0f * blendfactor) | ((unsigned int)(saturate(g) * 255.0f * blendfactor) << 8) | ((unsigned int)(saturate(b) * 255.0f * blendfactor) << 16);
	}

	bool operator==(const Color& other) const
		{return this->r == other.r && this->g == other.g && this->b == other.b && this->a == other.a;}
	bool operator!=(const Color& other) const
		{return this->r != other.r || this->g != other.g || this->b != other.b || this->a != other.a;}

/*D3DXCOLOR( DWORD argb );
D3DXCOLOR( CONST FLOAT * );
D3DXCOLOR( CONST D3DXFLOAT16 * );
D3DXCOLOR( CONST D3DCOLORVALUE& );

// casting
operator D3DCOLORVALUE* ();
operator CONST D3DCOLORVALUE* () const;

operator D3DCOLORVALUE& ();
operator CONST D3DCOLORVALUE& () const;

// assignment operators
D3DXCOLOR& operator += ( CONST D3DXCOLOR& );
D3DXCOLOR& operator -= ( CONST D3DXCOLOR& );
D3DXCOLOR& operator *= ( FLOAT );
D3DXCOLOR& operator /= ( FLOAT );

// unary operators
D3DXCOLOR operator + () const;
D3DXCOLOR operator - () const;

// binary operators
D3DXCOLOR operator + ( CONST D3DXCOLOR& ) const;
D3DXCOLOR operator - ( CONST D3DXCOLOR& ) const;
D3DXCOLOR operator * ( FLOAT ) const;
D3DXCOLOR operator / ( FLOAT ) const;

friend D3DXCOLOR operator * ( FLOAT, CONST D3DXCOLOR& );

BOOL operator == ( CONST D3DXCOLOR& ) const;
BOOL operator != ( CONST D3DXCOLOR& ) const;*/
};
Color* ClrAdd(Color* cout, const Color* c0, const Color* c1);
Color* ClrSub(Color* cout, const Color* c0, const Color* c1);
Color* ClrMul(Color* cout, const Color* c0, const Color* c1);
Color* ClrScale(Color* cout, const Color* cin, float scale);
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, float c1scale);
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, const Color* c1scale0, float c1scale1);
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, const Color* c1scale0, const Color* c1scale1, float c1scale2);

//-----------------------------------------------------------------------------
// Name: Color3
// Desc: Three component color class
//-----------------------------------------------------------------------------
struct Color3
{
public:
	float r, g, b;
	Color3(){};
	Color3(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	};
	Color3(const float* rgb)
	{
		this->r = rgb[0];
		this->g = rgb[1];
		this->b = rgb[2];
	};
	Color3(unsigned int rgb)
	{
		this->r = ((rgb >> 16) & 0xFF) / 255.0f;
		this->g = ((rgb >>  8) & 0xFF) / 255.0f;
		this->b = ((rgb >>  0) & 0xFF) / 255.0f;
	};
	operator unsigned int() const
	{
		return ((unsigned int)(saturate(r) * 255.0f)) | ((unsigned int)(saturate(g) * 255.0f) << 8) | ((unsigned int)(saturate(b) * 255.0f) << 16);
	}
	operator float*() {return &r;}
	operator const float* () const {return &r;}

	unsigned int ToDWORD() const
	{
		return (unsigned int)(saturate(r) * 255.0f) | ((unsigned int)(saturate(g) * 255.0f) << 8) | ((unsigned int)(saturate(b) * 255.0f) << 16);
	}
	unsigned int ToDWORD(float blendfactor) const
	{
		blendfactor = saturate(blendfactor);
		return (unsigned int)(saturate(r) * 255.0f * blendfactor) | ((unsigned int)(saturate(g) * 255.0f * blendfactor) << 8) | ((unsigned int)(saturate(b) * 255.0f * blendfactor) << 16);
	}
};
Color3* ClrAdd(Color3* cout, const Color3* c0, const Color3* c1);
Color3* ClrSub(Color3* cout, const Color3* c0, const Color3* c1);
Color3* ClrMul(Color3* cout, const Color3* c0, const Color3* c1);
Color3* ClrScale(Color3* cout, const Color3* cin, float scale);
Color3* ClrAddScaled(Color3* cout, const Color3* c0, const Color3* c1, float c1scale);
Color3* ClrAddScaled(Color3* cout, const Color3* c0, const Color3* c1, const Color3* c1scale0, float c1scale1);
Color3* ClrAddScaled(Color3* cout, const Color3* c0, const Color3* c1, const Color3* c1scale0, const Color3* c1scale1, float c1scale2);

//-----------------------------------------------------------------------------
// Name: Plane
// Desc: Four component plane class
//-----------------------------------------------------------------------------
struct Plane
{
public:
	float a, b, c, d;
	Plane() {}
	Plane(float a, float b, float c, float d)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
	};

/*Plane( CONST FLOAT* );
Plane( CONST D3DXFLOAT16* );

// casting
operator FLOAT* ();
operator CONST FLOAT* () const;

// assignment operators
D3DXPLANE& operator *= ( FLOAT );
D3DXPLANE& operator /= ( FLOAT );

// unary operators
D3DXPLANE operator + () const;
D3DXPLANE operator - () const;

// binary operators
D3DXPLANE operator * ( FLOAT ) const;
D3DXPLANE operator / ( FLOAT ) const;

friend D3DXPLANE operator * ( FLOAT, CONST D3DXPLANE& );

BOOL operator == ( CONST D3DXPLANE& ) const;
BOOL operator != ( CONST D3DXPLANE& ) const;*/
};


//-----------------------------------------------------------------------------
// Name: Matrix
// Desc: Four component square matrix
//-----------------------------------------------------------------------------
struct Matrix
{
public:
	float _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44;
	Matrix(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24,
		   float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44)
	{
		this->_11 = _11; this->_12 = _12; this->_13 = _13; this->_14 = _14;
		this->_21 = _21; this->_22 = _22; this->_23 = _23; this->_24 = _24;
		this->_31 = _31; this->_32 = _32; this->_33 = _33; this->_34 = _34;
		this->_41 = _41; this->_42 = _42; this->_43 = _43; this->_44 = _44;
	};
	Matrix(){};
	operator float*() {return &_11;}
	operator const float* () const {return &_11;}
	Matrix operator+(const Matrix &summand) const
	{
		return Matrix(_11 + summand._11, _12 + summand._12, _13 + summand._13, _14 + summand._14,
					  _21 + summand._21, _22 + summand._22, _23 + summand._23, _24 + summand._24,
					  _31 + summand._31, _32 + summand._32, _33 + summand._33, _34 + summand._34,
					  _41 + summand._41, _42 + summand._42, _43 + summand._43, _44 + summand._44);
	};
	Matrix operator-(const Matrix &subtrahend) const
	{
		return Matrix(_11 - subtrahend._11, _12 - subtrahend._12, _13 - subtrahend._13, _14 - subtrahend._14,
					  _21 - subtrahend._21, _22 - subtrahend._22, _23 - subtrahend._23, _24 - subtrahend._24,
					  _31 - subtrahend._31, _32 - subtrahend._32, _33 - subtrahend._33, _34 - subtrahend._34,
					  _41 - subtrahend._41, _42 - subtrahend._42, _43 - subtrahend._43, _44 - subtrahend._44);
	};
	Matrix operator*(const Matrix &factor) const
	{
		return Matrix(_11 * factor._11 + _12 * factor._21 + _13 * factor._31 + _14 * factor._41,
					  _11 * factor._12 + _12 * factor._22 + _13 * factor._32 + _14 * factor._42,
					  _11 * factor._13 + _12 * factor._23 + _13 * factor._33 + _14 * factor._43,
					  _11 * factor._14 + _12 * factor._24 + _13 * factor._34 + _14 * factor._44,
					  _21 * factor._11 + _22 * factor._21 + _23 * factor._31 + _24 * factor._41,
					  _21 * factor._12 + _22 * factor._22 + _23 * factor._32 + _24 * factor._42,
					  _21 * factor._13 + _22 * factor._23 + _23 * factor._33 + _24 * factor._43,
					  _21 * factor._14 + _22 * factor._24 + _23 * factor._34 + _24 * factor._44,
					  _31 * factor._11 + _32 * factor._21 + _33 * factor._31 + _34 * factor._41,
					  _31 * factor._12 + _32 * factor._22 + _33 * factor._32 + _34 * factor._42,
					  _31 * factor._13 + _32 * factor._23 + _33 * factor._33 + _34 * factor._43,
					  _31 * factor._14 + _32 * factor._24 + _33 * factor._34 + _34 * factor._44,
					  _41 * factor._11 + _42 * factor._21 + _43 * factor._31 + _44 * factor._41,
					  _41 * factor._12 + _42 * factor._22 + _43 * factor._32 + _44 * factor._42,
					  _41 * factor._13 + _42 * factor._23 + _43 * factor._33 + _44 * factor._43,
					  _41 * factor._14 + _42 * factor._24 + _43 * factor._34 + _44 * factor._44);
	};
	Vector4 operator*(const Vector4 &factor) const
	{
		return Vector4(_11 * factor.x + _12 * factor.y + _13 * factor.z + _14 * factor.w,
					   _21 * factor.x + _22 * factor.y + _23 * factor.z + _24 * factor.w,
					   _31 * factor.x + _32 * factor.y + _33 * factor.z + _34 * factor.w,
					   _41 * factor.x + _42 * factor.y + _43 * factor.z + _44 * factor.w);
	};
};

namespace Intersection
{
	typedef float (*RAY_TRIANGLE_INTERSECTION_FUNCTION)(const Vector3* raypos, const Vector3* raydir, const Vector3* v0, const Vector3* v1, const Vector3* v2, Vector2* uv);
	extern RAY_TRIANGLE_INTERSECTION_FUNCTION RayTriangle;
	float RayTriangleNC(const Vector3* raypos, const Vector3* raydir, const Vector3* v0, const Vector3* v1, const Vector3* v2, Vector2* uv);
	float RayTriangleBFC(const Vector3* raypos, const Vector3* raydir, const Vector3* v0, const Vector3* v1, const Vector3* v2, Vector2* uv);
	float RayTriangleFFC(const Vector3* raypos, const Vector3* raydir, const Vector3* v0, const Vector3* v1, const Vector3* v2, Vector2* uv);
	typedef float (*RAY_SPHERE_INTERSECTION_FUNCTION)(const Vector3* raypos, const Vector3* raydir, const Vector3* spherecenter, float sphereradius);
	extern RAY_SPHERE_INTERSECTION_FUNCTION RaySphere;
	float RaySphereBFC(const Vector3* raypos, const Vector3* raydir, const Vector3* spherecenter, float sphereradius);
	float RaySphereFFC(const Vector3* raypos, const Vector3* raydir, const Vector3* spherecenter, float sphereradius);
	float RayMovingSphereBFC(const Vector3* raypos, const Vector3* raydir, const Vector3* spherecenter, float sphereradius, const Vector3* spherevelocity);

	enum CullMode
	{
		_CM_NONE, CM_FRONT, CM_BACK
	};
	void SetCullMode(CullMode cm);
	CullMode GetCullMode();
	bool IsCullingDisabled();
	bool IsFrontFaceCullingEnabled();
	bool IsBackFaceCullingEnabled();
	void FlipCullMode();
}

class IPythonScriptEngine;
void RegisterScriptableMathClasses(const IPythonScriptEngine* pse);

#endif