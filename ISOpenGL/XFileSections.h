inline Result ReadMeshSection(std::ifstream* fs, MeshData *mdata)
{
	std::list<Template*>::const_iterator template_itr;
	PlainData plaindata;
	WORD token;
	UINT i;
	Result rlt;

	// '{'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_OBRACE)
		return ERR("");

	// 1 integer
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_INTEGER_LIST && token != TOKEN_PLAININTEGER)
		return ERR("");
	if(plaindata.len != 1)
		return ERR("");

	// numvertices
	mdata->numvertices = *plaindata.ptr.i;
	CHKALLOC(mdata->vertices = new MeshData::Vertex[mdata->numvertices]);
	if(fileformat == D3DXF_FILEFORMAT_TEXT)
	{
		// ';'
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_SEMICOLON)
			return ERR("");

		// numvertices vertex positions
		for(i = 0; i < mdata->numvertices; i++)
		{
			// 1st float (x coordinate)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			mdata->vertices[i].pos.x = *plaindata.ptr.f;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// 2nd float (y coordinate)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			mdata->vertices[i].pos.y = *plaindata.ptr.f;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// 3rd float (z coordinate)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			mdata->vertices[i].pos.z = *plaindata.ptr.f;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			if(i == mdata->numvertices - 1)
			{
				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}
			else
			{
				// ','
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_COMMA)
					return ERR("");
			}
		}
	}
	else
	{
		// numvertices * 3 floats
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_FLOAT_LIST)
			return ERR("");
		if(plaindata.len != mdata->numvertices * 3)
			return ERR("");
		for(i = 0; i < mdata->numvertices; i++)
		{
			mdata->vertices[i].pos.x = plaindata.ptr.f[i * 3 + 0];
			mdata->vertices[i].pos.y = plaindata.ptr.f[i * 3 + 1];
			mdata->vertices[i].pos.z = plaindata.ptr.f[i * 3 + 2];
		}
	}

	// 1 integer (text format), 1 + numfaces integers (binary format)
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_INTEGER_LIST && token != TOKEN_PLAININTEGER)
		return ERR("");

	// numfaces
	mdata->numfaces = *plaindata.ptr.i;
	CHKALLOC(mdata->indices = new INDEX_TYPE[3 * mdata->numfaces]);
	if(fileformat == D3DXF_FILEFORMAT_TEXT)
	{
		// ';'
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_SEMICOLON)
			return ERR("");

		// numfaces position indices
		for(i = 0; i < mdata->numfaces; i++)
		{
			// nFaceVertexIndices (only 3 supported)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER || *plaindata.ptr.i != 3)
				return ERR("");

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// 1st index
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER)
				return ERR("");
			mdata->indices[i * 3 + 0] = (INDEX_TYPE)*plaindata.ptr.i;

			// ','
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_COMMA)
				return ERR("");

			// 2nd index
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER)
				return ERR("");
			mdata->indices[i * 3 + 1] = (INDEX_TYPE)*plaindata.ptr.i;

			// ','
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_COMMA)
				return ERR("");

			// 3rd index
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER)
				return ERR("");
			mdata->indices[i * 3 + 2] = (INDEX_TYPE)*plaindata.ptr.i;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			if(i == mdata->numfaces - 1)
			{
				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}
			else
			{
				// ','
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_COMMA)
					return ERR("");
			}
		}
	}
	else
	{
		// numfaces faces: First element = numfaces; Every 4th element = nFaceVertexIndices (1, 5, 9, ...)
		for(i = 0; i < mdata->numfaces; i++)
		{
			// nFaceVertexIndices (only 3 supported)
			if(plaindata.ptr.i[i * 4 + 1] != 3)
				return ERR("");

			// 1st, 2nd and 3rd index
			mdata->indices[i * 3 + 0] = (INDEX_TYPE)plaindata.ptr.i[i * 4 + 2];
			mdata->indices[i * 3 + 1] = (INDEX_TYPE)plaindata.ptr.i[i * 4 + 3];
			mdata->indices[i * 3 + 2] = (INDEX_TYPE)plaindata.ptr.i[i * 4 + 4];
		}
	}

	return R_OK;
}

