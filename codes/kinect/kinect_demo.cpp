#include <iostream>
#include <vector>
#include <mutex>  // ✅ Include mutex for thread safety
#include <libfreenect.hpp>
#include <opencv2/opencv.hpp>

class KinectController : public Freenect::FreenectDevice {
private:
    std::vector<uint8_t> rgb_buffer;
    std::vector<uint16_t> depth_buffer;
    cv::Mat rgb_image, depth_image;
    std::mutex data_mutex;  // ✅ Define a mutex for thread safety
    bool new_rgb_frame = false;
    bool new_depth_frame = false;

public:
    KinectController(freenect_context* ctx, int index)
        : Freenect::FreenectDevice(ctx, index),
          rgb_buffer(640 * 480 * 3),
          depth_buffer(640 * 480) {}

    void VideoCallback(void* rgb, uint32_t timestamp) override {
        std::lock_guard<std::mutex> lock(data_mutex);  // ✅ Use the correct mutex name
        memcpy(rgb_buffer.data(), rgb, rgb_buffer.size());
        new_rgb_frame = true;
    }

    void DepthCallback(void* depth, uint32_t timestamp) override {
        std::lock_guard<std::mutex> lock(data_mutex);  // ✅ Use the correct mutex name
        memcpy(depth_buffer.data(), depth, depth_buffer.size() * sizeof(uint16_t));
        new_depth_frame = true;
    }

    bool getRGBD(cv::Mat &rgb, cv::Mat &depth) {
        std::lock_guard<std::mutex> lock(data_mutex);  // ✅ Use the correct mutex name
        if (!new_rgb_frame || !new_depth_frame) return false;

        // Convert RGB data to OpenCV Mat
        rgb = cv::Mat(480, 640, CV_8UC3, rgb_buffer.data()).clone();
        cv::cvtColor(rgb, rgb, cv::COLOR_RGB2BGR); // Convert RGB to BGR

        // Convert Depth data to OpenCV Mat
        depth = cv::Mat(480, 640, CV_16UC1, depth_buffer.data()).clone();

        new_rgb_frame = false;
        new_depth_frame = false;
        return true;
    }
};

int main() {
    Freenect::Freenect freenect;
    KinectController& kinect = freenect.createDevice<KinectController>(0);

    kinect.startVideo();
    kinect.startDepth();

    while (true) {
        cv::Mat rgb, depth;
        if (kinect.getRGBD(rgb, depth)) {
            // Normalize depth for visualization
            cv::Mat depth_display;
            depth.convertTo(depth_display, CV_8U, 255.0 / 4096.0);

            // Show images
            cv::imshow("Kinect RGB", rgb);
            cv::imshow("Kinect Depth", depth_display);
        }

        if (cv::waitKey(1) == 27) break; // Exit on 'ESC'
    }

    kinect.stopVideo();
    kinect.stopDepth();
    return 0;
}
