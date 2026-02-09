#include "DRM_can.h"

namespace DRM {

CanNode::CanNode(CanNetwork& bindTo) {
  bindTo.nodes_[bindTo.nodeCount_] = this;
  bindTo.nodeCount_++;
}


CanNetwork* CanNetwork::instances_[1] = {};

CanNetwork::CanNetwork(bool channel)
    : can_(nullptr)
    , nodes_{}
    , nodeCount_(0U)
    , lastUpdate_(0U) {

  if (channel == 0) {
    can_ = &CAN0;
    can_->setGeneralCallback(callback0);
  }
  can_->begin(CAN_BAUD_RATE);
  can_->watchFor();
  instances_[channel] = this;
}

void CanNetwork::callback0(CAN_FRAME* frame) {
  instances_[0]->onCallback(*frame);
}

bool CanNetwork::update() {
  uint32_t current = micros();
  if (current - lastUpdate_ < CAN_UPDATE_INTERVAL) {
    return false;
  }
  lastUpdate_ = current;

  for (uint32_t i = 0U; i < nodeCount_; i++) {
    CAN_FRAME frame;
    while (nodes_[i]->onSend(frame)) {
      can_->sendFrame(frame);
    }
    can_->sendFrame(frame);
  }
  return true;
}

void CanNetwork::onCallback(CAN_FRAME& frame) {
  for (uint32_t i = 0U; i < nodeCount_; i++) {
    if (nodes_[i]->onReceive(frame)) {
      break;
    }
  }
}

}  // namespace DRM
