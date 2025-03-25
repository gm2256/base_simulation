import os
import launch
import launch_ros
from launch import LaunchDescription
from launch.conditions import IfCondition
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch.conditions import UnlessCondition
from launch_ros.substitutions import FindPackageShare 
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument, GroupAction

def generate_launch_description():
    #setting 
    use_sim_time  = LaunchConfiguration('use_sim_time')
    slam_params_file = LaunchConfiguration('slam_params_file')
    async_or_sync = LaunchConfiguration('async_or_sync')   

    #name 
    pkg_name ='robot_description'
    slam_name = 'slam_toolbox' 
    
    #path
    slam_params_file_path = os.path.join(get_package_share_directory(pkg_name),'config','mapper_params.yaml')
    
    # Launch py
    launch_slam_sync = PathJoinSubstitution(
        [get_package_share_directory(slam_name), 'launch', 'online_sync_launch.py'])

    launch_slam_async = PathJoinSubstitution(
        [get_package_share_directory(slam_name), 'launch', 'online_async_launch.py'])
    
    # Launch Arguments
    declare_use_sim_time = DeclareLaunchArgument(
        name='use_sim_time', default_value='true')
    
    declare_slam_params_file = DeclareLaunchArgument(
        name='slam_params_file', default_value=slam_params_file_path,
        description='Opening param_file is set to True')
    
    
    declare_async_or_sync = DeclareLaunchArgument(
        name='async_or_sync', default_value='false',choices=['true', 'false'],
        description='Opening param_file is set to True')
   
    #'true'일 때 동기식 런치 파일이 실행되고, 'false'일 때 비동기식 런치 파일이 실행되도록 조건이 설정되어 있습니다
    slam = GroupAction([
        
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(launch_slam_sync),
            launch_arguments=[
                ('use_sim_time', use_sim_time),
                ('slam_params_file', slam_params_file)
            ],
            condition=IfCondition(async_or_sync)
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(launch_slam_async),
            launch_arguments=[
                ('use_sim_time', use_sim_time),
                ('slam_params_file', slam_params_file)
            ],
            condition=UnlessCondition(async_or_sync)
        )
    ])
         

    return LaunchDescription([
     declare_use_sim_time,
     declare_slam_params_file,
     declare_async_or_sync,
     slam
        
    ])