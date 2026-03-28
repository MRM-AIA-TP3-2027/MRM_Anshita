#include "rclcpp/rclcpp.hpp"
#include "cashier_system/srv/get_status.hpp"

using namespace std::chrono_literals;

class StatusNode : public rclcpp::Node {
public:
    StatusNode() : Node("status_node") {

        client_ = this->create_client<cashier_system::srv::GetStatus>("get_status");

        timer_ = this->create_wall_timer(
            5s,
            std::bind(&StatusNode::request_status, this)
        );

        RCLCPP_INFO(this->get_logger(), "Status Node Started");
    }

private:
    void request_status() {

        if (!client_->wait_for_service(1s)) {
            RCLCPP_WARN(this->get_logger(), "Waiting for service...");
            return;
        }

        auto request = std::make_shared<cashier_system::srv::GetStatus::Request>();

        client_->async_send_request(
            request,
            std::bind(&StatusNode::handle_response, this, std::placeholders::_1)
        );
    }

    void handle_response(
        rclcpp::Client<cashier_system::srv::GetStatus>::SharedFuture future)
    {
        auto response = future.get();

        RCLCPP_INFO(this->get_logger(), "------ STATUS ------");

        for (size_t i = 0; i < response->items.size(); i++) {
            RCLCPP_INFO(this->get_logger(), "%s : %d",
                        response->items[i].c_str(),
                        response->quantities[i]);
        }

        RCLCPP_INFO(this->get_logger(), "Total Income: %.2f",
                    response->total_income);
    }

    rclcpp::Client<cashier_system::srv::GetStatus>::SharedPtr client_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StatusNode>());
    rclcpp::shutdown();
    return 0;
}
