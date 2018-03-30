//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class EnginePtr : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal EnginePtr(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(EnginePtr obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~EnginePtr() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          ISMathPINVOKE.delete_EnginePtr(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public EnginePtr() : this(ISMathPINVOKE.new_EnginePtr__SWIG_0(), true) {
  }

  public EnginePtr(Engine src) : this(ISMathPINVOKE.new_EnginePtr__SWIG_1(Engine.getCPtr(src)), true) {
  }

  public Engine __deref__() {
    global::System.IntPtr cPtr = ISMathPINVOKE.EnginePtr___deref__(swigCPtr);
    Engine ret = (cPtr == global::System.IntPtr.Zero) ? null : new Engine(cPtr, false);
    return ret;
  }

  public System.IntPtr GetHInstance() {
		System.IntPtr cPtr = ISMathPINVOKE.EnginePtr_GetHInstance(swigCPtr);
		return cPtr;
	}

  public bool FindDLL(/*cstype*/ string dllname, SWIGTYPE_p_p_char missingdllname) {
    bool ret = ISMathPINVOKE.EnginePtr_FindDLL(swigCPtr, new ISMathPINVOKE.SWIGStringMarshal(dllname).swigCPtr, SWIGTYPE_p_p_char.getCPtr(missingdllname));
    return ret;
  }

  public void GetConsolePtr(Console execle) {
    ISMathPINVOKE.EnginePtr_GetConsolePtr(swigCPtr, Console.getCPtr(execle));
    if (ISMathPINVOKE.SWIGPendingException.Pending) throw ISMathPINVOKE.SWIGPendingException.Retrieve();
  }

  public Result Run(InitFunc initfunc, InitFunc postinitfunc, CyclicFunc cyclicfunc, QuitFunc quitfunc) {
    RefDelegate initfuncwrapper = delegate() {
      return Result.getCPtr(initfunc()).Handle;
    };
    RefDelegate postinitfuncwrapper = delegate() {
      return Result.getCPtr(initfunc()).Handle;
    };
    RefDelegate cyclicfuncwrapper = delegate() {
      return System.IntPtr.Zero;
    };
    RefDelegate quitfuncwrapper = delegate() {
      return System.IntPtr.Zero;
    };
    {
      Result ret = new Result(ISMathPINVOKE.EnginePtr_Run(swigCPtr, initfuncwrapper, postinitfuncwrapper, cyclicfuncwrapper, quitfuncwrapper), true);
      return ret;
    }
  }

  public void ChangeCyclicFunc(CyclicFunc newcyclicfunc) {
    RefDelegate newcyclicfuncwrapper = delegate() {
      return System.IntPtr.Zero;
    };
    {
      ISMathPINVOKE.EnginePtr_ChangeCyclicFunc(swigCPtr, newcyclicfuncwrapper);
    }
  }

  public bool Work() {
    bool ret = ISMathPINVOKE.EnginePtr_Work(swigCPtr);
    return ret;
  }

  public bool Wait() {
    bool ret = ISMathPINVOKE.EnginePtr_Wait(swigCPtr);
    return ret;
  }

  public void WakeUp() {
    ISMathPINVOKE.EnginePtr_WakeUp(swigCPtr);
  }

  public Result CreateTimer(SWIGTYPE_p_TIMERELAPSED_CALLBACK timercbk, uint interval, bool autoreset, SWIGTYPE_p_void user, SWIGTYPE_p_p_Timer tmr) {
    Result ret = new Result(ISMathPINVOKE.EnginePtr_CreateTimer(swigCPtr, SWIGTYPE_p_TIMERELAPSED_CALLBACK.getCPtr(timercbk), interval, autoreset, SWIGTYPE_p_void.getCPtr(user), SWIGTYPE_p_p_Timer.getCPtr(tmr)), true);
    if (ISMathPINVOKE.SWIGPendingException.Pending) throw ISMathPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Result ResetTimer(SWIGTYPE_p_Timer tmr, uint newinterval) {
    Result ret = new Result(ISMathPINVOKE.EnginePtr_ResetTimer(swigCPtr, SWIGTYPE_p_Timer.getCPtr(tmr), newinterval), true);
    return ret;
  }

  public void RemoveTimer(SWIGTYPE_p_Timer tmr) {
    ISMathPINVOKE.EnginePtr_RemoveTimer(swigCPtr, SWIGTYPE_p_Timer.getCPtr(tmr));
  }

}
