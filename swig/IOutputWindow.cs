//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class IOutputWindow : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal IOutputWindow(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(IOutputWindow obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~IOutputWindow() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_IOutputWindow(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public class Settings : global::System.IDisposable {
    private global::System.Runtime.InteropServices.HandleRef swigCPtr;
    protected bool swigCMemOwn;
  
    internal Settings(global::System.IntPtr cPtr, bool cMemoryOwn) {
      swigCMemOwn = cMemoryOwn;
      swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
    }
  
    internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Settings obj) {
      return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
    }
  
    ~Settings() {
      Dispose();
    }
  
    public virtual void Dispose() {
      lock(this) {
        if (swigCPtr.Handle != global::System.IntPtr.Zero) {
          if (swigCMemOwn) {
            swigCMemOwn = false;
            IronSightEnginePINVOKE.delete_IOutputWindow_Settings(swigCPtr);
          }
          swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
        }
        global::System.GC.SuppressFinalize(this);
      }
    }
  
    public SizeU backbuffersize {
      set {
        IronSightEnginePINVOKE.IOutputWindow_Settings_backbuffersize_set(swigCPtr, SizeU.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_Settings_backbuffersize_get(swigCPtr);
        SizeU ret = (cPtr == global::System.IntPtr.Zero) ? null : new SizeU(cPtr, false);
        return ret;
      } 
    }
  
    public System.IntPtr wnd {
      set {
        IronSightEnginePINVOKE.IOutputWindow_Settings_wnd_set(swigCPtr, value);
      } 
  		get {
  				System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_Settings_wnd_get(swigCPtr);
  				return cPtr;
  		}
  	
    }
  
    public bool enablemultisampling {
      set {
        IronSightEnginePINVOKE.IOutputWindow_Settings_enablemultisampling_set(swigCPtr, value);
      } 
      get {
        bool ret = IronSightEnginePINVOKE.IOutputWindow_Settings_enablemultisampling_get(swigCPtr);
        return ret;
      } 
    }
  
    public int screenidx {
      set {
        IronSightEnginePINVOKE.IOutputWindow_Settings_screenidx_set(swigCPtr, value);
      } 
      get {
        int ret = IronSightEnginePINVOKE.IOutputWindow_Settings_screenidx_get(swigCPtr);
        return ret;
      } 
    }
  
    public Settings() : this(IronSightEnginePINVOKE.new_IOutputWindow_Settings(), true) {
    }
  
  }

  public bool ismultisampled {
    get {
      bool ret = IronSightEnginePINVOKE.IOutputWindow_ismultisampled_get(swigCPtr);
      return ret;
    } 
  }

  public int screenidx {
    get {
      int ret = IronSightEnginePINVOKE.IOutputWindow_screenidx_get(swigCPtr);
      return ret;
    } 
  }

  public Color backcolor {
    set {
      IronSightEnginePINVOKE.IOutputWindow_backcolor_set(swigCPtr, Color.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_backcolor_get(swigCPtr);
      Color ret = (cPtr == global::System.IntPtr.Zero) ? null : new Color(cPtr, false);
      return ret;
    } 
  }

  public bool backcolorenabled {
    set {
      IronSightEnginePINVOKE.IOutputWindow_backcolorenabled_set(swigCPtr, value);
    } 
    get {
      bool ret = IronSightEnginePINVOKE.IOutputWindow_backcolorenabled_get(swigCPtr);
      return ret;
    } 
  }

  public virtual SizeU GetBackbufferSize() {
    SizeU ret = new SizeU(IronSightEnginePINVOKE.IOutputWindow_GetBackbufferSize(swigCPtr), true);
    return ret;
  }

  public virtual uint GetBackbufferWidth() {
    uint ret = IronSightEnginePINVOKE.IOutputWindow_GetBackbufferWidth(swigCPtr);
    return ret;
  }

  public virtual uint GetBackbufferHeight() {
    uint ret = IronSightEnginePINVOKE.IOutputWindow_GetBackbufferHeight(swigCPtr);
    return ret;
  }

  public virtual System.IntPtr GetHwnd() {
		System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_GetHwnd(swigCPtr);
		return cPtr;
	}

  public virtual ITexture GetBackbuffer() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_GetBackbuffer(swigCPtr);
    ITexture ret = (cPtr == global::System.IntPtr.Zero) ? null : new ITexture(cPtr, false);
    return ret;
  }

  public virtual ITexture SetBackbuffer(ITexture texbackbuffer) {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_SetBackbuffer(swigCPtr, ITexture.getCPtr(texbackbuffer));
    ITexture ret = (cPtr == global::System.IntPtr.Zero) ? null : new ITexture(cPtr, false);
    return ret;
  }

  public ITexture Backbuffer {
    set {
      IronSightEnginePINVOKE.IOutputWindow_Backbuffer_set(swigCPtr, ITexture.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_Backbuffer_get(swigCPtr);
      ITexture ret = (cPtr == global::System.IntPtr.Zero) ? null : new ITexture(cPtr, false);
      return ret;
    } 
  }

  public virtual Result Resize(uint width, uint height) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_Resize(swigCPtr, width, height), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual void Update() {
    IronSightEnginePINVOKE.IOutputWindow_Update(swigCPtr);
  }

  public virtual void Render() {
    IronSightEnginePINVOKE.IOutputWindow_Render(swigCPtr);
  }

  public virtual void RegisterForRendering(RenderFunc func, RenderType rendertype, SWIGTYPE_p_void user) {
    RenderFuncWrapper funcwrapper = delegate(global::System.IntPtr wndptr, global::System.IntPtr userptr) {
      func(new IOutputWindow(wndptr, false));
    };
    {
      IronSightEnginePINVOKE.IOutputWindow_RegisterForRendering__SWIG_0(swigCPtr, global::System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(funcwrapper), (int)rendertype, SWIGTYPE_p_void.getCPtr(user));
    }
  }

  public virtual void RegisterForRendering(RenderFunc func, RenderType rendertype) {
    RenderFuncWrapper funcwrapper = delegate(global::System.IntPtr wndptr, global::System.IntPtr userptr) {
      func(new IOutputWindow(wndptr, false));
    };
    {
      IronSightEnginePINVOKE.IOutputWindow_RegisterForRendering__SWIG_1(swigCPtr, global::System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(funcwrapper), (int)rendertype);
    }
  }

  public virtual void RegisterForRendering(IRenderable cls, RenderType rendertype) {
    IronSightEnginePINVOKE.IOutputWindow_RegisterForRendering__SWIG_2(swigCPtr, IRenderable.getCPtr(cls), (int)rendertype);
  }

  public virtual void DeregisterFromRendering(RenderFunc func, RenderType rendertype) {
    RenderFuncWrapper funcwrapper = delegate(global::System.IntPtr wndptr, global::System.IntPtr userptr) {
      func(new IOutputWindow(wndptr, false));
    };
    {
      IronSightEnginePINVOKE.IOutputWindow_DeregisterFromRendering__SWIG_0(swigCPtr, global::System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(funcwrapper), (int)rendertype);
    }
  }

  public virtual void DeregisterFromRendering(IRenderable cls, RenderType rendertype) {
    IronSightEnginePINVOKE.IOutputWindow_DeregisterFromRendering__SWIG_1(swigCPtr, IRenderable.getCPtr(cls), (int)rendertype);
  }

  public virtual void DeregisterFromRendering(IRenderable cls) {
    IronSightEnginePINVOKE.IOutputWindow_DeregisterFromRendering__SWIG_2(swigCPtr, IRenderable.getCPtr(cls));
  }

  public virtual void RegisterForUpdating(IUpdateable cls) {
    IronSightEnginePINVOKE.IOutputWindow_RegisterForUpdating__SWIG_0(swigCPtr, IUpdateable.getCPtr(cls));
  }

  public virtual void DeregisterFromUpdating(IUpdateable cls) {
    IronSightEnginePINVOKE.IOutputWindow_DeregisterFromUpdating(swigCPtr, IUpdateable.getCPtr(cls));
  }

  public virtual void SortRegisteredClasses(RenderType rendertype, Vector3 campos) {
    IronSightEnginePINVOKE.IOutputWindow_SortRegisteredClasses(swigCPtr, (int)rendertype, Vector3.getCPtr(campos));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual Result GetRegisteredClasses(SWIGTYPE_p_GETREGISTEREDCLASSES_CALLBACK cbk, SWIGTYPE_p_void cbkuser, RenderType rendertype) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_GetRegisteredClasses(swigCPtr, SWIGTYPE_p_GETREGISTEREDCLASSES_CALLBACK.getCPtr(cbk), SWIGTYPE_p_void.getCPtr(cbkuser), (int)rendertype), true);
		//
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual void DrawLine(Vector3 v0, Vector3 v1, Color c0, Color c1, RenderType rendertype) {
    IronSightEnginePINVOKE.IOutputWindow_DrawLine__SWIG_0(swigCPtr, Vector3.getCPtr(v0), Vector3.getCPtr(v1), Color.getCPtr(c0), Color.getCPtr(c1), (int)rendertype);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual void DrawLine(Vector3 v0, Vector3 v1, Color c0, Color c1) {
    IronSightEnginePINVOKE.IOutputWindow_DrawLine__SWIG_1(swigCPtr, Vector3.getCPtr(v0), Vector3.getCPtr(v1), Color.getCPtr(c0), Color.getCPtr(c1));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual void DrawLine(Vector2 v0, Vector2 v1, Color c0, Color c1, RenderType rendertype) {
    IronSightEnginePINVOKE.IOutputWindow_DrawLine__SWIG_2(swigCPtr, Vector2.getCPtr(v0), Vector2.getCPtr(v1), Color.getCPtr(c0), Color.getCPtr(c1), (int)rendertype);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual void DrawLine(Vector2 v0, Vector2 v1, Color c0, Color c1) {
    IronSightEnginePINVOKE.IOutputWindow_DrawLine__SWIG_3(swigCPtr, Vector2.getCPtr(v0), Vector2.getCPtr(v1), Color.getCPtr(c0), Color.getCPtr(c1));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual void DrawSprite(Sprite sprite) {
    IronSightEnginePINVOKE.IOutputWindow_DrawSprite(swigCPtr, Sprite.getCPtr(sprite));
  }

  public virtual Result EnableSprites(/*cstype*/ string spriteshaderfilename) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_EnableSprites(swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(spriteshaderfilename).swigCPtr), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result EnableLineDrawing(/*cstype*/ string lineshaderfilename) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_EnableLineDrawing(swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(lineshaderfilename).swigCPtr), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual void ShowFps() {
    IronSightEnginePINVOKE.IOutputWindow_ShowFps(swigCPtr);
  }

  public virtual void HideFps() {
    IronSightEnginePINVOKE.IOutputWindow_HideFps(swigCPtr);
  }

  public virtual void TakeScreenShot(/*cstype*/ string filename, SWIGTYPE_p_D3DX_IMAGE_FILEFORMAT fileformat) {
    IronSightEnginePINVOKE.IOutputWindow_TakeScreenShot(swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(filename).swigCPtr, SWIGTYPE_p_D3DX_IMAGE_FILEFORMAT.getCPtr(fileformat));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual void SetCamera(ICamera cam) {
    IronSightEnginePINVOKE.IOutputWindow_SetCamera(swigCPtr, ICamera.getCPtr(cam));
  }

  public virtual ICamera GetCamera() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_GetCamera(swigCPtr);
    ICamera ret = (cPtr == global::System.IntPtr.Zero) ? null : new ICamera(cPtr, false);
    return ret;
  }

  public virtual SWIGTYPE_p_D3D11_VIEWPORT GetViewport() {
    SWIGTYPE_p_D3D11_VIEWPORT ret = new SWIGTYPE_p_D3D11_VIEWPORT(IronSightEnginePINVOKE.IOutputWindow_GetViewport(swigCPtr), false);
    return ret;
  }

  public virtual SWIGTYPE_p_ID3D11Device GetDevice() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_GetDevice(swigCPtr);
    SWIGTYPE_p_ID3D11Device ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_ID3D11Device(cPtr, false);
    return ret;
  }

  public virtual SWIGTYPE_p_ID3D11DeviceContext GetDeviceContext() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_GetDeviceContext(swigCPtr);
    SWIGTYPE_p_ID3D11DeviceContext ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_ID3D11DeviceContext(cPtr, false);
    return ret;
  }

  public virtual Result MakeConsoleTarget(uint numlines, uint historysize) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_MakeConsoleTarget(swigCPtr, numlines, historysize), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateTexture(/*cstype*/ string filename, ITexture.Usage usage, bool deviceindependent, SWIGTYPE_p_p_ITexture tex) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateTexture__SWIG_0(swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(filename).swigCPtr, (int)usage, deviceindependent, SWIGTYPE_p_p_ITexture.getCPtr(tex)), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateTexture(uint width, uint height, ITexture.Usage usage, bool deviceindependent, SWIGTYPE_p_DXGI_FORMAT format, SWIGTYPE_p_p_ITexture tex) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateTexture__SWIG_1(swigCPtr, width, height, (int)usage, deviceindependent, SWIGTYPE_p_DXGI_FORMAT.getCPtr(format), SWIGTYPE_p_p_ITexture.getCPtr(tex)), true);
		//
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateTexture(uint width, uint height, ITexture.Usage usage, bool deviceindependent, SWIGTYPE_p_DXGI_FORMAT format, SWIGTYPE_p_void data, SWIGTYPE_p_p_ITexture tex) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateTexture__SWIG_2(swigCPtr, width, height, (int)usage, deviceindependent, SWIGTYPE_p_DXGI_FORMAT.getCPtr(format), SWIGTYPE_p_void.getCPtr(data), SWIGTYPE_p_p_ITexture.getCPtr(tex)), true);
		//
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateTexture(ITexture source, ITexture.Usage usage, bool deviceindependent, SWIGTYPE_p_p_ITexture tex) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateTexture__SWIG_3(swigCPtr, ITexture.getCPtr(source), (int)usage, deviceindependent, SWIGTYPE_p_p_ITexture.getCPtr(tex)), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual void RemoveObject(IObject obj) {
    IronSightEnginePINVOKE.IOutputWindow_RemoveObject(swigCPtr, IObject.getCPtr(obj));
  }

  public virtual void RemoveAllObjects() {
    IronSightEnginePINVOKE.IOutputWindow_RemoveAllObjects(swigCPtr);
  }

  public virtual Result CreateHUD(SWIGTYPE_p_p_IHUD hud) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateHUD(swigCPtr, SWIGTYPE_p_p_IHUD.getCPtr(hud)), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateCursor(SWIGTYPE_p_p_ID3dCursor cursor) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateCursor(swigCPtr, SWIGTYPE_p_p_ID3dCursor.getCPtr(cursor)), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateSpriteContainer(uint buffersize, SWIGTYPE_p_p_ISpriteContainer spc) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateSpriteContainer(swigCPtr, buffersize, SWIGTYPE_p_p_ISpriteContainer.getCPtr(spc)), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateD3dFont(SWIGTYPE_p_FontType type, SWIGTYPE_p_p_ID3dFont font) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateD3dFont(swigCPtr, SWIGTYPE_p_FontType.getCPtr(type), SWIGTYPE_p_p_ID3dFont.getCPtr(font)), true);
		//
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateSceneManager(SWIGTYPE_p_p_ISceneManager mgr) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateSceneManager(swigCPtr, SWIGTYPE_p_p_ISceneManager.getCPtr(mgr)), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateSkyBox(SWIGTYPE_p_p_ISkyBox sky) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateSkyBox(swigCPtr, SWIGTYPE_p_p_ISkyBox.getCPtr(sky)), true);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual Result CreateBoxedLevel(SWIGTYPE_p_a_3__unsigned_int chunksize, uint numchunkbuffers, ITexture texboxes, IRenderShader blevelshader, SWIGTYPE_p_p_IBoxedLevel blevel) {
		Result ret = new Result(IronSightEnginePINVOKE.IOutputWindow_CreateBoxedLevel(swigCPtr, SWIGTYPE_p_a_3__unsigned_int.getCPtr(chunksize), numchunkbuffers, ITexture.getCPtr(texboxes), IRenderShader.getCPtr(blevelshader), SWIGTYPE_p_p_IBoxedLevel.getCPtr(blevel)), true);
		//
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		return ret;
	}

  public virtual void Release() {
    IronSightEnginePINVOKE.IOutputWindow_Release(swigCPtr);
  }

  public void RegisterForUpdating(ICamera cam) {
    IronSightEnginePINVOKE.IOutputWindow_RegisterForUpdating__SWIG_1(swigCPtr, ICamera.getCPtr(cam));
  }

  public ITexture XCreateTexture(/*cstype*/ string filename, ITexture.Usage usage, bool deviceindependent) {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_XCreateTexture(swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(filename).swigCPtr, (int)usage, deviceindependent);
    ITexture ret = (cPtr == global::System.IntPtr.Zero) ? null : new ITexture(cPtr, false);
    return ret;
  }

  public IRenderShader CreateRenderShader(/*cstype*/ string filename) {
		global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_CreateRenderShader(swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(filename).swigCPtr);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		IRenderShader ret = (cPtr == global::System.IntPtr.Zero) ? null : new IRenderShader(cPtr, false);
		return ret;
	}

  public IObject CreateObject(/*cstype*/ string filename, bool loadtextures) {
		global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_CreateObject__SWIG_0(swigCPtr, new IronSightEnginePINVOKE.SWIGStringMarshal(filename).swigCPtr, loadtextures);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		IObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new IObject(cPtr, false);
		return ret;
	}

  public IObject CreateObject(IObject srcobj, bool newmaterials) {
		global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_CreateObject__SWIG_1(swigCPtr, IObject.getCPtr(srcobj), newmaterials);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		IObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new IObject(cPtr, false);
		return ret;
	}

  public IObject CreateObject(uint numvertices, uint numfaces) {
		global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_CreateObject__SWIG_2(swigCPtr, numvertices, numfaces);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		IObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new IObject(cPtr, false);
		return ret;
	}

  public IObject CreateObject(D3dShapeDesc shapedesc) {
		global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_CreateObject__SWIG_3(swigCPtr, D3dShapeDesc.getCPtr(shapedesc));
		//
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		IObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new IObject(cPtr, false);
		return ret;
	}

  public IObject CreateObject(SWIGTYPE_p_p_D3dShapeDesc shapedescs, uint numshapedescs) {
		global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_CreateObject__SWIG_4(swigCPtr, SWIGTYPE_p_p_D3dShapeDesc.getCPtr(shapedescs), numshapedescs);
		//
		if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
		IObject ret = (cPtr == global::System.IntPtr.Zero) ? null : new IObject(cPtr, false);
		return ret;
	}

  public IHUD XCreateHUD() {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_XCreateHUD(swigCPtr);
    IHUD ret = (cPtr == global::System.IntPtr.Zero) ? null : new IHUD(cPtr, false);
    return ret;
  }

  public ISpriteContainer XCreateSpriteContainer(uint buffersize) {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.IOutputWindow_XCreateSpriteContainer(swigCPtr, buffersize);
    ISpriteContainer ret = (cPtr == global::System.IntPtr.Zero) ? null : new ISpriteContainer(cPtr, false);
    return ret;
  }

}