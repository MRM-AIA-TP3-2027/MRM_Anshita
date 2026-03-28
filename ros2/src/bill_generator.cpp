#include "rclcpp/rclcpp.hpp"
#include "cashier_system/msg/bill.hpp"

using namespace std::chrono_literals;

class BillGenerator : public rclcpp::Node {
public:
    BillGenerator() : Node("bill_generator") {
        publisher_ = this->create_publisher<cashier_system::msg::Bill>("bill_topic", 10);

        timer_ = this->create_wall_timer(
            5s, std::bind(&BillGenerator::publish_bill, this));
    }

private:
    void publish_bill() {
        auto msg = cashier_system::msg::Bill();

        std::string item;
        int quantity;
        float price;

        std::cout << "\nEnter item name: ";
        std::cin >> item;

        std::cout << "Enter quantity: ";
        std::cin >> quantity;

        std::cout << "Enter price per unit: ";
        std::cin >> price;

        msg.item_name = item;
        msg.quantity = quantity;
        msg.price_per_unit = price;

        RCLCPP_INFO(this->get_logger(), "Publishing bill: %s x %d", item.c_str(), quantity);

        publisher_->publish(msg);
    }

    rclcpp::Publisher<cashier_system::msg::Bill>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BillGenerator>());
    rclcpp::shutdown();
    return 0;
}
