#ifndef DRM_CAN_H
#define DRM_CAN_H

#include <cstdint>
#include <esp32_can.h>

#include "DRM_config.h"

namespace DRM {

class CanNetwork;  // forward declaration

// Interface for nodes participating in a CAN network.
class CanNode {
 public:
  friend class CanNetwork;
  CanNode(CanNetwork& bindTo);

 private:
  // Returns true if this node handled the frame.
  // Returns false if the frame was ignored.
  virtual bool onReceive(CAN_FRAME& frame) = 0;

  // Returns true if more frames will follow.
  // Returns false if this was the last frame.
  //
  // Typical usage example:
  //   1st call -> fills frame #1, returns true
  //   2nd call -> fills frame #2, returns false
  // (No further calls until the next send sequence begins.)
  virtual bool onSend(CAN_FRAME& frame) = 0;
};


// Handles collection and dispatching of CAN frames for CanNode.
class CanNetwork {
 public:
  friend class CanNode;
  explicit CanNetwork(bool channel);

  static void callback0(CAN_FRAME* frame);

  // Returns true if updated successfully.
  // Returns false if called too soon.
  bool update();

 private:
  void onCallback(CAN_FRAME& frame);

  static CanNetwork* instances_[1];

  ESP32CAN* can_;
  CanNode* nodes_[CAN_MAX_NODES];
  uint32_t nodeCount_;
  uint32_t lastUpdate_;
};

}  // namespace DRM

#endif  // DRM_CAN_H
