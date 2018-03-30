#include "Havok.h"

Pose::Pose(const hkaSkeleton* skeleton, const hkArrayBase<hkpConstraintInstance*>& constraints)
{
	int numbones = skeleton->m_bones.getSize();
	hkLocalArray<hkQsTransform> transforms(numbones);
	hkQsTransform identitytransform(hkQsTransform::IDENTITY);
	for(int i = 0; i < numbones; i++)
		transforms.pushBack(identitytransform);

	//pose = new hkaPose(hkaPose::MODEL_SPACE, skeleton, transforms);
	pose = new hkaPose(skeleton);
	pose->setToReferencePose();

	// Get pivots from constraints
	int numconstraints = constraints.getSize();
	pivots = (Vector3**)malloc(numconstraints * sizeof(Vector3*));
	for(int i = 0; i < numconstraints; i++)
		pivots[i] = constraints[i]->getUserData() ? (Vector3*)constraints[i]->getUserData() : NULL;

	// Create array of local transforms (with center at the pivot)
	localtransforms = hkArray<hkQsTransform>(numbones, hkQsTransform(hkQsTransform::IDENTITY));

	for(int i = 0; i < numbones; i++)
		if(i >= 1 && pivots[i - 1])
		{
			localtransforms[i].setTranslation(*pivots[i - 1]);
			/*hkQsTransform transform(hkQsTransform::IDENTITY);
			transform.setTranslation(*pivots[i - 1]);
			pose->setBoneModelSpace(i, transform, hkaPose::PROPAGATE);*/
		}
	//pose->syncAll();
}

void Pose::GetRotation(Quaternion* qrot, int boneidx)
{
	memcpy(qrot, &localtransforms[boneidx].getRotation().m_vec.m_quad, 4 * sizeof(float));
}
void Pose::SetRotation(const Quaternion* qrot, int boneidx)
{
	localtransforms[boneidx].setRotation(*qrot);

	/*if(boneidx >= 1 && pivots[boneidx - 1])
	{
		hkQsTransform t0(hkQsTransform::IDENTITY);
		t0.setTranslation(hkVector4(-(*pivots[boneidx - 1])(0), -(*pivots[boneidx - 1])(1), -(*pivots[boneidx - 1])(2)));
		hkQsTransform t1(hkQsTransform::IDENTITY);
		t1.setTranslation(*pivots[boneidx - 1]);
		hkQsTransform r(hkQsTransform::IDENTITY);
		r.setRotation(localtransforms[boneidx].getRotation());
		hkQsTransform t(hkQsTransform::IDENTITY);
		t.setTranslation(localtransforms[boneidx].getTranslation());

		t1.setMul(t1, r);
		t1.setMul(t1, t0);
		t1.setMul(t1, t);
		pose->setBoneLocalSpace(boneidx, t1);
	}
	else*/
		pose->setBoneLocalSpace(boneidx, localtransforms[boneidx]);
		//pose->setBoneModelSpace(boneidx, localtransforms[boneidx], hkaPose::DONT_PROPAGATE);
}

void Pose::GetPosition(Vector3* vpos, int boneidx)
{
	hkQsTransform transform = pose->getBoneLocalSpace(boneidx);
	//vpos = (float*)&transform.getTranslation().m_quad; //EDIT: Not clear. vpos is only changed locally!
}
void Pose::SetPosition(const Vector3* vpos, int boneidx)
{
	hkQsTransform transform = pose->getBoneLocalSpace(boneidx);
	transform.setTranslation(*vpos);
	pose->setBoneLocalSpace(boneidx, transform);
}

Animation::Animation(RagdollEntity* parent, int numtracks, float duration) : parent(parent), enabled(false)
{
	ani = new hkaInterleavedUncompressedAnimation();
	ani->m_numberOfTransformTracks = numtracks;
	ani->m_numberOfFloatTracks = 0;
	ani->m_duration = duration;

	anibinding = new hkaAnimationBinding();
	anibinding->m_animation = ani;
	anibinding->setIdentity();

	anictrl = new hkaDefaultAnimationControl(anibinding);
}

bool Animation::SetEnabled(bool val)
{
	if(val != enabled)
	{
		enabled = val;
		if(enabled)
			parent->animatedskeleton->addAnimationControl(anictrl);
		else
			parent->animatedskeleton->removeAnimationControl(anictrl);
	}
	return enabled;

}

void Animation::AddFrame(IPose* frame)
{
	ani->m_transforms.insertAt(ani->m_transforms.getSize(), ((Pose*)frame)->pose->getSyncedPoseLocalSpace().begin(), ani->m_numberOfTransformTracks);
}

Result Animation::Serialize(const FilePath& filename)
{
/*hkObjectResource* loadedData = hkSerializeUtil::loadOnHeap(filename);
SomeClass* contents = loadedData->stealContents<SomeClass>(); // steal ownership
delete loadedData; // 'loadedData' no longer owns the data and may now be destroyed*/

Result rlt;
hkPackfileWriter::Options options;
CHKRESULT(hkSerializeUtil::saveTagfile(ani, hkaInterleavedUncompressedAnimationClass, hkOstream(filename).getStreamWriter(), HK_NULL, hkSerializeUtil::SAVE_TEXT_FORMAT));

	return R_OK;
}