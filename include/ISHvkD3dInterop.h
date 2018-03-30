#ifndef __ISHVKD3DINTEROP_H
#define __ISHVKD3DINTEROP_H

#ifndef LPRENDERSHADER
typedef class IRenderShader* LPRENDERSHADER;
#endif

#ifndef LPOBJECT
typedef class IObject* LPOBJECT;
#endif

#ifndef LPOUTPUTWINDOW
typedef class IOutputWindow* LPOUTPUTWINDOW;
#endif

enum BoxedLevelBoxShape : unsigned char
	{SHAPE_CUBIC, SHAPE_STAIRS};
#pragma pack(push, 1)
struct BoxedLevelBoxType
{
	BYTE textureindex[6]; // [front, back, left, right, top, bottom]
	BYTE GetFront() {return textureindex[0];} BYTE SetFront(BYTE val) {return textureindex[0] = val;} __declspec(property(get=GetFront, put=SetFront)) BYTE front;
	BYTE GetBack() {return textureindex[1];} BYTE SetBack(BYTE val) {return textureindex[1] = val;} __declspec(property(get=GetBack, put=SetBack)) BYTE back;
	BYTE GetLeft() {return textureindex[2];} BYTE SetLeft(BYTE val) {return textureindex[2] = val;} __declspec(property(get=GetLeft, put=SetLeft)) BYTE left;
	BYTE GetRight() {return textureindex[3];} BYTE SetRight(BYTE val) {return textureindex[3] = val;} __declspec(property(get=GetRight, put=SetRight)) BYTE right;
	BYTE GetTop() {return textureindex[4];} BYTE SetTop(BYTE val) {return textureindex[4] = val;} __declspec(property(get=GetTop, put=SetTop)) BYTE top;
	BYTE GetBottom() {return textureindex[5];} BYTE SetBottom(BYTE val) {return textureindex[5] = val;} __declspec(property(get=GetBottom, put=SetBottom)) BYTE bottom;
	BoxedLevelBoxShape shape;
	BYTE unused;
	void SetTextureIndices(BYTE idx) {textureindex[0] = textureindex[1] = textureindex[2] = textureindex[3] = textureindex[4] = textureindex[5] = idx;}
	bool operator==(const BoxedLevelBoxType& other) const
		{return memcmp(this, &other, sizeof(BoxedLevelBoxType)) == 0;}
	bool operator!=(const BoxedLevelBoxType& other) const
		{return memcmp(this, &other, sizeof(BoxedLevelBoxType)) != 0;}
};
#pragma pack(pop)
struct BoxedLevelBoxDesc
{
	UINT32 id; // lower 30 bits = type index; upper 2 bits = orientation (north east south or west)
	BoxedLevelBoxDesc() {}
	BoxedLevelBoxDesc(UINT32 typeidx, UINT32 orient = 0) : id((typeidx + 1) & 0x3FFFFFFF | orient << 30) {}
	bool operator==(const BoxedLevelBoxDesc& other) const
		{return this->id == other.id;}
	bool operator!=(const BoxedLevelBoxDesc& other) const
		{return this->id != other.id;}
	UINT32 GetTypeIdx() {return (id & 0x3FFFFFFF) - 1;}
	UINT32 GetOrientation() {return id >> 30;}
	bool IsBox() {return id != 0;}
};
struct BoxedLevelChunk
{
	BoxedLevelBoxDesc* boxmask;
	UINT numboxes;
	UINT chunkpos[3];
	const Vector3 GetPos(const UINT (&chunksize)[3]) const
		{return Vector3((float)chunkpos[0] * (float)chunksize[0], (float)chunkpos[1] * (float)chunksize[1], (float)chunkpos[2] * (float)chunksize[2]);}
	float GetPosX(const UINT (&chunksize)[3]) const
		{return (float)chunkpos[0] * (float)chunksize[0];}
	float GetPosY(const UINT (&chunksize)[3]) const
		{return (float)chunkpos[1] * (float)chunksize[1];}
	float GetPosZ(const UINT (&chunksize)[3]) const
		{return (float)chunkpos[2] * (float)chunksize[2];}
};

#endif