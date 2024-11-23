#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <map>

using namespace cv;

int main() {
    VideoCapture cap("D:/projects/Traffic-statistics/5jcg5vfx58-3/Videos 1/1/1.avi");
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open video file." << std::endl;
        return -1;
    }

    // Load DNN model for vehicle detection
    dnn::Net net = dnn::readNetFromDarknet("D:/projects/Traffic-statistics/yolov3.cfg",
                                           "D:/projects/Traffic-statistics/yolov3.weights");

    // Statistics variables
    std::map<std::string, int> vehicleCounts; // {"Car", "Bus", "Truck", "Motorcycle", "Person"}
    std::map<std::string, int> regionCounts;  // {"E", "N", "S", "W"}

    // Define colors for bounding boxes
    Scalar carColor(0, 255, 0), busColor(0, 0, 255), truckColor(255, 0, 0), motorcycleColor(255, 165, 0), personColor(255, 255, 0);

    // Create a window for displaying the video
    const int windowWidth = 800;
    const int windowHeight = 600;
    namedWindow("Traffic Monitoring", WINDOW_NORMAL);
    resizeWindow("Traffic Monitoring", windowWidth, windowHeight);

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        Mat resizedFrame;
        float resizeFactor = std::min(float(windowWidth) / frame.cols, float(windowHeight) / frame.rows);
        resize(frame, resizedFrame, Size(), resizeFactor, resizeFactor);

        Mat blob = dnn::blobFromImage(resizedFrame, 1 / 255.0, Size(416, 416), Scalar(0, 0, 0), true, false);
        net.setInput(blob);

        std::vector<Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        // Reset vehicle and region counts for the current frame
        std::map<std::string, int> frameVehicleCounts;
        std::map<std::string, int> frameRegionCounts = {{"E", 0}, {"N", 0}, {"S", 0}, {"W", 0}};

        int frameWidth = resizedFrame.cols;
        int frameHeight = resizedFrame.rows;

        for (size_t i = 0; i < outputs.size(); ++i) {
            float* data = (float*)outputs[i].data;
            for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
                float confidence = data[4];
                if (confidence > 0.5) {
                    int classId = std::max_element(data + 5, data + outputs[i].cols) - (data + 5);
                    Rect box;
                    int xCenter = static_cast<int>(data[0] * frameWidth);
                    int yCenter = static_cast<int>(data[1] * frameHeight);
                    int width = static_cast<int>(data[2] * frameWidth);
                    int height = static_cast<int>(data[3] * frameHeight);
                    box = Rect(xCenter - width / 2, yCenter - height / 2, width, height);

                    Scalar color;
                    std::string label;
                    if (classId == 2) {
                        frameVehicleCounts["Car"]++;
                        label = "Car";
                        color = carColor;
                    } else if (classId == 5) {
                        frameVehicleCounts["Bus"]++;
                        label = "Bus";
                        color = busColor;
                    } else if (classId == 7) {
                        frameVehicleCounts["Truck"]++;
                        label = "Truck";
                        color = truckColor;
                    } else if (classId == 1) {
                        frameVehicleCounts["Motorcycle"]++;
                        label = "Motorcycle";
                        color = motorcycleColor;
                    } else if (classId == 0) {
                        frameVehicleCounts["Person"]++;
                        label = "Person";
                        color = personColor;
                    } else {
                        continue;
                    }

                    // Determine region
                    std::string region;
                    if (xCenter < frameWidth / 2 && yCenter < frameHeight / 2) {
                        region = "N"; // North
                    } else if (xCenter < frameWidth / 2 && yCenter >= frameHeight / 2) {
                        region = "W"; // West
                    } else if (xCenter >= frameWidth / 2 && yCenter < frameHeight / 2) {
                        region = "E"; // East
                    } else {
                        region = "S"; // South
                    }
                    frameRegionCounts[region]++;

                    // Draw bounding box and label
                    rectangle(resizedFrame, box, color, 2);
                    putText(resizedFrame, label + " (" + region + ")", Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
                }
            }
        }

        int totalVehicles = 0;
        for (const auto& count : frameVehicleCounts) {
            totalVehicles += count.second;
        }

        int yOffset = 30;
        putText(resizedFrame, "Total Vehicles: " + std::to_string(totalVehicles), Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
        yOffset += 30;
        for (const auto& count : frameVehicleCounts) {
            putText(resizedFrame, count.first + ": " + std::to_string(count.second), Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
            yOffset += 30;
        }

        // Overlay region counts
        for (const auto& region : frameRegionCounts) {
            putText(resizedFrame, "Region " + region.first + ": " + std::to_string(region.second), Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
            yOffset += 30;
        }

        imshow("Traffic Monitoring", resizedFrame);
        if (waitKey(1) == 27) break;
    }

    return 0;
}
