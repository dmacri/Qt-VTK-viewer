#ifndef ELEMENT_H
#define ELEMENT_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unordered_map>

using namespace std;

class rgb{
	int red, green, blue, alpha;

	public:
    rgb(int red, int green, int blue, int alpha=0):red(red),green(green),blue(blue),alpha(alpha){

	}
	int getRed(){
		return this->red;
	}
	int getGreen(){
		return this->green;
	}
	int getBlue(){
		return this->blue;
	}
	int getAlpha(){
		return this->alpha;
	}
};

class Element{
	
public:
    Element(){}


	public:
		
		virtual void composeElement(char* str) = 0;
		
		virtual char* stringEncoding() = 0;
		
		virtual rgb* outputValue() = 0;

		virtual void startStep(int step) = 0;
	
};

#endif
