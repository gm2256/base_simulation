#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

class InitRobot : public rclcpp::Node
{
public:
    //start_node
    InitRobot()
    : Node("initial_pose_pub_node"), trial_count_(5)
    {
        init_pose_pub_ = this->create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>("/initialpose", 1);
        clicked_point_sub_ = this->create_subscription<geometry_msgs::msg::PointStamped>(
            "/clicked_point", 1,
            std::bind(&InitRobot::point_callback, this, std::placeholders::_1));

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&InitRobot::timer_callback, this));
    }

private:
    void timer_callback()
    {
        trial_count_--;
        init_pose(0.835 ,-0.221, 1.515);  // 초기 위치 설정

        if (trial_count_ <= 0)
        {
            timer_->cancel();
        }
    }

    void point_callback(const geometry_msgs::msg::PointStamped::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(), "Received Data:\n X: %f\n Y: %f\n Z: %f", msg->point.x, msg->point.y, msg->point.z);
        init_pose(msg->point.x, msg->point.y);
    }
    //func to send coodinate  
    void init_pose(double x, double y, double theta = 0.0)
    {
        tf2::Quaternion q;
        q.setRPY(0.0, 0.0, theta);

        geometry_msgs::msg::PoseWithCovarianceStamped pose_msg;
        pose_msg.header.stamp = this->now();
        pose_msg.header.frame_id = "map";

        pose_msg.pose.pose.position.x = x;
        pose_msg.pose.pose.position.y = y;
        pose_msg.pose.pose.position.z = 0.0;

        pose_msg.pose.pose.orientation = tf2::toMsg(q);

        RCLCPP_INFO(this->get_logger(), "Publishing Initial Position\n X: %f\n Y: %f", x, y);
        init_pose_pub_->publish(pose_msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr init_pose_pub_;
    rclcpp::Subscription<geometry_msgs::msg::PointStamped>::SharedPtr clicked_point_sub_;
    rclcpp::TimerBase::SharedPtr timer_;
    int trial_count_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<InitRobot>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
