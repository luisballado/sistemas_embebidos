#include <iostream>
#include <vector>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <libfreenect.hpp>

// PCL headers
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/octree/octree_pointcloud_pointvector.h>
#include <pcl/visualization/cloud_viewer.h>

using namespace std;
using namespace cv;

// Custom Kinect device class (depth only)
class MyFreenectDevice : public Freenect::FreenectDevice {
public:
    MyFreenectDevice(freenect_context* ctx, int index)
      : Freenect::FreenectDevice(ctx, index),
        depthMat(Size(640,480), CV_16UC1),
        m_newDepthFrame(false)
    { }

    // Depth callback invoked by libfreenect
    void DepthCallback(void* _depth, uint32_t timestamp) override {
        std::unique_lock<std::mutex> lock(m_depthMutex);
        Mat temp(Size(640,480), CV_16UC1, _depth);
        temp.copyTo(depthMat);
        m_newDepthFrame = true;
    }

    // Retrieve the most recent depth frame
    bool getDepth(Mat &output) {
        std::unique_lock<std::mutex> lock(m_depthMutex);
        if(m_newDepthFrame) {
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

    // Create a global point cloud (for accumulating data)
    typedef pcl::PointXYZ PointT;
    pcl::PointCloud<PointT>::Ptr globalCloud(new pcl::PointCloud<PointT>);

    // Create an octree for volumetric mapping (resolution: 10 cm)
    float resolution = 0.1f;
    pcl::octree::OctreePointCloudPointVector<PointT> octree(resolution);

    // PCL visualizer (will display the accumulated map)
    pcl::visualization::CloudViewer viewer("Volumetric Map (SLAM)");

    // Kinect intrinsic parameters (approximation)
    const float fx = 525.0f, fy = 525.0f;
    const float cx = 320.0f, cy = 240.0f;

    cout << "Press ESC in the OpenCV window to exit." << endl;

    while(!viewer.wasStopped()) {
        Mat depthFrame;
        if(device.getDepth(depthFrame)) {
            // Create a point cloud for the current frame (downsample for performance)
            pcl::PointCloud<PointT>::Ptr frameCloud(new pcl::PointCloud<PointT>);
            for (int y = 0; y < depthFrame.rows; y += 4) {
                for (int x = 0; x < depthFrame.cols; x += 4) {
                    uint16_t d = depthFrame.at<uint16_t>(y, x);
                    if(d == 0) continue; // skip invalid measurements
                    float depth = d / 1000.0f; // convert mm to meters
                    // Consider only valid depth range (e.g., 0.5 m to 5.0 m)
                    if(depth < 0.5f || depth > 5.0f)
                        continue;
                    PointT pt;
                    pt.z = depth;
                    pt.x = (x - cx) * depth / fx;
                    pt.y = (y - cy) * depth / fy;
                    frameCloud->push_back(pt);
                }
            }

            // Accumulate the current frame into the global map.
            // (In a real SLAM system, you'd transform the frameCloud according to robot pose.)
            *globalCloud += *frameCloud;

            // Update the octree map: clear previous octree and add the new cloud
            octree.deleteTree();
            octree.setInputCloud(globalCloud);
            octree.addPointsFromInputCloud();

            // Update the visualizer with the current global point cloud.
            viewer.showCloud(globalCloud);
        }

        // Exit if the user presses ESC in an OpenCV window (or using viewer.wasStopped())
        if (waitKey(30) == 27) {
            break;
        }
    }

    device.stopDepth();
    return 0;
}
