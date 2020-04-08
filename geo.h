#pragma once

#include <cmath>
#include <string>
#include <utility>
#include <sstream>

#include "utils.hpp"

using namespace std;

struct Stop{
  double latitude;
  double longitude;
};

struct StopRequest{
  Stop stop;
  unordered_map<string, double> neighs;
};

struct StopHasher{
  size_t operator()(const pair<string, Stop>& stop) const {
    const size_t coef = 39'916'801;
    const hash<double> double_hasher;
    return coef * double_hasher(stop.second.latitude) + double_hasher(stop.second.longitude);
  }
};

bool operator== (const Stop& lhs, const Stop& rhs){
  return (lhs.longitude == rhs.longitude) && (lhs.latitude == rhs.latitude);
}

const double pi = 3.1415926535;
const double Radius  = 6'371'000.0;

double ToRad(double val){
  return pi * val / 180.0;
}

double ComputeDistance(const Stop& lhs, const Stop& rhs){
  double curve = std::acos(std::sin(lhs.latitude)*std::sin(rhs.latitude) +
      std::cos(lhs.latitude)*std::cos(rhs.latitude) * std::cos(lhs.longitude - rhs.longitude));
  return curve * Radius;
}

StopRequest ReadStop (string line){
  Stop stop;
  string lat, lon;
  unordered_map<string, double> neighs;
  lat = ReadToken(line, ", ");
  lon = ReadToken(line, ", ");
  stop.latitude = ToRad(stod(lat));
  stop.longitude = ToRad(stod(lon));
  while(!line.empty()){
    double dist = stod(ReadToken(line, "m to "));
    neighs[ReadToken(line, ", ")] = dist;
  }
  return {stop, neighs};
}
