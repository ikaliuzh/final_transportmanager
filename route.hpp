#pragma once

#include <list>
#include <string>
#include <utility>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "geo.h"
#include "utils.hpp"

using namespace std;

using Distances = unordered_map<string, unordered_map<string, double>>;

class RouteBasic{
public:
  virtual double Length() const = 0;
  virtual double ActualTrack(const Distances&) const =0;
  virtual size_t StopsOnRoute() const = 0;
  virtual size_t UniqueStopsOnRoute() const = 0;
  virtual ~RouteBasic() = default;
};

namespace Route{
  using Holder = shared_ptr<RouteBasic>;

  enum struct Type {
    CIRCLED,
    LINEAR
  };

  struct Request{
    Type type;
    list<string> names;
  };

  class Circled : public RouteBasic{
  public:
    Circled(const list<string>& names, const unordered_map<string, Stop>& stops){
      for (const string& name : names){
        stops_.push_back(
          make_pair(name, stops.at(name))
        );
      }
    }

    double Length() const override final{
      double length = 0.0;
      if (stops_.begin() == stops_.end()){
        return length;
      }
      for (auto it = stops_.begin(); next(it) != stops_.end(); ++it){
        length += ComputeDistance(it->second, next(it)->second);
      }
      return length;
    }

    double ActualTrack(const Distances& DistFromTo) const override final{
      double length = 0.0;
      if (stops_.begin() == stops_.end()){
        return length;
      }
      for (auto it = stops_.begin(); next(it) != stops_.end(); ++it){
        if (DistFromTo.count(it->first) &&
            DistFromTo.at(it->first).count(next(it)->first)){
          length += DistFromTo.at(it->first).at(next(it)->first);
        }
        else {
          length += ComputeDistance(it->second, next(it)->second);
        }
      }
      return length;
    }

    size_t StopsOnRoute() const override final{
      return stops_.size();
    }

    size_t UniqueStopsOnRoute() const override final{
      unordered_set<pair<string, Stop>, StopHasher> un_stops(stops_.begin(), stops_.end());
      return un_stops.size();
    }

  protected:
    list<pair<string, Stop>> stops_;
  };

  class Linear : public Circled{
  public:
    Linear(const list<string>& names, const unordered_map<string, Stop>& stops)
      : Circled(names, stops) {
        if (stops_.begin() != stops_.end() && stops_.begin() != prev(stops_.end())){
          bool flag = true;
          for (auto it = prev(prev(stops_.end())); flag; --it){
            if (it == stops_.begin()) flag = false;
            stops_.push_back(*it);
          }
        }
      }
  };

  Holder Build(Request rq, const unordered_map<string, Stop>& stops){
    switch(rq.type){
      case Type::CIRCLED:
        return make_shared<Circled>(rq.names, stops);
      case Type::LINEAR:
        return make_shared<Linear>(rq.names, stops);
    }
    //For compiler not to be angry!
    return make_shared<Linear>(rq.names, stops);
  }

  Request Read(string line){
    list<string> names;
    if (line.find(">") != line.npos){
      while (!line.empty()){
        names.push_back(ReadToken(line, " > "));
      }
      return {Type::CIRCLED, names};
    }
    else {
      while (!line.empty()){
        names.push_back(ReadToken(line, " - "));
      }
      return {Type::LINEAR, names};
    }
  }
}//Route
