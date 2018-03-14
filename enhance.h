#include <string>
#include <thread>
#include "image.h"

#include <iostream>

int abs(int num){
	return (num < 0) ? -num:num;
};
class ImageEnhance
{
public:
	InImage* in;
	OutImage* out;
	ImageEnhance(std::string filename){
		in = new InImage(filename);
		out = NULL;
	};
	~ImageEnhance(){
		delete in;
		delete out;
	};
	void save(std::string filename){
		out->save(filename);
	};
	//hope to do some threading with this
	//sSize is the factor to increase the resolution by
	//1x1 pixel turns into a sSizexsSize square
	void enhance(int s = 2, int threshold = 2){
		
		//set the sSize
		sSize = s;
		//reset the current layer to zero
		cLayer = 0;
		//set the threshold
		uThres = threshold;
		//init the OutImage
		std::cout << "Creating Output Image" << std::endl;
		out = new OutImage(in->width * sSize, in->height * sSize, in->channels, 4);
		//start the enhancement
		for(int xoff = 0; xoff < 3; xoff+=2){
			for(int yoff = 0; yoff < 3; yoff+=2){
				std::cout << "Processing Layer " << cLayer << std::endl;
				std::thread t[in->channels];
				for(int c = 0; c < in->channels; c++){
					t[c] = std::thread(&ImageEnhance::processChannel, this, xoff, yoff, c);
				}
				for(int c = 0; c < in->channels; c++){
					t[c].join();
				}
				cLayer++;
			}
		}
		std::cout << "Finished Processing" << std::endl;
	};
private:
	//the current layer on the output image to write data to
	int cLayer;
	//scale factor
	int sSize;
	//upscale square if the max threshold is less then uThres
	int uThres;
	void processChannel(int x, int y, int c){
		for (int i = x; i < in->width - 2; i+=3){
			for (int j = y; j < in->height - 2; j+=3){
				enhanceSquare(i, j, c);
			}
		}
	};
	//enhance a 3x3 square of pixels at (x, y) on channel c
	void enhanceSquare(int x, int y, int c){
		int th1 = 0;
		int th1x = 0, th1y = 0;
		int th2 = -1;
		int th2x = 0, th2y = 0;
		//temp var to hold current threshold
		int th;
		for(int i = 1; i < 3; i++){
			th = abs(in->at(x + i, y, c) - in->at(x + i - 1, y, c));
			if(th > th1){
				//shift highest value into second value
				th2 = th1;
				th2x = th1x;
				th2y = th1y;
				//set max thres to calculated thres
				th1 = th;
				th1x = i;
				th1y = 0;
			} else if(th >= th2){
				th2 = th;
				th2x = i;
				th2y = 0;
			}
		}
		for(int i = 1; i < 3; i++){
			th = abs(in->at(x + 2, y + i, c) - in->at(x + 2, y + i - 1, c));
			if(th > th1){
				//shift highest value into second value
				th2 = th1;
				th2x = th1x;
				th2y = th1y;
				//set max thres to calculated thres
				th1 = th;
				th1x = 3;
				th1y = i;
			} else if(th >= th2){
				th2 = th;
				th2x = 3;
				th2y = i;
			}
		}
		for(int i = 1; i >= 0; i--){
			th = abs(in->at(x + i, y + 2, c) - in->at(x + i + 1, y + 2, c));
			if(th > th1){
				//shift highest value into second value
				th2 = th1;
				th2x = th1x;
				th2y = th1y;
				//set max thres to calculated thres
				th1 = th;
				th1x = i + 1;
				th1y = 3;
			} else if(th >= th2){
				th2 = th;
				th2x = i + 1;
				th2y = 3;
			}
		}
		for(int i = 1; i >= 0; i--){
			th = abs(in->at(x, y + i, c) - in->at(x, y + i + 1, c));
			if(th > th1){
				//shift highest value into second value
				th2 = th1;
				th2x = th1x;
				th2y = th1y;
				//set max thres to calculated thres
				th1 = th;
				th1x = 0;
				th1y = i + 1;
			} else if(th >= th2){
				th2 = th;
				th2x = 0;
				th2y = i + 1;
			}
		}
		//std::cout << "Threshold 1 " << th1 << " at (" << th1x << "," << th1y << ")" << std::endl;
		//std::cout << "Threshold 2 " << th2 << " at (" << th2x << "," << th2y << ")" << std::endl;
		//test if we should upscale
		if(th1 < uThres || th2 < uThres){
			//upsample pixel
			upSample(x, y, c);
			return;
		}
		//form line from thresholds
		//check for vertical line
		if(th1x == th2x){
			//std::cout << "Vert Line" << std::endl;
			//use the vertical line to draw new cluster
			int color1 = 0, color1count = 0;
			int color2 = 0, color2count = 0;
			for(int i = 0; i < 3; i++){
				if(i >= th1x){
					for(int j = 0; j < 3; j++){
						color2 += in->at(x + i, y + j, c);
						color2count++;
					}
				} 
				if(i <= th1x){
					for(int j = 0; j < 3; j++){
						color1 += in->at(x + i, y + j, c);
						color1count++;
					}
				}
			}
			if(color1count != 0){
				color1 /= color1count;
			}
			if(color2count != 0){
				color2 /= color2count;
			}
			for(int i = 0; i < 3 * sSize; i++){
				if(i > th1x * sSize){
					for(int j = 0; j < 3 * sSize; j++){
						out->at(x * sSize + i, y * sSize + j, c, cLayer) = color2;
					}
				} else {
					for(int j = 0; j < 3 * sSize; j++){
						out->at(x * sSize + i, y * sSize + j, c, cLayer) = color1;
					}
				}
			}
			return;
		}
		//use point slope form of equation to draw line on output
		//y-y1 = m(x-x1)
		float m = (th2y - th1y) / (th2x - th1x);

		int color1 = 0, color1count = 0; //TODO: find why both colors don't always get set
		int color2 = 0, color2count = 0; // seems to be when slope is negative color2 doesn't get set
		float lineY;
		for(int i = 0; i < 3 * sSize; i++){
			lineY = m * (float)(i - th1x * sSize) + th1y * sSize;
			for(int j = 0; j < 3 * sSize; j++){
				if((float)j >= lineY){
					color2 += in->at(x + i / sSize, y + j / sSize, c);
					color2count++;
				} 
				if((float)j <= lineY){
					color1 += in->at(x + i / sSize, y + j / sSize, c);
					color1count++;
				}
			}
		}
		if(color1count != 0){
			color1 /= color1count;
		}// else {
		//	std::cout << "1>" + std::to_string(m) + " (" + std::to_string(th1x) + "," + std::to_string(th1y) + ") (" + std::to_string(th2x) + "," + std::to_string(th2y) + ")\n";
		//}
		if(color2count != 0){
			color2 /= color2count;
		}// else {
		//	std::cout << "2>" + std::to_string(m) + " (" + std::to_string(th1x) + "," + std::to_string(th1y) + ") (" + std::to_string(th2x) + "," + std::to_string(th2y) + ")\n";
		//}

		for(int i = 0; i < 3 * sSize; i++){
			lineY = m * (float)(i - th1x) + th1y * sSize;
			for(int j = 0; j < 3 * sSize; j++){
				if((float)j >= lineY){
					out->at(x * sSize + i, y * sSize + j, c, cLayer) = color2;
				} else {
					out->at(x * sSize + i, y * sSize + j, c, cLayer) = color1;
				}
			}
		}
		
	};
	void upSample(int x, int y, int channel){
		//upsample a group of 3x3 pixels
		unsigned char value;
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++){
				value = in->at(x + i, y + j, channel);
				for(int k = 0; k < sSize; k++){
					for(int l = 0; l < sSize; l++){
						out->at( (x + i) * sSize + k, (y + j) * sSize + l, channel, cLayer) = value;
					}
				}
			}
		}
	};
	
};