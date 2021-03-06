//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class Result : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Result(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Result obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Result() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_Result(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public class Details : global::System.IDisposable {
    private global::System.Runtime.InteropServices.HandleRef swigCPtr;
    protected bool swigCMemOwn;
  
    internal Details(global::System.IntPtr cPtr, bool cMemoryOwn) {
      swigCMemOwn = cMemoryOwn;
      swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
    }
  
    internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Details obj) {
      return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
    }
  
    ~Details() {
      Dispose();
    }
  
    public virtual void Dispose() {
      lock(this) {
        if (swigCPtr.Handle != global::System.IntPtr.Zero) {
          if (swigCMemOwn) {
            swigCMemOwn = false;
            IronSightEnginePINVOKE.delete_Result_Details(swigCPtr);
          }
          swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
        }
        global::System.GC.SuppressFinalize(this);
      }
    }
  
  }

  public Result.Details details {
    set {
      IronSightEnginePINVOKE.Result_details_set(swigCPtr, Result.Details.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.Result_details_get(swigCPtr);
      Result.Details ret = (cPtr == global::System.IntPtr.Zero) ? null : new Result.Details(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_unsigned_int refcounter {
    set {
      IronSightEnginePINVOKE.Result_refcounter_set(swigCPtr, SWIGTYPE_p_unsigned_int.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.Result_refcounter_get(swigCPtr);
      SWIGTYPE_p_unsigned_int ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_unsigned_int(cPtr, false);
      return ret;
    } 
  }

  public Result() : this(IronSightEnginePINVOKE.new_Result__SWIG_0(), true) {
  }

  public Result(/*cstype*/ string msg) : this(IronSightEnginePINVOKE.new_Result__SWIG_1(new IronSightEnginePINVOKE.SWIGStringMarshal(msg).swigCPtr), true) {
  }

  public Result(/*cstype*/ string msg, string file, string func, int line) : this(IronSightEnginePINVOKE.new_Result__SWIG_2(new IronSightEnginePINVOKE.SWIGStringMarshal(msg).swigCPtr, file, func, line), true) {
  }

  public static void PrintLogMessage(/*cstype*/ string msg, /*cstype*/ string file, /*cstype*/ string func, int line) {
    IronSightEnginePINVOKE.Result_PrintLogMessage__SWIG_0(new IronSightEnginePINVOKE.SWIGStringMarshal(msg).swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(file).swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(func).swigCPtr, line);
  }

  public static void PrintLogMessage(/*cstype*/ string msg) {
    IronSightEnginePINVOKE.Result_PrintLogMessage__SWIG_1(new IronSightEnginePINVOKE.SWIGStringMarshal(msg).swigCPtr);
  }

  public static void PrintLogMessage(Result.Details details) {
    IronSightEnginePINVOKE.Result_PrintLogMessage__SWIG_2(Result.Details.getCPtr(details));
  }

  public Result(Result copy) : this(IronSightEnginePINVOKE.new_Result__SWIG_3(Result.getCPtr(copy)), true) {
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public /*cstype*/ string GetLastResult(bool msgonly) {
	string ret = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(IronSightEnginePINVOKE.Result_GetLastResult__SWIG_0(swigCPtr, msgonly));
	return ret;
}

  public /*cstype*/ string GetLastResult() {
	string ret = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(IronSightEnginePINVOKE.Result_GetLastResult__SWIG_1(swigCPtr));
	return ret;
}

}
