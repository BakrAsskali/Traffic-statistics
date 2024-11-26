#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>
#include <boost/beast/core/detail/base64.hpp>

using namespace cv;
using namespace std;
namespace websocket = boost::beast::websocket;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

std::mutex frameMutex;
Mat globalFrame;
json globalStats;

std::string base64_encode(const unsigned char* data, size_t len) {
    static const char base64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (len--) {
        char_array_3[i++] = *(data++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                encoded += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++)
            encoded += base64_chars[char_array_4[j]];

        while (i++ < 3)
            encoded += '=';
    }

    return encoded;
}

void processVideo(VideoCapture& cap, dnn::Net& net, int windowWidth, int windowHeight) {
    Scalar carColor(0, 255, 0);     // Green for cars
    Scalar busColor(255, 0, 0);     // Blue for buses
    Scalar truckColor(0, 0, 255);   // Red for trucks

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        Mat resizedFrame;
        float resizeFactor = std::min(float(windowWidth) / frame.cols, float(windowHeight) / frame.rows);
        resize(frame, resizedFrame, Size(), resizeFactor, resizeFactor);

        Mat blob = dnn::blobFromImage(resizedFrame, 1 / 255.0, Size(416, 416), Scalar(0, 0, 0), true, false);
        net.setInput(blob);

        vector<Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        // Reset active vehicle counts for the current frame
        map<string, int> activeVehicleCounts = {
            {"Car", 0},
            {"Bus", 0},
            {"Truck", 0}
        };

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

                    // Adjust class IDs based on your YOLO model's class mapping
                    switch(classId) {
                        case 2: // Car
                            activeVehicleCounts["Car"]++;
                            rectangle(resizedFrame, box, carColor, 2);
                            break;
                        case 5: // Bus (adjust based on your model)
                            activeVehicleCounts["Bus"]++;
                            rectangle(resizedFrame, box, busColor, 2);
                            break;
                        case 7: // Truck (adjust based on your model)
                            activeVehicleCounts["Truck"]++;
                            rectangle(resizedFrame, box, truckColor, 2);
                            break;
                    }
                }
            }
        }

        json stats = {
            {"ActiveVehicleCounts", activeVehicleCounts},
            {"TotalActiveVehicles", activeVehicleCounts["Car"] + activeVehicleCounts["Bus"] + activeVehicleCounts["Truck"]}
        };

        {
            lock_guard<std::mutex> lock(frameMutex);
            globalFrame = resizedFrame.clone();
            globalStats = stats;
        }
    }
}

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

                        json message = {
                            {"frame", encodedImage},
                            {"stats", globalStats}
                        };

                        ws.text(true);
                        string messageStr = message.dump();
                        ws.write(boost::asio::buffer(messageStr));
                    }
                }
                this_thread::sleep_for(chrono::milliseconds(30));
            }
        }
        catch (const std::exception& e) {
            cerr << "WebSocket error: " << e.what() << endl;
        }
    }
}

int main() {
    const std::string videoPath = "/home/bakr/CLionProjects/Projet/5jcg5vfx58-3/Videos 2/2/2.avi";
    VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        cerr << "Error: Cannot open video file." << endl;
        return -1;
    }

    dnn::Net net = dnn::readNetFromDarknet(
        "/home/bakr/CLionProjects/Projet/yolov3.cfg",
        "/home/bakr/CLionProjects/Projet/yolov3_final.weights"
    );

    int windowWidth = 800;
    int windowHeight = 600;

    thread videoProcessingThread(processVideo, ref(cap), ref(net), windowWidth, windowHeight);

    boost::asio::io_context ioc;
    thread serverThread(websocketServer, ref(ioc), 8080);

    videoProcessingThread.join();
    serverThread.join();

    return 0;
}