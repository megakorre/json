# Json parser c++

```c++

  /**
        the libary throws json::parse_exception
        witch inherits from std::exception

        // parse returns a pair with the parsed json_ref
        // and a index for parsing more content from the string
        json::parse(string& content, [int index]) -> pair<int, json::json_ref>

        PS: the json_ref are using std::shared_ptr
        
        json_ref methods:

                 reflection stuff:
                     virtual bool is_array()  
                     virtual bool is_record() 
                     virtual bool is_number() 
                     virtual bool is_string() 
                     virtual bool is_bool()   
                     virtual bool is_null()   
                 
                 extraction stuff:

                     virtual std::vector<json_ref> as_vector() 
                     virtual std::map<std::string, json_ref> as_map() 
                     virtual std::string as_string() 
                     virtual double as_number() 
                     virtual bool as_bool() 
  */

  auto json_resp = json::parse(content).second;
  
  auto results = v->as_map()["result"];
  
  for (auto some_thing : v->as_vector()) {
      cout << v->as_number() << endl;
  }
  
```
