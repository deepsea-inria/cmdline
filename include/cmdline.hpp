
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <map>
#include <functional>

#ifndef _PASL_UTIL_CMDLINE_H_
#define _PASL_UTIL_CMDLINE_H_

namespace pasl {
namespace util {
namespace cmdline {
  
namespace {
  int global_argc = -1;
  char** global_argv;
  
  bool print_warning_on_use_of_default_value = false;
  
  static void failure() {
    printf("Error illegal command line\n");
    exit(-1);
  }
  
  static void check_set() {
    if (global_argc == -1) {
      printf("you must call cmdline::set(argc,argv) in your main.");
      exit(-1);
    }
  }
  
  static void check(std::string name, bool result) {
    if (! result) {
      printf("missing command line argument %s\n", name.c_str());
      exit(-1);
    }
  }
  
  template <class T>
  void print_default(std::string name, T val, bool expected) {
    if (! expected || ! print_warning_on_use_of_default_value)
      return;
    std::cerr << "Warning: using default for " << name << " " << val << std::endl;
  }
  
  using type_t = enum {
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    STRING,
    BOOL
  };
  
  // Parsing of one value of a given type into a given addres
  static void parse_value(type_t type, void* dest, char* arg_value) {
    switch (type) {
      case INT: {
        int* vi = (int*) dest;
        *vi = atoi(arg_value);
        break; }
      case LONG: {
        long* vi = (long*) dest;
        sscanf(arg_value, "%ld", vi);
        break; }
      case BOOL: {
        bool* vb = (bool*) dest;
        *vb = (atoi(arg_value) != 0);
        break; }
      case FLOAT: {
        float* vf = (float*) dest;
        *vf = (float)atof(arg_value);
        break; }
      case DOUBLE: {
        double* vf = (double*) dest;
        *vf = atof(arg_value);
        break; }
      case STRING: {
        std::string* vs = (std::string*) dest;
        *vs = std::string(arg_value);
        break; }
      default: {
        printf("not yet supported");
        exit(-1); }
    }
  }
  
  static bool parse(type_t type, std::string name, void* dest) {
    check_set();
    for (int a = 1; a < global_argc; a++) {
      if (*(global_argv[a]) != '-')
        failure();
      char* arg_name = global_argv[a] + 1;
      if (arg_name[0] == '-') {
        if (name.compare(arg_name+1) == 0) {
          *((bool*) dest) = 1;
          return true;
        }
      } else {
        a++;
        if (a >= global_argc)
          failure();
        char* arg_value = global_argv[a];
        if (name.compare(arg_name) == 0) {
          parse_value(type, dest, arg_value);
          return true;
        }
      }
    }
    return false;
  }
  
} // end namespace
  
/*---------------------------------------------------------------------*/
/* Initialization */

void set(int argc, char** argv) {
  global_argc = argc;
  global_argv = argv;
}
  
/*---------------------------------------------------------------------*/
/* Specific parsing functions */

bool parse_bool(std::string name) {
  bool r;
  check (name, parse(BOOL, name, &r));
  return r;
}

int parse_int(std::string name) {
  int r;
  check (name, parse(INT, name, &r));
  return r;
}

long parse_long(std::string name) {
  long r;
  check (name, parse(LONG, name, &r));
  return r;
}

float parse_float(std::string name) {
  float r;
  check(name, parse(FLOAT, name, &r));
  return r;
}

double parse_double(std::string name) {
  double r;
  check(name, parse(DOUBLE, name, &r));
  return r;
}

std::string parse_string(std::string name) {
  std::string r;
  check(name, parse(STRING, name, &r));
  return r;
}
  
/*---------------------------------------------------------------------*/
/* Specific parsing functions with default values */

bool parse_or_default_bool(std::string name, bool d,
                           bool expected=true) {
  bool r;
  if (parse(BOOL, name, &r)) {
    return r;
  } else {
    print_default(name, d, expected);
    return d;
  }
}

int parse_or_default_int(std::string name, int d,
                         bool expected=true) {
  int r;
  if (parse(INT, name, &r)) {
    return r;
  } else {
    print_default(name, d, expected);
    return d;
  }
}

long parse_or_default_long(std::string name, long d,
                           bool expected=true) {
  long r;
  if (parse(LONG, name, &r)) {
    return r;
  } else {
    print_default(name, d, expected);
    return d;
  }
}

float parse_or_default_float(std::string name, float f,
                             bool expected=true) {
  float r;
  if (parse(FLOAT, name, &r)) {
    return r;
  } else {
    print_default(name, f, expected);
    return f;
  }
}

double parse_or_default_double(std::string name, double d,
                               bool expected=true) {
  double r;
  if (parse(DOUBLE, name, &r)) {
    return r;
  } else {
    print_default(name, d, expected);
    return d;
  }
}

std::string parse_or_default_string(std::string name, std::string d,
                                    bool expected=true) {
  std::string r;
  if (parse(STRING, name, &r)) {
    return r;
  } else {
    print_default(name, d, expected);
    return d;
  }
}
  
/*---------------------------------------------------------------------*/
/* Map */
  
template <class Item>
class map {
private:
  
  std::map<std::string, Item> m;
  
  void failwith(std::string parameter, std::string key) {
    std::cout << "Not found: -" << parameter << " " << key << std::endl;
    std::cout << "Valid arguments are: ";
    for (auto it = m.begin(); it != m.end(); it++)
      std::cout << it->first << " ";
    std::cout << std::endl;
    exit(1);
  }
  
public:
  
  void insert(std::string key, Item value) {
    m.insert(std::make_pair(key, value));
  }
  
  Item& find(std::string key, std::string parameter) {
    auto it = m.find(key);
    if (it == m.end()) {
      failwith(parameter, key);
    }
    return (*it).second;
  }
  
  Item& find_by_parameter(std::string parameter) {
    return find(parse_or_default_string(parameter, ""), parameter);
  }
  
  Item find_by_parameter_or_default(std::string parameter, const Item& dflt) {
    std::string key = parse_or_default_string(parameter, "");
    auto it = m.find(key);
    if (it == m.end())
      return Item(dflt);
    return (*it).second;
  }
  
  Item find_by_parameter_or_default_key(std::string parameter, std::string dflt) {
    std::string key = parse_or_default_string(parameter, dflt);
    return find(parameter, key);
  }
  
  template <class Body>
  void for_each_key(const Body& f) const {
    for (auto it = m.begin(); it != m.end(); it++) {
      f(it->first, it->second);
    }
  }
  
};
  
using dispatcher = map<std::function<void()>>;
  
void dispatch(dispatcher& d, std::string parameter) {
  d.find_by_parameter(parameter)();
}

void dispatch(dispatcher& d, std::string parameter, std::string dflt_key) {
  d.find_by_parameter_or_default_key(parameter, dflt_key)();
}
  
} // end namespace
} // end namespace
} // end namespace

#endif /*! _PASL_UTIL_CMDLINE_H_ */