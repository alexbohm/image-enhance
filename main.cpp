#include <CImg.h>
#include <iostream>
using namespace cimg_library;
//original image
CImg<unsigned char> orig(50, 50, 1, 3, 0); //(width, height, depth, channels, default color)
CImg<unsigned char> mod(50, 50, 1, 3, 0); //(width, height, depth, channels, default color)

void upSample(int factor = 2){
	char c;
	for(int channel = 0; channel < orig.spectrum(); channel++){
		for(int h = 0; h < orig.height(); h++){
			for(int w = 0; w < orig.width(); w++){
				//std::cout << (int)orig(j, i) << std::endl;
				c = orig(w, h, channel);
				for(int i = 0; i < factor; i++){
					for(int j = 0; j < factor; j++){
						mod(w * factor + j, h * factor + i, channel) = c;
					}
				}
			}
		}
	}
};
// ^ pyramid of death
int main(int argc, char const *argv[])
{
	//factor to increase image resolution by
	int factor = 5;
	//check if user passed a filename
	if(argc > 1){
		orig = CImg<unsigned char>(argv[1]);
	}
	//setup image to write enhanced original to
	mod = CImg<unsigned char>(orig.width() * factor, orig.height() * factor, orig.depth(), orig.spectrum(), 0);
	//up sample the image by factor
	upSample(factor);
	//save image
	mod.save("mod.png");
	return 0;
}