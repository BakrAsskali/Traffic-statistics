"use client";

import { Card } from "@/components/ui/card";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { Car, Bus, Truck, Timer, Activity } from "lucide-react";
import { useEffect, useRef, useState } from "react";
import WebSocketService from "@/utils/WebSocketService";

type VehicleStatsType = {
  ActiveVehicleCounts: {
    Car: number;
    Bus: number;
    Truck: number;
  };
  TotalActiveVehicles: number;
};

export function VehicleStats() {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [stats, setStats] = useState<VehicleStatsType>({
    ActiveVehicleCounts: { Car: 0, Bus: 0, Truck: 0 },
    TotalActiveVehicles: 0,
  });

  useEffect(() => {
    const handleData = (data: { frame?: string; stats?: VehicleStatsType }) => {
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
      }
    };

    // Use WebSocketService
    WebSocketService.addListener(handleData);
    WebSocketService.connect();

    return () => {
      WebSocketService.removeListener(handleData);
    };
  }, []);

  const vehicleStats = [
    {
      type: "Car",
      label: "Personal Vehicles",
      icon: Car,
      avgSpeed: 45.6,
      avgTime: 12.4,
      count: stats.ActiveVehicleCounts.Car,
      color: "text-blue-600",
      bgColor: "bg-blue-50",
      borderColor: "border-blue-100",
    },
    {
      type: "Bus",
      label: "Public Transport",
      icon: Bus,
      avgSpeed: 36.8,
      avgTime: 18.3,
      count: stats.ActiveVehicleCounts.Bus,
      color: "text-green-600",
      bgColor: "bg-green-50",
      borderColor: "border-green-100",
    },
    {
      type: "Truck",
      label: "Commercial Vehicles",
      icon: Truck,
      avgSpeed: 25.4,
      avgTime: 22.5,
      count: stats.ActiveVehicleCounts.Truck,
      color: "text-orange-600",
      bgColor: "bg-orange-50",
      borderColor: "border-orange-100",
    },
    {
      type: "Total",
      label: "Overall Average",
      icon: Timer,
      avgSpeed: 35.9,
      avgTime: 17.7,
      count: stats.TotalActiveVehicles,
      color: "text-purple-600",
      bgColor: "bg-purple-50",
      borderColor: "border-purple-100",
    },
  ];

  return (
      <Card className="overflow-hidden border-none shadow-lg">
        <div className="bg-primary/5 p-4 border-b">
          <h2 className="text-xl font-semibold text-white flex items-center gap-2">
            <Activity className="h-5 w-5" />
            Vehicle Performance Metrics
          </h2>
          <p className="text-sm text-gray-300 mt-1">
            Real-time speed and transit analysis by vehicle type
          </p>
        </div>
        <div className="p-4 space-y-6">
          {/* Summary Cards */}
          <div className="grid grid-cols-2 sm:grid-cols-4 gap-4">
            {vehicleStats.map((stat) => (
                <Card
                    key={stat.type}
                    className={`p-4 ${stat.bgColor} ${stat.borderColor}`}
                >
                  <div className="flex items-center gap-2 mb-2">
                    <div className={`p-1 rounded-full ${stat.bgColor}`}>
                      <stat.icon className={`h-4 w-4 ${stat.color}`} />
                    </div>
                    <p className={`text-sm font-medium ${stat.color}`}>
                      {stat.type}
                    </p>
                  </div>
                  <p className={`text-2xl font-bold ${stat.color}`}>{stat.count}</p>
                </Card>
            ))}
          </div>

          {/* Main Stats Table */}
          <div className="bg-white rounded-lg shadow-sm border">
            <Table>
              <TableHeader>
                <TableRow className="bg-gray-50">
                  <TableHead className="font-semibold">Vehicle Category</TableHead>
                  <TableHead className="text-right font-semibold">
                    Average Speed
                  </TableHead>
                  <TableHead className="text-right font-semibold">
                    Average Time
                  </TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {vehicleStats.map((stat) => (
                    <TableRow key={stat.type} className="hover:bg-gray-50">
                      <TableCell className="font-medium">
                        <div className="flex items-center gap-2">
                          <div className={`p-1.5 rounded ${stat.bgColor}`}>
                            <stat.icon className={`h-5 w-5 ${stat.color}`} />
                          </div>
                          <div>
                            <div className={`font-medium ${stat.color}`}>
                              {stat.type}
                            </div>
                            <div className="text-xs text-gray-500">
                              {stat.label}
                            </div>
                          </div>
                        </div>
                      </TableCell>
                      <TableCell
                          className={`text-right font-medium ${stat.color}`}
                      >
                        {stat.avgSpeed} km/h
                      </TableCell>
                      <TableCell
                          className={`text-right font-medium ${stat.color}`}
                      >
                        {stat.avgTime} sec
                      </TableCell>
                    </TableRow>
                ))}
              </TableBody>
            </Table>
          </div>
        </div>
      </Card>
  );
}
