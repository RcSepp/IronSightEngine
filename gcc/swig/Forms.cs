//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class Forms : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Forms(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Forms obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Forms() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_Forms(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public Forms() : this(ISMathPINVOKE.new_Forms__SWIG_0(), true) {
  }

  public Forms(SWIGTYPE_p_IForms src) : this(ISMathPINVOKE.new_Forms__SWIG_1(SWIGTYPE_p_IForms.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IForms __deref__() {
    global::System.IntPtr cPtr = ISMathPINVOKE.Forms___deref__(swigCPtr);
    SWIGTYPE_p_IForms ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IForms(cPtr, false);
    return ret;
  }

}
