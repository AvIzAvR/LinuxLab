#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

int serverSocket;
std::string currentDir;
std::string rootDir;

void command_INFO(int clientSocket) {
  std::ifstream file("info.txt");
  if (!file.good()) {
    send(clientSocket, "Failed to get information about server", 38, 0);
  } else {
    std::string fileData;
    std::string line;
    while (getline(file, line)) {
      fileData += line + "\n";
    }
    fileData = fileData.substr(0, fileData.size() - 1);
    send(clientSocket, fileData.c_str(), fileData.size(), 0);
  }
  file.close();
}

void command_CD(int clientSocket, std::string recieved) {
  std::string newDir = recieved.substr(3);
  if (!std::filesystem::exists(currentDir + newDir)) {
    send(clientSocket, "Invalid Path", 13, 0);
  } else {
    auto const normRoot = std::filesystem::canonical(rootDir);
    auto const normChild = std::filesystem::canonical(currentDir + newDir);

    auto itr = std::search(normChild.begin(), normChild.end(), normRoot.begin(),
                           normRoot.end());
    if (itr == normChild.begin()) {
      currentDir = normChild.string() + "/";
      std::string toSend =
          "set_path " + std::filesystem::relative(normChild, normRoot).string();
      send(clientSocket, toSend.c_str(), toSend.size(), 0);
    } else {
      send(clientSocket, "Invalid Path", 13, 0);
    }
  }
}

void command_LIST(int clientSocket) {
  std::string allData = "";
  for (const auto &entry : std::filesystem::directory_iterator(currentDir)) {
    allData += entry.path().filename().string();
    if (std::filesystem::is_symlink(entry)) {
      auto symlinkPath = std::filesystem::read_symlink(entry);
      if (symlinkPath.is_relative()) {
        symlinkPath = currentDir + symlinkPath.string();
      }
      if (std::filesystem::is_symlink(symlinkPath)) {
        allData += " -->> " + symlinkPath.string();
      } else {
        allData += " --> " + symlinkPath.string();
      }
    } else if (std::filesystem::is_directory(entry)) {
      allData += "/";
    }
    allData += "\n";
  }
  allData = allData.substr(0, allData.size() - 1);
  send(clientSocket, allData.c_str(), allData.size(), 0);
}

void client_work(int clientSocket) {
  while (true) {
    char buffer[1024] = {0};
    if (recv(clientSocket, buffer, sizeof(buffer), 0) == -1)
      break;
    std::string recieved(buffer, strlen(buffer));
    if (recieved.substr(0, 5) == "ECHO ") {
      send(clientSocket, recieved.substr(5).c_str(), recieved.size() - 5, 0);
    } else if (recieved.substr(0, 4) == "QUIT") {
      send(clientSocket, "BYE", 3, 0);
      // close(serverSocket);
      // return EXIT_SUCCESS;
      break;
    } else if (recieved.substr(0, 4) == "INFO") {
      command_INFO(clientSocket);
    } else if (recieved.substr(0, 3) == "CD ") {
      command_CD(clientSocket, recieved);
    } else if (recieved.substr(0, 4) == "LIST") {
      command_LIST(clientSocket);
    } else {
      send(clientSocket, "Invalid Command", 15, 0);
    }
  }

  std::cout << "Client Disconnected" << std::endl;
  close(clientSocket);
}

int main(int argC, char **argV) {
  if (argC < 2) {
    std::cout << "root path must be specified" << std::endl;
    return EXIT_FAILURE;
  } else {
    rootDir = std::string(argV[1], strlen(argV[1]));
  }
  currentDir = rootDir;
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(8080);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  if (bind(serverSocket, (struct sockaddr *)&serverAddress,
           sizeof(serverAddress)) == -1) {
    std::cout << "Error binding server" << std::endl;
    return EXIT_FAILURE;
  }
  listen(serverSocket, 5);
  std::cout << "Server started" << std::endl;
  while (true) {
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    std::cout << "Client Connected" << std::endl;
    std::thread thr([clientSocket] { client_work(clientSocket); });
    thr.detach();
  }

  close(serverSocket);
  return EXIT_SUCCESS;
}