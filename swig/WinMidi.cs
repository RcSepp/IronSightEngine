//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class WinMidi : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal WinMidi(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(WinMidi obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~WinMidi() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_WinMidi(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public WinMidi() : this(IronSightEnginePINVOKE.new_WinMidi__SWIG_0(), true) {
  }

  public WinMidi(SWIGTYPE_p_IWinMidi src) : this(IronSightEnginePINVOKE.new_WinMidi__SWIG_1(SWIGTYPE_p_IWinMidi.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IWinMidi __deref__() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.WinMidi___deref__(swigCPtr);
    SWIGTYPE_p_IWinMidi ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IWinMidi(cPtr, false);
    return ret;
  }

}
