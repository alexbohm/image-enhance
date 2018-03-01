#include <string>
#include <CImg.h>

#include <iostream>
using namespace cimg_library;
class OutImage
{
public:
	int width, height, channels;
	int layers;
	unsigned char **** v;
	OutImage(){
		v = new unsigned char ***[0];
	};
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
	void write(int x, int y, int channel, int layer, unsigned char color){
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
		v[y][x][layer][channel] = color;
	};
	void save(std::string filename){
		CImg<unsigned char> out(width, height, 1, channels, 0); //(width, height, depth, channels, default color)
		//std::cout << "Created out Image" << std::endl;
		unsigned long * pxl = new unsigned long[channels];
		//std::cout << "Created pxl" << std::endl;
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				//reset pxl to zero
				//memset (pxl, 0, channels);
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
					out(j, i, c) = (unsigned char)(pxl[c] / layers);
				}
			}
		}
		//std::cout << "Writing File" << std::endl;
		out.save(filename.c_str());
		std::cout << "Done Writing File " << filename << std::endl;
		delete[] pxl;
	}
	
};