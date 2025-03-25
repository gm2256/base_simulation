#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"
#include <std_msgs/msg/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include "std_srvs/srv/trigger.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <sstream>    // 문자열 스트림(std::stringstream)
#include <vector>     // 동적 배열(std::vector)
#include <mysql/mysql.h>
#include <iostream>
#include <string>

using namespace boost::asio;
//각도 변경, -> 안정적인 각도로 돌아가기
int orginal_angle[6] = {100,50 ,50,60,90,60}; 
// service -> 각도 변경 -> switch case 각도 -> 현재 각도 변경
//현재 각도
int servo_angle[6] = {0, 0, 0, 0, 0, 0};
//시리얼 통신 보내기 ->  모터 움직임 
void add(
    const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
    std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response,
    serial_port &serial, std::mutex &serial_mutex)
{
    std::string command = std::to_string(request->a) + "," + std::to_string(request->b);
    try {
        std::lock_guard<std::mutex> lock(serial_mutex);
        write(serial, buffer(command + "\n"));
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "명령어 전송: %s", command.c_str());
        response->sum = 1;
    } catch (std::exception &e) {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "시리얼 전송 오류: %s", e.what());
        response->sum = 0;
    }
}

void store_data(int num ,int angle){

   servo_angle[num] = angle;

}

//데이터 삽입하기 
// **기존 테이블(`sensor_readings`)에 데이터 삽입하는 함수**
bool insert_data_into_existing_table(int angle[6]) {
    MYSQL *conn;
    conn = mysql_init(nullptr);

    // MySQL 연결 확인
    if (!conn) {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "❌ MySQL 초기화 실패");
        return false;
    }

    // **MySQL 서버 연결 (비밀번호 및 데이터베이스 정보 수정 가능)**
    if (!mysql_real_connect(conn, "localhost", "gm2256", "1234", "cas", 0, nullptr, 0)) {
        std::cerr << "❌ MySQL 연결 실패: " << mysql_error(conn) << std::endl;
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "❌ MySQL 연결 실패");
        mysql_close(conn);
        return false;
    }

    // **INSERT 쿼리 작성**
    std::string query = "INSERT INTO data(motor_angle0, motor_angle1, motor_angle2, motor_angle3, motor_angle4, motor_angle5) VALUES (" +
                        std::to_string(angle[0]) + ", " + std::to_string(angle[1]) + ", " +
                        std::to_string(angle[2]) + ", " + std::to_string(angle[3]) + ", " +
                        std::to_string(angle[4]) + ", " + std::to_string(angle[5]) + ")";

    // **쿼리 실행**
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "❌ 쿼리 실행 실패: " << mysql_error(conn) << std::endl;
         RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "❌ 쿼리 실행 실패");
        mysql_close(conn);
        return false;
    }

    std::cout << "✅ 데이터 삽입 성공: " << query << std::endl;
    
    // MySQL 연결 종료
    mysql_close(conn);
    return true;
}

//데이터 저장하기
void save(
  const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
  std::shared_ptr<std_srvs::srv::Trigger::Response> response)
{ //데이터 베이스에 값 대입
  bool success = insert_data_into_existing_table(servo_angle);
  (void) request; 
  if (success){
  response->success = true;
  response->message = "save is success";
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Service called, sending response.");}
  else{ 
  response->success = false;
  response->message = "save is false";
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Service not called");

  }
}
  
//데이터 시작할 때, 초기 위치 잡아주기
void start (
  const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
  std::shared_ptr<std_srvs::srv::Trigger::Response> response,
  serial_port &serial, std::mutex &serial_mutex)
{ //먼저 값 넣어주고 시작 
  std::copy(std::begin(orginal_angle), std::end(orginal_angle), std::begin(servo_angle));
  //아두이노에 초기값 넣어주고 시작
  std::string start = std::to_string(servo_angle[0]) + "," + std::to_string(servo_angle[1])+"," + std::to_string(servo_angle[2]) + "," + std::to_string(servo_angle[3])+ "," + std::to_string(servo_angle[4])+ "," + std::to_string(servo_angle[5]);
  try {
        std::lock_guard<std::mutex> lock(serial_mutex);
        write(serial, buffer(start+ "\n"));
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "명령어 전송: %s", start.c_str());
        response->success = true;
        response->message = "성공했다";
  } catch (std::exception &e) {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "시리얼 전송 오류: %s", e.what());
        response->success = false;
        response->message = "실패했다";}
}

