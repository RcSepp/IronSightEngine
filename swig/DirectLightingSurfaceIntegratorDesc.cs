//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class DirectLightingSurfaceIntegratorDesc : SurfaceIntegratorDesc {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal DirectLightingSurfaceIntegratorDesc(global::System.IntPtr cPtr, bool cMemoryOwn) : base(IronSightEnginePINVOKE.DirectLightingSurfaceIntegratorDesc_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(DirectLightingSurfaceIntegratorDesc obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~DirectLightingSurfaceIntegratorDesc() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_DirectLightingSurfaceIntegratorDesc(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public DirectLightingSurfaceIntegratorDesc() : this(IronSightEnginePINVOKE.new_DirectLightingSurfaceIntegratorDesc(), true) {
  }

  public int maxdepth {
    set {
      IronSightEnginePINVOKE.DirectLightingSurfaceIntegratorDesc_maxdepth_set(swigCPtr, value);
    } 
    get {
      int ret = IronSightEnginePINVOKE.DirectLightingSurfaceIntegratorDesc_maxdepth_get(swigCPtr);
      return ret;
    } 
  }

  public DirectLightingSurfaceIntegratorDesc.Strategy strategy {
    set {
      IronSightEnginePINVOKE.DirectLightingSurfaceIntegratorDesc_strategy_set(swigCPtr, (int)value);
    } 
    get {
      DirectLightingSurfaceIntegratorDesc.Strategy ret = (DirectLightingSurfaceIntegratorDesc.Strategy)IronSightEnginePINVOKE.DirectLightingSurfaceIntegratorDesc_strategy_get(swigCPtr);
      return ret;
    } 
  }

  public enum Strategy {
    S_ALL,
    S_ONE
  }

}
