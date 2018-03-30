#include "Havok.h"
#include "ISDirect3D.h"
#include <Common/Visualize/Shape/hkDisplayCapsule.h>
#include <Common/Visualize/Shape/hkDisplaySphere.h>


#define SLICE_MULTIPLICATOR		2


//
// Geometry functions
//
/// Adds a geometry to the display world managed by this display handler.
/// Please refer to the class description for the meaning of id and tag.
hkResult LocalVisualizer::addGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint)
{
	for(int g = 0; g < geometries.getSize(); g++)
	{
		LPOBJECT obj = NULL;
		Result rlt; // EDIT: >>> Check results

		hkGeometry* geometry = geometries[g]->getGeometry();
		if(geometry && geometry->m_vertices.getSize() && geometry->m_triangles.getSize())
		{
			int numvertices = geometry->m_vertices.getSize();
			int numfaces = geometry->m_triangles.getSize();

			// Create object
			rlt = d3dwnd->CreateObject(numvertices, numfaces, &obj);

			// Store vertex data
			IObject::Vertex* vertices = new IObject::Vertex[numvertices]; // EDIT: A vertex format with only position data would be much more efficient!
			for(int i = 0; i < numvertices; i++)
			{
				vertices[i].pos = Vector3(geometry->m_vertices[i](0), geometry->m_vertices[i](1), geometry->m_vertices[i](2));
				vertices[i].nml = Vector3(0.0f, 0.0f, 0.0f);
				vertices[i].texcoord = Vector2(0.0f, 0.0f);
				vertices[i].blendidx = 0;
			}
			rlt = obj->SetVertexData(vertices);
			delete[] vertices;

			// Store index data
			UINT* indices = new UINT[numfaces * 3];
			for(int i = 0; i < numfaces; i++)
			{
				indices[i * 3 + 0] = geometry->m_triangles[i].m_a;
				indices[i * 3 + 1] = geometry->m_triangles[i].m_b;
				indices[i * 3 + 2] = geometry->m_triangles[i].m_c;
			}
			rlt = obj->SetIndexData(indices);
			delete[] indices;

			// Compute normals
			rlt = obj->ComputeNormals();
		}
		else
			switch(geometries[g]->getType())
			{
			case HK_DISPLAY_CAPSULE:
				{
					hkDisplayCapsule* capsule = (hkDisplayCapsule*)geometries[g];
					D3dCapsuleShapeDesc desc;
					desc.v0 = Vector3(capsule->getBottom()(0), capsule->getBottom()(1), capsule->getBottom()(2));
					desc.v1 = Vector3(capsule->getTop()(0), capsule->getTop()(1), capsule->getTop()(2));
					desc.radius = capsule->getRadius();
					desc.slices = capsule->getNumSides() * SLICE_MULTIPLICATOR;
					desc.stacks = capsule->getNumSides() * SLICE_MULTIPLICATOR;
					rlt = d3dwnd->CreateObject(desc, &obj);
				}
				break;

			case HK_DISPLAY_SPHERE:
				{
					hkDisplaySphere* displaysphere = (hkDisplaySphere*)geometries[g];
					D3dSphereShapeDesc desc;
					hkSphere sphere;
					displaysphere->getSphere(sphere);
					desc.radius = sphere.getRadius();
					desc.slices = displaysphere->getXRes() * SLICE_MULTIPLICATOR;
					desc.stacks = displaysphere->getYRes() * SLICE_MULTIPLICATOR;
					rlt = d3dwnd->CreateObject(desc, &obj);
				}
				break;
			}

		if(obj)
		{
			// Finish initialization
			d3dwnd->RegisterForRendering(obj, RT_FAST);
			rlt = obj->SetShader(shader);
			rlt = obj->CommitChanges();

			// Set orientation mode
			obj->orientmode = OM_MATRIX;

			// Update transform
			//transform.get4x4ColumnMajor((hkFloat32*)obj->transform); //EDIT: This throws Havok exceptions. Why?
			for(byte i = 0; i < 4; i++)
				memcpy(&obj->transform[4 * i], &transform.getColumn(i), 4 * sizeof(float)); //EDIT11: Not tested

			objids[id] = obj;
		}
	}

	return HK_SUCCESS;
}

/// Adds a single geometry.
//hkResult LocalVisualizer::addGeometry(hkDisplayGeometry* geometry, hkUlong id, int tag, hkUlong shapeIdHint);

/// Adds an instanced of a geometry that has already been added to the display world managed by this display handler.
/// Please refer to the class description for the meaning of id and tag.
hkResult LocalVisualizer::addGeometryInstance(hkUlong origianalGeomId, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint)
{
	return HK_SUCCESS;
}

/// Sets the color of a geometry previously added to the display world.
/// Please refer to the class description for the meaning of id and tag.
hkResult LocalVisualizer::setGeometryColor(int color, hkUlong id, int tag)
{
	std::map<hkUlong, LPVOID>::iterator pair = objids.find(id);

	if(pair != objids.end())
	{
		LPOBJECT obj = (LPOBJECT)pair->second;
		(*obj->mats)[0].diffuse.r = (float)(BYTE)(color >> 16) / 255.0f;
		(*obj->mats)[0].diffuse.g = (float)(BYTE)(color >>  8) / 255.0f;
		(*obj->mats)[0].diffuse.b = (float)(BYTE)(color >>  0) / 255.0f;
	}

	return HK_SUCCESS;
}

