#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/trigger.hpp"
//stop 상태로 가!! 요청 보내는 클라이언트 코드
int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("stop_command");

  auto client = node->create_client<std_srvs::srv::Trigger>("stop");

  while (!client->wait_for_service(std::chrono::seconds(1))) {
    RCLCPP_INFO(node->get_logger(), "Waiting for service...");
  }

  // 빈 요청 생성
  auto request = std::make_shared<std_srvs::srv::Trigger::Request>();

  auto result = client->async_send_request(request);

  if (rclcpp::spin_until_future_complete(node, result) == rclcpp::FutureReturnCode::SUCCESS) {
    RCLCPP_INFO(node->get_logger(), "Response: %s", result.get()->message.c_str());
  } else {
    RCLCPP_ERROR(node->get_logger(), "Service call failed.");
  }

  rclcpp::shutdown();
  return 0;
}
