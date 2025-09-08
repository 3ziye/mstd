
#ifndef __IP_STRING_H_
#define __IP_STRING_H_

#ifdef MSTD_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif // MSTD_WINDOWS

#include <stddef.h>
#include <cstdio>
#include <algorithm>
#include <string>

namespace mstd {

class ipv4_address {
public:
  ipv4_address() : ip() {}

  ipv4_address(unsigned char a,
               unsigned char b,
               unsigned char c,
               unsigned char d)
  {
    ip[0] = a;
    ip[1] = b;
    ip[2] = c;
    ip[3] = d;
  }

  explicit ipv4_address(const std::string& s) {
    int buf[4];
    if (std::sscanf(s.c_str(), "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]) != 4) {
      *this = none;
      return;
    }
    for (size_t i = 0; i < sizeof(buf)/sizeof(buf[0]); ++i) {
      ip[i] = buf[i];
      if (buf[i] < 0 || buf[i] > 255) {
        *this = none;
        return;
      }
    }
  }

  explicit ipv4_address(uint32_t ipaddr) {
	  ip[0] = (ipaddr >> 24) & 0xFF;
	  ip[1] = (ipaddr >> 16) & 0xFF;
	  ip[2] = (ipaddr >> 8) & 0xFF;
	  ip[3] = ipaddr & 0xFF;
  }

  bool operator==(const ipv4_address& p) const {
    return std::equal(ip, ip+sizeof(ip), p.ip);
  }
  bool operator!=(const ipv4_address& p) const {
    return !(*this == p);
  }
  bool operator<(const ipv4_address& p) const {
    return std::lexicographical_compare(ip, ip+sizeof(ip), p.ip, p.ip+sizeof(p.ip));
  }

  std::string to_string() const {
    char buf[16];
    std::sprintf(buf, "%d.%d.%d.%d", int(ip[0]), int(ip[1]), int(ip[2]), int(ip[3]));
    return buf;
  }

  uint32_t to_number() const {
	  uint32_t ipv4 = (
		  ip[0] << 24 |
		  ip[1] << 16 |
		  ip[2] << 8 |
		  ip[3]
		  );
	  return ipv4;
  }

  static const ipv4_address any;
  static const ipv4_address broadcast;
  static const ipv4_address loopback;
  static const ipv4_address none;

private:
  unsigned char ip[4];
};

std::string ip_to_string(const uint32_t ip);
uint32_t ip_from_string(const std::string &ip);

} // stdxx
#endif // #ifndef __IP_STRING_H_
