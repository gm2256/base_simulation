import os
import launch
import launch_ros
from launch import LaunchDescription
from launch.conditions import IfCondition
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, Command
from launch.conditions import UnlessCondition
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument, GroupAction 

# 모든 패키지 실행 ros2 run teleop_twist_keyboard teleop_twist_keyboard //[1743256528.397354798] [amcl]: Setting pose (234.815000): 0.835 -0.221 1.515


def generate_launch_description():
    #월드, rviz 인자
    rviz  = LaunchConfiguration('rviz')
    world = LaunchConfiguration('world')
    
    #경로 가져오기
    pkg_name ='robot_description'
    urdf_path = os.path.join(get_package_share_directory(pkg_name),'urdf','robot.urdf.xacro')
    world_path = os.path.join('/home/user/arm_ros2/src/robot_description/','worlds','maze2.sdf')
    
    # Launch Arguments
    use_sim_time = DeclareLaunchArgument(
        name='use_sim_time', default_value='true',
        description='Opens rviz is set to True')
    
    declare_rviz = DeclareLaunchArgument(
        name='rviz', default_value='true',
        description='Opens rviz is set to True')
    
    declare_world = DeclareLaunchArgument(
        name='world', default_value=world_path,
        description='Opens rviz is set to True')
    
    
    #다른 런치파일 가져오기
    rsp = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            os.path.join(get_package_share_directory(pkg_name), 'launch', 'upload.launch.py')
        ]),
        launch_arguments={'use_sim_time': 'true', 'urdf': urdf_path}.items()
    ) 
    
    # Launch the gazebo server to initialize the simulation
    gazebo_server = IncludeLaunchDescription(
                    PythonLaunchDescriptionSource([os.path.join(
                        get_package_share_directory('ros_gz_sim'), 'launch', 'gz_sim.launch.py'
                    )]), launch_arguments={'gz_args':['-r -s -v1 ',world], 'on_exit_shutdown': 'true'}.items()
    )

    # Always launch the gazebo client to visualize the simulation
    gazebo_client = IncludeLaunchDescription(
                    PythonLaunchDescriptionSource([os.path.join(
                        get_package_share_directory('ros_gz_sim'), 'launch', 'gz_sim.launch.py'
                    )]), launch_arguments={'gz_args': '-g '}.items()
    )
    
    
    # Spawn the robot in Gazebo
    spawn_entity = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=[
            "-name",
            "robot1",
            "-topic",
            "/robot_description",
            "-x",
            "0",
            "-y",
            "0",
            "-z",
            "0.1",
        ],
        output="screen",
    )

    # Launch the Gazebo-ROS bridge
    bridge_params = os.path.join(get_package_share_directory(pkg_name),'config','gz_bridge.yaml')
    ros_gz_bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            '--ros-args',
            '-p',
            f'config_file:={bridge_params}',]
    )
    #rivz config
    default_rviz_config_path = os.path.join(get_package_share_directory(pkg_name), 'rviz', 'urdf_config.rviz')
    # rviz2 실행 노드
    rviz2 = GroupAction(
        condition=IfCondition(rviz),
        actions=[Node(
                    package='rviz2',
                    executable='rviz2',
                    arguments=['-d', default_rviz_config_path],
                    parameters=[{'use_sim_time': LaunchConfiguration('use_sim_time')}],
                    output='screen',)]
    )

    return LaunchDescription([
        declare_rviz,
        declare_world,
        use_sim_time,
        rsp,
        gazebo_client,
        gazebo_server,
        spawn_entity,
        ros_gz_bridge,
        rviz2
        
    ])