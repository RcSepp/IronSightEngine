//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class PointI : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal PointI(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(PointI obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~PointI() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_PointI(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public int x {
    set {
      IronSightEnginePINVOKE.PointI_x_set(swigCPtr, value);
    } 
    get {
      int ret = IronSightEnginePINVOKE.PointI_x_get(swigCPtr);
      return ret;
    } 
  }

  public int y {
    set {
      IronSightEnginePINVOKE.PointI_y_set(swigCPtr, value);
    } 
    get {
      int ret = IronSightEnginePINVOKE.PointI_y_get(swigCPtr);
      return ret;
    } 
  }

  public PointI() : this(IronSightEnginePINVOKE.new_PointI__SWIG_0(), true) {
  }

  public PointI(int x, int y) : this(IronSightEnginePINVOKE.new_PointI__SWIG_1(x, y), true) {
  }

  public /*cstype*/ string ToString() {
	string ret = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(IronSightEnginePINVOKE.PointI_ToString(swigCPtr));
	return ret;
}

}
