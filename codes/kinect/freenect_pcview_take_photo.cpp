#include <iostream>
#include <libfreenect.hpp>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>

using namespace std;
using namespace cv;
using namespace std::chrono;

// Kinect device class
class MyFreenectDevice : public Freenect::FreenectDevice {
public:
    MyFreenectDevice(freenect_context* ctx, int index)
        : Freenect::FreenectDevice(ctx, index), m_newDepthFrame(false) {}

    void DepthCallback(void* _depth, uint32_t timestamp) override {
        std::unique_lock<std::mutex> lock(m_depthMutex);
        Mat temp(Size(640, 480), CV_16UC1, _depth);
        temp.copyTo(depthMat);
        m_newDepthFrame = true;
    }

    bool getDepth(Mat &output) {
        std::unique_lock<std::mutex> lock(m_depthMutex);
        if (m_newDepthFrame) {
            depthMat.copyTo(output);
            m_newDepthFrame = false;
            return true;
        }
        return false;
    }

private:
    Mat depthMat;
    bool m_newDepthFrame;
    std::mutex m_depthMutex;
};

int main() {
    // Create Kinect device
    Freenect::Freenect freenect;
    MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
    device.startDepth();

    // Get the start time to handle the 10-second intervals
    auto start_time = high_resolution_clock::now();

    // OpenCV window (optional, for showing live feed)
    namedWindow("Kinect Depth Frame", WINDOW_NORMAL);

    int photo_counter = 0;

    cout << "Press ESC to exit..." << endl;
    
    while (true) {
        Mat depthFrame;
        
        if (device.getDepth(depthFrame)) {
            // Convert depth data to a visualizable format (e.g., scaled to 8-bit for display)
            Mat displayFrame;
            depthFrame.convertTo(displayFrame, CV_8UC1, 255.0 / 2048.0);  // Normalize depth values for display

            // Show live depth image
            imshow("Kinect Depth Frame", displayFrame);

            // Check if it's time to save a photo (every 10 seconds)
            auto current_time = high_resolution_clock::now();
            auto duration = duration_cast<seconds>(current_time - start_time).count();

            if (duration >= 10) {
                // Save the current depth frame as an image file
                string filename = "depth_photo_" + to_string(photo_counter++) + ".png";
                imwrite(filename, displayFrame);  // Save the depth image as a PNG file
                cout << "Saved photo: " << filename << endl;

                // Reset the timer
                start_time = high_resolution_clock::now();
            }
        }

        // Exit the program if the user presses ESC
        if (waitKey(1) == 27) {
            break;
        }

        // Sleep for a while to avoid overloading the CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    device.stopDepth();
    destroyAllWindows();
    return 0;
}
