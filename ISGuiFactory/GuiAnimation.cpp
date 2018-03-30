#include "GuiFactory.h"

//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create storyboard
//-----------------------------------------------------------------------------
Result GuiAnimation::Create()
{
	Result rlt;

	CHKRESULT(animanager->CreateStoryboard(&stb));
	CHKRESULT(stb->SetStoryboardEventHandler(&eventhandler));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Schedule()
// Desc: Start animation
//-----------------------------------------------------------------------------
Result GuiAnimation::Schedule()
{
	Result rlt;

	// Schedule changes
	UI_ANIMATION_SECONDS time;
	CHKRESULT(anitimer->GetTime(&time));
	CHKRESULT(stb->Schedule(time));

	// Update animation manager
	CHKRESULT(animanager->Update(time));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: AddMove()
// Desc: Add control movement animation
//-----------------------------------------------------------------------------
Result GuiAnimation::AddMove(LPGUICONTROL ctrl, const Point<float>& destpos, double acceleration)
{
	GuiControl* c = reinterpret_cast<GuiControl*>(ctrl);
	Result rlt;

	IUIAnimationVariable *var_prim, *var_sec;
	float val_prim, val_sec;
	if(fabs(destpos.x - c->pos.GetX()) > fabs(destpos.y - c->pos.GetY()))
	{
		var_prim = c->pos.av_x;
		var_sec = c->pos.av_y;
		val_prim = destpos.x;
		val_sec = destpos.y;
	}
	else
	{
		var_prim = c->pos.av_y;
		var_sec = c->pos.av_x;
		val_prim = destpos.y;
		val_sec = destpos.x;
	}

	IUIAnimationTransition *trans_prim, *trans_sec;
	CHKRESULT(anitranslib->CreateParabolicTransitionFromAcceleration(val_prim, 0.0, acceleration, &trans_prim));
	CHKRESULT(stb->AddTransition(var_prim, trans_prim));
	trans_prim->Release();

	UI_ANIMATION_KEYFRAME keyframe_end;
	CHKRESULT(stb->AddKeyframeAfterTransition(trans_prim, &keyframe_end));

	CHKRESULT(anitranslib->CreateAccelerateDecelerateTransition(1.0, val_sec, 0.3, 0.3, &trans_sec));
	CHKRESULT(stb->AddTransitionBetweenKeyframes(var_sec, trans_sec, UI_ANIMATION_KEYFRAME_STORYBOARD_START, keyframe_end));
	trans_sec->Release();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: AddBlend()
// Desc: Add control alpha blending animation
//-----------------------------------------------------------------------------
Result GuiAnimation::AddBlend(LPGUICONTROL ctrl, float destalpha, double duration)
{
	GuiControl* c = reinterpret_cast<GuiControl*>(ctrl);
	Result rlt;

	IUIAnimationTransition* trans;
	CHKRESULT(anitranslib->CreateAccelerateDecelerateTransition(duration, (DOUBLE)destalpha, 1.0, 0.0, &trans));
	CHKRESULT(stb->AddTransition(c->alpha.av, trans));
	trans->Release();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: AddFloatChange()
// Desc: Add uniform value change for a float value
//-----------------------------------------------------------------------------
Result GuiAnimation::AddFloatChange(AnimatedValue<float>& var, const TransitionScript::IScript& desc)
{
	IUIAnimationTransition* trans;
	Result rlt;

	switch(desc.type)
	{
	case TransitionScript::T_Instantaneous:
		{
			TransitionScript::Instantaneous* s = (TransitionScript::Instantaneous*)&desc;
			CHKRESULT(anitranslib->CreateInstantaneousTransition(s->dest, &trans));
		}
		break;
	case TransitionScript::T_Linear:
		{
			TransitionScript::Linear* s = (TransitionScript::Linear*)&desc;
			CHKRESULT(anitranslib->CreateLinearTransition(s->duration, s->dest, &trans));
		}
		break;
	case TransitionScript::T_AccelerateDecelerate:
		{
			TransitionScript::AccelerateDecelerate* s = (TransitionScript::AccelerateDecelerate*)&desc;
			CHKRESULT(anitranslib->CreateAccelerateDecelerateTransition(s->duration, s->dest, s->accratio, s->decratio, &trans));
		}
		break;
	default:
		return ERR(String("Unsupported script type: ") << String(desc.type));

	/*CHKRESULT(anitranslib->CreateAccelerateDecelerateTransition(0.05, (DOUBLE)newval, 1.0, 0.0, &trans));
	//CHKRESULT(anitranslib->CreateLinearTransition(0.1f, (DOUBLE)newval, &trans));
	CHKRESULT(stb->AddTransition(var.av, trans));

if(newval == 1.0f)
{
	UI_ANIMATION_KEYFRAME keyframe_end;
	CHKRESULT(stb->AddKeyframeAfterTransition(trans, &keyframe_end));
	IUIAnimationTransition* trans2;
	CHKRESULT(anitranslib->CreateSinusoidalTransitionFromRange(1000.0, 0.8, 1.0, 1.0, UI_ANIMATION_SLOPE_DECREASING, &trans2));
	CHKRESULT(stb->AddTransition(var.av, trans2));
}*/
	}

	CHKRESULT(stb->AddTransition(var.av, trans));
	trans->Release();
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GuiAnimation::Release()
{
	Unregister();
	reinterpret_cast<GuiFactory*>((LPGUIFACTORY)gui)->animations.remove(this);

	RELEASE(stb);

	delete this;
}