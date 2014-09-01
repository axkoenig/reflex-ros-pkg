/////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2014 Open Source Robotics Foundation, Inc.
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef REFLEX_HAND_H
#define REFLEX_HAND_H

#include <netinet/in.h>
#include <vector>
#include <boost/function.hpp>
#include <string>

namespace reflex_hand
{

class ReflexHandState
{
public:
  static const int NUM_FINGERS = 3;
  static const int NUM_TACTILE = NUM_FINGERS * 9 + 11;
  uint32_t systime_us_;
  uint16_t tactile_pressures_[NUM_TACTILE];
  uint16_t tactile_temperatures_[NUM_TACTILE];
  uint16_t encoders_[NUM_FINGERS];
  uint8_t  dynamixel_error_states_[4];
  uint16_t dynamixel_angles_[4];
  uint16_t dynamixel_speeds_[4];
  uint16_t dynamixel_loads_[4];
  uint8_t  dynamixel_voltages_[4];
  uint8_t  dynamixel_temperatures_[4];
  ReflexHandState();
};

class ReflexHand
{
public:
  const static int NUM_SERVOS = 4;
  const static int PORT_BASE = 11333;
  static const float DYN_SCALE = (2*3.141596)/4095;
  static const float ENC_SCALE = (2*3.141596)/16383;
  enum ControlMode { CM_IDLE = 0, CM_VELOCITY = 1, CM_POSITION = 2 };

  typedef boost::function<void(const ReflexHandState * const)> StateCallback;
  void setStateCallback(StateCallback callback);

  ReflexHand(const std::string &interface);
  ~ReflexHand();
  bool listen(const double max_seconds);
  void setServoTargets(const uint16_t *targets);
  void setServoControlModes(const ControlMode *modes);
  void setServoControlModes(const ControlMode mode);
  bool happy() { return happy_; }
private:
  enum CommandPacket { CP_SET_SERVO_MODE = 1,
                       CP_SET_SERVO_TARGET = 2 };
  int tx_sock_, rx_sock_;
  sockaddr_in mcast_addr_;
  StateCallback state_cb_;
  ReflexHandState rx_state_;
  bool happy_;
  void tx(const uint8_t *msg, const uint16_t msg_len, const uint16_t port);
  void rx(const uint8_t *msg, const uint16_t msg_len);
};

}

#endif

