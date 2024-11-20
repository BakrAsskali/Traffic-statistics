#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>
#include <map>

using namespace cv;

int main() {
    VideoCapture cap("/home/bakr/CLionProjects/Projet/5jcg5vfx58-3/Videos 1/1/1.avi");
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open video file." << std::endl;
        return -1;
    }

    // Load DNN model for vehicle detection
    dnn::Net net = dnn::readNetFromDarknet("/home/bakr/CLionProjects/Projet/yolov3.cfg",
                                           "/home/bakr/CLionProjects/Projet/yolov3.weights");

    // Statistics variables
    int totalVehicles = 0;
    std::map<std::string, int> vehicleCounts; // {"Car": 0, "Bus": 0, "Truck": 0}

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        // Prepare the frame for DNN input
        Mat blob = dnn::blobFromImage(frame, 1 / 255.0, Size(416, 416), Scalar(0, 0, 0), true, false);
        net.setInput(blob);

        // Forward pass
        std::vector<Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        // Parse detections and update statistics
        for (size_t i = 0; i < outputs.size(); ++i) {
            float* data = (float*)outputs[i].data;
            for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
                float confidence = data[4];
                if (confidence > 0.5) { // Confidence threshold
                    int classId = std::max_element(data + 5, data + outputs[i].cols) - (data + 5);
                    if (classId == 2) {
                        vehicleCounts["Car"]++;
                        totalVehicles++;
                    } else if (classId == 5) {
                        vehicleCounts["Bus"]++;
                        totalVehicles++;
                    } else if (classId == 7) {
                        vehicleCounts["Truck"]++;
                        totalVehicles++;
                    }
                }
            }
        }

        // Overlay statistics on the frame
        std::string stats = "Total Vehicles: " + std::to_string(totalVehicles);
        putText(frame, stats, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

        int yOffset = 60;
        for (const auto& category : vehicleCounts) {
            std::string text = category.first + ": " + std::to_string(category.second);
            putText(frame, text, Point(10, yOffset), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
            yOffset += 30;
        }

        // Display the frame
        imshow("Traffic Monitoring", frame);

        if (waitKey(1) == 27) break; // Exit on 'ESC'
    }

    return 0;
}
