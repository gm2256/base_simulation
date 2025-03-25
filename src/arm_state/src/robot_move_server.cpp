#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <string>
#include <memory>

using namespace boost::asio;

// 서비스 요청 및 응답 처리 함수
void add(
    const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
    std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response,
    serial_port &serial) // serial 객체 참조로 전달
{
    // 아두이노로 보낼 명령어 생성
    std::string command = std::to_string(request->a) + "'" + std::to_string(request->b);
    // 시리얼 포트를 통해 명령 전송
    try {
        write(serial, buffer(command + "\n"));
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "명령어 전송: %s", command.c_str());
        response->sum = 1;
    } catch (std::exception &e) {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "시리얼 전송 오류: %s", e.what());
        response->sum = 0;
        return;
    }


}


int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    // Boost.Asio io_context 생성
    io_context io;

    // 시리얼 포트 초기화
    serial_port serial(io, "/dev/ttyACM0");
    try {
        serial.set_option(serial_port_base::baud_rate(9600));
        serial.set_option(serial_port_base::character_size(8));
        serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    } catch (std::exception &e) {
        std::cerr << "시리얼 포트 초기화 실패: " << e.what() << std::endl;
        return 1;
    }

    // ROS2 노드 생성
    auto node = rclcpp::Node::make_shared("add_two_ints_server");

    // 서비스 서버 생성 및 시리얼 포트 참조 전달
    auto service = node->create_service<example_interfaces::srv::AddTwoInts>(
        "add_two_ints",
        [&serial](const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
                  std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response) {
            add(request, response, serial);
        });

    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to add two ints.");

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}

