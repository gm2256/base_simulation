import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
def generate_launch_description():
    pkg_name ='robot_description'
    config_path = os.path.join(get_package_share_directory(pkg_name),'config')
    
    #parmameter_file
    map_file = os.path.join(config_path, 'map_server.yaml')
    amcl_file = os.path.join(config_path, 'amcl.yaml')
    nav2_file = os.path.join(config_path, 'navigation.yaml')

    #setting 
    lifecycle = ['map_server','amcl','planner_server','controller_server','bt_navigator','behavior_server']

    
    # gazebo , rviz launch 파일 가져오기
    sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            os.path.join(get_package_share_directory(pkg_name), 'launch', 'launch_sim.launch.py')
        ])) 
   
   # nav2 파일 가져오기
    nav2 = IncludeLaunchDescription(
    PythonLaunchDescriptionSource(
        os.path.join(get_package_share_directory('nav2_bringup'), 'launch', 'navigation_launch.py')
    ),
    launch_arguments={
        'use_sim_time': 'True',
        'params_file': nav2_file   # <-- 여기만 수정
    }.items()
)

    return LaunchDescription([
        sim,
        Node(
            package='nav2_map_server',
            executable='map_server',
            name='map_server',
            output='screen',
            parameters= [map_file]
        ),

        Node(
            package='nav2_amcl',
            executable='amcl',
            name='amcl',
            output='screen',
            parameters= [amcl_file]
        ),
        
        nav2,                   
        
        Node(
            package='nav2_lifecycle_manager',
            executable='lifecycle_manager',
            name='lifecycle_manager_map',
            output='screen',
            parameters=[{
                'use_sim_time': True,
                'autostart': True,
                'node_names': lifecycle
            }]
        )
       
    ])