//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class D3dConeShapeDesc : D3dShapeDesc {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal D3dConeShapeDesc(global::System.IntPtr cPtr, bool cMemoryOwn) : base(IronSightEnginePINVOKE.D3dConeShapeDesc_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(D3dConeShapeDesc obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~D3dConeShapeDesc() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_D3dConeShapeDesc(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public D3dConeShapeDesc() : this(IronSightEnginePINVOKE.new_D3dConeShapeDesc(), true) {
  }

  public Vector3 v0 {
    set {
      IronSightEnginePINVOKE.D3dConeShapeDesc_v0_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dConeShapeDesc_v0_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public Vector3 v1 {
    set {
      IronSightEnginePINVOKE.D3dConeShapeDesc_v1_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dConeShapeDesc_v1_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public float radius {
    set {
      IronSightEnginePINVOKE.D3dConeShapeDesc_radius_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.D3dConeShapeDesc_radius_get(swigCPtr);
      return ret;
    } 
  }

  public uint slices {
    set {
      IronSightEnginePINVOKE.D3dConeShapeDesc_slices_set(swigCPtr, value);
    } 
    get {
      uint ret = IronSightEnginePINVOKE.D3dConeShapeDesc_slices_get(swigCPtr);
      return ret;
    } 
  }

  public bool hasgroundfaces {
    set {
      IronSightEnginePINVOKE.D3dConeShapeDesc_hasgroundfaces_set(swigCPtr, value);
    } 
    get {
      bool ret = IronSightEnginePINVOKE.D3dConeShapeDesc_hasgroundfaces_get(swigCPtr);
      return ret;
    } 
  }

  public override uint GetVertexCount() {
    uint ret = IronSightEnginePINVOKE.D3dConeShapeDesc_GetVertexCount(swigCPtr);
    return ret;
  }

  public override uint GetFaceCount() {
    uint ret = IronSightEnginePINVOKE.D3dConeShapeDesc_GetFaceCount(swigCPtr);
    return ret;
  }

  public override D3dShapeDesc Copy() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dConeShapeDesc_Copy(swigCPtr);
    D3dShapeDesc ret = (cPtr == global::System.IntPtr.Zero) ? null : new D3dShapeDesc(cPtr, false);
    return ret;
  }

}
