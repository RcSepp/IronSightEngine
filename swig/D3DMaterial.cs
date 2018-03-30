//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class D3DMaterial : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal D3DMaterial(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(D3DMaterial obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~D3DMaterial() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_D3DMaterial(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public Color ambient {
    set {
      IronSightEnginePINVOKE.D3DMaterial_ambient_set(swigCPtr, Color.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3DMaterial_ambient_get(swigCPtr);
      Color ret = (cPtr == global::System.IntPtr.Zero) ? null : new Color(cPtr, false);
      return ret;
    } 
  }

  public Color diffuse {
    set {
      IronSightEnginePINVOKE.D3DMaterial_diffuse_set(swigCPtr, Color.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3DMaterial_diffuse_get(swigCPtr);
      Color ret = (cPtr == global::System.IntPtr.Zero) ? null : new Color(cPtr, false);
      return ret;
    } 
  }

  public Color specular {
    set {
      IronSightEnginePINVOKE.D3DMaterial_specular_set(swigCPtr, Color.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3DMaterial_specular_get(swigCPtr);
      Color ret = (cPtr == global::System.IntPtr.Zero) ? null : new Color(cPtr, false);
      return ret;
    } 
  }

  public float power {
    set {
      IronSightEnginePINVOKE.D3DMaterial_power_set(swigCPtr, value);
    } 
    get {
      float ret = IronSightEnginePINVOKE.D3DMaterial_power_get(swigCPtr);
      return ret;
    } 
  }

  public ITexture tex {
    set {
      IronSightEnginePINVOKE.D3DMaterial_tex_set(swigCPtr, ITexture.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3DMaterial_tex_get(swigCPtr);
      ITexture ret = (cPtr == global::System.IntPtr.Zero) ? null : new ITexture(cPtr, false);
      return ret;
    } 
  }

  public D3DMaterial() : this(IronSightEnginePINVOKE.new_D3DMaterial(), true) {
  }

}