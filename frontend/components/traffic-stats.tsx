"use client";

import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { Card } from "@/components/ui/card";
import { Navigation } from "lucide-react";
import { useEffect, useRef, useState } from "react";
import WebSocketService from "@/utils/WebSocketService";

const directions = [
  { code: "N", label: "North" },
  { code: "S", label: "South" },
  { code: "W", label: "West" },
  { code: "E", label: "East" },
  { code: "NE", label: "North East" },
  { code: "NW", label: "North West" },
  { code: "NS", label: "North-South" },
  { code: "SN", label: "South-North" },
  { code: "SE", label: "South East" },
  { code: "SW", label: "South West" },
  { code: "EN", label: "East-North" },
  { code: "ES", label: "East-South" },
  { code: "EW", label: "East-West" },
  { code: "WN", label: "West-North" },
  { code: "WE", label: "West-East" },
  { code: "WS", label: "West-South" },
];

export function TrafficStats() {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [vehicleCount, setVehicleCount] = useState(0);
  const [stats, setStats] = useState<{ [key: string]: any }>({});
  const [trafficData, setTrafficData] = useState<
      { direction: string; cars: number; buses: number; trucks: number }[]
  >([]);

  useEffect(() => {
    const handleData = (data: { frame?: string; stats?: any }) => {
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

      // Update stats and traffic data
      if (data.stats) {
        setStats(data.stats);
        setVehicleCount(data.stats.TotalVehicles || 0);
        const newTrafficData = directions.map((direction) => ({
          direction: direction.label,
          cars: data.stats.VehicleCounts?.[direction.code]?.Car || 0,
          buses: data.stats.VehicleCounts?.[direction.code]?.Bus || 0,
          trucks: data.stats.VehicleCounts?.[direction.code]?.Truck || 0,
        }));
        setTrafficData(newTrafficData);
      }
    };

    // Use WebSocketService
    WebSocketService.addListener(handleData);
    WebSocketService.connect();

    return () => {
      WebSocketService.removeListener(handleData);
    };
  }, []);

  return (
      <div className="space-y-6">
        {/* Summary Cards */}
        <div className="grid grid-cols-2 sm:grid-cols-4 gap-4">
          <Card className="p-4 bg-blue-50">
            <p className="text-sm text-blue-600 font-medium">Total Vehicles</p>
            <p className="text-2xl font-bold text-blue-700">{vehicleCount}</p>
          </Card>
          <Card className="p-4 bg-green-50">
            <p className="text-sm text-green-600 font-medium">Active Regions</p>
            <p className="text-2xl font-bold text-green-700">{directions.length}</p>
          </Card>
          <Card className="p-4 bg-purple-50">
            <p className="text-sm text-purple-600 font-medium">Avg Speed</p>
            <p className="text-2xl font-bold text-purple-700">{stats.AvgSpeed || 0} km/h</p>
          </Card>
          <Card className="p-4 bg-orange-50">
            <p className="text-sm text-orange-600 font-medium">Peak Hour</p>
            <p className="text-2xl font-bold text-orange-700">{stats.PeakHour || "N/A"}</p>
          </Card>
        </div>

        {/* Traffic Flow Table */}
        <div className="bg-white rounded-lg shadow-sm border">
          <div className="p-4 border-b bg-gray-50">
            <h3 className="font-semibold text-gray-900 flex items-center gap-2">
              <Navigation className="h-4 w-4" />
              Traffic Flow by Direction
            </h3>
            <p className="text-sm text-gray-500 mt-1">
              Real-time vehicle count across all monitored regions
            </p>
          </div>
          <Table>
            <TableHeader>
              <TableRow className="bg-gray-50">
                <TableHead className="font-semibold">Region Direction</TableHead>
                <TableHead className="text-right font-semibold text-blue-600">Cars</TableHead>
                <TableHead className="text-right font-semibold text-green-600">Buses</TableHead>
                <TableHead className="text-right font-semibold text-orange-600">Trucks</TableHead>
                <TableHead className="text-right font-semibold text-purple-600">Total</TableHead>
              </TableRow>
            </TableHeader>
            <TableBody>
              {trafficData.map(({ direction, cars, buses, trucks }) => (
                  <TableRow key={direction} className="hover:bg-gray-50">
                    <TableCell className="font-medium">
                      <div className="flex items-center gap-2">
                        <span className="text-gray-900">{direction}</span>
                      </div>
                    </TableCell>
                    <TableCell className="text-right text-blue-600">{cars}</TableCell>
                    <TableCell className="text-right text-green-600">{buses}</TableCell>
                    <TableCell className="text-right text-orange-600">{trucks}</TableCell>
                    <TableCell className="text-right font-semibold text-purple-600">
                      {cars + buses + trucks}
                    </TableCell>
                  </TableRow>
              ))}
            </TableBody>
          </Table>
        </div>
      </div>
  );
}
