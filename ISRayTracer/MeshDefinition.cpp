#include <ISEngine.h>//<ISTypes.h>
#include <ISMeshDefinition.h>
#include <fstream>

Result ParseMtlMaterial(FilePath filename, std::vector<MeshDefinition::Surface*>& surfaces)
{
	Result::PrintLogMessage(String("Parsing file: ") << filename);
	if(!filename.Exists())
		return ERR(String("File not found: ") << filename);
	char str[256], strfilename[260];
	MeshDefinition::Surface::Material* mtl = NULL;
	Result rlt;

	std::ifstream fs(filename, std::ios_base::in);

	while((int) fs.tellg() != -1)
	{
		fs >> str;

		if(strcmp(str, "newmtl") == 0)
        {
            // Set current material
            fs >> strfilename;
            
            mtl = 0;
            for(size_t i = 0; i < surfaces.size(); i++ )
            {
                if(surfaces[i]->name.Equals(strfilename))
                {
                    mtl = &surfaces[i]->material;
                    break;
                }
            }
        }

		if(!mtl)
            continue;

        if(strcmp(str, "Ka") == 0)
        {
            // Ambient color
            float r, g, b;
            fs >> r >> g >> b;
            mtl->ambient[0] = r;
            mtl->ambient[1] = g;
            mtl->ambient[2] = b;
        }
        else if(strcmp(str, "Kd") == 0)
        {
            // Diffuse color
            float r, g, b;
            fs >> r >> g >> b;
			mtl->diffuse[0] = r;
            mtl->diffuse[1] = g;
            mtl->diffuse[2] = b;
        }
        else if(strcmp(str, "Ks") == 0)
        {
            // Specular color
            float r, g, b;
            fs >> r >> g >> b;
			mtl->specular[0] = r;
            mtl->specular[1] = g;
            mtl->specular[2] = b;
        }
		else if(strcmp(str, "Ke") == 0)
        {
            // Emissive color
            float r, g, b;
            fs >> r >> g >> b;
			mtl->emissive[0] = r;
            mtl->emissive[1] = g;
            mtl->emissive[2] = b;
        }
        else if(strcmp(str, "d") == 0)
        {
            // Opacity
            fs >> mtl->transparency;
			mtl->transparency = 1.0f - mtl->transparency;
			mtl->transparency = mtl->transparency < 0.0f ? 0.0f : (mtl->transparency > 1.0f ? 1.0f : mtl->transparency); // Saturate
        }
		 else if(strcmp(str, "Tr") == 0)
        {
            // Transparency
            fs >> mtl->transparency;
			mtl->transparency = mtl->transparency < 0.0f ? 0.0f : (mtl->transparency > 1.0f ? 1.0f : mtl->transparency); // Saturate
        }
        else if(strcmp(str, "Ns") == 0)
        {
            // Power
            fs >> mtl->power;
        }
        else if(strcmp(str, "illum") == 0)
        {
            // Specular on/off
            int illum;
            fs >> illum;
            mtl->illum = (MeshDefinition::Surface::Material::IlluminationType)illum;
        }
        else if(strcmp(str, "map_Kd") == 0)
        {
            // Texture
            fs >> strfilename;
			mtl->texfilename = FilePath(strfilename);
			if(mtl->texfilename[0] == '/' || mtl->texfilename[0] == '\\')
				mtl->texfilename.Substring(1, &mtl->texfilename);
        }
	}

	fs.close();

	Result::PrintLogMessage("(DONE)");
	return R_OK;
}

template<class T> int InterpretIndex(int index, int vectordim, const std::vector<T>& elements)
{
	if(index < 0)
		return elements.size() / vectordim + index; // Negative indices refer to offsets of the current element
	else
		return--index; // Hint: OBJ format uses 1-based arrays
}

