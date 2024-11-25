"use client";

import { Card } from "@/components/ui/card";
import { VideoStream } from "@/components/video-stream";
import { TrafficStats } from "@/components/traffic-stats";
import { VehicleStats } from "@/components/vehicle-stats";
import { Activity, BarChart3 } from "lucide-react";

export default function Dashboard() {
  return (
    <div className="min-h-screen bg-gray-50 py-8">
      <div className="container mx-auto px-4">
        <header className="mb-8">
          <h1 className="text-3xl font-bold text-gray-900 flex items-center gap-2">
            <Activity className="h-8 w-8 text-primary" />
            Traffic Monitoring Dashboard
          </h1>
          <p className="text-muted-foreground mt-2">
            Real-time traffic analysis and monitoring system
          </p>
        </header>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-8">
          <div className="space-y-8">
            <Card className="overflow-hidden border-none shadow-lg">
              <div className="bg-primary/5 p-4 border-b">
                <h2 className="text-xl font-semibold text-white flex items-center gap-2">
                  Live Traffic Feed
                </h2>
              </div>
              <div className="p-4">
                <VideoStream />
              </div>
            </Card>
            <VehicleStats />
          </div>

          <div className="space-y-8">
            <Card className="overflow-hidden border-none shadow-lg">
              <div className="bg-primary/5 p-4 border-b">
                <h2 className="text-xl font-semibold text-white flex items-center gap-2">
                  <BarChart3 className="h-5 w-5" />
                  Traffic Statistics
                </h2>
              </div>
              <div className="p-4">
                <TrafficStats />
              </div>
            </Card>
          </div>
        </div>
      </div>
    </div>
  );
}