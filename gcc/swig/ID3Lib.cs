//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class ID3Lib : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal ID3Lib(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ID3Lib obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~ID3Lib() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_ID3Lib(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public ID3Lib() : this(ISMathPINVOKE.new_ID3Lib__SWIG_0(), true) {
  }

  public ID3Lib(SWIGTYPE_p_IID3Lib src) : this(ISMathPINVOKE.new_ID3Lib__SWIG_1(SWIGTYPE_p_IID3Lib.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IID3Lib __deref__() {
    global::System.IntPtr cPtr = ISMathPINVOKE.ID3Lib___deref__(swigCPtr);
    SWIGTYPE_p_IID3Lib ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IID3Lib(cPtr, false);
    return ret;
  }

}
