#include <iostream>
#include <libfreenect.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <curl/curl.h>    // For HTTP requests
#include <jsoncpp/json/json.h>  // For JSON parsing

//g++ kinect_volumetric_map.cpp -o kinect_volumetric_map     -I/usr/include/opencv4     -I/usr/include/pcl-1.12     -I/usr/include/libusb-1.0     -I/usr/include/eigen3     -L/usr/lib/x86_64-linux-gnu     -L/usr/local/lib     `pkg-config --cflags --libs opencv4`     -lfreenect -lpcl_common -lpcl_io -lpcl_visualization -lcurl -ljsoncpp


#define WIDTH 640
#define HEIGHT 480

const int mapWidth = 10; // Grid width (10m area)
const int mapHeight = 10; // Grid height (10m area)
int occupancyGrid[mapWidth][mapHeight]; // 2D occupancy grid (1 = occupied, 0 = free)

bool rotate = false;  // Rotate flag
int value = 0;        // Value from the API

std::string responseBuffer; // This will hold the response from the API

// Callback function to handle the response data from the API
size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realSize = size * nmemb;
    responseBuffer.append((char*)contents, realSize);
    return realSize;
}

// Function to process the depth data from Kinect
void depthCallback(freenect_device *dev, void *depth_data, uint32_t timestamp)
{
    unsigned short *depth = (unsigned short*)depth_data;

    // Clear the grid each time (you can also choose to update based on previous data)
    for (int i = 0; i < mapWidth; i++) {
        for (int j = 0; j < mapHeight; j++) {
            occupancyGrid[i][j] = 0; // Mark as free by default
        }
    }

    // Map Kinect depth data to occupancy grid
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // Convert Kinect's pixel (x, y) to map grid (i, j) coordinates
            int gridX = x * mapWidth / WIDTH;
            int gridY = y * mapHeight / HEIGHT;

            // Get the depth value (distance in mm)
            unsigned short d = depth[y * WIDTH + x];

            // If the distance is less than 1000 mm (1 meter), mark the cell as occupied
            if (d < 1000) {
                occupancyGrid[gridX][gridY] = 1; // Occupied (obstacle)
            }
        }
    }
}

// Function to initialize Kinect and start streaming data
void startKinect()
{
    // Initialize Kinect
    freenect_context *f_ctx;
    freenect_device *f_dev;
    if (freenect_init(&f_ctx, nullptr) < 0) {
        std::cerr << "Error initializing Kinect" << std::endl;
        return;
    }

    // Open Kinect device
    if (freenect_open_device(f_ctx, &f_dev, 0) < 0) {
        std::cerr << "Error opening Kinect device" << std::endl;
        freenect_shutdown(f_ctx);
        return;
    }

    // Set Kinect depth callback
    freenect_set_depth_callback(f_dev, depthCallback);
    freenect_start_depth(f_dev);

    // Run Kinect in a loop
    while (true) {
        freenect_process_events(f_ctx);
    }

    // Clean up
    freenect_close_device(f_dev);
    freenect_shutdown(f_ctx);
}

// Function to fetch data from the API (GET request)
bool fetchAPIData()
{
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Curl initialization failed" << std::endl;
        return false;
    }

    // Define the API endpoint URL
    const char *api_url = "https://luis.madlab.mx/app/slider";  // Replace with your actual API URL
    curl_easy_setopt(curl, CURLOPT_URL, api_url);
    
    // Set up the response callback to capture JSON data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    
    // Perform the GET request
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Curl request failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    // Parse the JSON response
    Json::Reader reader;
    Json::Value root;
    std::string response = responseBuffer;

    if (!reader.parse(response, root)) {
        std::cerr << "Failed to parse the API response" << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    // Get the values of 'rotate' and 'value'
    rotate = root["rotate"].asBool();
    value = root["value"].asInt();

    // Clean up the curl object
    curl_easy_cleanup(curl);

    return true;
}

// Function to take a photo using OpenCV
void takePhotoAndSave()
{
    cv::Mat depthImage = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC1); // Create a black image
    // Capture a frame from the Kinect (just using a dummy image here)
    // You can replace this with actual image capture from the Kinect

    cv::imwrite("photo.jpg", depthImage);  // Save photo as "photo.jpg"
    std::cout << "Photo taken and saved as photo.jpg" << std::endl;
}

// Function to upload the photo to Flask API
void uploadPhotoToAPI(const std::string &filename)
{
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Curl initialization failed" << std::endl;
        return;
    }

    // Define the Flask API endpoint for uploading photos
    const char *upload_url = "https://luis.madlab.mx/app/upload";  // Replace with your actual upload API URL

    // Prepare the file for upload
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file) {
        std::cerr << "Failed to open file for uploading" << std::endl;
        curl_easy_cleanup(curl);
        return;
    }

    // Set up the multipart form data for uploading
    curl_mime *mime = curl_mime_init(curl);
    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_name(part, "file");
    curl_mime_filedata(part, filename.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, upload_url);
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    // Perform the POST request to upload the photo
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Curl upload failed: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "File uploaded successfully!" << std::endl;
    }

    // Clean up
    fclose(file);
    curl_easy_cleanup(curl);
    curl_mime_free(mime);
}

// Main function
int main()
{
    // Start Kinect sensor in a separate thread
    std::thread kinectThread(startKinect);

    // OpenCV window to visualize the occupancy grid
    cv::Mat gridImage(mapHeight, mapWidth, CV_8UC1); // Black & white image for the grid
    cv::namedWindow("Occupancy Grid", cv::WINDOW_NORMAL);

    // Run the program continuously
    while (true) {
        // Fetch the data from the API
        if (fetchAPIData()) {
            std::cout << "Rotate: " << rotate << ", Value: " << value << std::endl;

            // If rotate is true, take a photo and upload it
            if (rotate) {
                takePhotoAndSave();
                uploadPhotoToAPI("photo.jpg");
            }
        }

        // Update the occupancy grid display
        for (int i = 0; i < mapWidth; i++) {
            for (int j = 0; j < mapHeight; j++) {
                if (occupancyGrid[i][j] == 1) {
                    gridImage.at<uchar>(j, i) = 255;
                } else {
                    gridImage.at<uchar>(j, i) = 0;
                }
            }
        }

        // Show the occupancy grid
        cv::imshow("Occupancy Grid", gridImage);

        // Exit on key press
        if (cv::waitKey(10) == 27) {
            break;
        }
    }

    kinectThread.join(); // Join Kinect thread after exiting

    return 0;
}
