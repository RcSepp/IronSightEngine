#ifndef __GUIANIMATION_H
#define __GUIANIMATION_H


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef void (__stdcall* ONANIMATE_CALLBACK)(LPVOID user);

class GuiAnimation;

template<class type = int> struct AnimatedPoint : public IUIAnimationVariableChangeHandler
{
	friend class GuiAnimation;
public:
	struct IChangeHandler
		{virtual void OnValueChanged(Point<type> oldpoint, Point<type> newpoint, ONANIMATE_CALLBACK cbkanimate, LPVOID cbkuser) = 0;};
private:
	IUIAnimationVariable *av_x, *av_y;
	IChangeHandler* handler;
	ONANIMATE_CALLBACK cbkanimate;
	LPVOID cbkuser;
public:

	AnimatedPoint(ONANIMATE_CALLBACK cbkanimate = NULL, LPVOID cbkuser = NULL) : cbkanimate(cbkanimate), cbkuser(cbkuser), handler(NULL)
	{
		animanager->CreateAnimationVariable(0.0, &av_x);
		animanager->CreateAnimationVariable(0.0, &av_y);
	};
	AnimatedPoint(type x, type y, ONANIMATE_CALLBACK cbkanimate = NULL, LPVOID cbkuser = NULL) : cbkanimate(cbkanimate), cbkuser(cbkuser), handler(NULL)
	{
		animanager->CreateAnimationVariable((DOUBLE)x, &av_x);
		animanager->CreateAnimationVariable((DOUBLE)y, &av_y);
	}
	~AnimatedPoint()
	{
		CancelPendingAnimations();
		av_x->Release();
		av_y->Release();
	}
	bool operator==(const AnimatedPoint<type>& p) const
	{
		DOUBLE tx, ty, px, py;
		this->av_x->GetValue(&tx);
		this->av_y->GetValue(&ty);
		p->av_x->GetValue(&px);
		p->av_y->GetValue(&py);
		return (type)tx == (type)px && (type)ty == (type)py;
	}
	bool operator!=(const AnimatedPoint<type>& p) const
	{
		DOUBLE tx, ty, px, py;
		this->av_x->GetValue(&tx);
		this->av_y->GetValue(&ty);
		p->av_x->GetValue(&px);
		p->av_y->GetValue(&py);
		return (type)tx != (type)px || (type)ty != (type)py;
	}
	AnimatedPoint<type> operator+(const AnimatedPoint<type> &summand) const
	{
		DOUBLE tx, ty, sx, sy;
		this->av_x->GetValue(&tx);
		this->av_y->GetValue(&ty);
		summand->av_x->GetValue(&sx);
		summand->av_y->GetValue(&sy);
		return AnimatedPoint<type>((type)(tx + sx), (type)(ty + sy));
	}
	AnimatedPoint<type> operator-(const AnimatedPoint<type> &subtrahend) const
	{
		DOUBLE tx, ty, sx, sy;
		this->av_x->GetValue(&tx);
		this->av_y->GetValue(&ty);
		subtrahend->av_x->GetValue(&sx);
		subtrahend->av_y->GetValue(&sy);
		return AnimatedPoint<type>((type)(tx - sx), (type)(ty - sy));
	}
	AnimatedPoint<type>& operator+=(const AnimatedPoint<type> &summand)
	{
		DOUBLE tx, ty, sx, sy;
		this->av_x->GetValue(&tx);
		this->av_y->GetValue(&ty);
		summand->av_x->GetValue(&sx);
		summand->av_y->GetValue(&sy);

		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);

		IUIAnimationTransition *trans_x, *trans_y;
		anitranslib->CreateInstantaneousTransition(tx + sx, &trans_x);
		stb->AddTransition(av_x, trans_x);
		trans_x->Release();
		anitranslib->CreateInstantaneousTransition(ty + sy, &trans_y);
		stb->AddTransition(av_y, trans_y);
		trans_y->Release();

		UI_ANIMATION_SECONDS time;
		anitimer->GetTime(&time);
		stb->Schedule(time);
		animanager->Update(time);
		stb->Release();

		return *this;
	}
	AnimatedPoint<type>& operator-=(const AnimatedPoint<type> &subtrahend)
	{
		DOUBLE tx, ty, sx, sy;
		this->av_x->GetValue(&tx);
		this->av_y->GetValue(&ty);
		subtrahend->av_x->GetValue(&sx);
		subtrahend->av_y->GetValue(&sy);

		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);

		IUIAnimationTransition *trans_x, *trans_y;
		anitranslib->CreateInstantaneousTransition(tx - sx, &trans_x);
		stb->AddTransition(av_x, trans_x);
		trans_x->Release();
		anitranslib->CreateInstantaneousTransition(ty - sy, &trans_y);
		stb->AddTransition(av_y, trans_y);
		trans_y->Release();

		UI_ANIMATION_SECONDS time;
		anitimer->GetTime(&time);
		stb->Schedule(time);
		animanager->Update(time);
		stb->Release();

		return *this;
	}

	String ToString() const
	{
		DOUBLE tx, ty, sx, sy;
		av_x->GetValue(&tx);
		av_y->GetValue(&ty);

		return String("Point(x = ") << (type)tx << String(", y = ") << (type)ty << String(")");
	}

	type GetX() const
	{
		DOUBLE tx;
		av_x->GetValue(&tx);
		return (type)tx;
	}
	type GetY() const
	{
		DOUBLE ty;
		av_y->GetValue(&ty);
		return (type)ty;
	}
	Point<type> GetPoint() const
	{
		DOUBLE tx, ty;
		av_x->GetValue(&tx);
		av_y->GetValue(&ty);
		return Point<type>((type)tx, (type)ty);
	}
	void SetX(type x)
	{
		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);

		IUIAnimationTransition *trans_x;
		anitranslib->CreateInstantaneousTransition((DOUBLE)x, &trans_x);
		stb->AddTransition(av_x, trans_x);
		trans_x->Release();

		UI_ANIMATION_SECONDS time;
		anitimer->GetTime(&time);
		stb->Schedule(time);
		animanager->Update(time);
		stb->Release();
	}
	void SetY(type y)
	{
		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);

		IUIAnimationTransition *trans_y;
		anitranslib->CreateInstantaneousTransition((DOUBLE)y, &trans_y);
		stb->AddTransition(av_y, trans_y);
		trans_y->Release();

		UI_ANIMATION_SECONDS time;
		anitimer->GetTime(&time);
		stb->Schedule(time);
		animanager->Update(time);
		stb->Release();
	}
	void SetPoint(const Point<type>& point)
	{
		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);

		IUIAnimationTransition *trans_x, *trans_y;
		anitranslib->CreateInstantaneousTransition((DOUBLE)point.x, &trans_x);
		stb->AddTransition(av_x, trans_x);
		trans_x->Release();
		anitranslib->CreateInstantaneousTransition((DOUBLE)point.y, &trans_y);
		stb->AddTransition(av_y, trans_y);
		trans_y->Release();

		UI_ANIMATION_SECONDS time;
		anitimer->GetTime(&time);
		stb->Schedule(time);
		animanager->Update(time);
		stb->Release();
	}

	void SetVariableChangeHandler(IChangeHandler* handler = NULL)
	{
		this->handler = handler;
		av_x->SetVariableChangeHandler(handler ? this : NULL);
		av_y->SetVariableChangeHandler(handler ? this : NULL);
	}
	HRESULT __stdcall OnValueChanged(IUIAnimationStoryboard *stb, IUIAnimationVariable *av, DOUBLE newval, DOUBLE oldval)
	{
		if(av == av_x)
		{
			DOUBLE ty;
			av_y->GetValue(&ty);
			handler->OnValueChanged(Point<type>((type)oldval, (type)ty), Point<type>((type)newval, (type)ty), cbkanimate, cbkuser);
		}
		else
		{
			DOUBLE tx;
			av_x->GetValue(&tx);
			handler->OnValueChanged(Point<type>((type)tx, (type)oldval), Point<type>((type)tx, (type)newval), cbkanimate, cbkuser);
		}
		return S_OK;
	}
	void CancelPendingAnimations()
	{
		IUIAnimationStoryboard *stb;
		av_x->GetCurrentStoryboard(&stb);
		while(stb)
		{
			stb->Abandon();
			av_x->GetCurrentStoryboard(&stb);
		}
		av_y->GetCurrentStoryboard(&stb);
		while(stb)
		{
			stb->Abandon();
			av_y->GetCurrentStoryboard(&stb);
		}
	}

	// >>> Simple IUnknown implementation
	DWORD iunknownrefctr;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
		{if(ppvObject) return S_OK; else return E_POINTER;}
	ULONG STDMETHODCALLTYPE AddRef()
		{return ++iunknownrefctr;}
	ULONG STDMETHODCALLTYPE Release()
		{long temp = --iunknownrefctr; if(!temp) delete this; return temp;}
	// <<< Simple IUnknown implementation
};

