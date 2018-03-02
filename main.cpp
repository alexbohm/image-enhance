#include "enhance.h"
#include <iostream>
int main(int argc, char const *argv[])
{
	ImageEnhance img("einstein.jpg");
	img.enhance();
	img.save("mod.bmp");
	return 0;
}