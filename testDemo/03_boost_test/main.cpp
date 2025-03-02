#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <chrono>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void run_http_server(net::io_context& ioc, const std::string& address, unsigned short port) {
    tcp::acceptor acceptor(ioc, tcp::endpoint(net::ip::make_address(address), port));
    acceptor.set_option(net::socket_base::reuse_address(true));

    while (true) {
        tcp::socket socket(ioc);
        beast::error_code ec;

        // 设置 accept 的超时时间为 3 秒
        net::steady_timer timer(ioc, std::chrono::seconds(3));
        timer.async_wait([&](beast::error_code ec) {
            if (!ec) {
                std::cout << "Accept timed out. Waiting again...\n";
            }
        });

        // 等待客户端连接，超时后继续等待
        acceptor.async_accept(socket, [&](beast::error_code ec) {
            if (!ec) {
                std::cout << "Client connected.\n";
            } else {
                std::cout << "Accept failed: " << ec.message() << "\n";
            }
        });

        // 等待 accept 完成或超时
        ioc.run_one();

        if (socket.is_open()) {
            // 获取客户端信息
            auto endpoint = socket.remote_endpoint(ec);
            if (!ec) {
                std::string client_ip = endpoint.address().to_string();
                unsigned short client_port = endpoint.port();
                std::cout << "Client IP: " << client_ip << ", Port: " << client_port << "\n";

                // 读取请求
                beast::flat_buffer buffer;
                http::request<http::dynamic_body> request;
                http::read(socket, buffer, request, ec);

                if (!ec) {
                    std::string method = http::to_string(request.method()).to_string();
                    std::string url = request.target().to_string();
                    std::cout << "Request Method: " << method << ", URL: " << url << "\n";

                    // 创建响应
                    http::response<http::string_body> response(http::status::ok, request.version());
                    response.set(http::field::content_type, "text/plain");
                    response.body() = "Hello, " + method + " request received!";
                    response.prepare_payload();

                    // 发送响应
                    http::write(socket, response, ec);
                } 
                else {
                    std::cout << "Failed to read request: " << ec.message() << "\n";
                }
            }
        }

        // 重置 socket 和 timer
        socket.close();
        timer.cancel();
    }
}

int main() {
    net::io_context ioc;
    std::cout << "Starting HTTP server on port 8080...\n";
    run_http_server(ioc, "0.0.0.0", 8080);
    return 0;
}