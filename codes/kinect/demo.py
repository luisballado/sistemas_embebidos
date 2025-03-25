import numpy as np
import open3d as o3d
import freenect
import cv2

# Kinect v1 Intrinsics (Focal Length & Optical Center)
FX, FY = 525.0, 525.0
CX, CY = 319.5, 239.5

def get_kinect_rgb():
    """Get RGB image from Kinect"""
    rgb_frame, _ = freenect.sync_get_video()
    return cv2.cvtColor(rgb_frame, cv2.COLOR_RGB2BGR)  # Convert to OpenCV format

def get_kinect_depth():
    """Get depth image from Kinect"""
    depth_frame, _ = freenect.sync_get_depth(format=freenect.DEPTH_MM)
    return depth_frame.astype(np.float32) / 1000.0  # Convert mm to meters

def create_point_cloud(depth_image, color_image):
    """Generate a point cloud from depth and RGB images"""
    rows, cols = depth_image.shape
    x, y = np.meshgrid(np.arange(cols), np.arange(rows))
    z = depth_image

    # Convert depth to 3D points
    x = (x - CX) * z / FX
    y = (y - CY) * z / FY

    valid = (z > 0) & (z < 5)  # Remove invalid points (distance range: 0-5m)
    points = np.vstack((x[valid], y[valid], z[valid])).T
    colors = color_image.reshape(-1, 3)[valid.ravel()] / 255.0  # Normalize colors

    # Convert to Open3D PointCloud
    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(points)
    pcd.colors = o3d.utility.Vector3dVector(colors)

    return pcd

# Open3D Visualization Setup
vis = o3d.visualization.Visualizer()
vis.create_window()

while True:
    # Get images from Kinect
    color = get_kinect_rgb()
    depth = get_kinect_depth()

    # Generate point cloud
    pcd = create_point_cloud(depth, color)

    # Update visualization
    vis.clear_geometries()
    vis.add_geometry(pcd)
    vis.poll_events()
    vis.update_renderer()

    # Show RGB image
    cv2.imshow("Kinect RGB", color)

    # Exit on 'q' key
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

vis.destroy_window()
cv2.destroyAllWindows()
