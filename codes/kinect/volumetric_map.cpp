#include <iostream>
#include <opencv2/opencv.hpp>
#include <libfreenect.hpp>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/octree/octree.h>
#include <pcl/visualization/cloud_viewer.h>

using namespace std;
using namespace cv;
using namespace pcl;

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

    // Initialize PCL Octree
    float resolution = 0.1f;  // 10 cm per voxel
    octree::OctreePointCloudSearch<PointXYZ> octree(resolution);
    PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);

    // PCL Visualizer
    pcl::visualization::CloudViewer viewer("3D Octree Viewer");

    while (!viewer.wasStopped()) {
        Mat depthFrame;

        if (device.getDepth(depthFrame)) {
            cloud->clear();

            // Convert Kinect Depth to 3D Points
            for (int y = 0; y < depthFrame.rows; y += 5) { // Downsampling for performance
                for (int x = 0; x < depthFrame.cols; x += 5) {
                    float depthValue = depthFrame.at<uint16_t>(y, x);

                    if (depthValue > 500 && depthValue < 5000) { // Ignore close and far points
                        float z = depthValue / 1000.0f; // Convert mm to meters
                        float x_real = (x - 320) * z * 0.00174; // Kinect scaling factor
                        float y_real = (y - 240) * z * 0.00174;

                        cloud->push_back(PointXYZ(x_real, y_real, z));
                    }
                }
            }

            // Update Octree Map
            octree.setInputCloud(cloud);
            octree.addPointsFromInputCloud();

            // Visualize the 3D Cloud
            viewer.showCloud(cloud);
        }

        if (waitKey(30) == 27) break; // Press ESC to exit
    }

    device.stopDepth();
    return 0;
}
