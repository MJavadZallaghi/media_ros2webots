#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "ros2webots_rosbot/rosbot_localization_node.hpp"

// Localization SDK main header
#include "ros2webots_localization_model_cg.h"

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "nav_msgs/msg/odometry.hpp"

using namespace std::chrono_literals;

class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("minimal_publisher"), count_(0)
    {
      publisher_ = this->create_publisher<std_msgs::msg::String>("topic_test", 10);
      publisherOdomData_ = this->create_publisher<nav_msgs::msg::Odometry>("ros2/rosbot/odometry", 10);
      timer_ = this->create_wall_timer(500ms, std::bind(&MinimalPublisher::timer_callback, this));
      timerOdomDataPub_ = this->create_wall_timer(50ms, std::bind(&MinimalPublisher::timerOdomDataPub_callback, this));

      // Adding the subscriber to the "topic_test"
      subscriber_ = this->create_subscription<std_msgs::msg::String>("topic_test", 10, std::bind(&MinimalPublisher::subscriber_callback, this, std::placeholders::_1));
      
      // Initialized Localozition SDK object
      localization_sdk_obj_.initialize();
    }

  private:
    void timer_callback()
    {
      auto message = std_msgs::msg::String();
      message.data = "Hello, world! " + std::to_string(count_++);
      RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
      publisher_->publish(message);
    }

    void timerOdomDataPub_callback() {
      RunLocalizationProcess();
      auto message = nav_msgs::msg::Odometry();
      auto current_time = std::chrono::system_clock::now();
      auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(current_time.time_since_epoch());
      message.header.stamp.sec = static_cast<uint32_t>(timestamp.count());
      message.header.stamp.nanosec = static_cast<uint32_t>((current_time.time_since_epoch() - timestamp).count());
      message.pose.pose.position.x = 1.3;
      publisherOdomData_->publish(message);
    }

    void RunLocalizationProcess() {
      // Set inputs
      localization_input_data.imu_orientation_vector[10] = 0.5;
      localization_sdk_obj_.setExternalInputs(&localization_input_data);
      // Do localization

      // Get outputs

    }

    void subscriber_callback(const std_msgs::msg::String::SharedPtr msg)
    {
      RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg->data.c_str());
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::TimerBase::SharedPtr timerOdomDataPub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisherOdomData_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscriber_;
    size_t count_;

    // Create Localizition SDK object
    ros2webots_localization_model_cg localization_sdk_obj_;

    // Localization input data struct
    ros2webots_localization_model_cg::ExtU_ros2webots_localization__T localization_input_data;

    // Localization output data struct
    ros2webots_localization_model_cg::ExtY_ros2webots_localization__T localization_output_data;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}
