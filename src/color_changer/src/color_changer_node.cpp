#include <memory>
#include <vector>
#include <cmath>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "my_msgs/msg/turtle_color.hpp"

using namespace std::chrono_literals;

class ColorChanger : public rclcpp::Node
{
public:
  ColorChanger()
  : Node("color_changer"), has_pose_(false)
  {
    // turtlesim 노드의 파라미터를 원격으로 바꾸기 위한 클라이언트
    param_client_ = std::make_shared<rclcpp::AsyncParametersClient>(this, "/turtlesim");

    // HMI로 보낼 커스텀 메시지 퍼블리셔
    color_publisher_ = this->create_publisher<my_msgs::msg::TurtleColor>("turtle_color", 10);

    // pose는 최신값만 저장해두고, 실제 처리는 타이머에서
    pose_subscription_ = this->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose",
      10,
      [this](const turtlesim::msg::Pose::SharedPtr msg) {
        latest_pose_ = *msg;
        has_pose_ = true;
      });

    // 요구사항: Color Changer는 10Hz로 동작 → 100ms 주기
    timer_ = this->create_wall_timer(
      100ms,
      [this]() {
        this->process();
      });
  }

private:
  void process()
  {
    if (!has_pose_) {
      return;
    }

    double theta_deg = latest_pose_.theta * 180.0 / M_PI;

    std::vector<rclcpp::Parameter> bg_params;
    my_msgs::msg::TurtleColor color_msg;

    if (theta_deg >= 0.0 && theta_deg < 90.0) {
      // a. 0~90도
      bg_params.push_back(rclcpp::Parameter("background_r", 255));
      bg_params.push_back(rclcpp::Parameter("background_g", 255));
      bg_params.push_back(rclcpp::Parameter("background_b", 255));
      color_msg.r = 1.0f; color_msg.g = 0.0f; color_msg.b = 0.0f;
    } else if (theta_deg >= 90.0 && theta_deg < 180.0) {
      // b. 90~180도
      bg_params.push_back(rclcpp::Parameter("background_r", 255));
      bg_params.push_back(rclcpp::Parameter("background_g", 0));
      bg_params.push_back(rclcpp::Parameter("background_b", 0));
      color_msg.r = 1.0f; color_msg.g = 0.0f; color_msg.b = 0.0f;
    } else if (theta_deg >= -180.0 && theta_deg < -90.0) {
      // c. -180~-90도
      bg_params.push_back(rclcpp::Parameter("background_r", 0));
      bg_params.push_back(rclcpp::Parameter("background_g", 255));
      bg_params.push_back(rclcpp::Parameter("background_b", 0));
      color_msg.r = 1.0f; color_msg.g = 0.0f; color_msg.b = 0.0f;
    } else {
      // d. -90~0도
      bg_params.push_back(rclcpp::Parameter("background_r", 0));
      bg_params.push_back(rclcpp::Parameter("background_g", 0));
      bg_params.push_back(rclcpp::Parameter("background_b", 255));
      color_msg.r = 0.0f; color_msg.g = 0.0f; color_msg.b = 1.0f;
    }

    param_client_->set_parameters(bg_params);
    color_publisher_->publish(color_msg);
  }

  std::shared_ptr<rclcpp::AsyncParametersClient> param_client_;
  rclcpp::Publisher<my_msgs::msg::TurtleColor>::SharedPtr color_publisher_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscription_;
  rclcpp::TimerBase::SharedPtr timer_;

  turtlesim::msg::Pose latest_pose_;
  bool has_pose_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ColorChanger>());
  rclcpp::shutdown();
  return 0;
}