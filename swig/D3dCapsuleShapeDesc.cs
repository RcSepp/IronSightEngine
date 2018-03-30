//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class D3dCapsuleShapeDesc : D3dShapeDesc {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal D3dCapsuleShapeDesc(global::System.IntPtr cPtr, bool cMemoryOwn) : base(IronSightEnginePINVOKE.D3dCapsuleShapeDesc_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(D3dCapsuleShapeDesc obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~D3dCapsuleShapeDesc() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_D3dCapsuleShapeDesc(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public D3dCapsuleShapeDesc() : this(IronSightEnginePINVOKE.new_D3dCapsuleShapeDesc(), true) {
  }

  public Vector3 v0 {
    set {
      IronSightEnginePINVOKE.D3dCapsuleShapeDesc_v0_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dCapsuleShapeDesc_v0_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public Vector3 v1 {
    set {
      IronSightEnginePINVOKE.D3dCapsuleShapeDesc_v1_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dCapsuleShapeDesc_v1_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public float radius {
    set {
      IronSightEnginePINVOKE.D3dCapsuleShapeDesc_radius_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.D3dCapsuleShapeDesc_radius_get(swigCPtr);
      return ret;
    } 
  }

  public uint stacks {
    set {
      IronSightEnginePINVOKE.D3dCapsuleShapeDesc_stacks_set(swigCPtr, value);
    } 
    get {
      uint ret = IronSightEnginePINVOKE.D3dCapsuleShapeDesc_stacks_get(swigCPtr);
      return ret;
    } 
  }

  public uint slices {
    set {
      IronSightEnginePINVOKE.D3dCapsuleShapeDesc_slices_set(swigCPtr, value);
    } 
    get {
      uint ret = IronSightEnginePINVOKE.D3dCapsuleShapeDesc_slices_get(swigCPtr);
      return ret;
    } 
  }

  public override uint GetVertexCount() {
    uint ret = IronSightEnginePINVOKE.D3dCapsuleShapeDesc_GetVertexCount(swigCPtr);
    return ret;
  }

  public override uint GetFaceCount() {
    uint ret = IronSightEnginePINVOKE.D3dCapsuleShapeDesc_GetFaceCount(swigCPtr);
    return ret;
  }

  public override D3dShapeDesc Copy() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dCapsuleShapeDesc_Copy(swigCPtr);
    D3dShapeDesc ret = (cPtr == global::System.IntPtr.Zero) ? null : new D3dShapeDesc(cPtr, false);
    return ret;
  }

}
