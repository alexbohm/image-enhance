#include <CImg.h>
#include "image.h"
#include <iostream>
#include <math.h>
//for linux compile with "g++ main.cpp -o main.out -lpthread -lX11"
using namespace cimg_library;
//original image
CImg<unsigned char> orig(50, 50, 1, 4, 0);//(width, height, depth, channels, default color)
OutImage* mod; //(50, 50, 3, 1); //(width, height, channels, layers)
void enhaceSquare(int w, int h, int channel, int layer, int sSize = 4){
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

	//if the square is mostly solid color
	//just fill in the higher res image with solid color
	if(th1 < 2 || th2 < 2){ //TODO: replace 2 with changeable thres
		//std::cout << "upsample" << std::endl;
		//TODO: fill in entire square and find why it doesn't stay inside the squares
		//return;
		unsigned char c;
		for(int y = h; y < h + 3; y++){
			for(int x = w; x < w + 3; x++){
				c = orig(x, y, channel);
				for(int uh = 0; uh < sSize; uh++){
					for(int uw = 0; uw < sSize; uw++){
						mod->write(x * (int)((double)sSize / 3.0) + uw, y * (int)((double)sSize / 3.0) + uh, channel, layer, c);
					}
				}
			}
		}
		return;
	}
	//apply new pixels to high res img
	//TODO: find why this doesn't throw any errors about div by 0
	//find slope between two colors
	if(th1x == th2x){
		//TODO: what if it is only two pxls
		//std::cout << "Equal" << std::endl;
		//color1 and color2 are the two colors on either side of the line
		//color above line
		int color1total = 0;
		int color1count = 0;
		//color below line
		int color2total = 0;
		int color2count = 0;
		for(int i = 0; i < 3; i++){
			if(i <= th1x - w){
				for(int j = 0; j < 3; j++){
					color1total += orig(w + i, h + j, channel);
					color1count++;
				}
			} else if(i > th1x - w){
				for(int j = 0; j < 3; j++){
					color2total += orig(w + i, h + j, channel);
					color2count++;
				}
			}
			
		}
		//std::cout << "Here" << std::endl;
		unsigned char color1 = 0;
		if(color1count != 0){
			color1 = color1total / color1count;
		} else {
			//std::cout << "V1" << std::endl;
		}
		unsigned char color2 = 0;
		if(color2count != 0){
			color2 = color2total / color2count;
		} else {
			//std::cout << "V2" << std::endl;
			color2 = color1;
		}
		//fill in areas with color1 and color2
		for(int i = 0; i < sSize; i++){
			for(int j = 0; j < sSize; j++){
				if(i <= th1x - w){
					mod->write((int)(w * ((double)sSize / 3.0)) + i, (int)(h * ((double)sSize / 3.0)) + j, channel, layer, color1);
				} else if(i > th1x - w){
					mod->write((int)(w * ((double)sSize / 3.0)) + i, (int)(h * ((double)sSize / 3.0)) + j, channel, layer, color2);
				}
			}
		}
	} else {
		double slope = ((double)th1y - th2y) / ((double)th1x - th2x);
		//TODO: fix undefined slope errors
		//color1 and color2 are the two colors on either side of the line
		//color above line
		int color1total = 0;
		int color1count = 0;
		//color below line
		int color2total = 0;
		int color2count = 0;
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++){
				if(j > i * slope){
					color1total += orig(w + i, h + j, channel);
					color1count++;
				} else if(j < i * slope){
					color2total += orig(w + i, h + j, channel);
					color2count++;
				}
			}
		}
		//std::cout << "Here" << std::endl;
		unsigned char color1 = 0;
		if(color1count != 0){
			color1 = color1total / color1count;
		} else {
			//std::cout << "N1" << std::endl;
		}
		unsigned char color2 = 0;
		if(color2count != 0){
			color2 = color2total / color2count;
		} else {
			//std::cout << "N2" << std::endl;
			color2 = color1;
		}
		//std::cout << "Not Here" << std::endl;
		//fill in square
		//color1 is used as the color on the line
		for(int i = 0; i < sSize; i++){
			for(int j = 0; j < sSize; j++){
				if(j >= i * slope){
					mod->write((int)(w * ((double)sSize / 3.0)) + i, (int)(h * ((double)sSize / 3.0)) + j, channel, layer, color1);
				} else if(j < i * slope){
					mod->write((int)(w * ((double)sSize / 3.0)) + i, (int)(h * ((double)sSize / 3.0)) + j, channel, layer, color2);
				}
			}
		}
	}
};
//function that enhances image and puts it into a higher res image
//TODO: once it is working make it threadsafe
//(thresh, size of each square in new image)
void enhanceImage(int sSize = 6){
	int layer = 0;
	for(int xoff = 0; xoff < 3; xoff++){
		for(int yoff = 0; yoff < 3; yoff++){
			std::cout << "Offset X " << xoff << " Y " << yoff << std::endl;
			for(int channel = 0; channel < orig.spectrum(); channel++){
				std::cout << "    Channel " << channel << std::endl;
				for(int h = 0; h < orig.height() - 3; h += 3){
					for(int w = 0; w < orig.width() - 3; w += 3){
						enhaceSquare(w + xoff, h + yoff, channel, layer, sSize);
					}
				}
			}
			layer++;
		}
	}
};

//don't compile
//int main(int argc, char const *argv[])
{
	//check if user passed a filename
	if(argc > 1){
		orig = CImg<unsigned char>(argv[1]);
	} else {
		orig = CImg<unsigned char>("landscape.jpg");
	}

	//setup image to write enhanced into
	//TODO: find why values other than 6 don't work
	int expand = 6; //3x3 square to expandXexpand square 
	mod = new OutImage(orig.width() * (expand / 3) + 6, orig.height() * (expand / 3) + 6, orig.spectrum(), 9);
	std::cout << "Starting Image" << std::endl;
	//pass enhancement function a threshold of 2
	enhanceImage(expand);
	//save image
	std::cout << "Saving Image" << std::endl;
	//TODO: consider using other format to save image (lossless?)
	(*mod).save("mod.bmp");
	return 0;
};