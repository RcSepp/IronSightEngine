//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class D3dBoxShapeDesc : D3dShapeDesc {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal D3dBoxShapeDesc(global::System.IntPtr cPtr, bool cMemoryOwn) : base(IronSightEnginePINVOKE.D3dBoxShapeDesc_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(D3dBoxShapeDesc obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~D3dBoxShapeDesc() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_D3dBoxShapeDesc(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public D3dBoxShapeDesc() : this(IronSightEnginePINVOKE.new_D3dBoxShapeDesc(), true) {
  }

  public Vector3 pos {
    set {
      IronSightEnginePINVOKE.D3dBoxShapeDesc_pos_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dBoxShapeDesc_pos_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public Vector3 size {
    set {
      IronSightEnginePINVOKE.D3dBoxShapeDesc_size_set(swigCPtr, Vector3.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dBoxShapeDesc_size_get(swigCPtr);
      Vector3 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector3(cPtr, false);
      return ret;
    } 
  }

  public override uint GetVertexCount() {
    uint ret = IronSightEnginePINVOKE.D3dBoxShapeDesc_GetVertexCount(swigCPtr);
    return ret;
  }

  public override uint GetFaceCount() {
    uint ret = IronSightEnginePINVOKE.D3dBoxShapeDesc_GetFaceCount(swigCPtr);
    return ret;
  }

  public override D3dShapeDesc Copy() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dBoxShapeDesc_Copy(swigCPtr);
    D3dShapeDesc ret = (cPtr == global::System.IntPtr.Zero) ? null : new D3dShapeDesc(cPtr, false);
    return ret;
  }

}
