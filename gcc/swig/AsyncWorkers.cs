//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class AsyncWorkers : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal AsyncWorkers(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(AsyncWorkers obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~AsyncWorkers() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_AsyncWorkers(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public AsyncWorkers() : this(ISMathPINVOKE.new_AsyncWorkers__SWIG_0(), true) {
  }

  public AsyncWorkers(SWIGTYPE_p_IAsyncWorkers src) : this(ISMathPINVOKE.new_AsyncWorkers__SWIG_1(SWIGTYPE_p_IAsyncWorkers.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IAsyncWorkers __deref__() {
    global::System.IntPtr cPtr = ISMathPINVOKE.AsyncWorkers___deref__(swigCPtr);
    SWIGTYPE_p_IAsyncWorkers ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IAsyncWorkers(cPtr, false);
    return ret;
  }

}