inline Result ReadMeshNormalsSection(std::ifstream* fs, MeshData *mdata)
{
	std::list<Template*>::const_iterator template_itr;
	PlainData plaindata;
	WORD token;
	UINT i;
	Result rlt;

	// '{'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_OBRACE)
		return ERR("");

	// 1 integer
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_INTEGER_LIST && token != TOKEN_PLAININTEGER)
		return ERR("");
	if(plaindata.len != 1)
		return ERR("");

	// numvertices
	if(*plaindata.ptr.i != mdata->numvertices)
		return ERR("");
	if(fileformat == D3DXF_FILEFORMAT_TEXT)
	{
		// ';'
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_SEMICOLON)
			return ERR("");

		// numvertices vertex normals
		for(i = 0; i < mdata->numvertices; i++)
		{
			// 1st float (x coordinate)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			mdata->vertices[i].nml.x = *plaindata.ptr.f;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// 2nd float (y coordinate)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			mdata->vertices[i].nml.y = *plaindata.ptr.f;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// 3rd float (z coordinate)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			mdata->vertices[i].nml.z = *plaindata.ptr.f;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			if(i == mdata->numvertices - 1)
			{
				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}
			else
			{
				// ','
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_COMMA)
					return ERR("");
			}
		}
	}
	else
	{
		// numvertices * 3 floats
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_FLOAT_LIST)
			return ERR("");
		if(plaindata.len != mdata->numvertices * 3)
			return ERR("");
		for(i = 0; i < mdata->numvertices; i++)
		{
			mdata->vertices[i].nml.x = plaindata.ptr.f[i * 3 + 0];
			mdata->vertices[i].nml.y = plaindata.ptr.f[i * 3 + 1];
			mdata->vertices[i].nml.z = plaindata.ptr.f[i * 3 + 2];
		}
	}

	// 1 integer (text format), 1 + numfaces integers (binary format)
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_INTEGER_LIST && token != TOKEN_PLAININTEGER)
		return ERR("");

	// numfaces
	if(*plaindata.ptr.i != mdata->numfaces)
		return ERR("");
	if(fileformat == D3DXF_FILEFORMAT_TEXT)
	{
		// ';'
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_SEMICOLON)
			return ERR("");

		// numfaces normal indices
		for(i = 0; i < mdata->numfaces; i++)
		{
			// nFaceVertexIndices (only 3 supported)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER || *plaindata.ptr.i != 3)
				return ERR("");

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// 1st index
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER)
				return ERR("");
			if((INDEX_TYPE)*plaindata.ptr.i != mdata->indices[i * 3 + 0])
				return ERR("");

			// ','
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_COMMA)
				return ERR("");

			// 2nd index
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER)
				return ERR("");
			if((INDEX_TYPE)*plaindata.ptr.i != mdata->indices[i * 3 + 1])
				return ERR("");

			// ','
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_COMMA)
				return ERR("");

			// 3rd index
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER)
				return ERR("");
			if((INDEX_TYPE)*plaindata.ptr.i != mdata->indices[i * 3 + 2])
				return ERR("");

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			if(i == mdata->numfaces - 1)
			{
				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}
			else
			{
				// ','
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_COMMA)
					return ERR("");
			}
		}
	}
	else
	{
		// numfaces faces: First element = numfaces; Every 4th element = nFaceVertexIndices (1, 5, 9, ...)
		for(i = 0; i < mdata->numfaces; i++)
		{
			// nFaceVertexIndices (only 3 supported)
			if(plaindata.ptr.i[i * 4 + 1] != 3)
				return ERR("");

			// 1st, 2nd and 3rd index
			if((INDEX_TYPE)plaindata.ptr.i[i * 4 + 2] != mdata->indices[i * 3 + 0])
				return ERR("");
			if((INDEX_TYPE)plaindata.ptr.i[i * 4 + 3] != mdata->indices[i * 3 + 1])
				return ERR("");
			if((INDEX_TYPE)plaindata.ptr.i[i * 4 + 4] != mdata->indices[i * 3 + 2])
				return ERR("");
		}
	}

	// '}'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_CBRACE)
		return ERR("");

	return R_OK;
}

inline Result ReadMeshTextureCoordsSection(std::ifstream* fs, MeshData *mdata)
{
	std::list<Template*>::const_iterator template_itr;
	PlainData plaindata;
	WORD token;
	UINT i;
	Result rlt;

	// '{'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_OBRACE)
		return ERR("");

	// 1 integer
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_INTEGER_LIST && token != TOKEN_PLAININTEGER)
		return ERR("");
	if(plaindata.len != 1)
		return ERR("");

	// numvertices
	if(*plaindata.ptr.i != mdata->numvertices)
		return ERR("");
	if(fileformat == D3DXF_FILEFORMAT_TEXT)
	{
		// ';'
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_SEMICOLON)
			return ERR("");

		// numvertices vertex texture coordinates
		for(i = 0; i < mdata->numvertices; i++)
		{
			// 1st float (u coordinate)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			mdata->vertices[i].texcoord.x = *plaindata.ptr.f;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// 2nd float (v coordinate)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			mdata->vertices[i].texcoord.y = *plaindata.ptr.f;

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			if(i == mdata->numvertices - 1)
			{
				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}
			else
			{
				// ','
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_COMMA)
					return ERR("");
			}
		}
	}
	else
	{
		// numvertices * 2 floats
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_FLOAT_LIST)
			return ERR("");
		if(plaindata.len != mdata->numvertices * 2)
			return ERR("");
		for(i = 0; i < mdata->numvertices; i++)
		{
			mdata->vertices[i].texcoord.x = abs(plaindata.ptr.f[i * 2 + 0]);
			mdata->vertices[i].texcoord.y = abs(plaindata.ptr.f[i * 2 + 1]);
		}
	}

	// '}'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_CBRACE)
		return ERR("");

	return R_OK;
}

