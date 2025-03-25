#include <iostream>
#include <libfreenect.hpp>

class KinectController {
private:
    freenect_context *f_ctx;
    freenect_device *f_dev;

public:
    KinectController() {
        // Initialize Freenect
        if (freenect_init(&f_ctx, NULL) < 0) {
            std::cerr << "Failed to initialize freenect!" << std::endl;
            exit(1);
        }

        // Open Kinect Device
        if (freenect_open_device(f_ctx, &f_dev, 0) < 0) {
            std::cerr << "Failed to open Kinect device!" << std::endl;
            freenect_shutdown(f_ctx);
            exit(1);
        }
    }

    ~KinectController() {
        freenect_close_device(f_dev);
        freenect_shutdown(f_ctx);
    }

    // Set Motor Tilt Angle (-30° to 30°)
    void setTilt(int angle) {
        if (angle < -30) angle = -30;
        if (angle > 30) angle = 30;
        freenect_set_tilt_degs(f_dev, angle);
        std::cout << "Motor set to: " << angle << "°" << std::endl;
    }

    // Set Kinect LED Color
    void setLED(int mode) {
        freenect_set_led(f_dev, static_cast<freenect_led_options>(mode));
        std::cout << "LED mode set to: " << mode << std::endl;
    }
};

int main() {
    KinectController kinect;

    int choice;
    while (true) {
        std::cout << "\n1. Move Motor Up\n2. Move Motor Down\n3. Set LED Color\n4. Exit\nChoose an option: ";
        std::cin >> choice;

        if (choice == 1) {
            kinect.setTilt(15); // Move up
        } else if (choice == 2) {
            kinect.setTilt(-15); // Move down
        } else if (choice == 3) {
            std::cout << "LED Modes:\n 0 = Off\n 1 = Green\n 2 = Red\n 3 = Yellow\n 4 = Blink Green\n 5 = Blink Red-Yellow\nChoose LED mode: ";
            int mode;
            std::cin >> mode;
            kinect.setLED(mode);
        } else if (choice == 4) {
            break;
        } else {
            std::cout << "Invalid option. Try again!" << std::endl;
        }
    }

    return 0;
}
