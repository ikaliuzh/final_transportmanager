#include <unordered_map>
#include <optional>
#include <iostream>
#include <string>
#include <deque>

#include "utils.hpp"
#include "route.hpp"
#include "json.h"
#include "geo.h"

using namespace std;

struct BusInfo{
  size_t NumberOfStops;
  size_t NumberOfUniqueStops;
  double ActualTrack;
  double Curvature;
};

ostream& operator<<(ostream& stream, const optional<BusInfo>& info){
  if (!info){
    stream << "not found";
  }
  else {
    stream << info->NumberOfStops << " stops on route, " <<
      info->NumberOfUniqueStops << " unique stops, " <<
      info->ActualTrack << " route length, " <<
      info->Curvature << " curvature";
  }
  return stream;
}

class TransportManager{
public:
  void AddStop(string name, StopRequest rq){
    stops_[name] = rq.stop;
    stops_info_[name] = {};
    for (const auto& [stop, dist] : rq.neighs){
      dist_from_to_[name][stop] = dist;
      if (dist_from_to_[stop].count(name) < 1){
        dist_from_to_[stop][name] = dist;
      }
    }
  }
  void AddRoute(string name, Route::Request rq){
    routes_[name] = Route::Build(rq, stops_);
    for (auto stop : rq.names){
      stops_info_[stop].insert(name);
    }
  }
  optional<BusInfo> GetBusInfo(const string& name) const {
    if (routes_.count(name) < 1){
      return nullopt;
    }
    else {
      auto ActualTrack = routes_.at(name)->ActualTrack(dist_from_to_);
      BusInfo info =
             {routes_.at(name)->StopsOnRoute(),
              routes_.at(name)->UniqueStopsOnRoute(),
              ActualTrack,
              ActualTrack * 1.0 /
                routes_.at(name)->Length()};
      return  info;
    }
  }

  optional<set<string>> GetStopInfo(const string& name) const{
    if (stops_info_.count(name) < 1){
      return nullopt;
    }
    return stops_info_.at(name);
  }

private:
  unordered_map<string, Route::Holder> routes_;
  unordered_map<string, Stop> stops_;
  unordered_map<string, set<string>> stops_info_;
  Distances dist_from_to_;
};

int main(){
  TransportManager tm;
  deque<pair<string, Route::Request>> BusQueue;
  size_t QI, QO;
  cin >> QI;
  cout.precision(15);
  string line;
  for (size_t i = 0; i <= QI; ++i){
    getline(cin, line);
    auto [command, rest] = SplitTwo(line);
    if (command == "Stop"){
      auto [name, args] = SplitTwo(rest, ": ");
      tm.AddStop(name, ReadStop(args));
    }
    else if (command == "Bus"){
      auto [name, args] = SplitTwo(rest, ": ");
      BusQueue.push_back({name, Route::Read(args)});
    }
  }
  for(const auto& bus : BusQueue){
    tm.AddRoute(bus.first, bus.second);
  }
  cin >> QO;
  for (size_t i = 0; i <= QO; ++i){
    getline(cin, line);
    auto [command, args] = SplitTwo(line);
    if (command == "Bus"){
      cout << "Bus " << args  << ": "
        << tm.GetBusInfo(args) << endl;
    }
    else if (command == "Stop"){
      cout << "Stop " << args  << ": "
        << tm.GetStopInfo(args) << endl;
    }
  }
  return 0;
}
