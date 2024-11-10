#include "login.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Please provide password" << std::endl;
        exit(0);
    }

    std::string password = argv[1];
    std::cout << "Provided password: '" << password << "'" << std::endl;
    std::cout << "Hashed password: '" << hash_password(password) << "'" << std::endl;

    return 0;
}
