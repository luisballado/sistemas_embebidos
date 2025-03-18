#include <iostream>
#include <opencv2/opencv.hpp>
#include <libfreenect.hpp>

//g++ my_kinect.cpp -o my_kinect `pkg-config --cflags --libs opencv4` -lfreenect -I/usr/include/libusb-1.0 -lusb-1.0


using namespace cv;
using namespace std;

// Kinect Class
class MyFreenectDevice : public Freenect::FreenectDevice {
public:
    MyFreenectDevice(freenect_context* ctx, int index)
        : Freenect::FreenectDevice(ctx, index), depthMat(Size(640, 480), CV_16UC1),
          rgbMat(Size(640, 480), CV_8UC3), newDepthFrame(false), newRgbFrame(false) {}

    void VideoCallback(void* rgb, uint32_t timestamp) override {
        Mat temp(Size(640, 480), CV_8UC3, rgb);
        cvtColor(temp, rgbMat, COLOR_RGB2BGR);
        newRgbFrame = true;
    }

    void DepthCallback(void* depth, uint32_t timestamp) override {
        Mat temp(Size(640, 480), CV_16UC1, depth);
        temp.copyTo(depthMat);
        newDepthFrame = true;
    }

    bool getRGB(Mat& output) {
        if (newRgbFrame) {
            rgbMat.copyTo(output);
            newRgbFrame = false;
            return true;
        }
        return false;
    }

    bool getDepth(Mat& output) {
        if (newDepthFrame) {
            depthMat.convertTo(output, CV_8U, 255.0 / 2048.0);
            newDepthFrame = false;
            return true;
        }
        return false;
    }

private:
    Mat depthMat;
    Mat rgbMat;
    bool newDepthFrame, newRgbFrame;
};

int main() {
    Freenect::Freenect freenect;
    MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

    device.startVideo();
    device.startDepth();

    while (true) {
        Mat rgbFrame, depthFrame;
        
        if (device.getRGB(rgbFrame) && device.getDepth(depthFrame)) {
            imshow("Kinect RGB", rgbFrame);
            imshow("Kinect Depth", depthFrame);
        }

        if (waitKey(30) == 27) // Exit on ESC key
            break;
    }

    device.stopVideo();
    device.stopDepth();
    return 0;
}
