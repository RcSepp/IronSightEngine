//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class Sprite : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Sprite(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Sprite obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Sprite() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_Sprite(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public Matrix worldmatrix {
    set {
      IronSightEnginePINVOKE.Sprite_worldmatrix_set(swigCPtr, Matrix.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.Sprite_worldmatrix_get(swigCPtr);
      Matrix ret = (cPtr == global::System.IntPtr.Zero) ? null : new Matrix(cPtr, false);
      return ret;
    } 
  }

  public Vector2 texcoord {
    set {
      IronSightEnginePINVOKE.Sprite_texcoord_set(swigCPtr, Vector2.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.Sprite_texcoord_get(swigCPtr);
      Vector2 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector2(cPtr, false);
      return ret;
    } 
  }

  public Vector2 texsize {
    set {
      IronSightEnginePINVOKE.Sprite_texsize_set(swigCPtr, Vector2.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.Sprite_texsize_get(swigCPtr);
      Vector2 ret = (cPtr == global::System.IntPtr.Zero) ? null : new Vector2(cPtr, false);
      return ret;
    } 
  }

  public Color color {
    set {
      IronSightEnginePINVOKE.Sprite_color_set(swigCPtr, Color.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.Sprite_color_get(swigCPtr);
      Color ret = (cPtr == global::System.IntPtr.Zero) ? null : new Color(cPtr, false);
      return ret;
    } 
  }

  public ITexture tex {
    set {
      IronSightEnginePINVOKE.Sprite_tex_set(swigCPtr, ITexture.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.Sprite_tex_get(swigCPtr);
      ITexture ret = (cPtr == global::System.IntPtr.Zero) ? null : new ITexture(cPtr, false);
      return ret;
    } 
  }

  public Sprite() : this(IronSightEnginePINVOKE.new_Sprite(), true) {
  }

}