//갑작스럽게 멈출때, 사
void stop (
  const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
  std::shared_ptr<std_srvs::srv::Trigger::Response> response,
  serial_port &serial, std::mutex &serial_mutex)
{ 
  //아두이노에 초기값 넣어주고 시작
  std::string stop = std::to_string(2);
  try {
        std::lock_guard<std::mutex> lock(serial_mutex);
        write(serial, buffer(stop+ "\n"));
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "명령어 전송: %s", stop.c_str());
        response->success = true;
        response->message = "성공했다";
  } catch (std::exception &e) {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "시리얼 전송 오류: %s", e.what());
        response->success = false;
        response->message = "실패했다";}
}

//시리얼 통신 읽기 -> 모터 움직임 각도 저장 
void publish_message(serial_port &serial, std::mutex &serial_mutex, rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher, rclcpp::Logger logger)
{
    try {
        std::lock_guard<std::mutex> lock(serial_mutex);
        boost::asio::streambuf input_buffer;
        boost::system::error_code ec;
        size_t bytes_read = read_until(serial, input_buffer, '\n', ec);
        if (ec) {
            RCLCPP_ERROR(logger, "시리얼 읽기 오류: %s", ec.message().c_str());
            return;
        }
        RCLCPP_INFO(logger, "🔹 publish_message() 실행됨");  // 실행됨
        std::istream is(&input_buffer);
        std::string received_data;
        std::getline(is, received_data);
        auto message = std_msgs::msg::String();
        message.data = received_data;
        RCLCPP_INFO(logger, "모터 '%s'", message.data.c_str());
        publisher->publish(message);
        //들어온 데이터 저장 
        std::string word;
        std::string words[3];  // 크기가 6인 문자열 배열
        std::stringstream ss(received_data);  // 문자열을 스트림으로 변환
        int word_index = 0;

        while (std::getline(ss, word, ',') && word_index < 3) {  // 6개까지만 저장
        words[word_index++] = word;
        }

        //데이터 저장
        store_data(stoi(words[0]),stoi(words[1]));
           
    } catch (const std::exception &e) {
        RCLCPP_ERROR(logger, "시리얼 읽기 중 예외 발생: %s", e.what());
    }
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    io_context io;
    serial_port serial(io, "/dev/ttyACM0");
    std::mutex serial_mutex;

    try {
        serial.set_option(serial_port_base::baud_rate(9600));
        serial.set_option(serial_port_base::character_size(8));
        serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    } catch (std::exception &e) {
        std::cerr << "시리얼 포트 초기화 실패: " << e.what() << std::endl;
        return 1;
    }
    
    auto node = rclcpp::Node::make_shared("add_two_ints_server");
    auto publisher = node->create_publisher<std_msgs::msg::String>("robot_state", 10);
    // send_angle data
    auto service = node->create_service<example_interfaces::srv::AddTwoInts>(
        "add_two_ints",
        [&serial, &serial_mutex](const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
                                 std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response) {
            add(request, response, serial, serial_mutex);
        });
    // save_angle data     
    auto service2 = node->create_service<std_srvs::srv::Trigger>(
    "save_state_data",
    std::bind(&save, std::placeholders::_1, std::placeholders::_2));
    // make state of start angle  
    auto service3 = node->create_service<std_srvs::srv::Trigger>(
    "start",
    std::bind(&start, std::placeholders::_1, std::placeholders::_2, std::ref(serial), std::ref(serial_mutex)));
    
    auto service4 = node->create_service<std_srvs::srv::Trigger>(
    "stop",
    std::bind(&stop, std::placeholders::_1, std::placeholders::_2, std::ref(serial), std::ref(serial_mutex)));

    auto timer = node->create_wall_timer(
        std::chrono::seconds(1),
        [&serial, &serial_mutex, publisher, logger = node->get_logger()]() {
            publish_message(serial, serial_mutex, publisher, logger);
        });

    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to add two ints.");
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}




