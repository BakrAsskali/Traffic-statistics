#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <numeric>
#include <string>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>

using namespace cv;
using namespace std;
namespace websocket = boost::beast::websocket;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

std::mutex frameMutex;
Mat globalFrame;
json globalStats;

struct VehicleStatistics {
    std::vector<float> speeds;  // Speeds in km/h
    std::vector<float> times;   // Times in seconds
};

// Helper function for Base64 encoding
std::string base64_encode(const unsigned char* data, size_t len) {
    static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string encoded;
    int i = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (len--) {
        char_array_3[i++] = *(data++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++) {
                encoded += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (int j = i; j < 3; j++) char_array_3[j] = '\0';
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (int j = 0; j < i + 1; j++) {
            encoded += base64_chars[char_array_4[j]];
        }

        while (i++ < 3) encoded += '=';
    }

    return encoded;
}

// Add direction mapping
const vector<string> directions = {"NE", "NW", "NS", "SN", "SE", "SW", "EN", "ES", "EW", "WN", "WE", "WS"};

// Function to calculate direction based on bounding box motion
string calculateDirection(const Point& prevCenter, const Point& currCenter) {
    int dx = currCenter.x - prevCenter.x;
    int dy = currCenter.y - prevCenter.y;

    if (dx > 0 && dy < 0) return "NE"; // Example: North-East
    if (dx > 0 && dy > 0) return "SE"; // Example: South-East
    if (dx < 0 && dy < 0) return "NW"; // Example: North-West
    if (dx < 0 && dy > 0) return "SW"; // Example: South-West
    if (dx == 0 && dy < 0) return "NS"; // Example: North-South
    if (dx == 0 && dy > 0) return "SN"; // Example: South-North
    if (dx > 0 && dy == 0) return "EW"; // Example: East-West
    if (dx < 0 && dy == 0) return "WE"; // Example: West-East
    }

// Track vehicles across frames
map<int, Point> previousCenters;

// Update `processVideo`
void processVideo(VideoCapture& cap, dnn::Net& net, int windowWidth, int windowHeight) {
    Scalar carColor(0, 255, 0), busColor(255, 0, 0), truckColor(0, 0, 255), motorcycleColor(255, 255, 0);
    map<string, VehicleStatistics> vehicleStats = {{"Car", {}}, {"Bus", {}}, {"Truck", {}}, {"Motorcycle", {}}};
    map<string, map<string, int>> directionStats;

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        Mat resizedFrame;
        float resizeFactor = min(float(windowWidth) / frame.cols, float(windowHeight) / frame.rows);
        resize(frame, resizedFrame, Size(), resizeFactor, resizeFactor);

        Mat blob = dnn::blobFromImage(resizedFrame, 1 / 255.0, Size(416, 416), Scalar(), true, false);
        net.setInput(blob);

        vector<Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        map<string, int> frameVehicleCounts = {{"Car", 0}, {"Bus", 0}, {"Truck", 0}, {"Motorcycle", 0}};
        directionStats.clear(); // Reset direction stats for the current frame

        for (size_t i = 0; i < outputs.size(); ++i) {
            float* data = (float*)outputs[i].data;
            for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
                float confidence = data[4];
                if (confidence > 0.5) {
                    int classId = max_element(data + 5, data + outputs[i].cols) - (data + 5);
                    int xCenter = int(data[0] * resizedFrame.cols);
                    int yCenter = int(data[1] * resizedFrame.rows);
                    int width = int(data[2] * resizedFrame.cols);
                    int height = int(data[3] * resizedFrame.rows);

                    Rect box(xCenter - width / 2, yCenter - height / 2, width, height);
                    Scalar color;
                    string label;

                    if (classId == 2) {  // Car
                        label = "Car";
                        color = carColor;
                    } else if (classId == 5) {  // Bus
                        label = "Bus";
                        color = busColor;
                    } else if (classId == 7) {  // Truck
                        label = "Truck";
                        color = truckColor;
                    } else {
                        continue;
                    }

                    frameVehicleCounts[label]++;
                    rectangle(resizedFrame, box, color, 2);
                    putText(resizedFrame, label, Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, color, 2);

                    // Calculate direction
                    Point currCenter(xCenter, yCenter);
                    if (previousCenters.count(classId)) {
                        string direction = calculateDirection(previousCenters[classId], currCenter);
                        directionStats[direction][label]++;
                    }
                    previousCenters[classId] = currCenter;
                }
            }
        }

        json stats = {
            {"ActiveVehicleCounts", frameVehicleCounts},
            {"TotalActiveVehicles", frameVehicleCounts["Car"] + frameVehicleCounts["Bus"] + frameVehicleCounts["Truck"] + frameVehicleCounts["Motorcycle"]},
            {"DirectionCounts", directionStats}
        };

        lock_guard<std::mutex> lock(frameMutex);
        globalFrame = resizedFrame.clone();
        globalStats = stats;

        if (waitKey(1) == 27) break; // Escape key
    }
}

// WebSocket server
void websocketServer(boost::asio::io_context& ioc, unsigned short port) {
    tcp::acceptor acceptor(ioc, tcp::endpoint(tcp::v4(), port));
    while (true) {
        try {
            tcp::socket socket(ioc);
            acceptor.accept(socket);
            websocket::stream<tcp::socket> ws(std::move(socket));
            ws.accept();

            while (true) {
                {
                    lock_guard<std::mutex> lock(frameMutex);
                    if (!globalFrame.empty()) {
                        vector<uchar> buffer;
                        imencode(".jpg", globalFrame, buffer);
                        string encodedImage = base64_encode(buffer.data(), buffer.size());

                        json message = {{"frame", encodedImage}, {"stats", globalStats}};
                        ws.text(true);
                        ws.write(boost::asio::buffer(message.dump()));
                    }
                }
                this_thread::sleep_for(chrono::milliseconds(1));
            }
        } catch (const std::exception& e) {
            cerr << "WebSocket error: " << e.what() << endl;
        }
    }
}

// Main function
int main() {
    const string videoPath = "/home/bakr/CLionProjects/Projet/5jcg5vfx58-3/Videos 2/2/2.avi";
    VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        cerr << "Error: Cannot open video file." << endl;
        return -1;
    }

    dnn::Net net = dnn::readNetFromDarknet("/home/bakr/CLionProjects/Projet/yolov3.cfg", "/home/bakr/CLionProjects/Projet/yolov3_final.weights");
    int windowWidth = 800, windowHeight = 600;

    thread videoProcessingThread(processVideo, ref(cap), ref(net), windowWidth, windowHeight);
    boost::asio::io_context ioc;
    thread serverThread(websocketServer, ref(ioc), 8080);

    videoProcessingThread.join();
    serverThread.join();
    return 0;
}
