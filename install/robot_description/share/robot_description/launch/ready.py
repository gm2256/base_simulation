from ament_index_python.packages import get_package_share_directory
import os
import xml.etree.ElementTree as ET
pkg_name ="robot_description"
pkg_path = get_package_share_directory(pkg_name)
world_file_name ='maze.world'

world_path = os.path.join(pkg_path, "worlds", world_file_name)

# 경로 출력해서 디버깅 가능
print("World path:", world_path)

#/home/user/arm_ros2/install/robot_description/share/robot_description/worlds/maze.world
#/home/user/arm_ros2/install/robot_description/share/robot_description/worlds/maze.world


