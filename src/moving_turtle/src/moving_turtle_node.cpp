#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class MovingTurtle : public rclcpp::Node
{
public:
  MovingTurtle()
  : Node("moving_turtle"), loop_count_(0), moving_straight_(true)
  {
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

    // 100Hz = 10ms 주기
    timer_ = this->create_wall_timer(
      10ms,
      [this]() {
        auto msg = geometry_msgs::msg::Twist();

        if (moving_straight_) {
          msg.linear.x = 2.0;
          msg.angular.z = 0.0;
        } else {
          msg.linear.x = 0.0;
          msg.angular.z = 1.0;
        }

        publisher_->publish(msg);

        loop_count_++;
        if (loop_count_ >= 100) {
          loop_count_ = 0;
          moving_straight_ = !moving_straight_;
        }
      });
  }

private:
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int loop_count_;
  bool moving_straight_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MovingTurtle>());
  rclcpp::shutdown();
  return 0;
}