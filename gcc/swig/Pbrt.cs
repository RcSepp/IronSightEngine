//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class Pbrt : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Pbrt(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Pbrt obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Pbrt() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_Pbrt(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public Pbrt() : this(ISMathPINVOKE.new_Pbrt__SWIG_0(), true) {
  }

  public Pbrt(SWIGTYPE_p_IPbrt src) : this(ISMathPINVOKE.new_Pbrt__SWIG_1(SWIGTYPE_p_IPbrt.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IPbrt __deref__() {
    global::System.IntPtr cPtr = ISMathPINVOKE.Pbrt___deref__(swigCPtr);
    SWIGTYPE_p_IPbrt ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IPbrt(cPtr, false);
    return ret;
  }

}