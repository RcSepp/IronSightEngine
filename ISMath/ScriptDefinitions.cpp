#ifdef MATH_SCRIPTABLE
#include "ISMath.h"
#include "ISPythonScriptEngine.h"

//#define BOOST_PYTHON_STATIC_LIB
#include <boost\python.hpp>
using namespace boost::python;

Quaternion* QuaternionRotationXWrapper(float pitch)
{
	Quaternion* qResult = new Quaternion();
	QuaternionRotationX(qResult, pitch);
	return qResult;
}
Quaternion* QuaternionRotationYWrapper(float yaw)
{
	Quaternion* qResult = new Quaternion();
	QuaternionRotationY(qResult, yaw);
	return qResult;
}
Quaternion* QuaternionRotationZWrapper(float roll)
{
	Quaternion* qResult = new Quaternion();
	QuaternionRotationZ(qResult, roll);
	return qResult;
}
Quaternion* QuaternionRotationAxisWrapper(const Vector3* axis, float angle)
{
	Quaternion* qResult = new Quaternion();
	QuaternionRotationAxis(qResult, axis, angle);
	return qResult;
}
Quaternion* QuaternionInverseWrapper(const Quaternion* q0)
{
	Quaternion* qResult = new Quaternion();
	QuaternionInverse(qResult, q0);
	return qResult;
}

Vector3* MulQuatVec3(const Quaternion& quat, const Vector3& vec) //EDIT: Test and clean up
{
	float num = quat.x * 2.0f;
	float num2 = quat.y * 2.0f;
	float num3 = quat.z * 2.0f;
	float num4 = quat.x * num;
	float num5 = quat.y * num2;
	float num6 = quat.z * num3;
	float num7 = quat.x * num2;
	float num8 = quat.x * num3;
	float num9 = quat.y * num3;
	float num10 = quat.w * num;
	float num11 = quat.w * num2;
	float num12 = quat.w * num3;
	Vector3* result = new Vector3();
	result->x = (1.0f - (num5 + num6)) * vec.x + (num7 - num12) * vec.y + (num8 + num11) * vec.z;
	result->y = (num7 + num12) * vec.x + (1.0f - (num4 + num6)) * vec.y + (num9 - num10) * vec.z;
	result->z = (num8 - num11) * vec.x + (num9 + num10) * vec.y + (1.0f - (num4 + num5)) * vec.z;
	return result;
}

BOOST_PYTHON_MODULE(Math)
{
	// >>> Structs

	class_<Vector2>("Vector2")
		.def(init<float, float>())
		.def_readwrite("x", &Vector2::x)
		.def_readwrite("y", &Vector2::y)
		.def(self += Vector2())
		.def(self -= Vector2())
		.def(self *= float())
		.def(self /= float())
		.def(self + Vector2())
		.def(self - Vector2())
		.def(self * float())
		.def(self / float())
		.def(self == Vector2())
		.def(self != Vector2())
	;
	class_<Vector3>("Vector3")
		.def(init<float, float, float>())
		.def_readwrite("x", &Vector3::x)
		.def_readwrite("y", &Vector3::y)
		.def_readwrite("z", &Vector3::z)
		.def(self += Vector3())
		.def(self -= Vector3())
		.def(self *= float())
		.def(self /= float())
		.def(self + Vector3())
		.def(self - Vector3())
		.def(self * float())
		.def(self / float())
		.def(self == Vector3())
		.def(self != Vector3())
	;
	class_<Quaternion>("Quaternion")
		.def(init<float, float, float, float>())
		.def_readwrite("x", &Quaternion::x)
		.def_readwrite("y", &Quaternion::y)
		.def_readwrite("z", &Quaternion::z)
		.def_readwrite("w", &Quaternion::w)
		.def(self += Quaternion())
		.def(self -= Quaternion())
		.def(self *= Quaternion())
		.def(self *= float())
		.def(self /= float())
		.def(self + Quaternion())
		.def(self - Quaternion())
		.def(self * Quaternion())
		.def("MulVec", &MulQuatVec3, POLICY_MANAGED_BY_PYTHON)
		.def(self * float())
		.def(self / float())
		.def(self == Quaternion())
		.def(self != Quaternion())
		.def("RotationX", &QuaternionRotationXWrapper, POLICY_MANAGED_BY_PYTHON).staticmethod("RotationX")
		.def("RotationY", &QuaternionRotationYWrapper, POLICY_MANAGED_BY_PYTHON).staticmethod("RotationY")
		.def("RotationZ", &QuaternionRotationZWrapper, POLICY_MANAGED_BY_PYTHON).staticmethod("RotationZ")
		.def("RotationAxis", &QuaternionRotationAxisWrapper, POLICY_MANAGED_BY_PYTHON).staticmethod("RotationAxis")
		.def("Inverse", &QuaternionInverseWrapper, POLICY_MANAGED_BY_PYTHON).staticmethod("Inverse")
		.def("ComputeAxis", &Quaternion::ComputeAxisWrapper, POLICY_MANAGED_BY_PYTHON)
		.def("ComputeAngle", &Quaternion::ComputeAngleWrapper)
	;
	class_<Color>("Color")
		.def(init<UINT>())
		.def(init<float, float, float, optional<float>>())
		.def_readwrite("r", &Color::r)
		.def_readwrite("g", &Color::g)
		.def_readwrite("b", &Color::b)
		.def_readwrite("a", &Color::a)
/*		.def(self += Color())
		.def(self -= Color())
		.def(self *= float())
		.def(self /= float())
		.def(self + Color())
		.def(self - Color())
		.def(self * float())
		.def(self / float())
		.def(self == Color())
		.def(self != Color())*/ //EDIT11
	;
	class_<Matrix>("Matrix")
		//.def(init<float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float>())
		.def_readwrite("_11", &Matrix::_11)
		.def_readwrite("_12", &Matrix::_12)
		.def_readwrite("_13", &Matrix::_13)
		.def_readwrite("_14", &Matrix::_14)
		.def_readwrite("_21", &Matrix::_21)
		.def_readwrite("_22", &Matrix::_22)
		.def_readwrite("_23", &Matrix::_23)
		.def_readwrite("_24", &Matrix::_24)
		.def_readwrite("_31", &Matrix::_31)
		.def_readwrite("_32", &Matrix::_32)
		.def_readwrite("_33", &Matrix::_33)
		.def_readwrite("_34", &Matrix::_34)
		.def_readwrite("_41", &Matrix::_41)
		.def_readwrite("_42", &Matrix::_42)
		.def_readwrite("_43", &Matrix::_43)
		.def_readwrite("_44", &Matrix::_44)
		.def(self + Matrix())
		.def(self - Matrix())
		.def(self * Matrix())
	;
}

void RegisterScriptableMathClasses(const IPythonScriptEngine* pse)
{
	if(pse)
		pse->AddModule("Math", &PyInit_Math);
}
#endif