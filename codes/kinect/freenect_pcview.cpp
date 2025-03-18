#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <libfreenect.hpp>

using namespace std;
using namespace cv;

// Custom Kinect device class using libfreenect's C++ API
class MyFreenectDevice : public Freenect::FreenectDevice {
public:
    MyFreenectDevice(freenect_context* ctx, int index)
      : Freenect::FreenectDevice(ctx, index),
        m_bufferVideo(640 * 480 * 3),
        m_bufferDepth(640 * 480),
        m_rgbMat(Size(640,480), CV_8UC3, m_bufferVideo.data()),
        m_depthMat(Size(640,480), CV_16UC1, m_bufferDepth.data()),
        m_newRgbFrame(false),
        m_newDepthFrame(false)
    { }

    // Called when a new RGB frame is available.
    void VideoCallback(void* rgb, uint32_t timestamp) override {
        std::unique_lock<std::mutex> lock(m_rgbMutex);
        uint8_t* ptr = static_cast<uint8_t*>(rgb);
        std::copy(ptr, ptr + m_bufferVideo.size(), m_bufferVideo.begin());
        m_newRgbFrame = true;
    }

    // Called when a new depth frame is available.
    void DepthCallback(void* depth, uint32_t timestamp) override {
        std::unique_lock<std::mutex> lock(m_depthMutex);
        uint16_t* ptr = static_cast<uint16_t*>(depth);
        std::copy(ptr, ptr + m_bufferDepth.size(), m_bufferDepth.begin());
        m_newDepthFrame = true;
    }

    bool getRGB(Mat& output) {
        std::unique_lock<std::mutex> lock(m_rgbMutex);
        if(m_newRgbFrame){
            m_rgbMat.copyTo(output);
            m_newRgbFrame = false;
            return true;
        }
        return false;
    }

    bool getDepth(Mat& output) {
        std::unique_lock<std::mutex> lock(m_depthMutex);
        if(m_newDepthFrame){
            m_depthMat.copyTo(output);
            m_newDepthFrame = false;
            return true;
        }
        return false;
    }

private:
    std::vector<uint8_t> m_bufferVideo;
    std::vector<uint16_t> m_bufferDepth;
    Mat m_rgbMat;
    Mat m_depthMat;
    bool m_newRgbFrame;
    bool m_newDepthFrame;
    std::mutex m_rgbMutex;
    std::mutex m_depthMutex;
};

// Dummy motor control functions – replace these with your own motor-control code.
void moveForward() {
    cout << "Motor Command: Move Forward" << endl;
}
void moveBackward() {
    cout << "Motor Command: Move Backward" << endl;
}
void turnLeft() {
    cout << "Motor Command: Turn Left" << endl;
}
void turnRight() {
    cout << "Motor Command: Turn Right" << endl;
}

int main() {
    Freenect::Freenect freenect;
    MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

    // Start the Kinect video and depth streams.
    device.startVideo();
    device.startDepth();

    while(true) {
        Mat rgbFrame, depthFrame;
        
        // Retrieve and display the RGB frame.
        if(device.getRGB(rgbFrame)) {
            imshow("RGB", rgbFrame);
        }
        
        // Retrieve and display the depth frame (converted to 8-bit for visualization).
        if(device.getDepth(depthFrame)) {
            Mat depth8;
            depthFrame.convertTo(depth8, CV_8U, 255.0/2048.0);
            imshow("Depth", depth8);
        }
        
        // Check for key input. waitKey returns an integer code.
        int key = waitKey(30);
        if(key == 27)  // ESC key exits the loop.
            break;
        
        // Handle arrow keys (the numeric codes below are common on many systems).
        switch(key) {
	  case 65362: // Up arrow
                moveForward();
                break;
            case 65364: // Down arrow
                moveBackward();
                break;
            case 65361: // Left arrow
                turnLeft();
                break;
            case 65363: // Right arrow
                turnRight();
                break;
            default:
                break;
		
        }
    }

    device.stopVideo();
    device.stopDepth();

    return 0;
}
