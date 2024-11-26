class WebSocketService {
    private static instance: WebSocketService;
    private url: string;
    private socket: WebSocket | null = null;
    private listeners: Array<(data: { frame?: string; stats?: any }) => void> = [];

    private constructor(url: string) {
        this.url = url;
    }

    static getInstance(url: string): WebSocketService {
        if (!WebSocketService.instance) {
            WebSocketService.instance = new WebSocketService(url);
        }
        return WebSocketService.instance;
    }

    connect() {
        if (this.socket && this.socket.readyState === WebSocket.OPEN) {
            return;
        }

        this.socket = new WebSocket(this.url);

        this.socket.onmessage = (event) => {
            try {
                const data = JSON.parse(event.data);
                this.listeners.forEach((listener) => listener(data));
            } catch (error) {
                console.error("WebSocket message parsing error:", error);
            }
        };

        this.socket.onerror = (error) => {
            console.error("WebSocket error:", error);
        };

        this.socket.onclose = () => {
            console.warn("WebSocket connection closed. Reconnecting...");
            setTimeout(() => this.connect(), 5000); // Reconnect after 5 seconds
        };
    }

    addListener(callback: (data: { frame?: string; stats?: any }) => void) {
        if (typeof callback === "function") {
            this.listeners.push(callback);
        }
    }

    removeListener(callback: (data: { frame?: string; stats?: any }) => void) {
        this.listeners = this.listeners.filter((listener) => listener !== callback);
    }

    sendMessage(message: unknown) {
        if (this.socket && this.socket.readyState === WebSocket.OPEN) {
            this.socket.send(JSON.stringify(message));
        } else {
            console.warn("WebSocket is not connected. Message not sent.");
        }
    }

    close() {
        if (this.socket) {
            this.socket.close();
        }
    }
}

const instance = WebSocketService.getInstance("ws://localhost:8080");
export default instance;
