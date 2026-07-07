import os
import xacro
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg     = get_package_share_directory('loki_robot')
    nav2_pkg = get_package_share_directory('nav2_bringup')

    robot_desc = xacro.process_file(
        os.path.join(pkg, 'urdf', 'robot.urdf.xacro')).toxml()

    # ── micro-ROS agent (ESP32 bridge) ────────────────────────────────────
    micro_ros_agent = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='micro_ros_agent',
        arguments=['udp4', '--port', '8888'],
        output='screen',
    )

    # ── Robot state publisher (URDF → TF tree) ────────────────────────────
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        parameters=[{
            'robot_description': robot_desc,
            'use_sim_time': False,
        }],
        output='screen',
    )

    # ── EKF — odom + IMU fusion ───────────────────────────────────────────
    ekf = Node(
        package='robot_localization',
        executable='ekf_node',
        name='ekf_filter_node',
        parameters=[os.path.join(pkg, 'config', 'ekf.yaml')],
        remappings=[('/odometry/filtered', '/odometry/filtered')],
        output='screen',
    )

    # ── Cartographer 2D SLAM ──────────────────────────────────────────────
    cartographer = Node(
        package='cartographer_ros',
        executable='cartographer_node',
        name='cartographer_node',
        parameters=[{'use_sim_time': False}],
        arguments=[
            '-configuration_directory', os.path.join(pkg, 'config'),
            '-configuration_basename', 'cartographer_lds_2d.lua',
        ],
        output='screen',
    )

    cartographer_occupancy_grid = Node(
        package='cartographer_ros',
        executable='cartographer_occupancy_grid_node',
        name='cartographer_occupancy_grid_node',
        parameters=[{'use_sim_time': False, 'resolution': 0.05}],
        output='screen',
    )

    # ── Nav2 navigation stack ─────────────────────────────────────────────
    nav2 = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_pkg, 'launch', 'navigation_launch.py')),
        launch_arguments={
            'params_file': os.path.join(pkg, 'config', 'navigation.yaml'),
            'use_sim_time': 'False',
        }.items(),
    )

    # Delay Nav2 to let Cartographer publish /map first
    nav2_delayed = TimerAction(period=3.0, actions=[nav2])

    return LaunchDescription([
        micro_ros_agent,
        robot_state_publisher,
        ekf,
        cartographer,
        cartographer_occupancy_grid,
        nav2_delayed,
    ])
