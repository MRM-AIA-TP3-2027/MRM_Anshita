#include "cashier_system/srv/get_status.hpp"
#include "rclcpp/rclcpp.hpp"
#include "cashier_system/msg/bill.hpp"
#include <map>

using std::placeholders::_1;

class InventoryNode : public rclcpp::Node {
public:
    InventoryNode() : Node("inventory_node"), total_income_(0.0) {

        subscription_ = this->create_subscription<cashier_system::msg::Bill>(
            "bill_topic", 10,
            std::bind(&InventoryNode::bill_callback, this, _1)
        );
        
        service_=this->create_service<cashier_system::srv::GetStatus>(
            "get_status",
            std::bind(&InventoryNode::handle_service,this,
            	      std::placeholders::_1, std::placeholders::_2)
        );

        RCLCPP_INFO(this->get_logger(), "Inventory Node Started");
    }

private:
	
    void bill_callback(const cashier_system::msg::Bill::SharedPtr msg) {
        std::string item = msg->item_name;
        int quantity = msg->quantity;
        float price = msg->price_per_unit;

        // Update inventory
        inventory_[item] += quantity;

        // Update total income
        total_income_ += quantity * price;

        // Print updated info
        RCLCPP_INFO(this->get_logger(),
            "Received: %s x %d | Total Income: %.2f",
            item.c_str(), quantity, total_income_);

        print_inventory();
    }

    void print_inventory() {
        RCLCPP_INFO(this->get_logger(), "Current Inventory:");

        for (auto &pair : inventory_) {
            RCLCPP_INFO(this->get_logger(), "%s : %d",
                        pair.first.c_str(), pair.second);
        }
    }
    
    void handle_service(
    const std::shared_ptr<cashier_system::srv::GetStatus::Request> request,
    std::shared_ptr<cashier_system::srv::GetStatus::Response> response)
{
    (void)request;

    for (auto &pair : inventory_) {
        response->items.push_back(pair.first);
        response->quantities.push_back(pair.second);
    }

    response->total_income = total_income_;
}

    rclcpp::Subscription<cashier_system::msg::Bill>::SharedPtr subscription_;

    rclcpp::Service<cashier_system::srv::GetStatus>::SharedPtr service_;

    std::map<std::string, int> inventory_;
    float total_income_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<InventoryNode>());
    rclcpp::shutdown();
    return 0;
}
