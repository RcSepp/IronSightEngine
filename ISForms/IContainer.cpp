#include "Forms.h"


//-----------------------------------------------------------------------------
// Name: AddControl()
// Desc: Add control to this WinForm
//-----------------------------------------------------------------------------
Result IContainer::AddControl(IControl* ctrl)
{
	Control* newctrl = (Control*)ctrl->_getbase();//&reinterpret_cast<Button*>(ctrl)->basecontrol;
	Result rlt;

	if(newctrl->parentform)
		CHKRESULT(newctrl->parentform->RemoveControl(ctrl));

	CHKRESULT(newctrl->OnAddControl(this, ctrl));

	controls.push_back(newctrl);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: RemoveControl()
// Desc: Remove control from this WinForm
//-----------------------------------------------------------------------------
Result IContainer::RemoveControl(IControl* ctrl)
{
	Control* disposedctrl = (Control*)ctrl->_getbase();//&reinterpret_cast<Button*>(ctrl)->basecontrol;
	Result rlt;

	CHKRESULT(disposedctrl->OnRemoveControl());

	controls.remove(disposedctrl);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: RemoveAllControls()
// Desc: Remove every control from this WinForm
//-----------------------------------------------------------------------------
Result IContainer::RemoveAllControls()
{
	Result rlt;

	std::list<Control*>::iterator iter;
	LIST_FOREACH(controls, iter)
		CHKRESULT((*iter)->OnRemoveControl());
	controls.clear();

	return R_OK;
}