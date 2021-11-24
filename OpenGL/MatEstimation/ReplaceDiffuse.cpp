//
// Created by winter on 2021/11/24.
//
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"
#include <iostream>

using namespace std;
void replace(int idx) {
    string mat_path = "/home/winter/MEDataset/demo_train/"+to_string(idx)+".png";
    string diffuse_path = "/home/winter/MEDataset/diffuse_train/"+to_string(idx)+"___diffuse.png";
    cout<<"from ["<<diffuse_path<<"] to ["<<mat_path<<"]"<<endl;
    int diffuse_width, mat_width, height, channel;
    auto *mat_data = stbi_load(mat_path.c_str(), &mat_width, &height, &channel, 0);
    auto *diffuse_data = stbi_load(diffuse_path.c_str(), &diffuse_width, &height, &channel, 0);
    cout<<diffuse_width<<" "<<mat_width<<" "<<height<<endl;
    for (int col=0; col<diffuse_width; col++) {
        for (int row=0; row<height; row++) {
            for (int c=0; c<channel; c++){
                mat_data[c + col*channel + row*mat_width*channel] = diffuse_data[c + col*channel + row*diffuse_width*channel];
            }
        }
    }
    stbi_write_png(mat_path.c_str(), mat_width, height, channel, mat_data, channel*mat_width);
    stbi_image_free(mat_data);
    stbi_image_free(diffuse_data);
}

int main() {
    for (int idx=1; idx<=30; idx++) {
        replace(idx);
    }
    return 0;
}
