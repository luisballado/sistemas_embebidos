#include <iostream>
#include <vector>
#include <mutex>
#include <map>
#include <cmath>
#include <libfreenect.hpp>
#include <opencv2/opencv.hpp>
#include <open3d/Open3D.h>

// Kinect controller class using libfreenect
class KinectController : public Freenect::FreenectDevice {
private:
    std::vector<uint8_t> rgb_buffer;
    std::vector<uint16_t> depth_buffer;
    std::mutex data_mutex;
    bool new_rgb_frame = false;
    bool new_depth_frame = false;

public:
    KinectController(freenect_context* ctx, int index)
        : Freenect::FreenectDevice(ctx, index),
          rgb_buffer(640 * 480 * 3),
          depth_buffer(640 * 480) {}

    // Called when a new RGB frame is available.
    void VideoCallback(void* rgb, uint32_t timestamp) override {
        std::lock_guard<std::mutex> lock(data_mutex);
        memcpy(rgb_buffer.data(), rgb, rgb_buffer.size());
        new_rgb_frame = true;
    }

    // Called when a new Depth frame is available.
    void DepthCallback(void* depth, uint32_t timestamp) override {
        std::lock_guard<std::mutex> lock(data_mutex);
        memcpy(depth_buffer.data(), depth, depth_buffer.size() * sizeof(uint16_t));
        new_depth_frame = true;
    }

    // Retrieve the latest RGB and Depth images as OpenCV Mats.
    bool getRGBD(cv::Mat &rgb, cv::Mat &depth) {
        std::lock_guard<std::mutex> lock(data_mutex);
        if (!new_rgb_frame || !new_depth_frame) return false;

        rgb = cv::Mat(480, 640, CV_8UC3, rgb_buffer.data()).clone();
        cv::cvtColor(rgb, rgb, cv::COLOR_RGB2BGR);
        depth = cv::Mat(480, 640, CV_16UC1, depth_buffer.data()).clone();

        new_rgb_frame = false;
        new_depth_frame = false;
        return true;
    }
};

// Convert Depth image to a 3D point cloud (vector of Eigen::Vector3d)
std::vector<Eigen::Vector3d> depthToPointCloud(cv::Mat &depth,
                                               float fx = 525.0, float fy = 525.0,
                                               float cx = 319.5, float cy = 239.5) {
    std::vector<Eigen::Vector3d> points;
    for (int y = 0; y < depth.rows; y++) {
        for (int x = 0; x < depth.cols; x++) {
            // Convert depth from millimeters to meters
            float d = depth.at<uint16_t>(y, x) / 1000.0f;
            if (d > 0.1f && d < 5.0f) {  // Filter out invalid depth values
                float X = (x - cx) * d / fx;
                float Y = (y - cy) * d / fy;
                float Z = d;
                points.push_back(Eigen::Vector3d(X, Y, Z));
            }
        }
    }
    return points;
}

// Group point cloud into voxels with a specified voxel size.
// We use a map keyed by voxel indices (Eigen::Vector3i). Then, for each occupied voxel, we
// compute its 3D position as voxel_index cast to double multiplied by voxel_size.
std::vector<std::shared_ptr<open3d::geometry::Voxel>> pointCloudToVoxels(std::vector<Eigen::Vector3d> &points, double voxel_size = 1.0) {
    std::map<Eigen::Vector3i, bool, std::less<Eigen::Vector3i>> voxel_map;
    for (const auto &p : points) {
        // Compute voxel index using rounding (explicitly cast to int)
        Eigen::Vector3i voxel_idx(
            static_cast<int>(std::round(p[0] / voxel_size)),
            static_cast<int>(std::round(p[1] / voxel_size)),
            static_cast<int>(std::round(p[2] / voxel_size))
        );
        voxel_map[voxel_idx] = true;
    }

    std::vector<std::shared_ptr<open3d::geometry::Voxel>> voxels;
    for (const auto &voxel : voxel_map) {
        // Convert voxel index to 3D position (as double) and multiply by voxel size.
        Eigen::Vector3d voxel_pos = voxel.first.cast<double>() * voxel_size;
        auto voxel_ptr = std::make_shared<open3d::geometry::Voxel>(voxel_pos);
        voxels.push_back(voxel_ptr);
    }

    return voxels;
}

int main() {
    // Initialize Kinect
    Freenect::Freenect freenect;
    KinectController &kinect = freenect.createDevice<KinectController>(0);
    kinect.startVideo();
    kinect.startDepth();

    // Create an Open3D VoxelGrid to store voxels.
    auto voxel_grid = std::make_shared<open3d::geometry::VoxelGrid>();
    voxel_grid->voxel_size_ = 1.0;  // 1 m voxel resolution

    // Create an Open3D Visualizer and add the voxel grid.
    auto vis = std::make_shared<open3d::visualization::Visualizer>();
    vis->CreateVisualizerWindow("Voxelized Kinect", 800, 600);
    vis->AddGeometry(voxel_grid);

    while (true) {
        cv::Mat rgb, depth;
        if (kinect.getRGBD(rgb, depth)) {
            // Convert depth image to point cloud.
            std::vector<Eigen::Vector3d> points = depthToPointCloud(depth);

            // Convert point cloud to voxels.
            std::vector<std::shared_ptr<open3d::geometry::Voxel>> voxels = pointCloudToVoxels(points);

            // Clear and update the voxel grid.
            voxel_grid->voxels_.clear();
            for (const auto &voxel : voxels) {
                voxel_grid->voxels_.push_back(*voxel);
            }

            // Update the visualizer with the new voxel grid.
            vis->UpdateGeometry();
            vis->PollEvents();
            vis->UpdateRender();

            // Optionally, show the RGB image using OpenCV.
            cv::imshow("Kinect RGB", rgb);
        }

        if (cv::waitKey(1) == 27) break; // Exit on 'ESC'
    }

    vis->DestroyVisualizerWindow();
    kinect.stopVideo();
    kinect.stopDepth();
    return 0;
}
