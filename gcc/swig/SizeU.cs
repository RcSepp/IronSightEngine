//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class SizeU : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal SizeU(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(SizeU obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~SizeU() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_SizeU(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public uint width {
    set {
      ISMathPINVOKE.SizeU_width_set(swigCPtr, value);
    } 
    get {
      uint ret = ISMathPINVOKE.SizeU_width_get(swigCPtr);
      return ret;
    } 
  }

  public uint height {
    set {
      ISMathPINVOKE.SizeU_height_set(swigCPtr, value);
    } 
    get {
      uint ret = ISMathPINVOKE.SizeU_height_get(swigCPtr);
      return ret;
    } 
  }

  public SizeU() : this(ISMathPINVOKE.new_SizeU__SWIG_0(), true) {
  }

  public SizeU(uint width, uint height) : this(ISMathPINVOKE.new_SizeU__SWIG_1(width, height), true) {
  }

  public /*cstype*/ string ToString() {
	string ret = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ISMathPINVOKE.SizeU_ToString(swigCPtr));
	return ret;
}

}
