#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost::asio;

class SimplePublisherNode : public rclcpp::Node {
public:
    SimplePublisherNode(serial_port &serial)
        : Node("com_node"), serial_(serial), input_buffer_(1024) {
        // 퍼블리셔 생성
        publisher_ = this->create_publisher<std_msgs::msg::String>("robot_state", 10);

        // 타이머 생성 (1초마다 메시지 발행)
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            std::bind(&SimplePublisherNode::publish_message, this));

        RCLCPP_INFO(this->get_logger(), "노드 초기화 완료");
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    serial_port &serial_;
    rclcpp::TimerBase::SharedPtr timer_;
    boost::asio::streambuf input_buffer_;

    void publish_message() {
        try {
            
            // 시리얼 데이터에서 한 줄씩 읽기
            boost::system::error_code ec;
            if (ec) {
                RCLCPP_ERROR(this->get_logger(), "시리얼 읽기 오류: %s", ec.message().c_str());
                return;
            }
            // read_until 함수가 실행되면 **input_buffer_**의 내용이 바뀜
            size_t bytes_read = read_until(serial_, input_buffer_, '\n', ec); 
            // 읽은 데이터를 문자열로 변환
            std::istream is(&input_buffer_);
            std::string received_data;
            std::getline(is, received_data); // 한 줄 읽기 (줄바꿈 문자 제거)

            // 메시지 발행
            auto message = std_msgs::msg::String();
            message.data = received_data;
            RCLCPP_INFO(this->get_logger(), "모터 각도: '%s'", message.data.c_str());
            publisher_->publish(message);
        } catch (const std::exception &e) {
            RCLCPP_ERROR(this->get_logger(), "시리얼 읽기 중 예외 발생: %s", e.what());
        }
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);

    io_context io;
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

    auto node = std::make_shared<SimplePublisherNode>(serial);
    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}



