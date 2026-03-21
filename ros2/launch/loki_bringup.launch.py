import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg = get_package_share_directory('loki_robot')
    nav2_pkg = get_package_share_directory('nav2_bringup')

    micro_ros_agent = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        arguments=['udp4', '--port', '8888'],
    )

    cartographer = Node(
        package='cartographer_ros',
        executable='cartographer_node',
        parameters=[{'use_sim_time': False}],
        arguments=[
            '-configuration_directory', os.path.join(pkg, 'config'),
            '-configuration_basename', 'cartographer_lds_2d.lua',
        ],
    )

    nav2 = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_pkg, 'launch', 'navigation_launch.py')),
        launch_arguments={'params_file': os.path.join(pkg, 'config', 'navigation.yaml'),
                          'use_sim_time': 'False'}.items(),
    )

    return LaunchDescription([micro_ros_agent, cartographer, nav2])
