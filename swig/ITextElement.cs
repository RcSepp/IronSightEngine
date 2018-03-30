//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class ITextElement : IElement {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal ITextElement(global::System.IntPtr cPtr, bool cMemoryOwn) : base(IronSightEnginePINVOKE.ITextElement_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ITextElement obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~ITextElement() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_ITextElement(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public /*cstype*/ string text {
	set {
		IronSightEnginePINVOKE.ITextElement_text_set(swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(value).swigCPtr);
	}

	get {
		string ret = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(IronSightEnginePINVOKE.ITextElement_text_get(swigCPtr));
		return ret;
	}

  }

  public SWIGTYPE_p_LPD3DFONT font {
    set {
      IronSightEnginePINVOKE.ITextElement_font_set(swigCPtr, SWIGTYPE_p_LPD3DFONT.getCPtr(value));
      if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_LPD3DFONT ret = new SWIGTYPE_p_LPD3DFONT(IronSightEnginePINVOKE.ITextElement_font_get(swigCPtr), true);
      if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public uint textflags {
    set {
      IronSightEnginePINVOKE.ITextElement_textflags_set(swigCPtr, value);
    } 
    get {
      uint ret = IronSightEnginePINVOKE.ITextElement_textflags_get(swigCPtr);
      return ret;
    } 
  }

}