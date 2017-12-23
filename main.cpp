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
//function that enhances image and puts it into a higher res image
//TODO: once it is working make it threadsafe
void enhanceImage(short differenceThreshold = 1){
	unsigned char data[8];
	for(int channel = 0; channel < orig.spectrum(); channel++){
		std::cout << "Channel " << channel << std::endl;
		for(int h = 0; h < orig.height() - 2; h += 3){
			for(int w = 0; w < orig.width() - 2; w += 3){
				//temp threshold
				int th;
				//maximum thresholds
				int th1 = 0;
				int th2 = -1;
				//maximum threshold positions
				int th1x = 0, th1y = 0;
				int th2x = 0, th2y = 0;
				//find max threshold in outer ring of pixels
				//top right two
				for(int i = w + 1; i <  w + 3; i++){
					th = std::abs((int)orig(i, h, channel) - (int)orig(i - 1, h, channel));
					if(th > th1){
						th1 = th;
						th1x = i;
						th1y = h;
					}else if(th > th2 && th < th1){
						th2 = th;
						th2x = i;
						th2y = h;
					}
				}
				//bottom right two
				for(int i = h + 1; i < h + 3; i++){
					th = std::abs((int)orig(w + 2, i, channel) - (int)orig(w + 2, i - 1, channel));
					if(th > th1){
						th1 = th;
						th1x = w + 2;
						th1y = i;
					}else if(th > th2 && th < th1){
						th2 = th;
						th2x = w + w;
						th2y = i;
					}
				}
				//bottom left two
				for(int i = w; i < w + 2; i++){
					th = std::abs((int)orig(i, h + 2, channel) - (int)orig(i + 1, h + 2, channel));
					if(th > th1){
						th1 = th;
						th1x = i;
						th1y = h + 2;
					}else if(th > th2 && th < th1){
						th2 = th;
						th2x = i;
						th2y = h + 2;
					}
				}
				//top left two
				for(int i = h; i < h + 2; i++){
					th = std::abs((int)orig(w, i, channel) - (int)orig(w, i + 1, channel));
					if(th > th1){
						th1 = th;
						th1x = w;
						th1y = i;
					}else if(th > th2 && th < th1){
						th2 = th;
						th2x = w;
						th2y = i;
					}
				}
				//done finding max thresholds
				/*
				std::cout << "th1 " << th1 << std::endl;
				std::cout << "th2 " << th2 << std::endl;
				//*/
				//if the square is mostly solid color
				//just fill in the higher res image with solid color
				if(th1 < differenceThreshold || th2 < differenceThreshold){
					//std::cout << "upsample" << std::endl;
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
				/*
				std::cout << "applying filter" << std::endl;
				std::cout << "th1 " << th1 << std::endl;
				std::cout << "th2 " << th2 << std::endl;
				//*/
				
				double slope = ((double)th1y - th2y) / ((double)th1x - th2x);
				for(int i = 0; i < 9; i++){
					//don't draw the pixel if the line goes past the borders
					if(i * slope > 8 || i * slope < 0){
						break;
					}
					//draw a pixel for the dividing line
					mod(w * 3 + i, h * 3 + i * slope, channel) = 255;
				}
				//TODO: using slope fill in two colors on either side of the line
				//using true values to properly fill it in
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
	std::cout << "Saving Image" << std::endl;
	//TODO: consider using other format to save image (lossless?)
	mod.save("mod.jpg");
	return 0;
};