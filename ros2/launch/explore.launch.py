from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    pkg   = get_package_share_directory('loki_robot')
    expl  = get_package_share_directory('explore_lite')
    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                os.path.join(expl, 'launch', 'explore.launch.py')),
            launch_arguments={
                'params_file': os.path.join(pkg, 'config', 'explore_lite.yaml'),
            }.items(),
        )
    ])
