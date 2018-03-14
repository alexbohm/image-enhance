#include "enhance.h"
#include <iostream>
int main(int argc, char const *argv[])
{	
	ImageEnhance img("landscape.jpg");
	img.enhance(2, 0);
	img.save("mod.bmp");
	return 0;
}