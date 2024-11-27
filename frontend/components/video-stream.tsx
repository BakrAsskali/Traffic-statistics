import React, { useState, useEffect, useRef } from "react";
import { Card } from "@/components/ui/card";
import WebSocketService from "@/utils/WebSocketService";

export function VideoStream() {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [vehicleCount, setVehicleCount] = useState(0);
  const [stats, setStats] = useState({});

  useEffect(() => {
    const handleData = (data: { frame?: string; stats?: { TotalActiveVehicles?: number; [key: string]: any } }) => {
      const canvas = canvasRef.current;

      // Decode and display image
      if (data.frame && canvas) {
        const ctx = canvas.getContext("2d");
        if (ctx) {
          const img = new Image();
          img.onload = () => {
            canvas.width = img.width;
            canvas.height = img.height;
            ctx.drawImage(img, 0, 0);
          };
          img.src = `data:image/jpeg;base64,${data.frame}`;
        }
      }

      // Update stats
      if (data.stats) {
        setStats(data.stats);
        setVehicleCount(data.stats.TotalActiveVehicles || 0);
      }
    };

    // Use the WebSocketService singleton
    WebSocketService.addListener(handleData);
    WebSocketService.connect();

    return () => {
      WebSocketService.removeListener(handleData);
    };
  }, []);

  return (
      <div className="space-y-4">
        <div className="relative rounded-lg overflow-hidden bg-gray-900">
          <canvas ref={canvasRef} className="w-full aspect-video object-cover" />
        </div>

        <div className="grid grid-cols-2 gap-4">
          {/* Active Vehicles Card */}
          <Card className="p-4 bg-green-50 border-green-100 flex flex-col justify-between">
            <p className="text-sm text-green-600 font-medium">Active Vehicles</p>
            <p className="text-2xl font-bold text-green-700">{vehicleCount}</p>
          </Card>

          {/* Raw Stats Card */}
          <Card className="p-4 bg-blue-50 border-blue-100 flex flex-col justify-between">
            <p className="text-sm text-blue-600 font-medium">Raw Stats</p>
            <pre className="text-xs overflow-y-auto max-h-24 break-words bg-blue-100 rounded p-2 text-blue-900">
            {JSON.stringify(stats, null, 2)}
          </pre>
          </Card>
        </div>
      </div>
  );
}
