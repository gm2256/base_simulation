cmake_minimum_required(VERSION 3.8)
project(localizer_server) 

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(rcl_interfaces REQUIRED)

add_executable(init_pose_pub src/init_robot.cpp)
add_executable(global_localization src/global_localization.cpp)

ament_target_dependencies(init_pose_pub rclcpp geometry_msgs tf2 tf2_geometry_msgs)
ament_target_dependencies(global_localization rclcpp std_srvs rcl_interfaces)

install(TARGETS
  init_pose_pub global_localization 
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
