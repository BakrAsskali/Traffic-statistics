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
  return (
    <div className="space-y-6">
      <div className="grid grid-cols-2 sm:grid-cols-4 gap-4">
        <Card className="p-4 bg-blue-50">
          <p className="text-sm text-blue-600 font-medium">Total Vehicles</p>
          <p className="text-2xl font-bold text-blue-700">1,234</p>
        </Card>
        <Card className="p-4 bg-green-50">
          <p className="text-sm text-green-600 font-medium">Active Regions</p>
          <p className="text-2xl font-bold text-green-700">16</p>
        </Card>
        <Card className="p-4 bg-purple-50">
          <p className="text-sm text-purple-600 font-medium">Avg Speed</p>
          <p className="text-2xl font-bold text-purple-700">45 km/h</p>
        </Card>
        <Card className="p-4 bg-orange-50">
          <p className="text-sm text-orange-600 font-medium">Peak Hour</p>
          <p className="text-2xl font-bold text-orange-700">17:00</p>
        </Card>
      </div>

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
              <TableHead className="text-right font-semibold">
                <span className="text-blue-600">Cars</span>
              </TableHead>
              <TableHead className="text-right font-semibold">
                <span className="text-green-600">Buses</span>
              </TableHead>
              <TableHead className="text-right font-semibold">
                <span className="text-orange-600">Trucks</span>
              </TableHead>
              <TableHead className="text-right font-semibold">
                <span className="text-purple-600">Total</span>
              </TableHead>
            </TableRow>
          </TableHeader>
          <TableBody>
            {directions.map((direction) => (
              <TableRow key={direction.code} className="hover:bg-gray-50">
                <TableCell className="font-medium">
                  <div className="flex items-center gap-2">
                    <span className="text-gray-900">{direction.label}</span>
                    <span className="text-xs bg-gray-100 px-2 py-0.5 rounded text-gray-600">
                      {direction.code}
                    </span>
                  </div>
                </TableCell>
                <TableCell className="text-right text-blue-600">0</TableCell>
                <TableCell className="text-right text-green-600">0</TableCell>
                <TableCell className="text-right text-orange-600">0</TableCell>
                <TableCell className="text-right font-semibold text-purple-600">0</TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </div>
    </div>
  );
}