inline Result ReadMeshMaterialListSection(std::ifstream* fs, const MeshData *mdata, IObject::AttributeRange** attrs, XMaterial** mats, UINT* nummats)
{
	std::list<Template*>::const_iterator template_itr;
	PlainData plaindata;
	WORD token;
	UINT i;
	Result rlt;

	// '{'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_OBRACE)
		return ERR("");

	UINT* attrids;
	bool freeattrids = false;
	if(fileformat == D3DXF_FILEFORMAT_TEXT)
	{
		// nummaterials
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_PLAININTEGER)
			return ERR("");
		*nummats = plaindata.ptr.i[0];

		// ';'
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_SEMICOLON)
			return ERR("");

		// numfaces
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_PLAININTEGER)
			return ERR("");
		if(plaindata.ptr.i[0] != mdata->numfaces)
			return ERR("");

		// ';'
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_SEMICOLON)
			return ERR("");

		// numfaces attribute ids
		CHKALLOC(attrids = new UINT[mdata->numfaces]);
		freeattrids = true;
		for(i = 0; i < mdata->numfaces; i++)
		{
			// attribute id
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAININTEGER)
				return ERR("");
			attrids[i] = plaindata.ptr.i[0];

			if(i == mdata->numfaces - 1)
			{
				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}
			else
			{
				// ','
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_COMMA)
					return ERR("");
			}
		}
	}
	else
	{
		// numfaces + 2 integers
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_INTEGER_LIST)
			return ERR("");
		if(plaindata.len != mdata->numfaces + 2)
			return ERR("");

		// nummaterials
		*nummats = plaindata.ptr.i[0];

		// numfaces
		if(plaindata.ptr.i[1] != mdata->numfaces)
			return ERR("");

		attrids = plaindata.ptr.i + 2;
	}

	IObject::AttributeRange* _attrs = *attrs = new IObject::AttributeRange[*nummats];
	XMaterial* _mats = *mats = new XMaterial[*nummats];
	CHKALLOC(attrs);

	int currentattridx = attrids[0];
	_attrs[currentattridx].attrid = currentattridx;
	_attrs[currentattridx].facestart = 0;
	_attrs[currentattridx].vertexstart = (UINT)-1;
	_attrs[currentattridx].vertexcount = 0;
	for(i = 0; i < mdata->numfaces; i++)
	{
		if(attrids[i] != currentattridx) {
			_attrs[currentattridx].facecount =  i - _attrs[currentattridx].facestart;
			_attrs[currentattridx].vertexcount -= _attrs[currentattridx].vertexstart; // Get VertexCount from VertexEnd

			currentattridx = attrids[i];
			_attrs[currentattridx].attrid = currentattridx;
			_attrs[currentattridx].facestart = i;
			_attrs[currentattridx].vertexstart = (UINT)-1;
			_attrs[currentattridx].vertexcount = 0;
		}
		_attrs[currentattridx].vertexstart = min(_attrs[currentattridx].vertexstart, mdata->indices[i * 3 + 0]);
		_attrs[currentattridx].vertexstart = min(_attrs[currentattridx].vertexstart, mdata->indices[i * 3 + 1]);
		_attrs[currentattridx].vertexstart = min(_attrs[currentattridx].vertexstart, mdata->indices[i * 3 + 2]);
		_attrs[currentattridx].vertexcount = max(_attrs[currentattridx].vertexcount, mdata->indices[i * 3 + 0]); // VertexEnd
		_attrs[currentattridx].vertexcount = max(_attrs[currentattridx].vertexcount, mdata->indices[i * 3 + 1]); // VertexEnd
		_attrs[currentattridx].vertexcount = max(_attrs[currentattridx].vertexcount, mdata->indices[i * 3 + 2]); // VertexEnd
	}
	_attrs[currentattridx].facecount =  mdata->numfaces - _attrs[currentattridx].facestart;
	_attrs[currentattridx].vertexcount -= _attrs[currentattridx].vertexstart; // Get VertexCount from VertexEnd
	if(freeattrids)
		delete[] attrids;

	for(i = 0; i < *nummats; i++)
	{
		// "Material"
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_PLAINTEXT && token != TOKEN_NAME)
			return ERR("");
		if(*plaindata.ptr.s != String("Material"))
			return ERR("");

		// '{'
		CHKRESULT(ParseToken(fs, &token, plaindata));
		if(token != TOKEN_OBRACE)
			return ERR("");

		if(fileformat == D3DXF_FILEFORMAT_TEXT)
		{
			// faceColor.rgba
			for(i = 0; i < 4; i++)
			{
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_PLAINDECIMAL)
					return ERR("");
				_mats->faceColor[i] = plaindata.ptr.f[0];

				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// power
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINDECIMAL)
				return ERR("");
			_mats->power = plaindata.ptr.f[0];

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// specularColor.rgb
			for(i = 0; i < 3; i++)
			{
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_PLAINDECIMAL)
					return ERR("");
				_mats->specularColor[i] = plaindata.ptr.f[0];

				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// emissiveColor.rgb
			for(i = 0; i < 3; i++)
			{
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_PLAINDECIMAL)
					return ERR("");
				_mats->emissiveColor[i] = plaindata.ptr.f[0];

				// ';'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_SEMICOLON)
					return ERR("");
			}

			// ';'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// "TextureFilename"
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_PLAINTEXT && token != TOKEN_NAME)
				return ERR("");
			if(*plaindata.ptr.s != String("TextureFilename"))
				return ERR("");

			// '{'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_OBRACE)
				return ERR("");

			// TextureFilename (if defined)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token == TOKEN_STRING)
			{
				if(plaindata.ptr.s)
					plaindata.ptr.s->Copy(&_mats[i].textureFilename);

				CHKRESULT(ParseToken(fs, &token, plaindata));
			}

			// ';'
			if(token != TOKEN_SEMICOLON)
				return ERR("");

			// '}'
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_CBRACE)
				return ERR("");

			CHKRESULT(ParseToken(fs, &token, plaindata));
		}
		else
		{
			// 11 floats (beeing the first 11 floats that make up the XMaterial struct)
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if(token != TOKEN_FLOAT_LIST)
				return ERR("");
			if(plaindata.len != 11)
				return ERR("");
			memcpy((float*)(_mats + i), plaindata.ptr.f, 11 * sizeof(float));

			// "TextureFilename"
			CHKRESULT(ParseToken(fs, &token, plaindata));
			if((token == TOKEN_PLAINTEXT || token == TOKEN_NAME) && *plaindata.ptr.s == String("TextureFilename"))
			{
				// '{'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_OBRACE)
					return ERR("");

				// TextureFilename
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_STRING)
					return ERR("");
				if(plaindata.ptr.s)
					plaindata.ptr.s->Copy(&_mats[i].textureFilename);

				// '}'
				CHKRESULT(ParseToken(fs, &token, plaindata));
				if(token != TOKEN_CBRACE)
					return ERR("");

				CHKRESULT(ParseToken(fs, &token, plaindata));
			}
		}

		// '}'
		if(token != TOKEN_CBRACE)
			return ERR("");
	}

	// '}'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_CBRACE)
		return ERR("");

	return R_OK;
}

inline Result ReadMeshFVFDataSection(std::ifstream* fs, MeshData *mdata)
{
	std::list<Template*>::const_iterator template_itr;
	PlainData plaindata;
	WORD token;
	UINT i;
	Result rlt;

	// '{'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_OBRACE)
		return ERR("");

	// numvertices + 2 integers
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_INTEGER_LIST)
		return ERR("");
	if(plaindata.len != mdata->numvertices + 2)
		return ERR("");

	// plaindata.ptr.i[0] == FVF code (ignored)

	// numvertices
	if(plaindata.ptr.i[1] != mdata->numvertices)
		return ERR("");

	UINT* blendindices = plaindata.ptr.i + 2;
	mdata->numblendparts = 0;
	for(i = 0; i < mdata->numvertices; i++)
	{
		mdata->vertices[i].blendidx = blendindices[i];
		mdata->numblendparts = max(mdata->numblendparts, blendindices[i]);
	}
	mdata->numblendparts++;

	// '}'
	CHKRESULT(ParseToken(fs, &token, plaindata));
	if(token != TOKEN_CBRACE)
		return ERR("");

	return R_OK;
}