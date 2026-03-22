#include <fstream>
#include <iostream>
#include <string>

#include "messages.h"


int main() {
    //relative path one layer up specified with two dots
    std::string filepath = "../data/08302019.NASDAQ_ITCH50";

    //establish connection to the binary file - we can then use method read, this will take current byte/bytes load it into RAM - We cannot load whole 4GB file there.
    std::ifstream file(filepath,std::ios::binary);

    //check if connection was established correctly
    if (!file.is_open()) {
        std::cerr << "[ERROR] The provided path is incorrect: " << filepath << std::endl;
        return 1;
    }
    std::cout << "[SYSTEM] File opened successfully" << std::endl;
    char message_type;
    while (file.read(&message_type,1)) {

        switch (message_type) {
            case 'S': {
                // in c++ we can step back seek -1 from our current position
                file.seekg(-1, std::ios::cur);
                SystemEventMessage message{};
                // "Take the address of message, and pretend it's just a pointer to raw chars"
                file.read(reinterpret_cast<char*>(&message), sizeof(message));
                break;
            }



        }


    }

}