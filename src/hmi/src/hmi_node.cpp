#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "my_msgs/msg/turtle_color.hpp"
#include "visualization_msgs/msg/marker.hpp"

using namespace std::chrono_literals;

class Hmi : public rclcpp::Node
{
public:
  Hmi()
  : Node("hmi"), has_pose_(false), has_color_(false)
  {
    marker_publisher_ = this->create_publisher<visualization_msgs::msg::Marker>("turtle_marker", 10);

    pose_subscription_ = this->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose",
      10,
      [this](const turtlesim::msg::Pose::SharedPtr msg) {
        latest_pose_ = *msg;
        has_pose_ = true;
      });

    color_subscription_ = this->create_subscription<my_msgs::msg::TurtleColor>(
      "turtle_color",
      10,
      [this](const my_msgs::msg::TurtleColor::SharedPtr msg) {
        latest_color_ = *msg;
        has_color_ = true;
      });

    // 요구사항: HMI는 60Hz로 동작 → 약 16.67ms 주기
    timer_ = this->create_wall_timer(
      16ms,
      [this]() {
        this->publish_marker();
      });
  }

private:
  void publish_marker()
  {
    if (!has_pose_ || !has_color_) {
      return;
    }

    visualization_msgs::msg::Marker marker;

    // a. header
    marker.header.frame_id = "map";
    marker.header.stamp = this->now();

    // b. type / action
    marker.type = visualization_msgs::msg::Marker::CUBE;
    marker.action = visualization_msgs::msg::Marker::ADD;

    // c. position
    marker.pose.position.x = latest_pose_.x;
    marker.pose.position.y = latest_pose_.y;
    marker.pose.position.z = 0.0;

    // d. orientation
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    // e. scale
    marker.scale.x = 1.0;
    marker.scale.y = 1.0;
    marker.scale.z = 1.0;

    // f. color
    marker.color.r = latest_color_.r;
    marker.color.g = latest_color_.g;
    marker.color.b = latest_color_.b;
    marker.color.a = 1.0;

    marker_publisher_->publish(marker);
  }

  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr marker_publisher_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscription_;
  rclcpp::Subscription<my_msgs::msg::TurtleColor>::SharedPtr color_subscription_;
  rclcpp::TimerBase::SharedPtr timer_;

  turtlesim::msg::Pose latest_pose_;
  my_msgs::msg::TurtleColor latest_color_;
  bool has_pose_;
  bool has_color_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Hmi>());
  rclcpp::shutdown();
  return 0;
}