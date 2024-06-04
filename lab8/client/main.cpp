#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

std::string currentDir = "";

int main() {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(8080);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  connect(clientSocket, (struct sockaddr *)&serverAddress,
          sizeof(serverAddress));

  while (true) {
    std::cout << currentDir << "> ";
    std::cout.flush();
    std::string command;
    std::getline(std::cin, command);
    send(clientSocket, command.c_str(), command.size(), 0);

    char buffer[1024] = {0};
    if (recv(clientSocket, buffer, sizeof(buffer), 0) == -1) {
      std::cout << "BYE" << std::endl;
      break;
    }

    std::string recieved(buffer, strlen(buffer));
    if (recieved.substr(0, 9) == "set_path ") {
      currentDir = recieved.substr(9);
      if (currentDir == ".") {
        currentDir = "";
      }
    } else {
      std::cout << recieved << std::endl;
    }
    if (recieved == "BYE")
      break;
  }

  close(clientSocket);

  return 0;
}