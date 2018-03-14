#include <string>

#ifdef __linux__
	#include <CImg.h>
	using namespace cimg_library;
#elif __APPLE__
	//apple includes
#endif
//for debug only
#include <iostream>
/*
#ifdef __linux__
#elif __APPLE__
#endif
*/
class OutImage
{
public:
	int width, height, channels;
	int layers;
	unsigned char **** v;
	OutImage(int w, int h, int c, int l){
		width = w;
		height = h;
		channels = c;
		layers = l;
		v = new unsigned char ***[height];
		for(int i = 0; i < height; i++){
			v[i] = new unsigned char **[width];
			for(int j = 0; j < width; j++){
				v[i][j] = new unsigned char*[layers];
				for(int l = 0; l < layers; l++){
					v[i][j][l] = new unsigned char[channels];
					for(int c = 0; c < channels; c++){
						v[i][j][l][c] = 0;
					}
				}
			}
		}
	}
	~OutImage(){
		//std::cout << "Deleting" << std::endl;
		for(int i = 0; i < height; i++){
			//std::cout << "i=" << i << std::endl;
			for(int j = 0; j < width; j++){
				//std::cout << "j=" << j << std::endl;
				for(int l = 0; l < layers; l++){
					//std::cout << "l=" << l << std::endl;
					delete[] v[i][j][l];
				}
				//std::cout << "deleting element " << i << " " << j << std::endl;
				delete[] v[i][j];
			}
			//std::cout << "deleting row " << i << std::endl;
			delete[] v[i];
			// std::cout << "deleting row worked " << i << std::endl;
		}
		delete[] v;
	};
	unsigned char& at(int x, int y, int channel, int layer){
/*		if(x >= width || x < 0){
			std::cout << "ERROR IN WIDTH" << std::endl;
			return;
		}
		if(y >= height || y < 0){
			std::cout << "ERROR IN HEIGHT" << std::endl;
			return;
		}
		if(channel >= channels || channel < 0){
			std::cout << "ERROR IN CHANNEL" << std::endl;
			return;
		}
		if(layer >= layers || layer < 0){
			std::cout << "ERROR IN LAYER" << std::endl;
			return;
		}*/
		return v[y][x][layer][channel];
	};
	void save(std::string filename){
		std::cout << "Writing to " + filename << std::endl;
		#ifdef __linux__
			CImg<unsigned char> out(width, height, 1, channels, 0); //(width, height, depth, channels, default color)
		#elif __APPLE__
		#endif
		unsigned long * pxl = new unsigned long[channels];
		//std::cout << "Created pxl" << std::endl;
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				//reset pxl to zero
				//memset (pxl, 0, channels); //might be faster than forloop
				for(int c = 0; c < channels; c++){
					pxl[c] = 0;
				}
				//add up each layer
				for(int l = 0; l < layers; l++){
					for(int c = 0; c < channels; c++){
/*						if(j >= width || j < 0){
							std::cout << "ERROR IN WIDTH" << std::endl;
							break;
						}
						if(i >= height || i < 0){
							std::cout << "ERROR IN HEIGHT" << std::endl;
							break;
						}
						if(c >= channels || c < 0){
							std::cout << "ERROR IN CHANNEL" << std::endl;
							break;
						}
						if(l >= layers || l < 0){
							std::cout << "ERROR IN LAYER" << std::endl;
							break;
						}*/
						pxl[c] += v[i][j][l][c];
						
					}
				}
				//average and save each channel
				for(int c = 0; c < channels; c++){
				#ifdef __linux__
					out(j, i, c) = (unsigned char)(pxl[c] / layers);
					
				#elif __APPLE__
					//
				#endif
					
				}
			}
		}
		//std::cout << "Writing File" << std::endl;
	#ifdef __linux__
		out.save(filename.c_str());
	#elif __APPLE__
		//save img to disk
	#endif
		std::cout << "Done Writing File " << filename << std::endl;
		delete[] pxl;
	}
	
};
class InImage
{
public:
	int width, height, channels;
	unsigned char *** data;
	InImage(std::string filename){
		std::cout << "Reading " + filename << std::endl;
	#ifdef __linux__
		CImg<unsigned char> img(filename.c_str());
		width = img.width();
		height = img.height();
		channels = img.spectrum();
	#elif __APPLE__
		width = 0;
		height = 0;
		channels = 0;
	#endif
		data = new unsigned char**[height];
		for(int i = 0; i < height; i++){
			data[i] = new unsigned char *[width];
			for(int j = 0; j < width; j++){
				data[i][j] = new unsigned char[channels];
				for(int c = 0; c < channels; c++){
					//read in pixel data
				#ifdef __linux__
					data[i][j][c] = img(j, i, c);
				#elif __APPLE__
					data[i][j][c] = 0;
				#endif
				}
			}
		}
		std::cout << "Read in " + filename << std::endl;
	};
	~InImage(){
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				delete[] data[i][j];
			}
			delete[] data[i];
		}
		delete[] data;
	};
	//do not return reference to keep img read-only
	unsigned char at(int x, int y, int c){
		return data[y][x][c];
	};
};