/* 
Fast Ray-Box Intersection
by Andrew Woo
from "Graphics Gems", Academic Press, 1990
*/

#define NUMDIM	3
#define RIGHT	0
#define LEFT	1
#define MIDDLE	2

bool IntersectRayAABB(const float* minB, const float* maxB, const float* origin, const float* dir, float* coord)
{
	bool inside = true;
	char quadrant[NUMDIM];
	register int i;
	int whichPlane;
	float maxT[NUMDIM];
	float candidatePlane[NUMDIM];

	// Find candidate planes; this loop can be avoided if rays cast all from the eye(assume perpsective view)
	for(i = 0; i < NUMDIM; i++)
		if(origin[i] < minB[i])
		{
			quadrant[i] = LEFT;
			candidatePlane[i] = minB[i];
			inside = false;
		}
		else if (origin[i] > maxB[i])
		{
			quadrant[i] = RIGHT;
			candidatePlane[i] = maxB[i];
			inside = false;
		}
		else
			quadrant[i] = MIDDLE;

	// Ray origin inside bounding box
	if(inside)
	{
		memcpy(coord, origin, NUMDIM * sizeof(float));
		return true;
	}


	// Calculate T distances to candidate planes
	for (i = 0; i < NUMDIM; i++)
		if (quadrant[i] != MIDDLE && dir[i] != 0.0f)
			maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
		else
			maxT[i] = -1.0f;

	// Get largest of the maxT's for final choice of intersection
	whichPlane = 0;
	for(i = 1; i < NUMDIM; i++)
		if(maxT[whichPlane] < maxT[i])
			whichPlane = i;

	// Check final candidate actually inside box
	if(maxT[whichPlane] < 0.0f)
		return false;
	for(i = 0; i < NUMDIM; i++)
		if(whichPlane != i)
		{
			coord[i] = origin[i] + maxT[whichPlane] *dir[i];
			if(coord[i] < minB[i] || coord[i] > maxB[i])
				return false;
		}
		else
			coord[i] = candidatePlane[i];
	return true; // ray hits box
}	