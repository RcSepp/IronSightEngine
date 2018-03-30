#ifndef __ISIMAGE_H
#define __ISIMAGE_H

struct Image
{
	unsigned char* data;
	int width, height, bpp;
	bool isdataowner;

	Image() : data(NULL), isdataowner(false) {}
	~Image() {if(data && isdataowner) delete[] data;}
	static Image* New(int width, int height, int bpp)
	{
		Image* newimage = new Image();
		newimage->width = width;
		newimage->height = height;
		newimage->bpp = bpp;
		newimage->data = new unsigned char[newimage->width * newimage->height * newimage->bpp];
		memset(newimage->data, 0, newimage->width * newimage->height * newimage->bpp);
		newimage->isdataowner = true;
		return newimage;
	}
};

Result LoadImage(FilePath filename, Image** img);
Result SaveImage(FilePath filename, Image* img);
Result SaveImage(LPVOID* data, DWORD* datasize, FilePath ext, Image* img);

#endif