#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

class DataGenerator : public rclcpp::Node
{
public:
  DataGenerator()
  : Node("data_generator_node"), count_(0)
  {
    publisher_ = this->create_publisher<std_msgs::msg::Int32>("synthetic", 10);
    timer_ = this->create_wall_timer(
      1s,
      [this]() {
        auto msg = std_msgs::msg::Int32();
        msg.data = ++count_;
        RCLCPP_INFO(this->get_logger(), "Publishing: '%d'", msg.data);
        publisher_->publish(msg);
      });
  }

private:
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int32_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DataGenerator>());
  rclcpp::shutdown();
  return 0;
}