Result ParseObjMesh(FilePath filename, MeshDefinition** meshdef)
{
	*meshdef = NULL;

	Result::PrintLogMessage(String("Parsing file: ") << filename);
	if(!filename.Exists())
		return ERR(String("File not found: ") << filename);
	char str[256];
	char strmatfilename[260], strfilename[260];
	strmatfilename[0] = '\0';
	bool isnewmat;
	Result rlt;

	MeshDefinition* _meshdef = *meshdef = new MeshDefinition();
	CHKALLOC(_meshdef);

	MeshDefinition::Surface* sfc = new MeshDefinition::Surface("DEFAULT_MTL");
	CHKALLOC(sfc);
	isnewmat = true;

	std::ifstream fs(filename, std::ios_base::in);

	char c;
	while((c = fs.peek()) != -1)
	{
		// Skip newlines, whitespaces and tabs
		if(c == '\r' || c == '\n' || c == '\t' || c == ' ')
		{
			fs.read(&c, 1); // Skip char
			continue;
		}

		fs >> str;

		if(strcmp(str, "v") == 0)
		{
			// Vertex position
			float x, y, z;
			fs >> x >> y >> z;
			_meshdef->positions.push_back(x);
			_meshdef->positions.push_back(y);
			_meshdef->positions.push_back(z);
		}
		else if(strcmp(str, "vt") == 0)
		{
			// Vertex texture coordinate
			float u, v;
			fs >> u >> v;
			_meshdef->texcoords.push_back(u);
			_meshdef->texcoords.push_back(v);
		}
		else if(strcmp(str, "vn") == 0)
		{
			// Vertex normal
			float x, y, z;
			fs >> x >> y >> z;
			_meshdef->normals.push_back(x);
			_meshdef->normals.push_back(y);
			_meshdef->normals.push_back(z);
		}
		else if(strcmp(str, "f") == 0)
		{
			// Face
			int posidx, texidx, nmlidx;

			for(int i = 0; i < 3; i++)
			{
				// Hint: OBJ format uses 1-based arrays
				fs >> posidx;
				sfc->positionindices.push_back(InterpretIndex(posidx, 3, _meshdef->positions));
				if(fs.peek() == '/')
				{
					fs.ignore(); // '/'
					if(fs.peek() != '/')
					{
						fs >> texidx;
						sfc->texcoordindices.push_back(InterpretIndex(texidx, 2, _meshdef->texcoords));
					}
					if(fs.peek() == '/')
					{
						fs.ignore(); // '/'
						fs >> nmlidx;
						sfc->normalindices.push_back(InterpretIndex(nmlidx, 3, _meshdef->normals));
					}
				}
			}

			// Skip white spaces
			char c;
			while((c = fs.peek()) == ' ' || c == '\t')
				fs.read(&c, 1);

			c = fs.peek();
			if(c == '-' || (c >= '0' && c <= '9'))
			{
				// A fourth number sequence identifies a quad
				// Add a whole triangle between the second to last index, the last index and the new index

				fs >> posidx;
				sfc->positionindices.push_back(sfc->positionindices[sfc->positionindices.size() - 3]);
				sfc->positionindices.push_back(sfc->positionindices[sfc->positionindices.size() - 2]);
				sfc->positionindices.push_back(InterpretIndex(posidx, 3, _meshdef->positions));
				if(fs.peek() == '/')
				{
					fs.ignore(); // '/'
					if(fs.peek() != '/')
					{
						fs >> texidx;
						sfc->texcoordindices.push_back(sfc->texcoordindices[sfc->texcoordindices.size() - 3]);
						sfc->texcoordindices.push_back(sfc->texcoordindices[sfc->texcoordindices.size() - 2]);
						sfc->texcoordindices.push_back(InterpretIndex(texidx, 2, _meshdef->texcoords));
					}
					if(fs.peek() == '/')
					{
						fs.ignore(); // '/'
						fs >> nmlidx;
						sfc->normalindices.push_back(sfc->normalindices[sfc->normalindices.size() - 3]);
						sfc->normalindices.push_back(sfc->normalindices[sfc->normalindices.size() - 2]);
						sfc->normalindices.push_back(InterpretIndex(nmlidx, 3, _meshdef->normals));
					}
				}
			}
		}
		else if(strcmp(str, "usemtl") == 0)
		{
			// Material
			fs >> strfilename;

			// Store or reject preceding material
			if(sfc->positionindices.empty())
				delete sfc;
			else if(!sfc->normalindices.empty() && sfc->normalindices.size() != sfc->positionindices.size())
			{
				Result::PrintLogMessage("The number of normal indices of the surface differs from the number of position indices. Ignoring surface");
				delete sfc;
			}
			else if(!sfc->texcoordindices.empty() && sfc->texcoordindices.size() != sfc->positionindices.size())
			{
				Result::PrintLogMessage("The number of texture coordinate indices of the surface differs from the number of position indices. Ignoring surface");
				delete sfc;
			}
			else if(isnewmat)
				_meshdef->surfaces.push_back(sfc);
            
			// Check if a surrface named strfilename already exists
			isnewmat = true;
			for(size_t i = 0; i < _meshdef->surfaces.size(); i++)
			{
				if(_meshdef->surfaces[i]->name.Equals(strfilename))
				{
					isnewmat = false;
					sfc = _meshdef->surfaces[i];
					break;
				}  
			}

			if(isnewmat)
			{
				sfc = new MeshDefinition::Surface(strfilename);
				CHKALLOC(sfc);
			}
		}
		else if(strcmp(str, "mtllib") == 0)
		{
			// Material library
			fs >> strmatfilename;
		}
	}
	fs.close();

	// Store or reject last material
	if(sfc->positionindices.empty())
		delete sfc;
	else if(!sfc->normalindices.empty() && sfc->normalindices.size() != sfc->positionindices.size())
	{
		Result::PrintLogMessage("The number of normal indices of the surface differs from the number of position indices. Ignoring surface");
		delete sfc;
	}
	else if(!sfc->texcoordindices.empty() && sfc->texcoordindices.size() != sfc->positionindices.size())
	{
		Result::PrintLogMessage("The number of texture coordinate indices of the surface differs from the number of position indices. Ignoring surface");
		delete sfc;
	}
	else if(isnewmat)
		_meshdef->surfaces.push_back(sfc);

	Result::PrintLogMessage("(DONE)");

	// Load material file
	if(strmatfilename[0])
	{
		FilePath currentdir;
		filename.GetParentDir(&currentdir);

		CHKRESULT(ParseMtlMaterial((FilePath)(currentdir << strmatfilename), _meshdef->surfaces));
	}

	return R_OK;
}

