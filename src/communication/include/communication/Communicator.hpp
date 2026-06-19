#pragma once
#include <communication/Concepts.hpp>
#include <communication/RosCommunicatorApp.hpp>
#include <memory>
#include <unordered_map>
#include <variant>

namespace HBR::Communication
{

// ---------------------------------------------------------------------------
// Closed-set variant types — extend these as new backends are added.
// Each variant holds owning pointers so the concrete types need not be movable.
// ---------------------------------------------------------------------------
using MessengerApp_VT = std::variant<
  std::unique_ptr<RosCommunicatorApp>
>;

using CommunicatorApp_VT = std::variant<
  std::unique_ptr<RosCommunicatorApp>
>;

// No concrete FullCommunicatorApp yet; monostate holds the slot open.
using FullCommunicatorApp_VT = std::variant<
  std::monostate
>;

// No concrete StreamerApp yet; monostate holds the slot open.
using StreamerApp_VT = std::variant<
  std::monostate
>;

// ---------------------------------------------------------------------------
// Communicator — installs and manages named communication apps.
// Acts as a switchboard: the node chooses which installed app to use for
// any given channel, regardless of the underlying transport.
// ---------------------------------------------------------------------------
class Communicator
{
public:
  template<MessengerApp M>
  STATUS InstallMessengerApp(const std::string & name)
  {
    messengers[name] = std::make_unique<M>();
    return OK;
  }

  template<CommunicatorApp C>
  STATUS InstallCommunicatorApp(const std::string & name)
  {
    communicators[name] = std::make_unique<C>();
    return OK;
  }

  template<FullCommunicatorApp FC>
  STATUS InstallFullCommunicatorApp(const std::string & name)
  {
    fullCommunicators[name] = std::make_unique<FC>();
    return OK;
  }

  template<StreamerApp S>
  STATUS InstallStreamingApp(const std::string & name)
  {
    streamers[name] = std::make_unique<S>();
    return OK;
  }

  STATUS UninstallApp(const std::string & name)
  {
    bool erased = messengers.erase(name) || communicators.erase(name) ||
      fullCommunicators.erase(name) || streamers.erase(name);
    return erased ? OK : ERROR;
  }

  // Retrieve a typed messenger app by name. Returns nullptr if not found or wrong type.
  template<MessengerApp M>
  M * GetMessengerApp(const std::string & name)
  {
    auto it = messengers.find(name);
    if (it == messengers.end()) {return nullptr;}
    auto * ptr = std::get_if<std::unique_ptr<M>>(&it->second);
    return ptr ? ptr->get() : nullptr;
  }

  template<CommunicatorApp C>
  C * GetCommunicatorApp(const std::string & name)
  {
    auto it = communicators.find(name);
    if (it == communicators.end()) {return nullptr;}
    auto * ptr = std::get_if<std::unique_ptr<C>>(&it->second);
    return ptr ? ptr->get() : nullptr;
  }

private:
  std::unordered_map<std::string, MessengerApp_VT> messengers;
  std::unordered_map<std::string, CommunicatorApp_VT> communicators;
  std::unordered_map<std::string, FullCommunicatorApp_VT> fullCommunicators;
  std::unordered_map<std::string, StreamerApp_VT> streamers;
};

static_assert(
  CommunicatorInterface<Communicator,
  RosCommunicatorApp,
  RosCommunicatorApp,
  ProxyFullCommunicatorApp,
  ProxyStreamerApp>);

} // namespace HBR::Communication
