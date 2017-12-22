#include <CImg.h>
#include <iostream>
#include <math.h>
//for linux compile with "g++ main.cpp -o main.out -lpthread -lX11"
using namespace cimg_library;
//original image
CImg<unsigned char> orig(50, 50, 1, 4, 0);//(width, height, depth, channels, default color)
CImg<unsigned char> mod(50, 50, 1, 4, 0); //(width, height, depth, channels, default color)

void upSample(int factor = 3){
	unsigned char c;
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
//function that enhances image and puts it into a higher res image
//TODO: once it is working make it threadsafe
void enhanceImage(short differenceThreshold = 1){
	unsigned char data[8];
	for(int channel = 0; channel < orig.spectrum(); channel++){
		std::cout << "Channel " << channel << std::endl;
		for(int h = 0; h < orig.height() - 2; h += 3){
			for(int w = 0; w < orig.width() - 2; w += 3){
				/*
					load channel data into array
				*/
				// 0 1 2
				// 7   3
				// 6 5 4
				//top 3 pxls
				data[0] = orig(w, h, channel);
				data[1] = orig(w + 1, h, channel);
				data[2] = orig(w + 2, h, channel);
				//right center
				data[3] = orig(w + 2, h + 1, channel);
				//bottom 3 pxls
				data[4] = orig(w + 2, h + 2, channel);
				data[5] = orig(w + 1, h + 2, channel);
				data[6] = orig(w, h + 2, channel);
				//left center
				data[7] = orig(w, h + 1, channel);
				/*
					enhance channel
				*/
				int th = 0;
				int th1 = 0;
				int th1_pos = 0;
				int th2 = -1;
				int th2_pos = 1;
				th = std::abs((int)data[7] - (int)data[0]);
				if(th > th1){
					th1 = th;
					th1_pos = 0;
				}
				for(int i = 1; i < 8; i++){
					th = std::abs((int)data[i] - (int)data[i-1]);
					if(th > th1){
						th1 = th;
						th1_pos = i;
					}
					if(th > th2 && th < th1){
						th2 = th;
						th2_pos = i;
					}
				}
				//if the square is mostly solid color
				//just fill in the higher res image
				if(th1 < differenceThreshold || th2 < differenceThreshold){
					std::cout << "upsample" << std::endl;
					unsigned char c;
					for(int y = h; y < h + 3; y++){
						for(int x = w; x < w + 3; x++){
							c = orig(x, y, channel);
							for(int uh = 0; uh < 3; uh++){
								for(int uw = 0; uw < 3; uw++){
									mod( x*3 + uw, y*3 + uh, channel) = c;
								}
							}
						}
					}
					continue;
				}
				//apply new pixels to high res img
				//*
				std::cout << "applying filter" << std::endl;
				//std::cout << "th1 " << th1 << " @ " << th1_pos << std::endl;
				//std::cout << "th2 " << th2 << " @ " << th2_pos << std::endl;
				//*/

				//TODO: use the two positions to make a line and connect them
			}
		}
	}
};


int main(int argc, char const *argv[])
{
	//check if user passed a filename
	if(argc > 1){
		orig = CImg<unsigned char>(argv[1]);
	}
	//setup image to write enhanced original to (for now 3x higher res)
	mod = CImg<unsigned char>(orig.width() * 3, orig.height() * 3, orig.depth(), orig.spectrum(), 0);
	//pass enhancement function a threshold of 2
	enhanceImage(2);
	//save image
	mod.save("mod.png");
	return 0;
}