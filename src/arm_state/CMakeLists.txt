cmake_minimum_required(VERSION 3.8)
project(arm_state)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_library(MYSQL_LIB mysqlclient HINTS /usr/lib/x86_64-linux-gnu)
include_directories(/usr/include/mysql)

# ROS 2 의존성
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)
find_package(std_srvs REQUIRED)

# 실행 파일 추가
add_executable(com src/robot_state_pub.cpp)
add_executable(arduino src/robot_state_sub.cpp)
add_executable(client src/robot_move_client.cpp)
add_executable(server src/robot_move_server.cpp)
add_executable(ard_pubser src/robot_all.cpp)
add_executable(save src/robot_state_save_client.cpp)
add_executable(start src/robot_state_start_client.cpp)
add_executable(stop src/robot_state_stop.cpp)

# 실행 파일에 의존성 연결
ament_target_dependencies(com rclcpp std_msgs)
ament_target_dependencies(arduino rclcpp std_msgs)
ament_target_dependencies(client rclcpp example_interfaces)
ament_target_dependencies(server rclcpp example_interfaces)
ament_target_dependencies(ard_pubser rclcpp std_msgs example_interfaces std_srvs)
ament_target_dependencies(save rclcpp std_srvs)
ament_target_dependencies(start rclcpp std_srvs)
ament_target_dependencies(stop rclcpp std_srvs)

# **MySQL 라이브러리 명확하게 연결**
target_link_libraries(ard_pubser ${MYSQL_LIB})
target_link_libraries(save ${MYSQL_LIB})

# 실행 파일 설치
install(TARGETS com arduino client server ard_pubser save start stop
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
