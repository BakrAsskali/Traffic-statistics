"use client";

import { useEffect, useRef } from "react";
import { Card } from "@/components/ui/card";
import { Camera } from "lucide-react";

export function VideoStream() {
  const videoRef = useRef<HTMLVideoElement>(null);

  useEffect(() => {
    if (videoRef.current) {
      videoRef.current.src = "https://example.com/traffic-feed";
    }
  }, []);

  return (
    <div className="space-y-4">
      <div className="relative rounded-lg overflow-hidden bg-gray-900">
        <video
          ref={videoRef}
          className="w-full aspect-video object-cover"
          autoPlay
          muted
          loop
        >
          <source src="/placeholder-video.mp4" type="video/mp4" />
          Your browser does not support the video tag.
        </video>
        
        {/* Region Overlays */}
        <div className="absolute inset-0 pointer-events-none">
          <div className="absolute top-4 left-4 bg-black/60 text-white px-3 py-1 rounded-full text-sm font-medium backdrop-blur-sm">
            <div className="flex items-center gap-1">
              <Camera className="h-4 w-4" />
              NE
            </div>
          </div>
          <div className="absolute top-4 right-4 bg-black/60 text-white px-3 py-1 rounded-full text-sm font-medium backdrop-blur-sm">
            <div className="flex items-center gap-1">
              <Camera className="h-4 w-4" />
              NW
            </div>
          </div>
          <div className="absolute bottom-4 left-4 bg-black/60 text-white px-3 py-1 rounded-full text-sm font-medium backdrop-blur-sm">
            <div className="flex items-center gap-1">
              <Camera className="h-4 w-4" />
              SE
            </div>
          </div>
          <div className="absolute bottom-4 right-4 bg-black/60 text-white px-3 py-1 rounded-full text-sm font-medium backdrop-blur-sm">
            <div className="flex items-center gap-1">
              <Camera className="h-4 w-4" />
              SW
            </div>
          </div>
        </div>
      </div>

      <div className="grid grid-cols-2 gap-4">
        <Card className="p-4 bg-green-50 border-green-100">
          <p className="text-sm text-green-600 font-medium">Active Vehicles</p>
          <p className="text-2xl font-bold text-green-700">24</p>
        </Card>
        <Card className="p-4 bg-blue-50 border-blue-100">
          <p className="text-sm text-blue-600 font-medium">Current Speed</p>
          <p className="text-2xl font-bold text-blue-700">45 km/h</p>
        </Card>
      </div>
    </div>
  );
}