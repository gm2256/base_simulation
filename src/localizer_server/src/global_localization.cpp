#include <chrono>
#include <memory>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/empty.hpp"
#include "rcl_interfaces/srv/set_parameters.hpp"
#include "rcl_interfaces/msg/parameter.hpp"
#include "rcl_interfaces/msg/parameter_value.hpp"

using namespace std::chrono_literals;

class GlobalLocalization : public rclcpp::Node
{
public:
    GlobalLocalization()
    : Node("global_localization_node")
    {   // 2 client -- 1)spread particle in map of entire 2)adjust particle number  
        relocalization_client_ = this->create_client<std_srvs::srv::Empty>("reinitialize_global_localization");
        param_client_ = this->create_client<rcl_interfaces::srv::SetParameters>("/amcl/set_parameters");

        // Wait for services to be available
        while (!relocalization_client_->wait_for_service(1s) || !param_client_->wait_for_service(1s)) {
            RCLCPP_INFO(this->get_logger(), "Waiting for services to be available...");
        }
    }
     
    void set_amcl_params(int max_particles, int min_particles)
    {
        auto request = std::make_shared<rcl_interfaces::srv::SetParameters::Request>();

        rcl_interfaces::msg::Parameter param_max;
        param_max.name = "max_particles";
        param_max.value.type = rcl_interfaces::msg::ParameterType::PARAMETER_INTEGER;
        param_max.value.integer_value = max_particles;

        rcl_interfaces::msg::Parameter param_min;
        param_min.name = "min_particles";
        param_min.value.type = rcl_interfaces::msg::ParameterType::PARAMETER_INTEGER;
        param_min.value.integer_value = min_particles;

        request->parameters.push_back(param_max);
        request->parameters.push_back(param_min);

        auto future = param_client_->async_send_request(request,
            std::bind(&GlobalLocalization::param_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "max_particles: %d", max_particles);
        RCLCPP_INFO(this->get_logger(), "min_particles: %d", min_particles);
    }

    void reinitialize_global_localization()
    {
        auto request = std::make_shared<std_srvs::srv::Empty::Request>();
        auto future = relocalization_client_->async_send_request(request);

        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future) ==
            rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_INFO(this->get_logger(), "Global localization reinitialized successfully.");
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to call reinitialize_global_localization service.");
        }
    }

private:
    void param_callback(rclcpp::Client<rcl_interfaces::srv::SetParameters>::SharedFuture future)
    {
        try {
            auto response = future.get();
            RCLCPP_INFO(this->get_logger(), "Parameter set succeeded.");
        } catch (const std::exception &e) {
            RCLCPP_ERROR(this->get_logger(), "Parameter set failed: %s", e.what());
        }
    }

    rclcpp::Client<std_srvs::srv::Empty>::SharedPtr relocalization_client_;
    rclcpp::Client<rcl_interfaces::srv::SetParameters>::SharedPtr param_client_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<GlobalLocalization>();

    node->set_amcl_params(20000, 1000);
    // start once
    rclcpp::spin_some(node);

    node->reinitialize_global_localization();

    std::this_thread::sleep_for(std::chrono::seconds(7));

    node->set_amcl_params(2000, 500);
    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
