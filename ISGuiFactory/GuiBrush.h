#ifndef __GUIGUIBRUSH_H
#define __GUIGUIBRUSH_H

class GuiBrush : public IGuiBrush, public IManagedObject, public IDestroyable
{
public:
	ID2D1Brush* d2dbrush;

	GuiBrush()
	{
		d2dbrush = NULL;
	}
	Result Init(RenderTarget *rendertarget)
	{
		if(rendertarget)
			return OnCreateDevice(rendertarget);
		return R_OK;
	}

	ID2D1Brush* GetD2DBrush(float alpha, RenderTarget *rendertarget)
	{
		if(!d2dbrush)
		{
			Result rlt;
			IFFAILED(OnCreateDevice(rendertarget))
				return NULL;
		}
		d2dbrush->SetOpacity(alpha);
		return d2dbrush;
	}

	virtual Result OnCreateDevice(RenderTarget *rendertarget) = 0;
	virtual void OnDestroyDevice()
	{
		RELEASE(d2dbrush);
	}
	virtual void Release() = 0;
};

class SolidBrush : public GuiBrush
{
public:
	D2D1_COLOR_F clr;
	SolidBrush(const D2D1_COLOR_F& clr) : GuiBrush()
	{
		this->clr = D2D1_COLOR_F(clr);
	}
	Result OnCreateDevice(RenderTarget *rendertarget)
	{
		Result rlt;
		CHKRESULT(rendertarget->context->CreateSolidColorBrush(clr, (ID2D1SolidColorBrush**)&d2dbrush));
		return R_OK;
	}

	void Release()
	{
		Unregister();

		delete this;
	}
};
class LinearGradientBrush : public GuiBrush
{
public:
	ID2D1GradientStopCollection* stopcollection;
	D2D1_GRADIENT_STOP* stops;
	UINT numstops;
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props;

	LinearGradientBrush(const D2D1_GRADIENT_STOP* stops, UINT numstops, const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& props) : GuiBrush(), stopcollection(NULL)
	{
		this->numstops = numstops;
		this->stops = new D2D1_GRADIENT_STOP[numstops];
		memcpy(this->stops, stops, numstops * sizeof(D2D1_GRADIENT_STOP));
		this->props = D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES(props);
	}
	Result OnCreateDevice(RenderTarget *rendertarget)
	{
		Result rlt;
		CHKRESULT(rendertarget->context->CreateGradientStopCollection(stops, numstops, &stopcollection));
		CHKRESULT(rendertarget->context->CreateLinearGradientBrush(props, stopcollection, (ID2D1LinearGradientBrush**)&d2dbrush));
		return R_OK;
	}
	void OnDestroyDevice()
	{
		RELEASE(stopcollection);
		GuiBrush::OnDestroyDevice();
	}

	void Release()
	{
		Unregister();

		delete[] stops;

		delete this;
	}
};

#endif