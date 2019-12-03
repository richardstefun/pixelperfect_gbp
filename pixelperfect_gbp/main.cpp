//
//  main.cpp
//  pixelperfect_gbp
//
//  Created by Richard Stefun on 7/4/19.
//  Copyright © 2019 Richard Stefun. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "lodepng.h"

using namespace std;

const unsigned char palette [4] = {0, 63, 127, 255};

vector<uint8_t> inputData;
vector<unsigned char> outputData;
unsigned char frame [144][160];


void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
    unsigned error = lodepng_encode24_file(filename, image, width, height);
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

void read_decode (string filename) { // reads and strips payload data
    ifstream f;
    f.open (filename);
    string line;
    while (getline(f, line)) {
        
        if(line[0] != '#' && line[0] != '!'){ //striped
            istringstream str(line);
            do {
                unsigned int value;
                string word;
                str >> word;
                if(word == ""){
                    //cout << "newline" << endl;
                }
                else {
                    value = (unsigned int)strtoul(word.c_str(), NULL, 16);
                    inputData.push_back(value);
                }
            }
            while (str);
        }
        
    }
    f.close();
}

void convertTile (uint8_t *tile) {
    unsigned int ypos = 0;
    unsigned int xpos = 0;
    unsigned int mpos = 0;
    
    for (int tiley = 0; tiley < 18; tiley++){
        for (int tilex = 0; tilex < 20; tilex++){
            for (int p = 0; p < 8; p++){
                unsigned int hb, lb;
                int fPos = 0;
                lb = tile[mpos];
                mpos++;
                hb = tile[mpos]; //get high byte low byte from array
                mpos++;
                for(int b = 7; b > -1; b--){
                    unsigned int pixel = 0;
                    pixel = ((lb & (1<<b)) >> b) | (((hb & (1<<b)) >> b) << 1);
                    frame[p+ypos][fPos+xpos] = pixel;
                    fPos++;
                }
            }
            xpos += 8;
        }
        ypos += 8;
        xpos = 0;
    }
}

void renderPNG(string filename){
    int p = 0;
    int f = 0;
    outputData.clear();
    outputData.resize(160*144*3);
    for(unsigned int i = 0; i < outputData.size(); i++){
        outputData[i] = palette[3-(*(*frame + f))];
        if(p < 2){
            p++;
        }
        else{
            p = 0;
                f++;
        }
    }
    encodeOneStep(filename.c_str(), outputData.data(), 160, 144);
}

bool fexists(string filename)
{
    ifstream ifile(filename);
    return (bool)ifile;
}

int main(int argc, const char * argv[]) {
    string input = "";
    string output = "";
    if(argc == 3) {
        input = argv[1];
        output = argv[2];
    }
    else {
        cout << "You haven't defined correct input/output files in argument" << endl;
        cout << "Try this: pixelperfect-gbp <gameboyPhotoData.txt> <photo.png>" << endl;
        return 1;
    }
    if(!fexists(input)){
        cout << "Input file does not exist" << endl;
        return 1;
    }
    cout << "Decoding..." << endl;
    read_decode(input);
    cout << "Converting..." << endl;
    convertTile(inputData.data());
    cout << "Rendering..." << endl;
    renderPNG(output);
    return 0;
}