template<class type = int> struct AnimatedSize : public IUIAnimationVariableChangeHandler
{
	friend class GuiAnimation;
public:
	struct IChangeHandler
		{virtual void OnValueChanged(::Size<type> oldsize, ::Size<type> newsize, ONANIMATE_CALLBACK cbkanimate, LPVOID cbkuser) = 0;};
private:
	IUIAnimationVariable *av_width, *av_height;
	IChangeHandler* handler;
	ONANIMATE_CALLBACK cbkanimate;
	LPVOID cbkuser;
public:

	AnimatedSize(ONANIMATE_CALLBACK cbkanimate = NULL, LPVOID cbkuser = NULL) : cbkanimate(cbkanimate), cbkuser(cbkuser), handler(NULL)
	{
		animanager->CreateAnimationVariable(0.0, &av_width);
		animanager->CreateAnimationVariable(0.0, &av_height);
	};
	AnimatedSize(type width, type height, ONANIMATE_CALLBACK cbkanimate = NULL, LPVOID cbkuser = NULL) : cbkanimate(cbkanimate), cbkuser(cbkuser), handler(NULL)
	{
		animanager->CreateAnimationVariable((DOUBLE)width, &av_width);
		animanager->CreateAnimationVariable((DOUBLE)height, &av_height);
	}
	~AnimatedSize()
	{
		CancelPendingAnimations();
		av_width->Release();
		av_height->Release();
	}
	bool operator==(const AnimatedSize<type>& p) const
	{
		DOUBLE tw, th, pw, ph;
		this->av_width->GetValue(&tw);
		this->av_height->GetValue(&th);
		p->av_width->GetValue(&pw);
		p->av_height->GetValue(&ph);
		return (type)tw == (type)pw && (type)th == (type)ph;
	}
	bool operator!=(const AnimatedSize<type>& p) const
	{
		DOUBLE tw, th, pw, ph;
		this->av_width->GetValue(&tw);
		this->av_height->GetValue(&th);
		p->av_width->GetValue(&pw);
		p->av_height->GetValue(&ph);
		return (type)tw != (type)pw || (type)th != (type)ph;
	}

	String ToString() const
	{
		DOUBLE tw, th, sw, sh;
		av_width->GetValue(&tw);
		av_height->GetValue(&th);

		return String("Size(width = ") << (type)tw << String(", height = ") << (type)th << String(")");
	}

	type GetWidth() const
	{
		DOUBLE tw;
		av_width->GetValue(&tw);
		return (type)tw;
	}
	type GetHeight() const
	{
		DOUBLE th;
		av_height->GetValue(&th);
		return (type)th;
	}
	::Size<type> GetSize() const
	{
		DOUBLE tw, th;
		av_width->GetValue(&tw);
		av_height->GetValue(&th);
		return ::Size<type>((type)tw, (type)th);
	}
	void SetWidth(type width)
	{
		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);

		IUIAnimationTransition *trans_w;
		anitranslib->CreateInstantaneousTransition((DOUBLE)width, &trans_w);
		stb->AddTransition(av_width, trans_w);
		trans_w->Release();

		UI_ANIMATION_SECONDS time;
		anitimer->GetTime(&time);
		stb->Schedule(time);
		animanager->Update(time);
		stb->Release();
	}
	void SetHeight(type height)
	{
		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);

		IUIAnimationTransition *trans_h;
		anitranslib->CreateInstantaneousTransition((DOUBLE)height, &trans_h);
		stb->AddTransition(av_height, trans_h);
		trans_h->Release();

		UI_ANIMATION_SECONDS time;
		anitimer->GetTime(&time);
		stb->Schedule(time);
		animanager->Update(time);
		stb->Release();
	}
	void SetSize(const ::Size<type>& size)
	{
		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);

		IUIAnimationTransition *trans_w, *trans_h;
		anitranslib->CreateInstantaneousTransition((DOUBLE)size.width, &trans_w);
		stb->AddTransition(av_width, trans_w);
		trans_w->Release();
		anitranslib->CreateInstantaneousTransition((DOUBLE)size.height, &trans_h);
		stb->AddTransition(av_height, trans_h);
		trans_h->Release();

		UI_ANIMATION_SECONDS time;
		anitimer->GetTime(&time);
		stb->Schedule(time);
		animanager->Update(time);
		stb->Release();
	}

	void SetVariableChangeHandler(IChangeHandler* handler = NULL)
	{
		this->handler = handler;
		av_width->SetVariableChangeHandler(handler ? this : NULL);
		av_height->SetVariableChangeHandler(handler ? this : NULL);
	}
	HRESULT __stdcall OnValueChanged(IUIAnimationStoryboard *stb, IUIAnimationVariable *av, DOUBLE newval, DOUBLE oldval)
	{
		if(av == av_width)
		{
			DOUBLE th;
			av_height->GetValue(&th);
			handler->OnValueChanged(::Size<type>((type)oldval, (type)th), ::Size<type>((type)newval, (type)th), cbkanimate, cbkuser);
		}
		else
		{
			DOUBLE tw;
			av_width->GetValue(&tw);
			handler->OnValueChanged(::Size<type>((type)tw, (type)oldval), ::Size<type>((type)tw, (type)newval), cbkanimate, cbkuser);
		}
		return S_OK;
	}
	void CancelPendingAnimations()
	{
		IUIAnimationStoryboard *stb;
		av_width->GetCurrentStoryboard(&stb);
		while(stb)
		{
			stb->Abandon();
			av_width->GetCurrentStoryboard(&stb);
		}
		av_height->GetCurrentStoryboard(&stb);
		while(stb)
		{
			stb->Abandon();
			av_height->GetCurrentStoryboard(&stb);
		}
	}

	// >>> Simple IUnknown implementation
	DWORD iunknownrefctr;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
		{if(ppvObject) return S_OK; else return E_POINTER;}
	ULONG STDMETHODCALLTYPE AddRef()
		{return ++iunknownrefctr;}
	ULONG STDMETHODCALLTYPE Release()
		{long temp = --iunknownrefctr; if(!temp) delete this; return temp;}
	// <<< Simple IUnknown implementation
};

