import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg = get_package_share_directory('loki_robot')

    with open(os.path.join(pkg, 'urdf', 'robot.urdf.xacro'), 'r') as f:
        robot_desc = f.read()

    return LaunchDescription([
        # TF tree — required for Cartographer to find base_link → laser transform
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            parameters=[{'robot_description': robot_desc, 'use_sim_time': False}],
            output='screen',
        ),
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
            name='cartographer_occupancy_grid_node',
            parameters=[{'use_sim_time': False, 'resolution': 0.05}],
            output='screen',
        ),
    ])