#include <ISMath.h>
Result ComputeTangentsAndBinormals(FilePath filename, MeshDefinition* meshdef)
{
	Result::PrintLogMessage(String("Computing tangents and binormals for: ") << filename);

	// Allocate memory
	meshdef->binormals.resize(meshdef->normals.size());
	meshdef->tangents.resize(meshdef->normals.size());

	Vector2 t0, t1, t2, t01, t02;
	Vector3 p0, p1, p2, p01, p02, t, b, t_, b_, n;
	float det;
	for(size_t s = 0; s < meshdef->surfaces.size(); s++)
	{
		MeshDefinition::Surface* sfc = meshdef->surfaces[s];
		for(size_t i = 0; i < sfc->positionindices.size(); i += 3)
		{
			// Get position and texture coordinate vectors
			Vec2Set(&t0, &meshdef->texcoords[2 * sfc->texcoordindices[i + 0]]);
			Vec2Set(&t1, &meshdef->texcoords[2 * sfc->texcoordindices[i + 1]]);
			Vec2Set(&t2, &meshdef->texcoords[2 * sfc->texcoordindices[i + 2]]);
			Vec3Set(&p0, &meshdef->positions[3 * sfc->positionindices[i + 0]]);
			Vec3Set(&p1, &meshdef->positions[3 * sfc->positionindices[i + 1]]);
			Vec3Set(&p2, &meshdef->positions[3 * sfc->positionindices[i + 2]]);

			Vec2Sub(&t01, &t1, &t0);
			Vec2Sub(&t02, &t2, &t0);
			Vec3Sub(&p01, &p1, &p0);
			Vec3Sub(&p02, &p2, &p0);

			// | p01.x p01.y p01.z |   | t01.x -t01y |   | t.x t.y t.z |
			// | p02.x p02.y p02.z | = | t02.x -t02y | * | b.x b.y b.z |
			// Inverse | t01 t02 |' matrix to compute | t b |' matrix
			det = (t01.x * t02.y - t01.y * t02.x);
			t.x = (t02.y * p01.x - t01.y * p02.x) / det;
			t.y = (t02.y * p01.y - t01.y * p02.y) / det;
			t.z = (t02.y * p01.z - t01.y * p02.z) / det;
			b.x = (t02.x * p01.x - t01.x * p02.x) / det;
			b.y = (t02.x * p01.y - t01.x * p02.y) / det;
			b.z = (t02.x * p01.z - t01.x * p02.z) / det;
			Vec3Normalize(&t);
			Vec3Normalize(&b);

			// Tilt tangent and binormal vectors to keep them orthogonal to a normal vector that is not alligned with the geometric surface normal ([t, b, n] -> [t_, b_, n])
			Vec3Set(&n, &meshdef->normals[3 * sfc->normalindices[i + 0]]);
			Vec3Cross(&b_, &n, &t);
			Vec3Cross(&t_, &b_, &n);
			memcpy(&meshdef->binormals[3 * sfc->normalindices[i + 0]], &b_, 3 * sizeof(float));
			memcpy(&meshdef->tangents[3 * sfc->normalindices[i + 0]], &t_, 3 * sizeof(float));

			Vec3Set(&n, &meshdef->normals[3 * sfc->normalindices[i + 1]]);
			Vec3Cross(&b_, &n, &t);
			Vec3Cross(&t_, &b_, &n);
			memcpy(&meshdef->binormals[3 * sfc->normalindices[i + 1]], &b_, 3 * sizeof(float));
			memcpy(&meshdef->tangents[3 * sfc->normalindices[i + 1]], &t_, 3 * sizeof(float));

			Vec3Set(&n, &meshdef->normals[3 * sfc->normalindices[i + 2]]);
			Vec3Cross(&b_, &n, &t);
			Vec3Cross(&t_, &b_, &n);
			memcpy(&meshdef->binormals[3 * sfc->normalindices[i + 2]], &b_, 3 * sizeof(float));
			memcpy(&meshdef->tangents[3 * sfc->normalindices[i + 2]], &t_, 3 * sizeof(float));
		}
	}

	return R_OK;
}