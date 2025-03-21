#include <iostream>
#include <opencv2/opencv.hpp>
#include <libfreenect.hpp>

using namespace cv;
using namespace std;

// Kinect Device Wrapper
class MyFreenectDevice : public Freenect::FreenectDevice {
public:
    MyFreenectDevice(freenect_context* ctx, int index)
        : Freenect::FreenectDevice(ctx, index), depthMat(Size(640, 480), CV_16UC1),
          newDepthFrame(false) {}

    void DepthCallback(void* depth, uint32_t timestamp) override {
        Mat temp(Size(640, 480), CV_16UC1, depth);
        temp.copyTo(depthMat);
        newDepthFrame = true;
    }

    bool getDepth(Mat& output) {
        if (newDepthFrame) {
            depthMat.copyTo(output);
            newDepthFrame = false;
            return true;
        }
        return false;
    }

private:
    Mat depthMat;
    bool newDepthFrame;
};

int main() {
    Freenect::Freenect freenect;
    MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

    device.startDepth();

    const int voxelSize = 60; // Size of "3D" voxel blocks (bigger = more pixelated effect)

    while (true) {
        Mat depthFrame, voxelFrame = Mat::zeros(480, 640, CV_8UC3);

        if (device.getDepth(depthFrame)) {
            for (int y = 0; y < depthFrame.rows; y += voxelSize) {
                for (int x = 0; x < depthFrame.cols; x += voxelSize) {
                    int depthValue = depthFrame.at<uint16_t>(y, x);

                    // Only draw voxels for specific depth intervals (10 cm steps)
                    if (depthValue > 500 && depthValue % 100 == 0) {
                        int brightness = 255 - min(depthValue / 8, 255); // Simulate depth shading
                        rectangle(voxelFrame, Point(x, y), Point(x + voxelSize, y + voxelSize),
                                  Scalar(brightness, brightness, brightness), FILLED);
                    }
                }
            }

            imshow("3D Voxel Depth View", voxelFrame);
        }

        if (waitKey(30) == 27) // Exit on ESC key
            break;
    }

    device.stopDepth();
    return 0;
}
