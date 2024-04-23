#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <algorithm>

extern char **environ;

char readInput()
{
    std::cout << "Введите символ (+, *, &, q): ";
    char input;
    std::cin >> input;
    return input;
}

//print and sort env vars
void printSortedEnv() {
    std::vector<std::string> env_vars;
    for (char **env = environ; *env != nullptr; ++env) {
        env_vars.push_back(*env);
    }

    std::setlocale(LC_COLLATE, "C");
    std::sort(env_vars.begin(), env_vars.end(), [](const std::string& a, const std::string& b) {
        return std::strcoll(a.c_str(), b.c_str()) < 0;
    });

    std::cout << "Sorted environment variables:" << std::endl;
    for (const std::string& env_var : env_vars) {
        std::cout << env_var << std::endl;
    }
}

char* getPathFromEnv()
{
    char *child_path = getenv("CHILD_PATH");
    if (child_path == nullptr)
    {
        std::cerr << "Ошибка: переменная среды CHILD_PATH не установлена" << std::endl;
        exit(EXIT_FAILURE);
    }
    return child_path;
}

char* getPathFromArg(char *envp[])
{
    char *child_path;
    for (char **env = envp; *env != nullptr; ++env)
    {
        if (strncmp(*env, "CHILD_PATH=", 11) == 0)
        {
            child_path = *env + 11;
            return child_path;
        }
    }
    std::cerr << "Ошибка: переменная среды CHILD_PATH не найдена в аргументах командной строки" << std::endl;
    exit(EXIT_FAILURE);
}

char* getPathFromExtEnv()
{
    char *child_path;
    for (char **env = environ; *env != nullptr; ++env)
    {
        if (strncmp(*env, "CHILD_PATH=", 11) == 0)
        {
            child_path = *env + 11;
            return child_path;
        }
    }
    std::cerr << "Ошибка: переменная среды CHILD_PATH не найдена во внешнем окружении" << std::endl;
    exit(EXIT_FAILURE);
}
// start child process
void spawnChild(char *child_path, char *child_argv[], char **envp)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "Ошибка: fork() не удался" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // child process

        if (child_path == nullptr)
        {
            std::cerr << "Ошибка: путь к программе child не найден" << std::endl;
            exit(EXIT_FAILURE);
        }

        execve(child_path, child_argv, envp);
        std::cerr << "Ошибка: execve() не удался" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[], char *envp[])
{
    char* child_path;
    char input;

    printSortedEnv();

    while ((input = readInput()) != 'q')
    {
        switch (input)
        {
        case '+':
        {
            child_path = getPathFromEnv();
            spawnChild(child_path, argv, envp);
            break;
        }
        case '*':
        {
            child_path = getPathFromArg(envp);
            spawnChild(child_path, argv, envp);
            break;
        }
        case '&':
        {
            child_path = getPathFromExtEnv();
            spawnChild(child_path, argv, envp);
            break;
        }
        default:
            std::cout << "Недопустимый символ. Повторите ввод." << std::endl;
            break;
        }
    }
    
    return EXIT_SUCCESS;
}
