from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg = get_package_share_directory('loki_robot')
    return LaunchDescription([
        Node(
            package='cartographer_ros',
            executable='cartographer_node',
            name='cartographer',
            parameters=[{'use_sim_time': False}],
            arguments=[
                '-configuration_directory', os.path.join(pkg, 'config'),
                '-configuration_basename', 'cartographer_lds_2d.lua',
            ],
            output='screen',
        ),
        Node(
            package='cartographer_ros',
            executable='cartographer_occupancy_grid_node',
            parameters=[{'use_sim_time': False, 'resolution': 0.05}],
        ),
    ])
