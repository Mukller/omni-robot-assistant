import os
import xacro
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, SetEnvironmentVariable
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg = get_package_share_directory('loki_robot')
    gazebo_pkg = get_package_share_directory('gazebo_ros')

    # Make Gazebo find our SDF model
    model_path = os.path.join(pkg, 'sdf')

    return LaunchDescription([
        SetEnvironmentVariable('GAZEBO_MODEL_PATH', model_path),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                os.path.join(gazebo_pkg, 'launch', 'gazebo.launch.py')),
            launch_arguments={'world': os.path.join(pkg, 'sdf', 'empty.world')}.items(),
        ),

        Node(
            package='gazebo_ros',
            executable='spawn_entity.py',
            arguments=['-entity', 'loki', '-file',
                       os.path.join(pkg, 'sdf', 'makerspet_loki', 'model.sdf'),
                       '-x', '0', '-y', '0', '-z', '0.01'],
            output='screen',
        ),

        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{
                'robot_description': xacro.process_file(
                    os.path.join(pkg, 'urdf', 'robot.urdf.xacro')).toxml(),
                'use_sim_time': True,
            }],
        ),
    ])
