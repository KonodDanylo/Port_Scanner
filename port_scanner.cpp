#include <iostream>
#include <fstream>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <regex> // Added regex header

#pragma comment(lib, "ws2_32.lib")

// Helper function to remove ANSI escape sequences from the output string
std::string removeEscapeSequences(const std::string &input)
{
    static const std::regex escapeSeqRegex("\x1b\\[[0-9;-]*[A-Za-z]");
    return std::regex_replace(input, escapeSeqRegex, "");
}

std::vector<int> getFamousPorts()
{
    return {1, 3, 7, 9, 13, 17, 19, 20, 21, 22, 23, 25, 37, 42, 49, 53, 70, 79, 80, 81,
            82, 88, 102, 110, 113, 119, 123, 135, 137, 139, 143, 161, 179, 194, 389, 443,
            444, 445, 465, 514, 515, 543, 544, 548, 554, 587, 631, 646, 873, 990, 993, 995,
            1080, 1099, 1433, 1434, 1521, 1701, 1723, 1812, 1813, 2049, 2082, 2083, 2100,
            2222, 2375, 2376, 2483, 2484, 3306, 3389, 3689, 3690, 3899, 4333, 4353, 4567,
            4569, 4643, 5000, 5001, 5060, 5222, 5269, 5353, 5432, 5500, 5631, 5666, 5672,
            5800, 5900, 5901, 5984, 5985, 6379, 6436, 6443, 6464, 6666, 6881, 6882, 6883,
            6884, 6885, 6886, 6887, 6888, 6889, 6969, 7000, 7001, 7002, 7070, 8000, 8008,
            8009, 8010, 8080, 8081, 8443, 8888, 9000, 9001, 9090, 9091, 9200, 9418, 9999,
            10000, 10001, 10002, 10003, 10004, 10009, 10010, 10011, 10012, 10013, 10014,
            10015, 10016, 10017, 10018, 10019, 10020, 10021, 10022, 10023, 10024, 10025,
            10082, 11110, 27374};
}

std::mutex printMutex;

void scanPort(const std::string &host, int port, std::ostream &output, std::atomic<int> &completedCount)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::lock_guard<std::mutex> lock(printMutex);
        output << "Failed to initialize Winsock." << '\n';
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        std::lock_guard<std::mutex> lock(printMutex);
        output << "Failed to create socket." << '\n';
        WSACleanup();
        return;
    }

    struct sockaddr_in serverAddr
    {
    };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &(serverAddr.sin_addr)) <= 0)
    {
        std::lock_guard<std::mutex> lock(printMutex);
        output << "Invalid address or address not supported." << '\n';
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::lock_guard<std::mutex> lock(printMutex);
        output << std::setw(5) << std::left << port << " [CLOSED]" << '\n';
    }
    else
    {
        std::lock_guard<std::mutex> lock(printMutex);
        output << std::setw(5) << std::left << port << " [OPEN]" << '\n';
        closesocket(sockfd);
    }

    WSACleanup();

    completedCount++;
}

int main()
{
    std::string host;
    std::vector<int> ports = getFamousPorts();

    std::cout << "Enter the target host: ";
    std::cin >> host;

    std::string logOption;
    std::cout << "Do you want to save the output to a file? (yes/no): ";
    std::cin >> logOption;

    std::ofstream outputFile;
    if (logOption == "yes")
    {
        std::string filename;
        std::cout << "Enter the filename to save the output: ";
        std::cin >> filename;
        outputFile.open(filename);
        if (!outputFile)
        {
            std::cout << "Failed to open the file for writing." << '\n';
            return 1;
        }
    }

    std::ostream &output = (logOption == "yes") ? outputFile : std::cout;

    std::atomic<int> completedCount(0);
    std::atomic<int> totalPorts(ports.size());

    output << "Port    Status" << '\n';
    output << "-------------------" << '\n';

    std::vector<std::thread> threads;
    for (int port : ports)
    {
        threads.emplace_back(std::thread(scanPort, host, port, std::ref(output), std::ref(completedCount)));
    }

    while (completedCount < totalPorts)
    {
        {
            std::lock_guard<std::mutex> lock(printMutex);
            output << "Progress: " << completedCount << "/" << totalPorts << " ports scanned." << '\n';
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    if (logOption == "yes")
    {
        outputFile << "All ports scanned." << '\n';
        outputFile.close();
    }
    else
    {
        std::cout << "All ports scanned." << '\n';
    }

    return 0;
}
