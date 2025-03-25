//ROS2 
#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"
#include <chrono> //시간 관련 기능 제공
#include <cstdlib> //C 표준 라이브러리로, 문자열을 숫자로 변환하는 atoll 함수 사용
#include <memory> //스마트 포인터 사용(std::shared_ptr)

using namespace std::chrono_literals;

int main(int argc, char **argv)
{ //초기화
  rclcpp::init(argc, argv);
  //인자가 3이 아니면 오류 메세지 뜨고 반환  
  if (argc != 3) {
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "usage: put motor_number X  and angle Y");
      return 1;
  }  
  // 노드 공유포인터 만들기
  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("add_two_ints_client");
  // 노드 이름설정, 타입 지정
  rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client =
    node->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");
  //타입 요청 공유포인터화 내부 설정 
  auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
  request->a = atoll(argv[1]);
  request->b = atoll(argv[2]);
  //서비스 서버가 실행중인지 확인한다.
  while (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
      return 0;
    }
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not available, waiting again...");
  }
 // 결과 보내기    
  auto result = client->async_send_request(request);
  // Wait for the result.
  if (rclcpp::spin_until_future_complete(node, result) ==
    rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "motor %d 가 %d각도만큼 움직였다.", request->a , request ->b);
  } else {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service add_two_ints");
  }

  rclcpp::shutdown();
  return 0;
}
