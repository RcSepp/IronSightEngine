//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class XAudio2 : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal XAudio2(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(XAudio2 obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~XAudio2() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_XAudio2(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public XAudio2() : this(ISMathPINVOKE.new_XAudio2__SWIG_0(), true) {
  }

  public XAudio2(SWIGTYPE_p_IXAudio2 src) : this(ISMathPINVOKE.new_XAudio2__SWIG_1(SWIGTYPE_p_IXAudio2.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IXAudio2 __deref__() {
    global::System.IntPtr cPtr = ISMathPINVOKE.XAudio2___deref__(swigCPtr);
    SWIGTYPE_p_IXAudio2 ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IXAudio2(cPtr, false);
    return ret;
  }

}