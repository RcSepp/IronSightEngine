//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class SizeI : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal SizeI(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(SizeI obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~SizeI() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_SizeI(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public int width {
    set {
      IronSightEnginePINVOKE.SizeI_width_set(swigCPtr, value);
    } 
    get {
      int ret = IronSightEnginePINVOKE.SizeI_width_get(swigCPtr);
      return ret;
    } 
  }

  public int height {
    set {
      IronSightEnginePINVOKE.SizeI_height_set(swigCPtr, value);
    } 
    get {
      int ret = IronSightEnginePINVOKE.SizeI_height_get(swigCPtr);
      return ret;
    } 
  }

  public SizeI() : this(IronSightEnginePINVOKE.new_SizeI__SWIG_0(), true) {
  }

  public SizeI(int width, int height) : this(IronSightEnginePINVOKE.new_SizeI__SWIG_1(width, height), true) {
  }

  public /*cstype*/ string ToString() {
	string ret = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(IronSightEnginePINVOKE.SizeI_ToString(swigCPtr));
	return ret;
}

}