template<class type = int> struct AnimatedValue : public IUIAnimationVariableChangeHandler
{
	friend class GuiAnimation;
public:
	struct IChangeHandler
		{virtual void OnValueChanged(type val, ONANIMATE_CALLBACK cbkanimate, LPVOID cbkuser) = 0;};
private:
	IUIAnimationVariable *av;
	IChangeHandler* handler;
	ONANIMATE_CALLBACK cbkanimate;
	LPVOID cbkuser;
public:

	AnimatedValue(ONANIMATE_CALLBACK cbkanimate = NULL, LPVOID cbkuser = NULL) : cbkanimate(cbkanimate), cbkuser(cbkuser), handler(NULL)
	{
		animanager->CreateAnimationVariable(0.0, &av);
	};
	AnimatedValue(type val, ONANIMATE_CALLBACK cbkanimate = NULL, LPVOID cbkuser = NULL) : cbkanimate(cbkanimate), cbkuser(cbkuser), handler(NULL)
	{
		animanager->CreateAnimationVariable((DOUBLE)val, &av);
	}
	~AnimatedValue()
	{
		CancelPendingAnimations();
		av->Release();
	}
	bool operator==(const AnimatedValue<type>& p) const
	{
		DOUBLE t, p;
		this->av->GetValue(&t);
		p->av->GetValue(&p);
		return (type)t == (type)p;
	}
	bool operator!=(const AnimatedValue<type>& p) const
	{
		DOUBLE t, p;
		this->av->GetValue(&t);
		p->av->GetValue(&p);
		return (type)t != (type)p;
	}

	type GetVal() const
	{
		DOUBLE t;
		av->GetValue(&t);
		return (type)t;
	}
	void SetVal(type val)
	{
		IUIAnimationStoryboard *stb;
		animanager->CreateStoryboard(&stb);
		if(stb)
		{
			IUIAnimationTransition *trans;
			anitranslib->CreateInstantaneousTransition((DOUBLE)val, &trans);
			stb->AddTransition(av, trans);
			trans->Release();

			UI_ANIMATION_SECONDS time;
			anitimer->GetTime(&time);
			stb->Schedule(time);
			animanager->Update(time);
			stb->Release();
		}
		else
			LOG("Error creating storyboard");
	}

	void SetVariableChangeHandler(IChangeHandler* handler = NULL)
	{
		this->handler = handler;
		av->SetVariableChangeHandler(handler ? this : NULL);
	}
	HRESULT __stdcall OnValueChanged(IUIAnimationStoryboard *stb, IUIAnimationVariable *av, DOUBLE newval, DOUBLE oldval)
	{
		handler->OnValueChanged((type)newval, cbkanimate, cbkuser);
		return S_OK;
	}
	void CancelPendingAnimations()
	{
		IUIAnimationStoryboard *stb;
		av->GetCurrentStoryboard(&stb);
		while(stb)
		{
			stb->Abandon();
			av->GetCurrentStoryboard(&stb);
		}
	}

	// >>> Simple IUnknown implementation
	DWORD iunknownrefctr;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
		{if(ppvObject) return S_OK; else return E_POINTER;}
	ULONG STDMETHODCALLTYPE AddRef()
		{return ++iunknownrefctr;}
	ULONG STDMETHODCALLTYPE Release()
		{long temp = --iunknownrefctr; if(!temp) delete this; return temp;}
	// <<< Simple IUnknown implementation
};

