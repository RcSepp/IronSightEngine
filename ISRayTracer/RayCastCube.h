enum RayCastCubeResult
{
	RCCR_WEST, RCCR_EAST, RCCR_BOTTOM, RCCR_TOP, RCCR_SOUTH, RCCR_NORTH, RCCR_INVALID
};

// Trace a ray from v inside a unit cube in direction d to the surface of the cube
RayCastCubeResult RayCastCube(float* v, const float* d, int* b)
{
	v[0] -= (float)b[0];
	v[1] -= (float)b[1];
	v[2] -= (float)b[2];

	float x, y, z;
	if(d[0] > 0.0f)
	{
		y = v[1] + (d[1] * (1.0f - v[0])) / d[0];
		if(y >= 0.0f && y <= 1.0f)
		{
			if(d[2] > 0.0f)
			{
				x = v[0] + (d[0] * (1.0f - v[2])) / d[2];
				if(x >= 0.0f && x <= 1.0f)
				{
					// Top face hit
					y = v[1] + (d[1] * (1.0f - v[2])) / d[2];
					v[0] = (float)b[0] + x; v[1] = (float)b[1] + y; v[2] = (float)b[2] + 1.0f; b[2]++;
					return RCCR_TOP;
				}
			}
			if(d[2] < 0.0f)
			{
				x = v[0] - (d[0] * v[2]) / d[2];
				if(x >= 0.0f && x <= 1.0f)
				{
					// Bottom face hit
					y = v[1] - (d[1] * v[2]) / d[2];
					v[0] = (float)b[0] + x; v[1] = (float)b[1] + y; v[2] = (float)b[2] + 0.0f; b[2]--;
					return RCCR_BOTTOM;
				}
			}
			// East face hit
			z = v[2] + (d[2] * (1.0f - v[0])) / d[0];
			v[0] = (float)b[0] + 1.0f; v[1] = (float)b[1] + y; v[2] = (float)b[2] + z; b[0]++;
			return RCCR_EAST;
		}
	}

	if(d[0] < 0.0f)
	{
		y = v[1] - (d[1] * v[0]) / d[0];
		if(y >= 0.0f && y <= 1.0f)
		{
			if(d[2] > 0.0f)
			{
				x = v[0] + (d[0] * (1.0f - v[2])) / d[2];
				if(x >= 0.0f && x <= 1.0f)
				{
					// Top face hit
					y = v[1] + (d[1] * (1.0f - v[2])) / d[2];
					v[0] = (float)b[0] + x; v[1] = (float)b[1] + y; v[2] = (float)b[2] + 1.0f; b[2]++;
					return RCCR_TOP;
				}
			}
			if(d[2] < 0.0f)
			{
				x = v[0] - (d[0] * v[2]) / d[2];
				if(x >= 0.0f && x <= 1.0f)
				{
					// Bottom face hit
					y = v[1] - (d[1] * v[2]) / d[2];
					v[0] = (float)b[0] + x; v[1] = (float)b[1] + y; v[2] = (float)b[2] + 0.0f; b[2]--;
					return RCCR_BOTTOM;
				}
			}
			// West face hit
			z = v[2] - (d[2] * v[0]) / d[0];
			v[0] = (float)b[0] + 0.0f; v[1] = (float)b[1] + y; v[2] = (float)b[2] + z; b[0]--;
			return RCCR_WEST;
		}
	}

	if(d[1] > 0.0f)
	{
		x = v[0] + (d[0] * (1.0f - v[1])) / d[1];
		if(x >= 0.0f && x <= 1.0f)
		{
			if(d[2] > 0.0f)
			{
				y = v[1] + (d[1] * (1.0f - v[2])) / d[2];
				if(y >= 0.0f && y <= 1.0f)
				{
					// Top face hit
					x = v[0] + (d[0] * (1.0f - v[2])) / d[2];
					v[0] = (float)b[0] + x; v[1] = (float)b[1] + y; v[2] = (float)b[2] + 1.0f; b[2]++;
					return RCCR_TOP;
				}
			}
			if(d[2] < 0.0f)
			{
				y = v[1] - (d[1] * v[2]) / d[2];
				if(y >= 0.0f && y <= 1.0f)
				{
					// Bottom face hit
					x = v[0] - (d[0] * v[2]) / d[2];
					v[0] = (float)b[0] + x; v[1] = (float)b[1] + y; v[2] = (float)b[2] + 0.0f; b[2]--;
					return RCCR_BOTTOM;
				}
			}
			// North face hit
			z = v[2] + (d[2] * (1.0f - v[1])) / d[1];
			v[0] = (float)b[0] + x; v[1] = (float)b[1] + 1.0f; v[2] = (float)b[2] + z; b[1]++;
			return RCCR_NORTH;
		}
	}

	if(d[1] < 0.0f)
	{
		x = v[0] - (d[0] * v[1]) / d[1];
		if(x >= 0.0f && x <= 1.0f)
		{
			if(d[2] > 0.0f)
			{
				y = v[1] + (d[1] * (1.0f - v[2])) / d[2];
				if(y >= 0.0f && y <= 1.0f)
				{
					// Top face hit
					x = v[0] + (d[0] * (1.0f - v[2])) / d[2];
					v[0] = (float)b[0] + x; v[1] = (float)b[1] + y; v[2] = (float)b[2] + 1.0f; b[2]++;
					return RCCR_TOP;
				}
			}
			if(d[2] < 0.0f)
			{
				y = v[1] - (d[1] * v[2]) / d[2];
				if(y >= 0.0f && y <= 1.0f)
				{
					// Bottom face hit
					x = v[0] - (d[0] * v[2]) / d[2];
					v[0] = (float)b[0] + x; v[1] = (float)b[1] + y; v[2] = (float)b[2] + 0.0f; b[2]--;
					return RCCR_BOTTOM;
				}
			}
			// South face hit
			z = v[2] - (d[2] * v[1]) / d[1];
			v[0] = (float)b[0] + x; v[1] = (float)b[1] + 0.0f; v[2] = (float)b[2] + z; b[1]--;
			return RCCR_SOUTH;
		}
	}

	if(d[2] > 0.0f)
	{
		// Top face hit
		v[0] += (float)b[0]; v[1] += (float)b[1]; v[2] = (float)b[2] + 1.0f; b[2]++;
		return RCCR_TOP;
	}
	else
	{
		// Bottom face hit
		v[0] += (float)b[0]; v[1] += (float)b[1]; v[2] = (float)b[2] + 0.0f; b[2]--;
		return RCCR_BOTTOM;
	}
}