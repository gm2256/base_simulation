#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>  // 메시지 타입 헤더 포함

class SimpleSubscriberNode : public rclcpp::Node {
public:
    SimpleSubscriberNode() : Node("arduino") {  // 노드 이름 설정
        // 서브스크라이버 생성
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "robot_state", 10,
            std::bind(&SimpleSubscriberNode::topic_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "아두이노에서 송신 ");
    }

private:
    // 서브스크라이버 포인터
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;

    // 토픽 메시지를 처리하는 콜백 함수 --> msg 공유포인터로 변환 그중에 data 가져오기
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(), "받은 메시지: '%s'", msg->data.c_str());
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);  // ROS 2 초기화
    rclcpp::spin(std::make_shared<SimpleSubscriberNode>());  // 노드 실행
    rclcpp::shutdown();  // ROS 2 종료
    return 0;
}
