import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg = get_package_share_directory('loki_robot')
    return LaunchDescription([
        Node(
            package='teleop_twist_keyboard',
            executable='teleop_twist_keyboard',
            name='teleop_keyboard',
            parameters=[os.path.join(pkg, 'config', 'teleop_keyboard.yaml')],
            output='screen',
            prefix='xterm -e',
        ),
    ])
