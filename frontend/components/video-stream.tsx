import React, { useState, useEffect, useRef } from "react";
import { Card } from "@/components/ui/card";

export function VideoStream() {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [vehicleCount, setVehicleCount] = useState(0);
  const [stats, setStats] = useState({});

  useEffect(() => {
    const socket = new WebSocket("ws://localhost:8080");
    const canvas = canvasRef.current;

    socket.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);

        // Decode and display image
        if (data.frame && canvas) {
          const ctx = canvas.getContext('2d');
          const img = new Image();
          img.onload = () => {
            canvas.width = img.width;
            canvas.height = img.height;
            ctx?.drawImage(img, 0, 0);
          };
          img.src = `data:image/jpeg;base64,${data.frame}`;
        }

        // Update stats
        if (data.stats) {
          setStats(data.stats);
          setVehicleCount(data.stats.VehicleCounts?.Car || 0);
        }
      } catch (error) {
        console.error("WebSocket message parsing error:", error);
      }
    };

    socket.onerror = (error) => {
      console.error("WebSocket error:", error);
    };

    return () => socket.close();
  }, []);

  return (
      <div className="space-y-4">
        <div className="relative rounded-lg overflow-hidden bg-gray-900">
          <canvas
              ref={canvasRef}
              className="w-full aspect-video object-cover"
          />
        </div>

        <div className="grid grid-cols-2 gap-4">
          <Card className="p-4 bg-green-50 border-green-100">
            <p className="text-sm text-green-600 font-medium">Active Vehicles</p>
            <p className="text-2xl font-bold text-green-700">{vehicleCount}</p>
          </Card>
          <Card className="p-4 bg-blue-50 border-blue-100">
            <p className="text-sm text-blue-600 font-medium">Raw Stats</p>
            <pre className="text-xs">{JSON.stringify(stats, null, 2)}</pre>
          </Card>
        </div>
      </div>
  );
}