#include <ISSceneDefinition.h>
#include <fstream>

#define READELEMENTALLOCSIZE	256
Result ReadXmlElement(std::ifstream& fs, char** elementstr)
{
	*elementstr = NULL;
	char *linestart, *line, c;
	int bracketlevel = 1;
	bool incomment = false;

	// Skip trailing white spaces
	while((c = fs.peek()) != -1)
	{
		fs.read(&c, 1);
		if(c != ' ' && c != '\t' && c != '\r' && c != '\n')
			break;
	}
	if(c == -1)
		return R_OK;

	// Validate begin of XML element
	if(c != '<')
		return ERR("Syntax error: Expected '<' char");

	*elementstr = linestart = line = new char[READELEMENTALLOCSIZE]; // Allocate some memory
	CHKALLOC(linestart);

	while(fs.peek() != -1) // Break on EOF
	{
		fs.read(&c, 1); // Read char

		if(incomment)
		{
			// Check for comment end sequence
			if(c != '-')
				continue;
			fs.read(&c, 1); // Read char
			if(c != '-')
				continue;
			fs.read(&c, 1); // Read char
			if(c != '>')
				continue;

			// Close comment
			incomment = false;
			continue;
		}

		// Check for comment start sequence
		if(c == '-' && line[-1] == '-' && line[-2] == '!' && line[-3] == '<')
		{
			incomment = true;
			line -= 3; bracketlevel--; // Ignore comment start sequence
			continue;
		}

		if(c == '<')
			bracketlevel++;
		else if(c == '>' && --bracketlevel == 0)
			break; // Break on '>'
		else if(c == '\r' || c == '\n')
			continue; // Skip new line characters

		if((line - linestart) % READELEMENTALLOCSIZE == READELEMENTALLOCSIZE - 1) // -1 ... NULL-char
		{
			size_t len = line - linestart;
			*elementstr = linestart = (char*)realloc(linestart, len + 1 + READELEMENTALLOCSIZE); // Allocate more memory // +1 ... NULL-char
			line = linestart + len;
			CHKALLOC(linestart);
		}

		*line++ = c; // Add char
	}

	*line = '\0'; // Add '\0'-char

	return R_OK;
}
static const size_t strlen_script = strlen("</script>");
Result ReadXmlScriptElement(std::ifstream& fs, char** elementstr)
{
	*elementstr = NULL;
	char *linestart, *line, c;

	*elementstr = linestart = line = new char[READELEMENTALLOCSIZE]; // Allocate some memory
	CHKALLOC(linestart);

	bool scriptclosed = false;
	while(fs.peek() != -1) // Break on EOF
	{
		fs.read(&c, 1); // Read char

		if((line - linestart) % READELEMENTALLOCSIZE == READELEMENTALLOCSIZE - 1) // -1 ... NULL-char
		{
			size_t len = line - linestart;
			*elementstr = linestart = (char*)realloc(linestart, len + 1 + READELEMENTALLOCSIZE); // Allocate more memory // +1 ... NULL-char
			line = linestart + len;
			CHKALLOC(linestart);
		}

		*line++ = c; // Add char

		// Check for script end sequence
		if((size_t)(line - linestart) >= strlen_script)
			if(strncmp(line - strlen_script, "</script>", strlen_script) == 0)
			{
				scriptclosed = true;
				line -= strlen_script;
				break;
			}
	}

	*line = '\0'; // Add '\0'-char

	return scriptclosed ? R_OK : ERR("Syntax error: Unexpected end of file in script expansion (expected </script>)");
}

