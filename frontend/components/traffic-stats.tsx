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
  { code: "NE", label: "North East" },
  { code: "NW", label: "North West" },
  { code: "SE", label: "South East" },
  { code: "SW", label: "South West" },
  { code: "EW", label: "East-West" },
  { code: "WE", label: "West-East" },
];

export function TrafficStats() {
  const [vehicleCount, setVehicleCount] = useState(0);
  const [stats, setStats] = useState<{ [key: string]: any }>({});
  const [trafficData, setTrafficData] = useState<
      { direction: string; cars: number; buses: number; trucks: number; total: number }[]
  >([]);

  useEffect(() => {
    const handleData = (data: { stats?: any }) => {
      if (data.stats) {
        setStats(data.stats);
        setVehicleCount(data.stats.TotalActiveVehicles || 0);

        // Map direction counts to the traffic data table
        const newTrafficData = directions.map(({ code, label }) => {
          const counts = data.stats.DirectionCounts?.[code] || {};
          const cars = counts.Car || 0;
          const buses = counts.Bus || 0;
          const trucks = counts.Truck || 0;

          return {
            direction: label,
            cars,
            buses,
            trucks,
            total: cars + buses + trucks,
          };
        });

        setTrafficData(newTrafficData);
      }
    };

    WebSocketService.addListener(handleData);
    WebSocketService.connect();

    return () => {
      WebSocketService.removeListener(handleData);
      WebSocketService.close();
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
              {trafficData.map(({ direction, cars, buses, trucks, total }) => (
                  <TableRow key={direction} className="hover:bg-gray-50">
                    <TableCell className="font-medium text-gray-900">{direction}</TableCell>
                    <TableCell className="text-right text-blue-600">{cars}</TableCell>
                    <TableCell className="text-right text-green-600">{buses}</TableCell>
                    <TableCell className="text-right text-orange-600">{trucks}</TableCell>
                    <TableCell className="text-right font-semibold text-purple-600">{total}</TableCell>
                  </TableRow>
              ))}
            </TableBody>
          </Table>
        </div>
      </div>
  );
}
