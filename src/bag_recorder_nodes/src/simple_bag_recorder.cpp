#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "rosbag2_cpp/writer.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

class SimpleBagRecorder : public rclcpp::Node
{
public:
  SimpleBagRecorder()
  : Node("simple_bag_recorder")
  {
    writer_ = std::make_unique<rosbag2_cpp::Writer>();
    writer_->open("timed_synthetic_bag");

    subscription_ = this->create_subscription<std_msgs::msg::Int32>(
      "synthetic",
      10,
      [this](std::shared_ptr<rclcpp::SerializedMessage> msg) {
        rclcpp::Time time_stamp = this->now();
        writer_->write(msg, "synthetic", "std_msgs/msg/Int32", time_stamp);
      });
  }

private:
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
  std::unique_ptr<rosbag2_cpp::Writer> writer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SimpleBagRecorder>());
  rclcpp::shutdown();
  return 0;
}