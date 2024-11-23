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

    // Statistics variables (no need for global totalVehicles anymore)
    std::map<std::string, int> vehicleCounts; // {"Car": 0, "Bus": 0, "Truck": 0, "Motorcycle": 0, "Person": 0}

    // Define colors for bounding boxes (for each vehicle type)
    Scalar carColor(0, 255, 0); // Green
    Scalar busColor(0, 0, 255); // Red
    Scalar truckColor(255, 0, 0); // Blue
    Scalar motorcycleColor(255, 165, 0); // Orange
    Scalar personColor(255, 255, 0); // Yellow

    // Set a fixed size for the window (adjust as needed)
    const int windowWidth = 800;  // Set the desired width
    const int windowHeight = 600; // Set the desired height

    // Create a window with a fixed size
    namedWindow("Traffic Monitoring", WINDOW_NORMAL);
    resizeWindow("Traffic Monitoring", windowWidth, windowHeight);

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        // Resize the frame to fit the window
        Mat resizedFrame;
        float resizeFactor = std::min(float(windowWidth) / frame.cols, float(windowHeight) / frame.rows);
        resize(frame, resizedFrame, Size(), resizeFactor, resizeFactor);

        // Prepare the frame for DNN input
        Mat blob = dnn::blobFromImage(resizedFrame, 1 / 255.0, Size(416, 416), Scalar(0, 0, 0), true, false);
        net.setInput(blob);

        // Forward pass
        std::vector<Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        // Reset vehicle count for each frame
        std::map<std::string, int> frameVehicleCounts;

        // Parse detections and update statistics
        for (size_t i = 0; i < outputs.size(); ++i) {
            float* data = (float*)outputs[i].data;
            for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
                float confidence = data[4];
                if (confidence > 0.5) { // Confidence threshold
                    int classId = std::max_element(data + 5, data + outputs[i].cols) - (data + 5);
                    Rect box;
                    int xCenter = static_cast<int>(data[0] * resizedFrame.cols);
                    int yCenter = static_cast<int>(data[1] * resizedFrame.rows);
                    int width = static_cast<int>(data[2] * resizedFrame.cols);
                    int height = static_cast<int>(data[3] * resizedFrame.rows);
                    box = Rect(xCenter - width / 2, yCenter - height / 2, width, height);

                    // Draw bounding box and label
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
                        continue; // Skip non-vehicle or irrelevant classes
                    }

                    // Draw the rectangle and label
                    rectangle(resizedFrame, box, color, 2);
                    putText(resizedFrame, label, Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
                }
            }
        }

        // Calculate total vehicles for the frame by summing individual counts
        int totalVehicles = 0;
        for (const auto& category : frameVehicleCounts) {
            totalVehicles += category.second;
        }

        // Overlay statistics at the top of the frame
        int yOffset = 30;
        putText(resizedFrame, "Total Vehicles: " + std::to_string(totalVehicles), Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
        yOffset += 30;
        for (const auto& category : frameVehicleCounts) {
            std::string text = category.first + ": " + std::to_string(category.second);
            putText(resizedFrame, text, Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
            yOffset += 30;
        }

        // Display the frame
        imshow("Traffic Monitoring", resizedFrame);

        if (waitKey(1) == 27) break; // Exit on 'ESC'
    }

    return 0;
}
