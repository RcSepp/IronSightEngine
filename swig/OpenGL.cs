//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class OpenGL : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal OpenGL(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(OpenGL obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~OpenGL() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_OpenGL(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public OpenGL() : this(IronSightEnginePINVOKE.new_OpenGL__SWIG_0(), true) {
  }

  public OpenGL(SWIGTYPE_p_IOpenGL src) : this(IronSightEnginePINVOKE.new_OpenGL__SWIG_1(SWIGTYPE_p_IOpenGL.getCPtr(src)), true) {
  }

  public SWIGTYPE_p_IOpenGL __deref__() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.OpenGL___deref__(swigCPtr);
    SWIGTYPE_p_IOpenGL ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_IOpenGL(cPtr, false);
    return ret;
  }

}