/// Sets the transparency of a geometry previously added to the display world.
hkResult LocalVisualizer::setGeometryTransparency(float alpha, hkUlong id, int tag)
{
	std::map<hkUlong, LPVOID>::iterator pair = objids.find(id);

	if(pair != objids.end())
	{
		LPOBJECT obj = (LPOBJECT)pair->second;
		(*obj->mats)[0].diffuse.a = alpha;
	}

	return HK_SUCCESS;
}

/// Updates the transform of a body in the display world.
/// Please refer to the class description for the meaning of id and tag.
hkResult LocalVisualizer::updateGeometry(const hkTransform& transform, hkUlong id, int tag)
{
	std::map<hkUlong, LPVOID>::iterator pair = objids.find(id);

	if(pair != objids.end())
	{
		LPOBJECT obj = (LPOBJECT)pair->second;

		// Update transform
		for(byte i = 0; i < 4; i++)
			memcpy(&obj->transform[4 * i], &transform.getColumn(i), 4 * sizeof(float)); //EDIT11: Not tested
	}

	return HK_SUCCESS;
}

/// Updates the transform of a body in the display world. The matrix can include
/// scale. Scale is, however, only supported for geometries of type hkDisplayMesh.
hkResult LocalVisualizer::updateGeometry( const hkMatrix4& transform, hkUlong id, int tag )
{
	return HK_SUCCESS;
}

/// Skins a body in the display world. Only geometries of type hkDisplayMesh can be skinned.
hkResult LocalVisualizer::skinGeometry(hkUlong* ids, int numIds, const hkMatrix4* poseModel, int numPoseModel, const hkMatrix4& worldFromModel, int tag )
{
	return HK_SUCCESS;
}

/// Removes a geometry from the display world managed by this display handler.
/// Please refer to the class description for the meaning of id and tag.
hkResult LocalVisualizer::removeGeometry(hkUlong id, int tag, hkUlong shapeIdHint)
{
	if(!d3d)
		return HK_SUCCESS; // No need to remove objects if d3d has already cleaned up

	std::map<hkUlong, LPVOID>::iterator pair = objids.find(id);

	if(pair != objids.end())
	{
		LPOBJECT obj = (LPOBJECT)pair->second;
		objids.erase(pair);

		// Remove object
		d3dwnd->RemoveObject(obj);
	}

	return HK_SUCCESS;
}
						

//
// Camera Functionality
//
hkResult LocalVisualizer::updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name)
{
	return HK_SUCCESS;
}

//
// Immediate Mode Functions
//

/// Puts a display point into the display buffer for display in the next frame.
hkResult LocalVisualizer::displayPoint(const hkVector4& position, int colour, int id, int tag)
{
	return HK_SUCCESS;
}

/// Puts a display line into the display buffer for display in the next frame.
hkResult LocalVisualizer::displayLine(const hkVector4& start, const hkVector4& end, int color, int id, int tag)
{
	d3dwnd->DrawLine(Vector3(start(0), start(1), start(2)), Vector3(end(0), end(1), end(2)), Color(color), Color(color));
	return HK_SUCCESS;
}

/// Puts a display triangle into the display buffer for display in the next frame.
hkResult LocalVisualizer::displayTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c, int colour, int id, int tag)
{
	return HK_SUCCESS;
}

/// Outputs user text to the display. The manner in which the text
/// is displayed depends on the implementation of the display handler. It is not ok to ignore the data.
hkResult LocalVisualizer::displayText(const char* text, int color, int id, int tag)
{
	return HK_SUCCESS;
}

/// Outputs 3D text. Same as displayText() but with position.
hkResult LocalVisualizer::display3dText(const char* text, const hkVector4& pos, int color, int id, int tag)
{
	return HK_SUCCESS;
}

/// Display general annotation data. It is up to the handler implementation whether this shows up or not
/// in the graphics display or on some other output stream. It is ok to ignore the data.
//hkResult LocalVisualizer::displayAnnotation(const char* text, int id, int tag);

/// Displays a bone. This is optional and not all display handlers implement this interface
hkResult LocalVisualizer::displayBone(const hkVector4& a, const hkVector4& b, const hkQuaternion& orientation, int color, int tag)
{
	d3dwnd->DrawLine(Vector3(a(0), a(1), a(2)), Vector3(b(0), b(1), b(2)), Color(color), Color(color));
	return HK_SUCCESS;
}

/// Displays the geometries
hkResult LocalVisualizer::displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int id, int tag)
{
	return HK_SUCCESS;
}

/// Displays the geometries without transform
hkResult LocalVisualizer::displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, int color, int id, int tag)
{
	return HK_SUCCESS;
}		

//
// Statistics functions (ideally these would be in a separate interface to the display handler)
//
hkResult LocalVisualizer::sendMemStatsDump(const char* data, int length)
{
	return HK_SUCCESS;
}


/// Ensures that the current immediate mode display information
/// will be preserved and merged with all new immediate mode data
/// for the next step/frame.
hkResult LocalVisualizer::holdImmediate()
{
	return HK_SUCCESS;
}