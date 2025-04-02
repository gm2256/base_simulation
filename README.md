# **simple setting** - there are not nav and amcl 

## **version**
gazebo_sim_harmonic, ros2 jazzy , ubuntu 24.04

## **urdf**
robot -- camera , lidar, casterwheel, 2 wheel 
## **ready
### 1) make a forder
  cd ~/arm_ros/src
2) make python package named robot_description 
ros2 pkg create robot_description --build-type ament_python --dependencies rclpy std_msgs
4) git clone file

##  **start**
ros2 launch robot_description launch_sim.launch.py

##  **mapping**
ros2 launch robot_description map_building.launch.py


![스크린샷 2025-03-25 16-37-44](https://github.com/user-attachments/assets/9e1e8b2d-6a20-45d1-9fff-08de4e1e28f6)
