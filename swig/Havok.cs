//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class Havok : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Havok(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Havok obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Havok() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_Havok(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public Havok() : this(IronSightEnginePINVOKE.new_Havok__SWIG_0(), true) {
  }

  public Havok(SWIGTYPE_p_IHavok src) : this(IronSightEnginePINVOKE.new_Havok__SWIG_1(SWIGTYPE_p_IHavok.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IHavok __deref__() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.Havok___deref__(swigCPtr);
    SWIGTYPE_p_IHavok ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IHavok(cPtr, false);
    return ret;
  }

}
