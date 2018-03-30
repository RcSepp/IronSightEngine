#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: SceneManager()
// Desc: Constructor
//-----------------------------------------------------------------------------
SceneManager::SceneManager() : rootnode(NULL) // The root node doesn't need to know it's parent, since it cannot be removed
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize scene manager
//-----------------------------------------------------------------------------
Result SceneManager::Init(OutputWindow* wnd)
{
	this->wnd = wnd;
	if(!wnd)
		return ERR("Parameter wnd is NULL");

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Update all scene node transforms and call Update() on all updatable scene objects
//-----------------------------------------------------------------------------
void SceneManager::Update()
{
	rootnode.Update();
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Call Render() on all renderable objects of the given render type
//-----------------------------------------------------------------------------
void SceneManager::Render(RenderType rendertype)
{
	rootnode.Render(rendertype);
}

//-----------------------------------------------------------------------------
// Name: GetRegisteredClasses()
// Desc: Fire callback for all classes registered for rendering in rendering order
//-----------------------------------------------------------------------------
Result SceneManager::GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype)
{
	return rootnode.GetRegisteredClasses(cbk, cbkuser, rendertype);
}

ISceneNode* SceneManager::CreateNode()
{
	scenenodes.push_back(SceneNode(this));
	SceneNode* newchildnode = &scenenodes.back();

	return (ISceneNode*)newchildnode;
}

ISceneNode* SceneManager::CloneNode(ISceneNode* srcnode)
{
	scenenodes.push_back(SceneNode(this, (SceneNode*)srcnode));
	SceneNode* newchildnode = &scenenodes.back();

	return (ISceneNode*)newchildnode;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void SceneManager::Release()
{
	wnd->scenemanagers.remove(this);

	delete this;
}


SceneNode::SceneNode(SceneManager* parent) : parent(parent), parentnode(NULL), pos(0.0f, 0.0f, 0.0f), scl(1.0f, 1.0f, 1.0f), qrot(0.0f, 0.0f, 0.0f, 1.0f)
{
	MatrixIdentity(&worldtransform);
	MatrixIdentity(&parenttransform);
}
SceneNode::SceneNode(SceneManager* parent, SceneNode* src) : parent(parent), parentnode(NULL), pos(src->pos), scl(src->scl), qrot(src->qrot)
{
	MatrixIdentity(&worldtransform);
	MatrixIdentity(&parenttransform);
	autorenderenabled = src->autorenderenabled;

	moveables.assign(src->moveables.begin(), src->moveables.end());
	for(int i = 0; i < 4; i++)
		renderables[i].assign(src->renderables[i].begin(), src->renderables[i].end());
	
	std::list<SceneNode*>::iterator niter;
	LIST_FOREACH(src->childnodes, niter)
		childnodes.push_back((SceneNode*)parent->CloneNode(*niter));
}

void SceneNode::SetTransform(const Matrix* transform)
{
	this->parenttransform = *transform;
}

void SceneNode::Update()
{
	Matrix localtransform;
	GetTransform(&localtransform, TS_PARENT);
worldtransform = localtransform * parenttransform; // EDIT: Do with MatrixMultiply() function instead

	// Move attached moveables
	std::list<IMoveable*>::iterator riter;
	LIST_FOREACH(moveables, riter)
		(*riter)->SetTransform(&worldtransform);

	// Update child nodes
	std::list<SceneNode*>::iterator niter;
	LIST_FOREACH(childnodes, niter)
	{
		(*niter)->SetTransform(&worldtransform);
		(*niter)->Update();
	}
}

void SceneNode::Render(RenderType rendertype)
{
	// Move and render attached renderables
	std::list<IRenderable*>::iterator riter;
	LIST_FOREACH(renderables[rendertype], riter)
	{
		(*riter)->SetTransform(&worldtransform);
		(*riter)->Render(rendertype);
	}

	// Render child nodes
	std::list<SceneNode*>::iterator niter;
	LIST_FOREACH(childnodes, niter)
		(*niter)->Render(rendertype);
}

//-----------------------------------------------------------------------------
// Name: GetRegisteredClasses()
// Desc: Fire callback for all classes registered for rendering in rendering order
//-----------------------------------------------------------------------------
Result SceneNode::GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype)
{
	Result rlt;

	// Move and add attached renderables
	std::list<IRenderable*>::iterator riter;
	LIST_FOREACH(renderables[rendertype], riter)
	{
		(*riter)->SetTransform(&worldtransform);
		CHKRESULT(cbk(*riter, cbkuser));
	}

	// Continue with child nodes
	std::list<SceneNode*>::iterator niter;
	LIST_FOREACH(childnodes, niter)
		CHKRESULT((*niter)->GetRegisteredClasses(cbk, cbkuser, rendertype));

	return R_OK;
}

void SceneNode::AddChild(ISceneNode* node)
{
	SceneNode* _node = (SceneNode*)node;
	childnodes.push_back(_node);
	_node->parentnode = this;
}

void SceneNode::AddMoveable(IMoveable* moveable)
{
	moveables.push_back(moveable);
}
void SceneNode::AddRenderable(IRenderable* renderable, RenderType rendertype)
{
	renderables[rendertype].push_back(renderable);
}

void SceneNode::Translate(const Vector3* deltapos, TransformSpace ts)
{
	Vector3 transformeddeltapos;
	TransformToParentSpace(&transformeddeltapos, deltapos, ts);
	Vec3Add(&pos, &pos, &transformeddeltapos);
}
void SceneNode::TranslateX(const float deltapos, TransformSpace ts)
{
	Vector3 transformeddeltapos(deltapos, 0.0f, 0.0f);
	TransformToParentSpace(&transformeddeltapos, &transformeddeltapos, ts);
	Vec3Add(&pos, &pos, &transformeddeltapos);
}
void SceneNode::TranslateY(const float deltapos, TransformSpace ts)
{
	Vector3 transformeddeltapos(0.0f, deltapos, 0.0f);
	TransformToParentSpace(&transformeddeltapos, &transformeddeltapos, ts);
	Vec3Add(&pos, &pos, &transformeddeltapos);
}
void SceneNode::TranslateZ(const float deltapos, TransformSpace ts)
{
	Vector3 transformeddeltapos(0.0f, 0.0f, deltapos);
	TransformToParentSpace(&transformeddeltapos, &transformeddeltapos, ts);
	Vec3Add(&pos, &pos, &transformeddeltapos);
}

void SceneNode::SetPosition(const Vector3* pos, TransformSpace ts)
{
	TransformToParentSpace(&this->pos, pos, ts);
}

void SceneNode::SetScale(const Vector3* scale)
{
	scl = *scale;
}

void SceneNode::RotateX(float angle)
{
	Quaternion qrotx;
	QuaternionMultiply(&qrot, QuaternionRotationX(&qrotx, angle));
}
void SceneNode::RotateY(float angle)
{
	Quaternion qroty;
	QuaternionMultiply(&qrot, QuaternionRotationY(&qroty, angle));
}
void SceneNode::RotateZ(float angle)
{
	Quaternion qrotz;
	QuaternionMultiply(&qrot, QuaternionRotationZ(&qrotz, angle));
}
void SceneNode::RotateAxis(const Vector3* axis, float angle)
{
	Quaternion qrotaxis;
	QuaternionMultiply(&qrot, QuaternionRotationAxis(&qrotaxis, axis, angle));
}
void SceneNode::RotateQuaternion(const Quaternion* qrot)
{
	QuaternionMultiply(&this->qrot, qrot);
}

void SceneNode::SetOrientation(const Quaternion* qrot, TransformSpace ts)
{
	switch(ts)
	{
	case TS_PARENT:
		memcpy(&this->qrot, qrot, sizeof(Quaternion));
		break;

	case TS_LOCAL:
		QuaternionMultiply(&this->qrot, qrot); // TS_LOCAL -> TS_PARENT
		break;

	case TS_WORLD:
		{
			// Set pos to oldpos * invworldpos * newworldpos
			Matrix invworldtransform, qrottransform;
			GetTransform(&invworldtransform, TS_WORLD);
			MatrixInverse(&invworldtransform, &invworldtransform);
			MatrixRotationQuaternion(&qrottransform, qrot);
			worldtransform = invworldtransform * qrottransform; // EDIT: Do with MatrixMultiply() function instead
			Vec3Set(&this->pos, 0.0f, 0.0f, 0.0f);
			Vec3TransformCoord(&this->pos, &this->pos, &worldtransform);
			Vec3Inverse(&this->pos, &this->pos);
			
			// Set qrot to oldqrot * invworldqrot * newworldqrot
			Quaternion worldqrot, invworldqrot;
			GetOrientation(&worldqrot, TS_WORLD);
			QuaternionInverse(&invworldqrot, &worldqrot);
			QuaternionMultiply(&this->qrot, &invworldqrot);
			QuaternionMultiply(&this->qrot, qrot);
		}
		break;
	}
}

Quaternion* SceneNode::GetOrientation(Quaternion* qrot, TransformSpace ts)
{
	switch(ts)
	{
	case TS_PARENT:
		memcpy(qrot, &this->qrot, sizeof(Quaternion));
		return qrot;

	case TS_LOCAL:
		QuaternionIdentity(qrot);
		return qrot;

	case TS_WORLD:
		{
// TS_LOCAL -> TS_WORLD
GetOrientation(qrot, TS_PARENT); // TS_LOCAL -> TS_PARENT
if(parentnode)
{
	// Recursion up to root scene node
	Quaternion transform;
	parentnode->GetOrientation(&transform, TS_WORLD);
*qrot = *qrot * transform; // EDIT: Do with QuaternionMultiply() function instead
}
		}
		return qrot;

	default:
		Result::PrintLogMessage(String("GetOrientation() not defined for transform space ") << String((int)ts));
		return qrot;
	}
}

Matrix* SceneNode::GetTransform(Matrix* mout, TransformSpace ts)
{
if(ts == TS_LOCAL)
{
	 // TS_LOCAL -> TS_LOCAL
	MatrixIdentity(mout);
	return mout;
}
if(ts == TS_PARENT)
{
	// TS_LOCAL -> TS_PARENT
	Matrix mpos, mrot, mscl;
	MatrixTranslation(&mpos, pos.x, pos.y, pos.z);
	MatrixRotationQuaternion(&mrot, &qrot);
	MatrixScaling(&mscl, scl.x, scl.y, scl.z);
Matrix mtransform = mscl * mrot * mpos; // EDIT: Do with MatrixMultiply() function instead
memcpy(mout, &mtransform, sizeof(Matrix));
	return mout;
}

// TS_LOCAL -> TS_WORLD
GetTransform(mout, TS_PARENT); // TS_LOCAL -> TS_PARENT
if(parentnode)
{
	// Recursion up to root scene node
	Matrix transform;
	parentnode->GetTransform(&transform, TS_WORLD);
*mout = *mout * transform; // EDIT: Do with MatrixMultiply() function instead
}
	return mout;
}

Vector3* SceneNode::TransformToParentSpace(Vector3* vout, const Vector3* v0, TransformSpace ts)
{
	switch(ts)
	{
	case TS_LOCAL:
		{
			Matrix transform;
			GetTransform(&transform, TS_PARENT);
			Vec3TransformCoord(vout, v0, &transform); // TS_LOCAL -> TS_PARENT
		}
		return vout;

	case TS_WORLD:
		{
			Matrix transform, invtransform;
			GetTransform(&transform, TS_WORLD);
			MatrixInverse(&invtransform, &transform);
			Vec3TransformCoord(vout, v0, &invtransform); // TS_WORLD -> TS_LOCAL
			GetTransform(&transform, TS_PARENT);
			Vec3TransformCoord(vout, v0, &transform); // TS_LOCAL -> TS_PARENT
		}
		return vout;

	default:
		Result::PrintLogMessage(String("TransformToParentSpace() not defined for transform space ") << String((int)ts));
	case TS_PARENT:
		memcpy(vout, v0, sizeof(Vector3));
		return vout;
	}
}

Vector3* SceneNode::TransformToLocalSpace(Vector3* vout, const Vector3* v0, TransformSpace ts)
{
	return vout;
}