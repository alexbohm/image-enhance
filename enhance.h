#include <string>
#include "image.h"

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
	void enhance(int sSize = 6){
		//for now just make a copy
		//TODO: rewrite oldAlgo.cpp into this
		out = new OutImage(in->width, in->height, in->channels, 9);
		for (int i = 0; i < in->width; i++){
			for (int j = 0; j < in->height; j++){
				for(int c = 0; c < in->channels; c++){
					out->at(i, j, c, 0) = in->at(i, j, c);
				}
			}
		}
	};
	
};