//============================================================================
// Name        : pbuh0005-Assignment_1.cpp
// Author      : Patrick Buhagiar
// Version     : 1.3
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "jbutil.h"

jbutil::image<double> FilterImage(jbutil::image<double> imIn, jbutil::matrix<double> kernel);


int main(int argc, char *argv[]) {
	std::cout << "Filter Image, by Patrick Buhagiar (pbuh0005)." << std::endl; // prints !!!Hello World!!!

	std::string infile = argv[1]; //eg "/space/home/pbuh0005/cuda-workspace/pbuh0005-Assignment_1/src/input_images/deer.pgm";
	std::string outfile = argv[2]; //eg "/space/home/pbuh0005/cuda-workspace/pbuh0005-Assignment_1/src/output_images/deer_out.pgm";

	// load image
	jbutil::image<double> image_in;
	std::ifstream file_in(infile.c_str());
	image_in.load(file_in);

	//Load kernel from file
	//First two elements are kernel size
	const char* filename = argv[3]; // eg "/home/patrickbuhagiar/cuda-workspace/pbuh0005-Assignment_1/src/gaussian_filter_5x5.txt";
	std::ifstream inFile(filename);
	if(!inFile) {
		std::cout << std::endl << "Failed to open Kernel. Wrong path? " << filename;
		return 1;
	}
	double n = 0;

	std::vector<double> kernel_vector;
	std::vector<double> size;

	int a = 0;
	while(inFile >> n) {
		if (a<2){
			size.push_back(n);
		} else {
			kernel_vector.push_back(n);
		}
		a++;
	}
	assert(size[0] > 0 && size[1] >0); //check if kernel size is > 0
	assert(size.size() == 2 && kernel_vector.size() == size[0]*size[1]); //kernel size should match kernel vector length

	std::cout << "Kernel Size: " << size[0] << " " << size[1] << std::endl;
	std::cout << "Kernel Elements: " << kernel_vector.size() << std::endl;

	//set Kernel values as matrix
	jbutil::matrix<double> kernel;
	kernel.resize(size[0],size[1]);
	double * x;
	int k=0;
	std::cout << "Kernel: " << std::endl;
	for (int i=0;i<kernel.get_rows();i++){
		for (int j=0;j<kernel.get_cols();j++){
			x = &kernel.operator ()(i,j); //points to pixel address
			*x = kernel_vector[k];// 1.0/(kernel.get_rows()*kernel.get_cols()); //set pointed location value
			std::cout << *x << "  ";
			k++;
		}
		std::cout << std::endl;
	}

	// start timer
	double t = jbutil::gettime();

	//process
	jbutil::image<double> image_out = FilterImage(image_in,kernel);

	// stop timer
	t = jbutil::gettime() - t;

	// save image
	std::ofstream file_out(outfile.c_str());
	image_out.save(file_out);

	// show time taken
	std::cerr << "Time taken: " << t << "s" << std::endl;
	return 0;
}

jbutil::image<double> FilterImage(jbutil::image<double> imIn, jbutil::matrix<double> kernel){
	//obtain kernel and image sizes and other parameters
	int y_kernel = kernel.get_rows();
	int x_kernel = kernel.get_cols();
	int x_image = imIn.get_cols();
	int y_image = imIn.get_rows();
	int n_kernel = y_kernel*x_kernel;

	//extract first channel as matrix.
	jbutil::matrix<double> imTemp;
	imTemp = imIn.get_channel(0);

	jbutil::matrix<double> temp;
	temp.resize(y_image - (y_kernel-1), x_image - (x_kernel-1));

	double sum=0;
	double k=0;
	double pixel = 0;
	int offset_x = 0;
	int offset_y = 0;
	int r=0 , c = 0;
	for (int row = 0; row < y_image - (y_kernel-1); row++){
		for (int col = 0; col < x_image - (x_kernel-1); col++){
			sum = 0;
			for (int n = 0; n < n_kernel; n++){
				//obtain pixel in kernel sized block from image
				if (x_kernel > 1 && y_kernel > 1){
					offset_y = n/x_kernel;
					offset_x = n%y_kernel;
				} else if (x_kernel == 1){
					offset_y = 0;
					offset_x = n%y_kernel;
				} else if (y_kernel == 1){
					offset_y = n/x_kernel;
					offset_y = 0;
				} else {
					offset_y = 0;
					offset_x = 0;
				}
				r = row + offset_y;
				c = col + offset_x;
				assert(r>=0 && c>=0);
				pixel = imTemp.operator ()(r,c);
				//multiple with respecting kernel value
				k = kernel.operator ()(offset_y, offset_x);
				sum += pixel*k;
			}
			if (sum > 1){
				sum = 1.00;
			}
			if (sum < 0){
				sum = 0.00;
			}
			temp.operator ()(row,col) = sum;
		}
	}

	//export
	jbutil::image<double> imOut(y_image - (y_kernel-1), x_image - (x_kernel-1), 1, 255);
	imOut.set_channel(0, temp);

	return imOut;
}



