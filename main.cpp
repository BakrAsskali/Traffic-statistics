#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
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

    // Define colors for bounding boxes (for each vehicle type)
    Scalar carColor(0, 255, 0); // Green
    Scalar busColor(0, 0, 255); // Red
    Scalar truckColor(255, 0, 0); // Blue
    Scalar motorcycleColor(255, 165, 0); // Orange
    Scalar personColor(255, 255, 0); // Yellow

    // Define regions
    const std::string regions[] = {"N", "E", "S", "W"};

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

        // Calculate region boundaries
        int midX = resizedFrame.cols / 2;
        int midY = resizedFrame.rows / 2;

        // Draw region boundaries
        line(resizedFrame, Point(midX, 0), Point(midX, resizedFrame.rows), Scalar(255, 255, 255), 2);
        line(resizedFrame, Point(0, midY), Point(resizedFrame.cols, midY), Scalar(255, 255, 255), 2);

        // Add region labels
        putText(resizedFrame, "N", Point(midX - 50, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(resizedFrame, "E", Point(resizedFrame.cols - 60, midY - 10), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(resizedFrame, "S", Point(midX - 20, resizedFrame.rows - 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(resizedFrame, "W", Point(20, midY - 10), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        // Prepare the frame for DNN input
        Mat blob = dnn::blobFromImage(resizedFrame, 1 / 255.0, Size(416, 416), Scalar(0, 0, 0), true, false);
        net.setInput(blob);

        std::vector<Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        // Vehicle statistics
        std::map<std::string, int> frameVehicleCounts;
        std::map<std::string, int> regionCounts = {{"N", 0}, {"E", 0}, {"S", 0}, {"W", 0}};

        for (size_t i = 0; i < outputs.size(); ++i) {
            float* data = (float*)outputs[i].data;
            for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
                float confidence = data[4];
                if (confidence > 0.5) {
                    int classId = std::max_element(data + 5, data + outputs[i].cols) - (data + 5);
                    Rect box;
                    int xCenter = static_cast<int>(data[0] * resizedFrame.cols);
                    int yCenter = static_cast<int>(data[1] * resizedFrame.rows);
                    int width = static_cast<int>(data[2] * resizedFrame.cols);
                    int height = static_cast<int>(data[3] * resizedFrame.rows);
                    box = Rect(xCenter - width / 2, yCenter - height / 2, width, height);

                    Scalar color;
                    std::string label;
                    if (classId == 2) {  // Car
                        frameVehicleCounts["Car"]++;
                        label = "Car";
                        color = carColor;
                    } else if (classId == 5) {  // Bus
                        frameVehicleCounts["Bus"]++;
                        label = "Bus";
                        color = busColor;
                    } else if (classId == 7) {  // Truck
                        frameVehicleCounts["Truck"]++;
                        label = "Truck";
                        color = truckColor;
                    } else if (classId == 1) {  // Motorcycle
                        frameVehicleCounts["Motorcycle"]++;
                        label = "Motorcycle";
                        color = motorcycleColor;
                    } else if (classId == 0) {  // Person
                        frameVehicleCounts["Person"]++;
                        label = "Person";
                        color = personColor;
                    } else {
                        continue;
                    }

                    // Determine region
                    std::string region;
                    if (xCenter < midX && yCenter < midY) {
                        region = "N";
                    } else if (xCenter >= midX && yCenter < midY) {
                        region = "E";
                    } else if (xCenter >= midX && yCenter >= midY) {
                        region = "S";
                    } else {
                        region = "W";
                    }
                    regionCounts[region]++;

                    // Draw rectangle and labels
                    rectangle(resizedFrame, box, color, 2);
                    putText(resizedFrame, label + " (" + region + ")", Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
                }
            }
        }

        // Display total vehicle counts
        int yOffset = 30;
        putText(resizedFrame, "Vehicle Statistics:", Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
        yOffset += 30;
        for (const auto& category : frameVehicleCounts) {
            std::string text = category.first + ": " + std::to_string(category.second);
            putText(resizedFrame, text, Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
            yOffset += 30;
        }

        // Display regional vehicle counts
        yOffset += 20; // Add spacing
        putText(resizedFrame, "Vehicles by Region:", Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
        yOffset += 30;
        for (const auto& region : regionCounts) {
            std::string text = region.first + ": " + std::to_string(region.second);
            putText(resizedFrame, text, Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
            yOffset += 30;
        }

        imshow("Traffic Monitoring", resizedFrame);

        if (waitKey(1) == 27) break; // Exit on 'ESC'
    }

    return 0;
}
