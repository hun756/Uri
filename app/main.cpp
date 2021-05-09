#include "uri.hpp"
#include <iostream>

int main(int arg, const char* argv[])
{
    Uri::Uri uri;

    if (uri.parseFromString("https://www.example.com/foo/bar"))
    {
        std::clog << "Loggin was runned ...!" << std::endl;

        std::cerr << "Hello World..!" << std::endl;
    }
}