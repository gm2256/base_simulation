import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution, Command
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_name ='robot_description'
    #path
    config_path = os.path.join(get_package_share_directory(pkg_name),'config')
    map_file = "/home/user/arm_ros2/global_map.yaml"
    amcl_file = os.path.join(config_path, 'amcl.yaml')
    
    use_sim_time = DeclareLaunchArgument(
        'use_sim_time', default_value='True',
        description='Flag to enable use_sim_time'
    )

    # Path to the Slam Toolbox launch file
    nav2_localization_launch_path = os.path.join(
        get_package_share_directory('nav2_bringup'),
        'launch',
        'localization_launch.py'
    )
    
    localization_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(nav2_localization_launch_path),
        launch_arguments={
                'use_sim_time': LaunchConfiguration('use_sim_time'),
                'params_file': amcl_file,
                'map': map_file,
        }.items()
    )

    return LaunchDescription([
    use_sim_time,
    localization_launch
             
    ])
