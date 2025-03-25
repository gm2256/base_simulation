import os
import launch
import launch_ros
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration, Command
from ament_index_python.packages import get_package_share_directory

# 모든 패키지 실행
def generate_launch_description():
    # Launch Arguments 선언
    use_sim_time = LaunchConfiguration('use_sim_time')
    urdf = LaunchConfiguration('urdf')
    
    # 패키지 및 URDF 파일 경로 설정
    pkg_path = get_package_share_directory('robot_description')
    urdf_path = os.path.join(pkg_path, 'urdf', 'robot.urdf.xacro')
    
    # Declare launch arguments
    declare_use_sim_time = DeclareLaunchArgument(
            'use_sim_time', default_value='true',
            description='Use sim time if true')

    declare_urdf = DeclareLaunchArgument(
            name='urdf', default_value=urdf_path,
            description='Path to the robot description file')
    
    # robot_state_publisher 노드 설정
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time,'robot_description': Command(['xacro ', urdf])}]
    )

    # Launch Description 반환
    return LaunchDescription([
        declare_urdf,
        declare_use_sim_time,
        robot_state_publisher
    ])


