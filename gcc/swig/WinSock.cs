//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class WinSock : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal WinSock(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(WinSock obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~WinSock() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_WinSock(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public WinSock() : this(ISMathPINVOKE.new_WinSock__SWIG_0(), true) {
  }

  public WinSock(SWIGTYPE_p_IWinSock src) : this(ISMathPINVOKE.new_WinSock__SWIG_1(SWIGTYPE_p_IWinSock.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IWinSock __deref__() {
    global::System.IntPtr cPtr = ISMathPINVOKE.WinSock___deref__(swigCPtr);
    SWIGTYPE_p_IWinSock ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IWinSock(cPtr, false);
    return ret;
  }

}
