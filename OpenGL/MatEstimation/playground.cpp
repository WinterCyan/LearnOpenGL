//
// Created by winter on 2021/11/18.
//

#include <string>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

int main()
{
    std::string path = "/home/winter/MEDataset/eval";
    for (const auto & entry : fs::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        std::cout << entry.path().c_str() << std::endl;
    }
}
