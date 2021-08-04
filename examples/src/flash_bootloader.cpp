#include <chrono>
#include <string>

#include "depthai/depthai.hpp"

int main(int argc, char** argv) {
    using namespace std::chrono;

    dai::DeviceBootloader::Type blType = dai::DeviceBootloader::Type::USB;
    if(argc > 1) {
        std::string blTypeStr(argv[1]);
        if(blTypeStr == "usb") {
            blType = dai::DeviceBootloader::Type::USB;
        } else if(blTypeStr == "network") {
            blType = dai::DeviceBootloader::Type::NETWORK;
        } else {
            std::cout << "Specify either 'usb' or 'network' bootloader type\n";
            return 0;
        }
    } else {
        std::cout << "Usage: " << argv[0] << " <usb/network>\n";
        return 0;
    }

    std::cout << "Warning! Flashing bootloader can potentially soft brick your device and should be done with caution." << std::endl;
    std::cout << "Do not unplug your device while the bootloader is flashing." << std::endl;
    std::cout << "Type 'y' and press enter to proceed, otherwise exits: ";
    if(std::cin.get() != 'y') {
        std::cout << "Prompt declined, exiting..." << std::endl;
        return -1;
    }

    bool found = false;
    dai::DeviceInfo info;
    std::tie(found, info) = dai::DeviceBootloader::getFirstAvailableDevice();
    if(!found) {
        std::cout << "No device found to flash. Exiting." << std::endl;
        return -1;
    }

    // Open DeviceBootloader and allow flashing bootloader
    dai::DeviceBootloader bl(info, true);
    auto currentBlType = bl.getType();

    // Check if bootloader type is the same
    if(currentBlType != blType) {
        std::cout << "Are you sure you want to flash '" << blType << "' bootloader over current '" << currentBlType << "' bootloader?" << std::endl;
        std::cout << "Type 'y' and press enter to proceed, otherwise exits: ";
        std::cin.ignore();
        if(std::cin.get() != 'y') {
            std::cout << "Prompt declined, exiting..." << std::endl;
            return -1;
        }
    }

    // Create a progress callback lambda
    auto progress = [](float p) { std::cout << "Flashing Progress..." << p * 100 << "%" << std::endl; };

    std::cout << "Flashing " << blType << " bootloader..." << std::endl;
    auto t1 = steady_clock::now();
    bool success = false;
    std::string message;
    std::tie(success, message) = bl.flashBootloader(dai::DeviceBootloader::Memory::FLASH, blType, progress);
    if(success) {
        std::cout << "Flashing successful. Took " << duration_cast<milliseconds>(steady_clock::now() - t1).count() << "ms" << std::endl;
    } else {
        std::cout << "Flashing failed: " << message << std::endl;
    }
    return 0;
}