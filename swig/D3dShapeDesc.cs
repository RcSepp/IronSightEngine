//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class D3dShapeDesc : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal D3dShapeDesc(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(D3dShapeDesc obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~D3dShapeDesc() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_D3dShapeDesc(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public Vector2 texcoordscale {
    set {
      IronSightEnginePINVOKE.D3dShapeDesc_texcoordscale_set(swigCPtr, Vector2.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dShapeDesc_texcoordscale_get(swigCPtr);
      Vector2 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector2(cPtr, false);
      return ret;
    } 
  }

  public D3dShapeId GetId() {
    D3dShapeId ret = (D3dShapeId)IronSightEnginePINVOKE.D3dShapeDesc_GetId(swigCPtr);
    return ret;
  }

  public virtual uint GetVertexCount() {
    uint ret = IronSightEnginePINVOKE.D3dShapeDesc_GetVertexCount(swigCPtr);
    return ret;
  }

  public virtual uint GetFaceCount() {
    uint ret = IronSightEnginePINVOKE.D3dShapeDesc_GetFaceCount(swigCPtr);
    return ret;
  }

  public virtual D3dShapeDesc Copy() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3dShapeDesc_Copy(swigCPtr);
    D3dShapeDesc ret = (cPtr == global::System.IntPtr.Zero) ? null : new D3dShapeDesc(cPtr, false);
    return ret;
  }

}