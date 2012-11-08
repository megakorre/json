
#include "json.h"
#include <vector>
#include <functional>
#include <iostream>

using namespace std;

vector<string> errors;
int tests_run;

void test(string msg, function<bool()> ft) {
  tests_run++;
  if (! ft() ) {
    errors.push_back(msg);
  }
}

void print_res() {
  cout << "ran " << tests_run << " tests failed: " << errors.size() << endl << endl;
  
  for (auto t: errors) {
    cout << "* " << t << endl;
  }
}

int main() {

  test("parse true", []() {
      string c("true");
      auto t = json::parse(c).second;
      
      return t->is_bool() && t;
    });

  test("parse string", []() {
      string c("\"hej\"");

      auto s = json::parse(c).second->as_string();

      return s == "hej";
    });

  test("parse null", []() {
      string c("null");

      auto n = json::parse(c).second;

      return n->is_null();
    });

  test("parse number", []() {
      string c("0.23");

      auto n = json::parse(c).second;

      return n->is_number() && !n->is_null() && n->as_number() == 0.23;
    });

  test("parse array", []() {
      string c("[1,2,3,4]");      
      
      auto a = json::parse(c).second;

      vector<json::json_ref> v = a->as_vector();
      
      return v[0]->as_number() == 1 &&
        v[1]->as_number() == 2 &&
        v[2]->as_number() == 3 &&
        v[3]->as_number() == 4;
    });
  

  test("parse record", []() {
      string c("{ \"hej\": null }");
      
      auto a = json::parse(c).second->as_map();

      cout << "here?? " << a.size() <<  endl;
      for (auto p: a) {
        cout << "key: " << p.first << endl;
      }
      
      return a["hej"]->is_null();
      
    });


  string c("{ \"hej\"  : [1,2,3,4], \
                  \"what\" : { \"some\" : true },       \
                  \"123\"  : \"hello\",                 \
                  \"you\"  : null }");

  test("nested nastiness vector", [&c]() {
      return json::parse(c).second->as_map()["hej"]->as_vector()[2]->as_number() == 3;
    });
  
  test("nested nastiness null", [&c]() {
      return json::parse(c).second->as_map()["you"]->is_null();
    });

  test("nested nastiness number", [&c]() {
      return json::parse(c).second->as_map()["123"]->as_string() == "hello";
    });
  
  test("nested nastiness map", [&c]() {
      return json::parse(c).second->as_map()["what"]->as_map()["some"]->as_bool();
    });

  print_res();
}
