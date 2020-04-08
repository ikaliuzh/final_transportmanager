#pragma once

#include <string>
#include <utility>
#include <set>
#include <list>
#include <optional>
#include <iostream>

using namespace std;

pair<string, optional<string>> SplitTwoStrict(string s, string delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

pair<string, string> SplitTwo(string s, string delimiter = " ") {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string ReadToken(string& s, string delimiter = " ") {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

template <typename T>
ostream& operator<< (ostream& stream, const list<T>& l){
  for (const auto& i : l){
    stream << i << " ";
  }
  return stream;
}

template <typename T>
ostream& operator<< (ostream& stream, const set<T>& l){
  for (const auto& i : l){
    stream << i << " ";
  }
  return stream;
}

template <typename T>
ostream& operator<< (ostream& stream, const optional<set<T>>& s){
  if (!s){
    stream << "not found";
  }
  else {
    if (s->size() < 1){
      stream << "no buses";
    }
    else {
      stream << "buses " << *s;
    }
  }
  return stream;
}