//-----------------------------------------------------------------------------
// Name: GuiAnimation
// Desc: Wrapper class for an IUIAnimationStoryboard
//-----------------------------------------------------------------------------
class GuiAnimation : public IGuiAnimation, public IManagedObject
{
private:
	IUIAnimationStoryboard *stb;
	struct EventHandler : public IUIAnimationStoryboardEventHandler
	{
		GuiAnimation* parent;

		HRESULT __stdcall OnStoryboardStatusChanged(IUIAnimationStoryboard *stb, UI_ANIMATION_STORYBOARD_STATUS oldstatus, UI_ANIMATION_STORYBOARD_STATUS status)
		{
			if(status == UI_ANIMATION_STORYBOARD_FINISHED || status == UI_ANIMATION_STORYBOARD_CANCELLED)
				parent->done = true;
			return S_OK;
		}
		HRESULT __stdcall OnStoryboardUpdated(IUIAnimationStoryboard *storyboard) {return S_OK;}

		// >>> Simple IUnknown implementation
		DWORD iunknownrefctr;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
			{if(ppvObject) return S_OK; else return E_POINTER;}
		ULONG STDMETHODCALLTYPE AddRef()
			{return ++iunknownrefctr;}
		ULONG STDMETHODCALLTYPE Release()
			{long temp = --iunknownrefctr; if(!temp) delete this; return temp;}
		// <<< Simple IUnknown implementation
	} eventhandler;

public:
	bool done;

	GuiAnimation()
	{
		done = false;
		eventhandler.parent = this;
	}

	Result Create();
	Result Schedule();

	Result AddMove(LPGUICONTROL ctrl, const Point<float>& destpos, double acceleration);
	Result AddBlend(LPGUICONTROL ctrl, float destalpha, double duration);
	Result AddFloatChange(AnimatedValue<float>& var, const TransitionScript::IScript& desc);

	void Release();
};

#endif