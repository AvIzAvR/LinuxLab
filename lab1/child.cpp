#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>

extern char **environ;

void print_environment() {
    std::cout << "Environment:" << std::endl;
    for (char **env = environ; *env != nullptr; env++) {
        std::cout << *env << std::endl;
    }
}

int main(int argc, char *argv[]) {
    std::cout << "Child process" << std::endl;
    std::cout << "PID: " << getpid() << std::endl;
    std::cout << "PPID: " << getppid() << std::endl;

    if (argc != 3) { // Изменение количества аргументов для добавления имени процесса
        std::cerr << "Usage: " << argv[0] << " <env_file> <child_name>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Отображение кастомного имени процесса
    std::cout << "Child name: " << argv[2] << std::endl;

    std::ifstream env_file(argv[1]);
    if (!env_file.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string env_name;
    while (env_file >> env_name) {
        char *value = getenv(env_name.c_str());
        if (value != nullptr) {
            std::cout << env_name << "=" << value << std::endl;
        }
    }

    env_file.close();
    return 0;
}