void SplitEx(char* str, std::vector<char*>& words)
{
	char *wordstart = str, *c = str, *newword;
	bool inquotes = false;

	while(*c != '\0')
	{
		if(((*c == ' ' || *c == '(' || *c == ')' || *c == '=') && !inquotes) || *c == '\"' || *c == '\'')
		{
			if(c != wordstart)
			{
				newword = new char[c - wordstart + 1];
				memcpy(newword, wordstart, c - wordstart + 1);
				newword[c - wordstart] = '\0';
				words.push_back(newword);
			}

			if(*c != ' ')
			{
				newword = new char[2];
				newword[0] = *c;
				newword[1] = '\0';
				words.push_back(newword);
			}

			if(*c == '\"' || *c == '\"')
				inquotes = !inquotes; // Next token is quoted and should only be terminated by quotes

			wordstart = ++c;
		}
		else
			++c;
	}

	if(c != wordstart)
	{
		newword = new char[c - wordstart + 1];
		memcpy(newword, wordstart, c - wordstart + 1);
		newword[c - wordstart] = '\0';
		words.push_back(newword);
	}
}

struct XmlProperty
{
	String name, value;

	XmlProperty(String name, String value)
	{
		this->name = name;
		this->value = value;
	}
};

struct XmlElement
{
	String name, script;
	XmlElement* parentelement;
	std::list<XmlElement> childelements;
	std::list<XmlProperty> properties;

	XmlElement(String name, XmlElement* parentelement)
	{
		this->name = name;
		this->parentelement = parentelement;
	}
};

Result ParseXml(FilePath filename, XmlElement** rootelement)
{
	*rootelement = NULL;

	if(!filename.Exists())
		return ERR(String("File not found: ") << filename);
	Result rlt;

	/*// Get parent directory
	FilePath parentdir;
	filename.GetParentDir(&parentdir);*/

	std::ifstream fs(filename, std::ios_base::in);
	char* elementstr;
	std::vector<char*> words;
	XmlElement *currentelement = NULL, *newelement;

	CHKRESULT(ReadXmlElement(fs, &elementstr));
	while(elementstr)
	{
		switch(elementstr[0])
		{
		case '?':
			if(elementstr[strlen(elementstr) - 1] != '?')
				return ERR("Syntax error: XML element starting with '<?' must end with '?>'");
			break; // Skip '<? ... ?>' section

		case '!':
			/*SplitEx(elementstr + 1, words);
			if(words.size() != 9 || strcmp(words[0], "DOCTYPE") != 0 || strcmp(words[4], "SYSTEM") != 0)
				break;
Result::PrintLogMessage(words[2]);
Result::PrintLogMessage(words[7]);*/
			break; // Skip '<! ... >' section

		case '/':
			// Close element
			if(currentelement == NULL || strcmp(elementstr + 1, currentelement->name) != 0)
				return ERR(String("Syntax error: unexpected XML element: ") << elementstr);
			currentelement = currentelement->parentelement;
			break;

		default:
			SplitEx(elementstr, words);
			if(words.size() == 0)
				break;

			// Add new element
			if(currentelement)
			{
				currentelement->childelements.push_back(XmlElement(words[0], currentelement));
				newelement = &currentelement->childelements.back();
				if(strcmp(words[words.size() - 1], "/") == 0)
					{delete words[words.size() - 1]; words.pop_back();}
				else
					currentelement = newelement;
			}
			else
			{
				if(strncmp(words[words.size() - 1], "/", 1) == 0)
					return ERR("Syntax error: First element can't be inline element");
				CHKALLOC(*rootelement = currentelement = newelement = new XmlElement(words[0], NULL));
			}

			// Add properties to new element
			for(int i = 1; i <= (int)words.size() - 5; i += 5)
			{
				if(strncmp(words[i + 1], "=", 1) != 0 || strncmp(words[i + 2], "\"", 1) != 0 || strncmp(words[i + 4], "\"", 1) != 0)
					break;
				newelement->properties.push_back(XmlProperty(words[i + 0], words[i + 3]));
			}
		}

		// Free memory
		delete[] elementstr;
		std::vector<char*>::iterator iter;
		LIST_FOREACH(words, iter)
			delete[] *iter;
		words.clear();

		if(currentelement && strcmp(currentelement->name, "script") == 0)
		{
			CHKRESULT(ReadXmlScriptElement(fs, &elementstr));
			currentelement->script = String::StealReference(elementstr);
			elementstr = new char[strlen_script - 2 + 1]; // strlen("</script>") - strlen("<>") + strlen("\0")
			strcpy(elementstr, "/script");
		}
		else
			CHKRESULT(ReadXmlElement(fs, &elementstr));
	}

	if(currentelement)
		return ERR(String("Syntax error: Missing '</") << currentelement->name << String(">' element"));

	fs.close();

	return R_OK;
}

