#include <iostream>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/serialization.hpp"
#include "rosbag2_cpp/reader.hpp"
#include "turtlesim/msg/pose.hpp"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  if (argc < 2) {
    std::cerr << "사용법: ros2 run bag_reading_cpp simple_bag_reader <bag 경로>" << std::endl;
    return 1;
  }

  // 1. Reader 생성 + bag 파일 열기
  rosbag2_cpp::Reader reader;
  reader.open(argv[1]);

  // 2. Pose 메시지 역직렬화를 위한 Serialization 객체
  rclcpp::Serialization<turtlesim::msg::Pose> serialization;

  // 3. bag에 메시지가 남아있는 동안 반복해서 읽기
  while (reader.has_next()) {
    auto bag_message = reader.read_next();

    // 직렬화된 데이터를 실제 Pose 객체로 변환
    turtlesim::msg::Pose pose;
    rclcpp::SerializedMessage serialized_msg(*bag_message->serialized_data);
    serialization.deserialize_message(&serialized_msg, &pose);

    std::cout << "(" << pose.x << ", " << pose.y << ")" << std::endl;
  }

  rclcpp::shutdown();
  return 0;
}