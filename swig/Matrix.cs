//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class Matrix : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Matrix(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Matrix obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Matrix() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_Matrix(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public float _11 {
    set {
      IronSightEnginePINVOKE.Matrix__11_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__11_get(swigCPtr);
      return ret;
    } 
  }

  public float _12 {
    set {
      IronSightEnginePINVOKE.Matrix__12_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__12_get(swigCPtr);
      return ret;
    } 
  }

  public float _13 {
    set {
      IronSightEnginePINVOKE.Matrix__13_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__13_get(swigCPtr);
      return ret;
    } 
  }

  public float _14 {
    set {
      IronSightEnginePINVOKE.Matrix__14_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__14_get(swigCPtr);
      return ret;
    } 
  }

  public float _21 {
    set {
      IronSightEnginePINVOKE.Matrix__21_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__21_get(swigCPtr);
      return ret;
    } 
  }

  public float _22 {
    set {
      IronSightEnginePINVOKE.Matrix__22_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__22_get(swigCPtr);
      return ret;
    } 
  }

  public float _23 {
    set {
      IronSightEnginePINVOKE.Matrix__23_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__23_get(swigCPtr);
      return ret;
    } 
  }

  public float _24 {
    set {
      IronSightEnginePINVOKE.Matrix__24_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__24_get(swigCPtr);
      return ret;
    } 
  }

  public float _31 {
    set {
      IronSightEnginePINVOKE.Matrix__31_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__31_get(swigCPtr);
      return ret;
    } 
  }

  public float _32 {
    set {
      IronSightEnginePINVOKE.Matrix__32_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__32_get(swigCPtr);
      return ret;
    } 
  }

  public float _33 {
    set {
      IronSightEnginePINVOKE.Matrix__33_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__33_get(swigCPtr);
      return ret;
    } 
  }

  public float _34 {
    set {
      IronSightEnginePINVOKE.Matrix__34_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__34_get(swigCPtr);
      return ret;
    } 
  }

  public float _41 {
    set {
      IronSightEnginePINVOKE.Matrix__41_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__41_get(swigCPtr);
      return ret;
    } 
  }

  public float _42 {
    set {
      IronSightEnginePINVOKE.Matrix__42_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__42_get(swigCPtr);
      return ret;
    } 
  }

  public float _43 {
    set {
      IronSightEnginePINVOKE.Matrix__43_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__43_get(swigCPtr);
      return ret;
    } 
  }

  public float _44 {
    set {
      IronSightEnginePINVOKE.Matrix__44_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.Matrix__44_get(swigCPtr);
      return ret;
    } 
  }

  public Matrix(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) : this(IronSightEnginePINVOKE.new_Matrix__SWIG_0(_11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44), true) {
  }

  public Matrix() : this(IronSightEnginePINVOKE.new_Matrix__SWIG_1(), true) {
  }

}
