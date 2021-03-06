//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class Color : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Color(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Color obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Color() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_Color(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public float r {
    set {
      ISMathPINVOKE.Color_r_set(swigCPtr, value);
    } 
    get {
      float ret = ISMathPINVOKE.Color_r_get(swigCPtr);
      return ret;
    } 
  }

  public float g {
    set {
      ISMathPINVOKE.Color_g_set(swigCPtr, value);
    } 
    get {
      float ret = ISMathPINVOKE.Color_g_get(swigCPtr);
      return ret;
    } 
  }

  public float b {
    set {
      ISMathPINVOKE.Color_b_set(swigCPtr, value);
    } 
    get {
      float ret = ISMathPINVOKE.Color_b_get(swigCPtr);
      return ret;
    } 
  }

  public float a {
    set {
      ISMathPINVOKE.Color_a_set(swigCPtr, value);
    } 
    get {
      float ret = ISMathPINVOKE.Color_a_get(swigCPtr);
      return ret;
    } 
  }

  public Color() : this(ISMathPINVOKE.new_Color__SWIG_0(), true) {
  }

  public Color(float r, float g, float b, float a) : this(ISMathPINVOKE.new_Color__SWIG_1(r, g, b, a), true) {
  }

  public Color(float r, float g, float b) : this(ISMathPINVOKE.new_Color__SWIG_2(r, g, b), true) {
  }

  public Color(SWIGTYPE_p_float rgba) : this(ISMathPINVOKE.new_Color__SWIG_3(SWIGTYPE_p_float.getCPtr(rgba)), true) {
  }

  public Color(uint argb) : this(ISMathPINVOKE.new_Color__SWIG_4(argb), true) {
  }

}
