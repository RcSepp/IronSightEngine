//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class FFmpeg : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal FFmpeg(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(FFmpeg obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~FFmpeg() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_FFmpeg(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public FFmpeg() : this(ISMathPINVOKE.new_FFmpeg__SWIG_0(), true) {
  }

  public FFmpeg(SWIGTYPE_p_IFFmpeg src) : this(ISMathPINVOKE.new_FFmpeg__SWIG_1(SWIGTYPE_p_IFFmpeg.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IFFmpeg __deref__() {
    global::System.IntPtr cPtr = ISMathPINVOKE.FFmpeg___deref__(swigCPtr);
    SWIGTYPE_p_IFFmpeg ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IFFmpeg(cPtr, false);
    return ret;
  }

}