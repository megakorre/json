#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include <iostream>

namespace json {  
  struct JsonBase {
    typedef std::shared_ptr<JsonBase> json_ref;
    
    virtual bool is_array()  { return false; }
    virtual bool is_record() { return false; }
    virtual bool is_number() { return false; }
    virtual bool is_string() { return false; }
    virtual bool is_bool()   { return false; }
    virtual bool is_null()   { return false; }
    
    virtual std::vector<json_ref> as_vector() { /* TODO throw error */ }
    virtual std::map<std::string, json_ref> as_map() { /* TODO throw error */ }
    virtual std::string as_string() { /* TODO throw error */ }
    virtual double as_number() { /* TODO throw error */ }
    virtual bool as_bool() { /* TODO throw error */ }

    // TODO: steal methods or ref or something

    virtual operator bool() { return true; }
  };

  typedef JsonBase::json_ref json_ref;

  struct JsonString : public JsonBase {
    virtual bool is_string() { return true; }    
    virtual std::string as_string() { return _val; }

    std::string _val;
    
    JsonString(std::string val) : _val(val) {}
  };

  struct JsonBool : public JsonBase {
    virtual bool is_bool() { return true; }
    virtual operator bool() { return _bool; }
    virtual bool as_bool() { return _bool; }

    bool _bool;

    JsonBool(bool _bool) : _bool(_bool) {}
  };

  struct JsonNull : public JsonBase {
    virtual bool is_null() { return true; }

  };

  struct JsonArray : public JsonBase {
    std::vector<json_ref> _array;
    JsonArray(std::vector<json_ref> _array)
      : _array(_array) {}

    virtual bool is_array() { return true; }
    
    virtual std::vector<json_ref> as_vector() { return _array; }
  };

  struct JsonRecord : public JsonBase {
    std::map<std::string, json_ref> _map;
    JsonRecord(std::map<std::string, json_ref> _map)
      : _map(_map) {}
    
    virtual bool is_record() { return true; }
    virtual std::map<std::string, json_ref> as_map() { return _map; }
  };

  struct JsonNumber : public JsonBase {
    double _double;
    JsonNumber(double d)
      : _double(d) {}

    virtual bool is_number() { return true; }
    virtual double as_number() { return _double; }
  };

  int skip_white_space(std::string& content, int i) {
    // std::cout << " : " << i << " " << (i > content.length()) << std::endl;
    while (true) {
      if (content[i] == ' ' || content[i] == '\t' || content[i] == '\n') {
        i++;
        continue;
      }
      return i;
    }
    // TODO: throw parse error here
  }
  
  std::string numbers("0123456789");

  bool is_number(std::string& content, int i) {
    return numbers.find(content[i]) != std::string::npos;
  }
  
  bool is_exact(std::string& content, int i, std::string excact) {
    return content.substr(i, excact.length()) == excact;
  }

  typedef std::pair<int, json_ref> parse_res;

  parse_res parse_string(std::string& content, int i) {
    bool escape = false;
    std::stringstream s;
    for (i++; i < content.length(); i++) {
      if (escape) {
        s << content[i];        
        escape = false;
        continue;
      }
      
      if (content[i] == '\\') {
        escape = true;
        continue;
      }
      
      if (content[i] == '"') {
        return std::make_pair(i + 1, std::make_shared<JsonString>(s.str()));
      }
      
      s << content[i];
    }
    // TODO: add parse error
  }

  parse_res parse(std::string&, int);

  parse_res parse_record(std::string& content, int i) {
    std::map<std::string, json_ref> res;
    bool first = true;
    i++;
    while (true) {            
      i = skip_white_space(content, i);
            
      if (content[i] == '}') {
        return std::make_pair(i + 1, std::make_shared<JsonRecord>(res));
        
      }
      if (!first) {
        if (content[i] != ',') { /* TODO throw error */ } 
        i = skip_white_space(content, i + 1);
      }
      first = false;

      if (content[i] != '"') { /* TODO throw error */ } 
      
      auto resp_k = parse_string(content, i);
      std::string key = resp_k.second->as_string();

      i = skip_white_space(content, resp_k.first);
      
      if (content[i] != ':') { /* TODO throw error */ } 
      
      auto resp_v = parse(content, i + 1);

      i = resp_v.first;
      res[key] = resp_v.second;
    }
  }

  parse_res parse_array(std::string& content, int i) {
    std::vector<json_ref> res;
    bool first = true;
    i++;
    while(true) {
      
      i = skip_white_space(content, i);
      
      if (content[i] == ']') return std::make_pair(i + 1, std::make_shared<JsonArray>(res));      

      if (!first) {
        if (content[i] != ',') { /* TODO throw error */ } 
        i = skip_white_space(content, i + 1);
      }
      first = false;
      
      auto resp = parse(content, i);
      i = resp.first;
      res.push_back(resp.second);
    }
  }

  parse_res parse_number(std::string& content, int i) {
    std::stringstream s(content);
    s.seekg(i);
    
    double d;
    s >> d;
    
    return std::make_pair(s.tellg(), std::make_shared<JsonNumber>(d));
  }

  std::shared_ptr<JsonBool>  js_true  = std::make_shared<JsonBool>(true);
  std::shared_ptr<JsonBool>  js_false = std::make_shared<JsonBool>(false);
  std::shared_ptr<JsonNull>  js_null  = std::make_shared<JsonNull>();
  
  parse_res parse(std::string& content, int i = 0) {
    int index = skip_white_space(content, i);
        
    if (content[index] == '"') return parse_string(content, index);
    if (content[index] == '{') return parse_record(content, index);
    if (content[index] == '[') return parse_array(content, index);

    if (is_number(content, i)) return parse_number(content, index);
    
    if (is_exact(content, index, "null"))  return std::make_pair(i + 5, js_null);
    if (is_exact(content, index, "true"))  return std::make_pair(i + 5, js_true);
    if (is_exact(content, index, "false")) return std::make_pair(i + 6, js_false);

    // TODO: throw parse error
  }
}