Result ParseXmlScene(FilePath filename, SceneDefinition** scenedef)
{
	*scenedef = NULL;
	XmlElement* rootelement;
	Result rlt;

	Result::PrintLogMessage(String("Parsing file: ") << filename);
	CHKRESULT(ParseXml(filename, &rootelement));

	if(rootelement->name != String("scene"))
		return ERR("Scene definition error: Root XML element must be 'scene'");

	*scenedef = new SceneDefinition();
	SceneDefinition* _scenedef = *scenedef;
	std::list<XmlProperty>::const_iterator propiter;
	std::list<XmlElement>::const_iterator childiter, childiter2, childiter3, childiter4;

	// Set scene properties
	LIST_FOREACH(rootelement->properties, propiter)
		if(propiter->name == String("output_file"))
			_scenedef->output_file = propiter->value;

	// Set scene elements
	LIST_FOREACH(rootelement->childelements, childiter)
	{
		if(childiter->name == String("background_color"))
		{
			// Set background color properties
			LIST_FOREACH(childiter->properties, propiter)
			{
				if(propiter->name == String("r"))
					_scenedef->background_color[0] = propiter->value.ToFloat();
				else if(propiter->name == String("g"))
					_scenedef->background_color[1] = propiter->value.ToFloat();
				else if(propiter->name == String("b"))
					_scenedef->background_color[2] = propiter->value.ToFloat();
			}
		}
		else if(childiter->name == String("camera"))
		{
			// Set camera elements
			LIST_FOREACH(childiter->childelements, childiter2)
			{
				if(childiter2->name == String("position"))
				{
					// Set position properties
					LIST_FOREACH(childiter2->properties, propiter)
					{
						if(propiter->name == String("x"))
							_scenedef->camera.position[0] = propiter->value.ToFloat();
						else if(propiter->name == String("y"))
							_scenedef->camera.position[1] = propiter->value.ToFloat();
						else if(propiter->name == String("z"))
							_scenedef->camera.position[2] = propiter->value.ToFloat();
					}
				}
				else if(childiter2->name == String("lookat"))
				{
					// Set lookat properties
					LIST_FOREACH(childiter2->properties, propiter)
					{
						if(propiter->name == String("x"))
							_scenedef->camera.lookat[0] = propiter->value.ToFloat();
						else if(propiter->name == String("y"))
							_scenedef->camera.lookat[1] = propiter->value.ToFloat();
						else if(propiter->name == String("z"))
							_scenedef->camera.lookat[2] = propiter->value.ToFloat();
					}
				}
				else if(childiter2->name == String("up"))
				{
					// Set up properties
					LIST_FOREACH(childiter2->properties, propiter)
					{
						if(propiter->name == String("x"))
							_scenedef->camera.up[0] = propiter->value.ToFloat();
						else if(propiter->name == String("y"))
							_scenedef->camera.up[1] = propiter->value.ToFloat();
						else if(propiter->name == String("z"))
							_scenedef->camera.up[2] = propiter->value.ToFloat();
					}
				}
				else if(childiter2->name == String("horizontal_fov"))
				{
					// Set horizontal_fov properties
					LIST_FOREACH(childiter2->properties, propiter)
						if(propiter->name == String("angle"))
							_scenedef->camera.horizontal_fov = propiter->value.ToFloat();
				}
				else if(childiter2->name == String("resolution"))
				{
					// Set resolution properties
					LIST_FOREACH(childiter2->properties, propiter)
					{
						if(propiter->name == String("horizontal"))
							_scenedef->camera.resolution[0] = propiter->value.ToInt();
						else if(propiter->name == String("vertical"))
							_scenedef->camera.resolution[1] = propiter->value.ToInt();
					}
				}
				else if(childiter2->name == String("max_bounces"))
				{
					// Set max_bounces properties
					LIST_FOREACH(childiter2->properties, propiter)
						if(propiter->name == String("n"))
							_scenedef->camera.max_bounces = propiter->value.ToInt();
				}
			}
		}
		else if(childiter->name == String("lights"))
		{
			// Add light
			LIST_FOREACH(childiter->childelements, childiter2)
			{
				SceneDefinition::Light* currentlight;
				if(childiter2->name == String("ambient_light"))
				{
					// Create ambient light and set specific properties and elements
					_scenedef->lights.push_back(currentlight = new SceneDefinition::AmbientLight());
				}
				else if(childiter2->name == String("point_light"))
				{
					// Create point light and set specific properties and elements
					SceneDefinition::PointLight* currentpointlight = new SceneDefinition::PointLight();
					_scenedef->lights.push_back(currentlight = currentpointlight);
					LIST_FOREACH(childiter2->childelements, childiter3)
					{
						if(childiter3->name == String("position"))
						{
							// Set position properties
							LIST_FOREACH(childiter3->properties, propiter)
							{
								if(propiter->name == String("x"))
									currentpointlight->position[0] = propiter->value.ToFloat();
								else if(propiter->name == String("y"))
									currentpointlight->position[1] = propiter->value.ToFloat();
								else if(propiter->name == String("z"))
									currentpointlight->position[2] = propiter->value.ToFloat();
							}
						}
					}
				}
				else if(childiter2->name == String("parallel_light"))
				{
					// Create point light and set specific properties and elements
					SceneDefinition::ParallelLight* currentparallellight = new SceneDefinition::ParallelLight();
					_scenedef->lights.push_back(currentlight = currentparallellight);
					LIST_FOREACH(childiter2->childelements, childiter3)
					{
						if(childiter3->name == String("direction"))
						{
							// Set position properties
							LIST_FOREACH(childiter3->properties, propiter)
							{
								if(propiter->name == String("x"))
									currentparallellight->direction[0] = propiter->value.ToFloat();
								else if(propiter->name == String("y"))
									currentparallellight->direction[1] = propiter->value.ToFloat();
								else if(propiter->name == String("z"))
									currentparallellight->direction[2] = propiter->value.ToFloat();
							}
						}
					}
				}
				else
					break;

				// Set general light elements
				LIST_FOREACH(childiter2->childelements, childiter3)
				{
					if(childiter3->name == String("color"))
					{
						// Set color properties
						LIST_FOREACH(childiter3->properties, propiter)
						{
							if(propiter->name == String("r"))
								currentlight->color[0] = propiter->value.ToFloat();
							else if(propiter->name == String("g"))
								currentlight->color[1] = propiter->value.ToFloat();
							else if(propiter->name == String("b"))
								currentlight->color[2] = propiter->value.ToFloat();
						}
					}
				}
			}
		}
		else if(childiter->name == String("surfaces"))
		{
			// Add surface
			LIST_FOREACH(childiter->childelements, childiter2)
			{
				SceneDefinition::Surface* currentsurface;
				if(childiter2->name == String("sphere"))
				{
					// Create sphere surface and set specific properties and elements
					SceneDefinition::Sphere* currentsphere = new SceneDefinition::Sphere();
					_scenedef->surfaces.push_back(currentsurface = currentsphere);
					LIST_FOREACH(childiter2->properties, propiter)
						if(propiter->name == String("radius"))
							currentsphere->radius = propiter->value.ToFloat();
					LIST_FOREACH(childiter2->childelements, childiter3)
					{
						if(childiter3->name == String("position"))
						{
							// Set position properties
							LIST_FOREACH(childiter3->properties, propiter)
							{
								if(propiter->name == String("x"))
									currentsphere->position[0] = propiter->value.ToFloat();
								else if(propiter->name == String("y"))
									currentsphere->position[1] = propiter->value.ToFloat();
								else if(propiter->name == String("z"))
									currentsphere->position[2] = propiter->value.ToFloat();
							}
						}
					}
				}
				else if(childiter2->name == String("mesh"))
				{
					// Create mesh surface and set specific properties and elements
					SceneDefinition::Mesh* currentmesh = new SceneDefinition::Mesh();
					_scenedef->surfaces.push_back(currentsurface = currentmesh);
					LIST_FOREACH(childiter2->properties, propiter)
						if(propiter->name == String("name"))
							currentmesh->mesh_file = propiter->value;
				}
				else if(childiter2->name == String("vmesh"))
				{
					// Create voxel mesh surface and set specific properties and elements
					SceneDefinition::VMesh* currentvmesh = new SceneDefinition::VMesh();
					_scenedef->surfaces.push_back(currentsurface = currentvmesh);
					LIST_FOREACH(childiter2->properties, propiter)
						if(propiter->name == String("name"))
							currentvmesh->voxel_file = propiter->value;
				}
				else
					break;

				// Set general surface properties and elements
				LIST_FOREACH(childiter2->properties, propiter)
					if(propiter->name == String("key"))
						currentsurface->key = propiter->value;
				LIST_FOREACH(childiter2->childelements, childiter3)
				{
					if(childiter3->name == String("material_solid"))
					{
						SceneDefinition::SolidMaterial* currentmaterial = new SceneDefinition::SolidMaterial();
						currentsurface->material = currentmaterial;
						// Set solid material elements
						LIST_FOREACH(childiter3->childelements, childiter4)
						{
							if(childiter4->name == String("color"))
							{
								// Set color properties
								LIST_FOREACH(childiter4->properties, propiter)
								{
									if(propiter->name == String("r"))
										currentmaterial->color[0] = propiter->value.ToFloat();
									else if(propiter->name == String("g"))
										currentmaterial->color[1] = propiter->value.ToFloat();
									else if(propiter->name == String("b"))
										currentmaterial->color[2] = propiter->value.ToFloat();
								}
							}
						}
					}
					else if(childiter3->name == String("material_textured"))
					{
						SceneDefinition::TexturedMaterial* currentmaterial = new SceneDefinition::TexturedMaterial();
						currentsurface->material = currentmaterial;
						// Set textured material elements
						LIST_FOREACH(childiter3->childelements, childiter4)
						{
							if(childiter4->name == String("texture"))
							{
								// Set texture properties
								LIST_FOREACH(childiter4->properties, propiter)
									if(propiter->name == String("name"))
										currentmaterial->texture_file = propiter->value;
							}
							else if(childiter4->name == String("normalmap"))
							{
								// Set texture properties
								LIST_FOREACH(childiter4->properties, propiter)
									if(propiter->name == String("name"))
										currentmaterial->normalmap_file = propiter->value;
							}
						}
					}
					else if(childiter3->name == String("transform"))
					{
						currentsurface->transform = new SceneDefinition::Transform();
						// Set textured material elements
						LIST_FOREACH(childiter3->childelements, childiter4)
						{
							if(childiter4->name == String("translate"))
							{
								// Set texture properties
								LIST_FOREACH(childiter4->properties, propiter)
								{
									if(propiter->name == String("x"))
										currentsurface->transform->translate[0] = propiter->value.ToFloat();
									else if(propiter->name == String("y"))
										currentsurface->transform->translate[1] = propiter->value.ToFloat();
									else if(propiter->name == String("z"))
										currentsurface->transform->translate[2] = propiter->value.ToFloat();
								}
							}
							else if(childiter4->name == String("scale"))
							{
								// Set texture properties
								LIST_FOREACH(childiter4->properties, propiter)
								{
									if(propiter->name == String("x"))
										currentsurface->transform->scale[0] = propiter->value.ToFloat();
									else if(propiter->name == String("y"))
										currentsurface->transform->scale[1] = propiter->value.ToFloat();
									else if(propiter->name == String("z"))
										currentsurface->transform->scale[2] = propiter->value.ToFloat();
								}
							}
							else if(childiter4->name == String("rotateX"))
							{
								// Set texture properties
								LIST_FOREACH(childiter4->properties, propiter)
									if(propiter->name == String("theta"))
										currentsurface->transform->rotate[0] = propiter->value.ToFloat();
							}
							else if(childiter4->name == String("rotateY"))
							{
								// Set texture properties
								LIST_FOREACH(childiter4->properties, propiter)
									if(propiter->name == String("theta"))
										currentsurface->transform->rotate[1] = propiter->value.ToFloat();
							}
							else if(childiter4->name == String("rotateZ"))
							{
								// Set texture properties
								LIST_FOREACH(childiter4->properties, propiter)
									if(propiter->name == String("theta"))
										currentsurface->transform->rotate[2] = propiter->value.ToFloat();
							}
						}
						continue;
					}
					else
						continue;

					// Set general material elements
					LIST_FOREACH(childiter3->childelements, childiter4)
					{
						if(childiter4->name == String("phong"))
						{
							// Set phong properties
							LIST_FOREACH(childiter4->properties, propiter)
							{
								if(propiter->name == String("ka"))
									currentsurface->material->phong[0] = propiter->value.ToFloat();
								else if(propiter->name == String("kd"))
									currentsurface->material->phong[1] = propiter->value.ToFloat();
								else if(propiter->name == String("ks"))
									currentsurface->material->phong[2] = propiter->value.ToFloat();
								else if(propiter->name == String("exponent"))
									currentsurface->material->phong[3] = propiter->value.ToFloat();
								else if(propiter->name == String("ke"))
									currentsurface->material->phong[4] = propiter->value.ToFloat();
							}
						}
						else if(childiter4->name == String("reflectance"))
						{
							// Set reflectance properties
							LIST_FOREACH(childiter4->properties, propiter)
								if(propiter->name == String("r"))
									currentsurface->material->reflectance = propiter->value.ToFloat();
						}
						else if(childiter4->name == String("transmittance"))
						{
							// Set transmittance properties
							LIST_FOREACH(childiter4->properties, propiter)
								if(propiter->name == String("t"))
									currentsurface->material->transmittance = propiter->value.ToFloat();
						}
						else if(childiter4->name == String("refraction"))
						{
							// Set refraction properties
							LIST_FOREACH(childiter4->properties, propiter)
								if(propiter->name == String("iof"))
									currentsurface->material->refraction = propiter->value.ToFloat();
						}
					}
				}
			}
		}
		else if(childiter->name == String("animation"))
		{
			// Add animation
			_scenedef->animation = new SceneDefinition::Animation();
			// Set animation elements
			LIST_FOREACH(childiter->childelements, childiter2)
			{
				if(childiter2->name == String("interval"))
				{
					// Set position properties
					LIST_FOREACH(childiter2->properties, propiter)
						if(propiter->name == String("fps"))
							_scenedef->animation->interval = propiter->value.ToFloat();
				}
				else if(childiter2->name == String("script"))
				{
					// Add script
					_scenedef->animation->scripts.push_back(SceneDefinition::AnimationScript());
					SceneDefinition::AnimationScript* currentscript = &_scenedef->animation->scripts.back();
					currentscript->script = childiter2->script;
					// Set texture properties
					LIST_FOREACH(childiter2->properties, propiter)
					{
						if(propiter->name == String("type"))
							currentscript->type = propiter->value;
						else if(propiter->name == String("starttime"))
							currentscript->starttime = propiter->value.ToFloat();
						else if(propiter->name == String("duration"))
							currentscript->duration = propiter->value.ToFloat();
						else if(propiter->name == String("endtime"))
							currentscript->endtime = propiter->value.ToFloat();
					}
				}
			}
		}
	}

	delete rootelement;

	Result::PrintLogMessage("(DONE)");
	return R_OK;
}